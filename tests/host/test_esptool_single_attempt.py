from __future__ import annotations

import argparse
import contextlib
import importlib
from pathlib import Path
import sys
import tempfile
import unittest
from unittest import mock


REPOSITORY_ROOT = Path(__file__).resolve().parents[2]
TOOL_ROOT = REPOSITORY_ROOT / "tools" / "first_flash"
sys.path.insert(0, str(TOOL_ROOT))
import esptool_single_attempt as harness  # noqa: E402


class SerialProhibitionMixin:
    def setUp(self) -> None:
        serial = importlib.import_module("serial")
        list_ports = importlib.import_module("serial.tools.list_ports")
        self.serial_for_url = mock.patch.object(
            serial,
            "serial_for_url",
            side_effect=AssertionError("real serial open is prohibited in host tests"),
        )
        self.serial_class = mock.patch.object(
            serial,
            "Serial",
            side_effect=AssertionError("real serial open is prohibited in host tests"),
        )
        self.comports = mock.patch.object(
            list_ports,
            "comports",
            side_effect=AssertionError("serial-port enumeration is prohibited in host tests"),
        )
        self.serial_for_url_mock = self.serial_for_url.start()
        self.serial_class_mock = self.serial_class.start()
        self.comports_mock = self.comports.start()
        self.addCleanup(self.serial_for_url.stop)
        self.addCleanup(self.serial_class.stop)
        self.addCleanup(self.comports.stop)
        self.addCleanup(self._assert_no_serial_activity)

    def _assert_no_serial_activity(self) -> None:
        self.serial_for_url_mock.assert_not_called()
        self.serial_class_mock.assert_not_called()
        self.comports_mock.assert_not_called()


class InstallationGuardTests(SerialProhibitionMixin, unittest.TestCase):
    def test_exact_version_allows_audit(self) -> None:
        result = harness.audit()
        self.assertEqual(result["installation"]["esptool_version"], "4.12.dev3")
        self.assertFalse(result["serial_open"])
        self.assertFalse(result["port_enumeration"])

    def test_other_version_is_rejected(self) -> None:
        with self.assertRaises(harness.HarnessError):
            harness._validate_version("4.12")

    def test_expected_source_structure_is_present(self) -> None:
        harness.installed_integrity()
        modules = harness._import_audited_modules()
        result = harness.verify_runtime_structure(
            modules, require_audited_defaults=True
        )
        self.assertEqual(result["values"]["outer_operation_attempts"], 2)
        self.assertEqual(result["values"]["block_attempts"], 3)

    def test_missing_patch_point_is_rejected(self) -> None:
        modules = harness._import_audited_modules()
        loader_class = modules["loader"].ESPLoader
        original = loader_class.WRITE_FLASH_ATTEMPTS
        del loader_class.WRITE_FLASH_ATTEMPTS
        try:
            with self.assertRaisesRegex(harness.HarnessError, "patch point"):
                harness.verify_runtime_structure(
                    modules, require_audited_defaults=True
                )
        finally:
            loader_class.WRITE_FLASH_ATTEMPTS = original

    def test_esptool_config_override_is_rejected(self) -> None:
        modules = harness._import_audited_modules()
        with mock.patch.dict(
            "os.environ", {"ESPTOOL_CFGFILE": "synthetic-config-path"}, clear=False
        ):
            with self.assertRaisesRegex(harness.HarnessError, "configuration"):
                harness.verify_runtime_structure(
                    modules, require_audited_defaults=True
                )

    def test_installed_package_integrity_is_unchanged(self) -> None:
        before = harness.installed_integrity()
        with harness.single_attempt_overrides():
            pass
        after = harness.installed_integrity()
        self.assertEqual(before, after)


class ExactlyOnceTests(SerialProhibitionMixin, unittest.TestCase):
    def _write_args(self, source) -> argparse.Namespace:
        return argparse.Namespace(
            addr_filename=[(harness.CANDIDATE_OFFSET, source)],
            encrypt=False,
            encrypt_files=None,
            ignore_flash_encryption_efuse_setting=False,
            force=False,
            flash_size="keep",
            erase_all=False,
            no_stub=True,
            compress=False,
            flash_mode="keep",
            flash_freq="keep",
            no_progress=True,
        )

    def test_outer_operation_failure_is_called_once(self) -> None:
        modules = harness._import_audited_modules()
        cmds = modules["cmds"]
        loader = modules["loader"]
        serial_exception = importlib.import_module("serial").SerialException

        class FakeEsp(loader.ESPLoader):
            CHIP_NAME = "ESP32-S3"
            FLASH_ENCRYPTED_WRITE_ALIGN = 16
            FLASH_SECTOR_SIZE = 0x1000
            FLASH_WRITE_SIZE = 0x400
            IS_STUB = False
            secure_download_mode = False
            calls = 0

            def __init__(self):
                pass

            def get_secure_boot_enabled(self):
                return False

            def get_encrypted_download_disabled(self):
                return False

            def get_flash_encryption_enabled(self):
                return False

            def flash_begin(self, *_args, **_kwargs):
                self.calls += 1
                raise serial_exception("synthetic first operation failure")

        fake = FakeEsp()
        with tempfile.NamedTemporaryFile("w+b", suffix=".bin") as source:
            source.write(b"\xE9\x00\x00\x00")
            source.seek(0)
            with harness.single_attempt_overrides(modules):
                with mock.patch.object(cmds, "detect_flash_size", return_value="32MB"):
                    with self.assertRaises(serial_exception):
                        cmds.write_flash(fake, self._write_args(source))
        self.assertEqual(fake.calls, 1)

    def test_block_failure_is_called_once(self) -> None:
        modules = harness._import_audited_modules()
        loader = modules["loader"]

        class FakeLoader:
            ESP_FLASH_DATA = loader.ESPLoader.ESP_FLASH_DATA
            calls = 0

            @staticmethod
            def checksum(_data):
                return 0

            def check_command(self, *_args, **_kwargs):
                self.calls += 1
                raise loader.FatalError("synthetic first block failure")

            def trace(self, *_args, **_kwargs):
                raise AssertionError("retry trace must not be reached")

        fake = FakeLoader()
        with harness.single_attempt_overrides(modules):
            with self.assertRaises(loader.FatalError):
                loader.ESPLoader.flash_block(fake, b"\x00" * 16, 0)
        self.assertEqual(fake.calls, 1)

    def test_sync_failure_is_called_once(self) -> None:
        modules = harness._import_audited_modules()
        loader = modules["loader"]

        class FakePort:
            def reset_input_buffer(self):
                pass

            def inWaiting(self):
                return 0

            def read(self, _length):
                return b""

            def flushOutput(self):
                pass

        class FakeLoader:
            USES_RFC2217 = False
            _port = FakePort()
            sync_calls = 0

            def flush_input(self):
                pass

            def sync(self):
                self.sync_calls += 1
                raise loader.FatalError("synthetic first sync failure")

        fake = FakeLoader()
        reset_calls = 0

        def reset_once():
            nonlocal reset_calls
            reset_calls += 1

        result = harness._single_sync_connect_attempt(fake, reset_once)
        self.assertIsInstance(result, loader.FatalError)
        self.assertEqual(fake.sync_calls, 1)
        self.assertEqual(reset_calls, 1)

    def test_audit_proves_all_enforced_attempts_are_one(self) -> None:
        result = harness.audit()
        self.assertEqual(result["enforced_attempts"], harness.ENFORCED_ATTEMPTS)


class FailClosedValidationTests(SerialProhibitionMixin, unittest.TestCase):
    def test_missing_authorization_rejects_device_mode(self) -> None:
        with self.assertRaisesRegex(harness.HarnessError, "authorization"):
            harness.execute_device(None)

    def test_wrong_artifact_hash_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as temp_dir:
            artifact = Path(temp_dir) / "candidate.bin"
            artifact.write_bytes(b"not the reviewed candidate")
            with self.assertRaisesRegex(harness.HarnessError, "SHA-256|length"):
                harness.validate_artifact(
                    artifact,
                    expected_size=artifact.stat().st_size,
                    expected_sha256="0" * 64,
                    require_staging_path=False,
                )

    def test_range_mismatch_is_rejected(self) -> None:
        valid = {
            "offset": harness.CANDIDATE_OFFSET,
            "length": harness.CANDIDATE_SIZE,
            "end_exclusive": harness.CANDIDATE_END_EXCLUSIVE,
            "erase_start": harness.ERASE_START,
            "erase_end_exclusive": harness.ERASE_END_EXCLUSIVE,
            "erase_length": harness.ERASE_LENGTH,
        }
        for field in valid:
            case = dict(valid)
            case[field] += 1
            with self.subTest(field=field):
                with self.assertRaises(harness.HarnessError):
                    harness.validate_geometry(**case)

    def test_current_staging_manifest_validates_offline(self) -> None:
        manifest = (
            harness.PACKAGE_ROOTS[0] / "FIRST_FLASH_PACKAGE_MANIFEST.txt"
        )
        result = harness.validate_manifest(manifest)
        self.assertEqual(result["authorization"], "NONE")
        self.assertEqual(result["candidate"]["sha256"], harness.CANDIDATE_SHA256)

    def test_sensitive_connection_output_is_filtered(self) -> None:
        raw = "\n".join(
            (
                "MAC: 7C:DF:A1:12:34:56",
                "Unique chip ID: 123456789",
                "Key digest: DEADBEEF",
                "Writing at 0x00020000...",
            )
        )
        sanitized = harness.sanitize_sensitive_output(raw)
        for secret in ("7C:DF:A1:12:34:56", "123456789", "DEADBEEF"):
            self.assertNotIn(secret, sanitized)
        self.assertIn("Writing at 0x00020000...", sanitized)

    def test_offline_cli_modes_do_not_enter_device_mode(self) -> None:
        output = importlib.import_module("io").StringIO()
        with mock.patch.object(
            harness,
            "execute_device",
            side_effect=AssertionError("offline mode entered device logic"),
        ):
            exit_code = harness.main(["--self-test"], output=output)
        self.assertEqual(exit_code, 0)
        self.assertIn('"result": "PASS"', output.getvalue())


if __name__ == "__main__":
    unittest.main()
