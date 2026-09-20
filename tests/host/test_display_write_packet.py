from __future__ import annotations

import importlib.util
import io
from pathlib import Path
import sys
import unittest
from unittest import mock


ROOT = Path(__file__).resolve().parents[2]
MODULE_PATH = ROOT / "tools" / "display_validation" / "esptool_one_attempt.py"
SPEC = importlib.util.spec_from_file_location("display_write_packet", MODULE_PATH)
assert SPEC is not None and SPEC.loader is not None
packet = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = packet
SPEC.loader.exec_module(packet)


class DisplayWritePacketTests(unittest.TestCase):
    def test_exact_reviewed_args_are_accepted(self):
        packet.validate_reviewed_args(list(packet.EXPECTED_ESPTOOL_ARGS))

    def test_any_argument_change_is_rejected(self):
        args = list(packet.EXPECTED_ESPTOOL_ARGS)
        args[args.index("COM7")] = "COM8"
        with self.assertRaises(packet.PacketError):
            packet.validate_reviewed_args(args)

    def test_missing_authorization_refuses_device_mode_before_execution(self):
        output = io.StringIO()
        with mock.patch.object(packet, "execute_device") as execute:
            result = packet.main(
                ["--device", "--", *packet.EXPECTED_ESPTOOL_ARGS], output=output
            )
        self.assertEqual(result, 2)
        execute.assert_not_called()

    def test_audit_mode_rejects_authorization_argument(self):
        output = io.StringIO()
        with mock.patch.object(packet, "validate_local_evidence") as evidence:
            result = packet.main(
                [
                    "--audit",
                    "--authorization",
                    str(packet.EXPECTED_AUTHORIZATION_PATH),
                    "--",
                    *packet.EXPECTED_ESPTOOL_ARGS,
                ],
                output=output,
            )
        self.assertEqual(result, 2)
        evidence.assert_not_called()

    def test_device_mode_validates_before_loading_device_modules(self):
        with mock.patch.object(packet, "validate_authorization") as authorization:
            authorization.side_effect = packet.PacketError("blocked")
            with mock.patch.object(packet, "validate_local_evidence") as evidence:
                result = packet.execute_device
                with self.assertRaises(packet.PacketError):
                    result(
                        packet.EXPECTED_AUTHORIZATION_PATH,
                        list(packet.EXPECTED_ESPTOOL_ARGS),
                        io.StringIO(),
                    )
            evidence.assert_not_called()


if __name__ == "__main__":
    unittest.main()
