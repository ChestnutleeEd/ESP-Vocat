"""Fail-closed, single-attempt guard for the audited ESP-IDF esptool install.

The offline modes in this module never enumerate or open serial ports.  Device
mode exists only for a future, independently reviewed authorization document.
It deliberately has no default port, artifact, address, or authorization.
"""

from __future__ import annotations

import argparse
import contextlib
import functools
import hashlib
import importlib
import importlib.metadata
import inspect
import io
import json
import os
from pathlib import Path
import re
import sys
from typing import Any, Iterator, Mapping, TextIO


SUPPORTED_ESPTOOL_VERSION = "4.12.dev3"
SUPPORTED_PYTHON_VERSION = (3, 13, 9)
SUPPORTED_PYTHON = Path(
    r"E:\Espressif\tools\python\v5.5.4\venv\Scripts\python.exe"
)
SUPPORTED_EXECUTABLE = Path(
    r"E:\Espressif\tools\python\v5.5.4\venv\Scripts\esptool.exe"
)
EXPECTED_EXECUTABLE_SHA256 = (
    "A1F90587C195E8C824FE30D80B2D6062CE244CD346118502F08AFDC4496F8AF4"
)
SUPPORTED_PACKAGE_ROOT = Path(
    r"E:\Espressif\tools\python\v5.5.4\venv\Lib\site-packages\esptool"
)
SUPPORTED_DIST_INFO = Path(
    r"E:\Espressif\tools\python\v5.5.4\venv\Lib"
    r"\site-packages\esptool-4.12.dev3.dist-info"
)

EXPECTED_SOURCE_FILE_COUNT = 28
EXPECTED_SOURCE_TREE_ENTRY_COUNT = 30
EXPECTED_SOURCE_TREE_SHA256 = (
    "7BA6E754C34E202DEB9EF5C6E24376CDA93F24BD4DDB3B70E392BBA6CD5C7610"
)
EXPECTED_SOURCE_SHA256 = {
    "__init__.py": "805D998B4DE6A2FDE6CF9F922F21B245F67BDC929C364067F4533696810C60B7",
    "cmds.py": "D9786DDA365985A97504153B5A45BEBF55BDF5B8272BF16891CA682B47E2C3AF",
    "config.py": "BFEACFAA457A10866D816385622427987A45B480D255CE1F10A41ABA64F7AB07",
    "loader.py": "6C5F0C4A9D2047ADB1C9164AEE66208018789A0F6531922204F6B280168C80E8",
    "reset.py": "410334DF7CB09CAFC01EFA8D32ACB1903BBC5BA6F03DF3A0B39823C433B0EDEF",
    "targets/esp32s3.py": (
        "2629FDDE0F4EA7D0D56011E83506CF49A161AEC219F1901EF38B119F3C2C421F"
    ),
}

AUDITED_DEFAULTS = {
    "outer_operation_attempts": 2,
    "block_attempts": 3,
    "connect_attempts": 7,
    "open_port_attempts": 1,
    "sync_attempts_per_connect": 5,
    "reset_reopen_attempts": 3,
}
ENFORCED_ATTEMPTS = {
    "outer_operation_attempts": 1,
    "block_attempts": 1,
    "connect_attempts": 1,
    "open_port_attempts": 1,
    "sync_attempts_per_connect": 1,
    "reset_reopen_attempts": 1,
}

PACKAGE_ROOTS = (
    Path(r"D:\ESP-VoCat_First_Flash_Packages\2026-07-26"),
    Path(r"E:\ESP-VoCat_First_Flash_Packages\2026-07-26"),
)
CANDIDATE_FILENAME = "pcb_v1_first_flash_smoke_test.bin"
CANDIDATE_SIZE = 160832
CANDIDATE_SHA256 = (
    "1B72A60DE9C9BB42DE401A58D7772B0525AFBC4DC3D8C85BB550D2D758F99DDC"
)
CANDIDATE_OFFSET = 0x00020000
CANDIDATE_END_EXCLUSIVE = 0x00047440
CANDIDATE_LAST_BYTE = 0x0004743F
ERASE_START = 0x00020000
ERASE_END_EXCLUSIVE = 0x00048000
ERASE_LENGTH = 0x00028000
ROLLBACK_FILENAME = "original_xiaozhi_ota_0_full_partition.bin"
ROLLBACK_SIZE = 0x003F0000
ROLLBACK_SHA256 = (
    "C8A2FE4AB0F9B7C823F1DCFDFC926682C9DBF1B079056461DC6F18A93A345D0E"
)
ROLLBACK_OFFSET = 0x00020000
ROLLBACK_END_EXCLUSIVE = 0x00410000
RECOVERY_FULL_IMAGE_SHA256 = (
    "72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001"
)

AUTHORIZATION_SCHEMA = "esp-vocat-first-flash-authorization-v1"
AUTHORIZATION_STATUS = "EXPLICIT HUMAN FLASH AUTHORIZATION"
CHANGE_NAME = "prepare-pcb-v1-first-flash-smoke-test"

MAC_PATTERN = re.compile(r"(?i)(?<![0-9a-f])(?:[0-9a-f]{2}[:-]){5}[0-9a-f]{2}(?![0-9a-f])")
SENSITIVE_LINE_PATTERN = re.compile(
    r"(?i)\b(?:base\s+mac|mac\s+address|mac:|unique\s+chip\s+id|"
    r"key\s+digest|credential|password|secret|access\s+token|refresh\s+token)\b"
)
PORT_PATTERN = re.compile(r"(?i)^COM[1-9][0-9]*$")


class HarnessError(RuntimeError):
    """A fail-closed validation or execution error."""


def _canonical(path: Path) -> str:
    return os.path.normcase(os.path.normpath(str(path.resolve())))


def _sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as source:
        for block in iter(lambda: source.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest().upper()


def _validate_version(version: str) -> None:
    if version != SUPPORTED_ESPTOOL_VERSION:
        raise HarnessError(
            f"Unsupported esptool version {version!r}; "
            f"required {SUPPORTED_ESPTOOL_VERSION!r}."
        )


def _source_tree_digest(package_root: Path, dist_info: Path) -> dict[str, Any]:
    source_files = sorted(
        package_root.rglob("*.py"),
        key=lambda path: path.relative_to(package_root).as_posix(),
    )
    metadata_files = (dist_info / "METADATA", dist_info / "entry_points.txt")
    for path in (*source_files, *metadata_files):
        if not path.is_file():
            raise HarnessError(f"Required installed esptool file is missing: {path}")

    tree_hash = hashlib.sha256()
    for path in (*source_files, *metadata_files):
        if path.is_relative_to(package_root):
            relative = "esptool/" + path.relative_to(package_root).as_posix()
        else:
            relative = "dist-info/" + path.name
        file_hash = _sha256(path)
        tree_hash.update(relative.encode("utf-8") + b"\0" + bytes.fromhex(file_hash))

    return {
        "source_file_count": len(source_files),
        "tree_entry_count": len(source_files) + len(metadata_files),
        "source_tree_sha256": tree_hash.hexdigest().upper(),
    }


def installed_integrity() -> dict[str, Any]:
    """Return and validate the audited installation identity without device I/O."""
    if sys.version_info[:3] != SUPPORTED_PYTHON_VERSION:
        raise HarnessError(
            "Unsupported Python version "
            f"{sys.version_info.major}.{sys.version_info.minor}.{sys.version_info.micro}; "
            "the audited interpreter is 3.13.9."
        )
    if _canonical(Path(sys.executable)) != _canonical(SUPPORTED_PYTHON):
        raise HarnessError(
            f"Wrong Python interpreter: {Path(sys.executable).resolve()}; "
            f"required {SUPPORTED_PYTHON}."
        )

    try:
        distribution = importlib.metadata.distribution("esptool")
    except importlib.metadata.PackageNotFoundError as error:
        raise HarnessError("The audited esptool distribution is not installed.") from error
    _validate_version(distribution.version)

    package_root = Path(
        distribution.locate_file("esptool/__init__.py")
    ).resolve().parent
    dist_info = package_root.parent / f"esptool-{SUPPORTED_ESPTOOL_VERSION}.dist-info"
    if _canonical(package_root) != _canonical(SUPPORTED_PACKAGE_ROOT):
        raise HarnessError(
            f"Unexpected esptool package root: {package_root}; "
            f"required {SUPPORTED_PACKAGE_ROOT}."
        )
    if _canonical(dist_info) != _canonical(SUPPORTED_DIST_INFO):
        raise HarnessError(
            f"Unexpected esptool metadata path: {dist_info}; "
            f"required {SUPPORTED_DIST_INFO}."
        )
    if not SUPPORTED_EXECUTABLE.is_file():
        raise HarnessError(f"Audited esptool executable is missing: {SUPPORTED_EXECUTABLE}")
    executable_hash = _sha256(SUPPORTED_EXECUTABLE)
    if executable_hash != EXPECTED_EXECUTABLE_SHA256:
        raise HarnessError(
            f"Audited esptool executable changed: {executable_hash}."
        )

    tree = _source_tree_digest(package_root, dist_info)
    expected_tree = {
        "source_file_count": EXPECTED_SOURCE_FILE_COUNT,
        "tree_entry_count": EXPECTED_SOURCE_TREE_ENTRY_COUNT,
        "source_tree_sha256": EXPECTED_SOURCE_TREE_SHA256,
    }
    if tree != expected_tree:
        raise HarnessError(
            f"Installed esptool source tree does not match the audit: {tree!r}."
        )

    relevant_hashes = {
        relative: _sha256(package_root / relative)
        for relative in EXPECTED_SOURCE_SHA256
    }
    if relevant_hashes != EXPECTED_SOURCE_SHA256:
        raise HarnessError(
            "One or more audited esptool source files changed: "
            f"{relevant_hashes!r}."
        )

    return {
        "python": str(Path(sys.executable).resolve()),
        "python_version": ".".join(str(value) for value in sys.version_info[:3]),
        "esptool_version": distribution.version,
        "package_root": str(package_root),
        "dist_info": str(dist_info),
        "executable": str(SUPPORTED_EXECUTABLE),
        "executable_sha256": executable_hash,
        **tree,
        "relevant_source_sha256": relevant_hashes,
    }


def _import_audited_modules() -> dict[str, Any]:
    modules = {
        "esptool": importlib.import_module("esptool"),
        "cmds": importlib.import_module("esptool.cmds"),
        "config": importlib.import_module("esptool.config"),
        "loader": importlib.import_module("esptool.loader"),
        "reset": importlib.import_module("esptool.reset"),
    }
    package_file = Path(inspect.getfile(modules["esptool"])).resolve()
    if _canonical(package_file.parent) != _canonical(SUPPORTED_PACKAGE_ROOT):
        raise HarnessError(f"Imported esptool from an unexpected path: {package_file}")
    _validate_version(getattr(modules["esptool"], "__version__", ""))
    return modules


def verify_runtime_structure(
    modules: Mapping[str, Any], *, require_audited_defaults: bool = True
) -> dict[str, Any]:
    """Verify every runtime override point and its audited control flow."""
    esptool_module = modules["esptool"]
    cmds = modules["cmds"]
    config = modules["config"]
    loader = modules["loader"]
    reset = modules["reset"]

    _, config_path = config.load_config_file()
    if config_path is not None or os.environ.get("ESPTOOL_CFGFILE") is not None:
        raise HarnessError(
            "An esptool configuration file or ESPTOOL_CFGFILE override is present; "
            "the audited no-configuration baseline is required."
        )

    required_attributes = (
        (loader, "WRITE_BLOCK_ATTEMPTS"),
        (loader, "DEFAULT_CONNECT_ATTEMPTS"),
        (loader, "DEFAULT_OPEN_PORT_ATTEMPTS"),
        (loader.ESPLoader, "WRITE_FLASH_ATTEMPTS"),
        (loader.ESPLoader, "connect"),
        (loader.ESPLoader, "_connect_attempt"),
        (loader.ESPLoader, "flash_block"),
        (reset.ResetStrategy, "__call__"),
        (cmds, "write_flash"),
        (cmds, "DEFAULT_CONNECT_ATTEMPTS"),
        (esptool_module, "DEFAULT_CONNECT_ATTEMPTS"),
        (esptool_module, "DEFAULT_OPEN_PORT_ATTEMPTS"),
        (esptool_module, "get_port_list"),
    )
    for owner, name in required_attributes:
        if not hasattr(owner, name):
            owner_name = getattr(owner, "__name__", type(owner).__name__)
            raise HarnessError(f"Required esptool patch point is missing: {owner_name}.{name}")

    source_requirements = (
        (
            cmds.write_flash,
            "for attempt in range(1, esp.WRITE_FLASH_ATTEMPTS + 1):",
            "outer whole-operation retry loop",
        ),
        (
            loader.ESPLoader.flash_block,
            "range(WRITE_BLOCK_ATTEMPTS - 1, -1, -1)",
            "block retry loop",
        ),
        (
            loader.ESPLoader._connect_attempt,
            "for _ in range(5):",
            "sync retry loop",
        ),
        (
            reset.ResetStrategy.__call__,
            "for retry in reversed(range(3)):",
            "reset reopen retry loop",
        ),
    )
    for function, marker, description in source_requirements:
        try:
            source = inspect.getsource(function)
        except (OSError, TypeError) as error:
            raise HarnessError(f"Cannot inspect {description}.") from error
        if marker not in source:
            raise HarnessError(
                f"Audited source structure changed at {description}; "
                f"missing marker {marker!r}."
            )

    signatures = {
        "cmds.write_flash": str(inspect.signature(cmds.write_flash)),
        "ESPLoader.connect": str(inspect.signature(loader.ESPLoader.connect)),
        "ESPLoader._connect_attempt": str(
            inspect.signature(loader.ESPLoader._connect_attempt)
        ),
        "ESPLoader.flash_block": str(inspect.signature(loader.ESPLoader.flash_block)),
        "ResetStrategy.__call__": str(inspect.signature(reset.ResetStrategy.__call__)),
        "esptool.main": str(inspect.signature(esptool_module.main)),
    }
    expected_signatures = {
        "cmds.write_flash": "(esp, args)",
        "ESPLoader.connect": (
            "(self, mode='default_reset', attempts=7, detecting=False, warnings=True)"
        ),
        "ESPLoader._connect_attempt": (
            "(self, reset_strategy, mode='default_reset')"
        ),
        "ESPLoader.flash_block": (
            "(self, data, seq, timeout=3, encrypted=False)"
        ),
        "ResetStrategy.__call__": "(self)",
        "esptool.main": "(argv=None, esp=None)",
    }
    if require_audited_defaults and signatures != expected_signatures:
        raise HarnessError(f"Audited esptool call signatures changed: {signatures!r}.")

    values = {
        "outer_operation_attempts": loader.ESPLoader.WRITE_FLASH_ATTEMPTS,
        "block_attempts": loader.WRITE_BLOCK_ATTEMPTS,
        "connect_attempts": loader.DEFAULT_CONNECT_ATTEMPTS,
        "open_port_attempts": loader.DEFAULT_OPEN_PORT_ATTEMPTS,
        "cmds_connect_attempts": cmds.DEFAULT_CONNECT_ATTEMPTS,
        "cli_connect_attempts": esptool_module.DEFAULT_CONNECT_ATTEMPTS,
        "cli_open_port_attempts": esptool_module.DEFAULT_OPEN_PORT_ATTEMPTS,
    }
    expected_values = {
        "outer_operation_attempts": AUDITED_DEFAULTS["outer_operation_attempts"],
        "block_attempts": AUDITED_DEFAULTS["block_attempts"],
        "connect_attempts": AUDITED_DEFAULTS["connect_attempts"],
        "open_port_attempts": AUDITED_DEFAULTS["open_port_attempts"],
        "cmds_connect_attempts": AUDITED_DEFAULTS["connect_attempts"],
        "cli_connect_attempts": AUDITED_DEFAULTS["connect_attempts"],
        "cli_open_port_attempts": AUDITED_DEFAULTS["open_port_attempts"],
    }
    if require_audited_defaults and values != expected_values:
        raise HarnessError(f"Audited esptool constants changed: {values!r}.")
    return {"signatures": signatures, "values": values}


def _single_sync_connect_attempt(self: Any, reset_strategy: Any, mode: str = "default_reset"):
    """Audited ESPLoader._connect_attempt behavior with one sync transmission."""
    loader = importlib.import_module("esptool.loader")
    last_error = None
    boot_log_detected = False
    download_mode = False

    if mode == "no_reset_no_sync":
        return last_error

    if mode != "no_reset":
        if not self.USES_RFC2217:
            self._port.reset_input_buffer()
        reset_strategy()
        waiting = self._port.inWaiting()
        read_bytes = self._port.read(waiting)
        data = re.search(
            b"boot:(0x[0-9a-fA-F]+)(.*waiting for download)?",
            read_bytes,
            re.DOTALL,
        )
        if data is not None:
            boot_log_detected = True
            boot_mode = data.group(1)
            download_mode = data.group(2) is not None

    try:
        self.flush_input()
        self._port.flushOutput()
        self.sync()
        return None
    except loader.FatalError as error:
        last_error = error

    if boot_log_detected:
        last_error = loader.FatalError(
            "Wrong boot mode detected ({})! "
            "The chip needs to be in download mode.".format(
                boot_mode.decode("utf-8")
            )
        )
        if download_mode:
            last_error = loader.FatalError(
                "Download mode successfully detected, but getting no sync reply: "
                "The serial TX path seems to be down."
            )
    return last_error


def _single_reset_call(self: Any) -> None:
    """Run exactly one reset/open sequence and propagate every failure."""
    if not self.port.isOpen():
        self.port.open()
    self.reset()


def _deny_port_enumeration(*_args: Any, **_kwargs: Any) -> Any:
    raise HarnessError("Serial-port enumeration is prohibited by this harness.")


@contextlib.contextmanager
def single_attempt_overrides(
    modules: Mapping[str, Any] | None = None,
) -> Iterator[dict[str, Any]]:
    """Apply process-local overrides and restore all module state on exit."""
    selected = dict(modules or _import_audited_modules())
    verify_runtime_structure(selected, require_audited_defaults=True)
    esptool_module = selected["esptool"]
    cmds = selected["cmds"]
    loader = selected["loader"]
    reset = selected["reset"]

    originals = {
        "outer": loader.ESPLoader.WRITE_FLASH_ATTEMPTS,
        "block": loader.WRITE_BLOCK_ATTEMPTS,
        "loader_connect": loader.DEFAULT_CONNECT_ATTEMPTS,
        "loader_open": loader.DEFAULT_OPEN_PORT_ATTEMPTS,
        "cmds_connect": cmds.DEFAULT_CONNECT_ATTEMPTS,
        "cli_connect": esptool_module.DEFAULT_CONNECT_ATTEMPTS,
        "cli_open": esptool_module.DEFAULT_OPEN_PORT_ATTEMPTS,
        "connect": loader.ESPLoader.connect,
        "connect_attempt": loader.ESPLoader._connect_attempt,
        "reset_call": reset.ResetStrategy.__call__,
        "get_port_list": esptool_module.get_port_list,
        "loader_comports": loader.list_ports.comports if loader.list_ports else None,
    }
    environment = {
        name: os.environ.get(name)
        for name in ("ESPTOOL_CONNECT_ATTEMPTS", "ESPTOOL_OPEN_PORT_ATTEMPTS")
    }

    original_connect = loader.ESPLoader.connect

    @functools.wraps(original_connect)
    def single_connect(
        self: Any,
        mode: str = "default_reset",
        attempts: int = 1,
        detecting: bool = False,
        warnings: bool = True,
    ) -> Any:
        if attempts != 1:
            raise HarnessError(
                f"Connection attempts must be exactly 1, got {attempts!r}."
            )
        return original_connect(
            self,
            mode=mode,
            attempts=1,
            detecting=detecting,
            warnings=warnings,
        )

    try:
        loader.ESPLoader.WRITE_FLASH_ATTEMPTS = 1
        loader.WRITE_BLOCK_ATTEMPTS = 1
        loader.DEFAULT_CONNECT_ATTEMPTS = 1
        loader.DEFAULT_OPEN_PORT_ATTEMPTS = 1
        cmds.DEFAULT_CONNECT_ATTEMPTS = 1
        esptool_module.DEFAULT_CONNECT_ATTEMPTS = 1
        esptool_module.DEFAULT_OPEN_PORT_ATTEMPTS = 1
        loader.ESPLoader.connect = single_connect
        loader.ESPLoader._connect_attempt = _single_sync_connect_attempt
        reset.ResetStrategy.__call__ = _single_reset_call
        esptool_module.get_port_list = _deny_port_enumeration
        if loader.list_ports is not None:
            loader.list_ports.comports = _deny_port_enumeration
        os.environ["ESPTOOL_CONNECT_ATTEMPTS"] = "1"
        os.environ["ESPTOOL_OPEN_PORT_ATTEMPTS"] = "1"

        final_values = {
            "outer_operation_attempts": loader.ESPLoader.WRITE_FLASH_ATTEMPTS,
            "block_attempts": loader.WRITE_BLOCK_ATTEMPTS,
            "connect_attempts": loader.DEFAULT_CONNECT_ATTEMPTS,
            "open_port_attempts": loader.DEFAULT_OPEN_PORT_ATTEMPTS,
            "sync_attempts_per_connect": 1,
            "reset_reopen_attempts": 1,
        }
        if final_values != ENFORCED_ATTEMPTS:
            raise HarnessError(f"Single-attempt overrides did not take effect: {final_values!r}")
        if loader.ESPLoader._connect_attempt is not _single_sync_connect_attempt:
            raise HarnessError("The one-sync override did not take effect.")
        if reset.ResetStrategy.__call__ is not _single_reset_call:
            raise HarnessError("The one-reset override did not take effect.")
        yield final_values
    finally:
        loader.ESPLoader.WRITE_FLASH_ATTEMPTS = originals["outer"]
        loader.WRITE_BLOCK_ATTEMPTS = originals["block"]
        loader.DEFAULT_CONNECT_ATTEMPTS = originals["loader_connect"]
        loader.DEFAULT_OPEN_PORT_ATTEMPTS = originals["loader_open"]
        cmds.DEFAULT_CONNECT_ATTEMPTS = originals["cmds_connect"]
        esptool_module.DEFAULT_CONNECT_ATTEMPTS = originals["cli_connect"]
        esptool_module.DEFAULT_OPEN_PORT_ATTEMPTS = originals["cli_open"]
        loader.ESPLoader.connect = originals["connect"]
        loader.ESPLoader._connect_attempt = originals["connect_attempt"]
        reset.ResetStrategy.__call__ = originals["reset_call"]
        esptool_module.get_port_list = originals["get_port_list"]
        if loader.list_ports is not None and originals["loader_comports"] is not None:
            loader.list_ports.comports = originals["loader_comports"]
        for name, value in environment.items():
            if value is None:
                os.environ.pop(name, None)
            else:
                os.environ[name] = value


def validate_geometry(
    *,
    offset: int,
    length: int,
    end_exclusive: int,
    erase_start: int,
    erase_end_exclusive: int,
    erase_length: int,
) -> dict[str, int]:
    expected = {
        "offset": CANDIDATE_OFFSET,
        "length": CANDIDATE_SIZE,
        "end_exclusive": CANDIDATE_END_EXCLUSIVE,
        "erase_start": ERASE_START,
        "erase_end_exclusive": ERASE_END_EXCLUSIVE,
        "erase_length": ERASE_LENGTH,
    }
    actual = {
        "offset": offset,
        "length": length,
        "end_exclusive": end_exclusive,
        "erase_start": erase_start,
        "erase_end_exclusive": erase_end_exclusive,
        "erase_length": erase_length,
    }
    if actual != expected:
        raise HarnessError(f"Artifact/range geometry mismatch: {actual!r}.")
    if offset + length != end_exclusive:
        raise HarnessError("Artifact end-exclusive does not equal offset plus length.")
    if erase_start > offset or erase_end_exclusive < end_exclusive:
        raise HarnessError("Erase envelope does not contain the artifact range.")
    if erase_end_exclusive - erase_start != erase_length:
        raise HarnessError("Erase envelope length is inconsistent.")
    return actual


def validate_artifact(
    path: Path,
    *,
    expected_size: int = CANDIDATE_SIZE,
    expected_sha256: str = CANDIDATE_SHA256,
    require_staging_path: bool = True,
) -> dict[str, Any]:
    resolved = path.resolve()
    if require_staging_path:
        allowed = {
            (root / CANDIDATE_FILENAME).resolve()
            for root in PACKAGE_ROOTS
        }
        if resolved not in allowed:
            raise HarnessError(f"Artifact is outside the exact staging paths: {resolved}")
    if not resolved.is_file():
        raise HarnessError(f"Artifact is missing: {resolved}")
    size = resolved.stat().st_size
    if size != expected_size:
        raise HarnessError(
            f"Artifact length mismatch: got {size}, expected {expected_size}."
        )
    digest = _sha256(resolved)
    if digest != expected_sha256.upper():
        raise HarnessError(
            f"Artifact SHA-256 mismatch: got {digest}, expected {expected_sha256}."
        )
    return {"path": str(resolved), "size": size, "sha256": digest}


def _manifest_required_lines(root: Path) -> tuple[str, ...]:
    return (
        "Status: HOST-ONLY STAGING",
        "Status: NOT AUTHORIZED",
        "Status: DO NOT EXECUTE",
        f"Package root: {root}",
        f"Candidate filename: {CANDIDATE_FILENAME}",
        f"Candidate size: {CANDIDATE_SIZE} bytes (0x00027440)",
        f"Candidate SHA-256: {CANDIDATE_SHA256}",
        "Candidate proposed offset: 0x00020000 (131072)",
        "Candidate length: 0x00027440 (160832)",
        "Candidate end-exclusive: 0x00047440 (291904)",
        "Aligned erase start: 0x00020000 (131072)",
        "Aligned erase end-exclusive: 0x00048000 (294912)",
        "Aligned erase length: 0x00028000 (163840 bytes)",
        f"Rollback filename: {ROLLBACK_FILENAME}",
        f"Rollback size: {ROLLBACK_SIZE} bytes (0x003F0000)",
        f"Rollback SHA-256: {ROLLBACK_SHA256}",
        f"Original full-backup SHA-256: {RECOVERY_FULL_IMAGE_SHA256}",
        "Device access authorization: NONE",
        "Flash authorization: NONE",
    )


def validate_manifest(path: Path) -> dict[str, Any]:
    resolved = path.resolve()
    allowed = {(root / "FIRST_FLASH_PACKAGE_MANIFEST.txt").resolve() for root in PACKAGE_ROOTS}
    if resolved not in allowed:
        raise HarnessError(f"Manifest is outside the exact staging paths: {resolved}")
    if not resolved.is_file():
        raise HarnessError(f"Manifest is missing: {resolved}")
    text = resolved.read_text(encoding="utf-8")
    root = resolved.parent
    missing = [line for line in _manifest_required_lines(root) if line not in text]
    if missing:
        raise HarnessError(f"Manifest fields are missing or changed: {missing!r}.")
    candidate = validate_artifact(root / CANDIDATE_FILENAME)
    rollback = validate_artifact(
        root / ROLLBACK_FILENAME,
        expected_size=ROLLBACK_SIZE,
        expected_sha256=ROLLBACK_SHA256,
        require_staging_path=False,
    )
    geometry = validate_geometry(
        offset=CANDIDATE_OFFSET,
        length=CANDIDATE_SIZE,
        end_exclusive=CANDIDATE_END_EXCLUSIVE,
        erase_start=ERASE_START,
        erase_end_exclusive=ERASE_END_EXCLUSIVE,
        erase_length=ERASE_LENGTH,
    )
    return {
        "manifest": str(resolved),
        "candidate": candidate,
        "rollback": rollback,
        "geometry": geometry,
        "authorization": "NONE",
    }


def sanitize_sensitive_output(text: str) -> str:
    output: list[str] = []
    for line in text.splitlines():
        if SENSITIVE_LINE_PATTERN.search(line):
            output.append("[REDACTED SENSITIVE CONNECTION DETAIL]")
        else:
            output.append(MAC_PATTERN.sub("[REDACTED MAC]", line))
    sanitized = "\n".join(output)
    if MAC_PATTERN.search(sanitized) or SENSITIVE_LINE_PATTERN.search(sanitized):
        raise HarnessError("Sensitive esptool output could not be filtered.")
    return sanitized


def validate_output_filter() -> dict[str, bool]:
    sample = "\n".join(
        (
            "MAC: 7C:DF:A1:12:34:56",
            "Base MAC address AA-BB-CC-DD-EE-FF",
            "Unique chip ID: 123456789",
            "Key digest: DEADBEEF",
            "Access token: test-only-secret",
            "Writing at 0x00020000...",
        )
    )
    sanitized = sanitize_sensitive_output(sample)
    forbidden = (
        "7C:DF:A1:12:34:56",
        "AA-BB-CC-DD-EE-FF",
        "123456789",
        "DEADBEEF",
        "test-only-secret",
    )
    if any(value in sanitized for value in forbidden):
        raise HarnessError("Sensitive-output filter self-test failed.")
    return {
        "mac_filtered": True,
        "unique_id_filtered": True,
        "key_digest_filtered": True,
        "credential_filtered": True,
    }


def _require_mapping(value: Any, name: str) -> Mapping[str, Any]:
    if not isinstance(value, Mapping):
        raise HarnessError(f"Authorization field {name!r} must be an object.")
    return value


def _require_exact(mapping: Mapping[str, Any], name: str, expected: Any) -> None:
    if mapping.get(name) != expected:
        raise HarnessError(
            f"Authorization field {name!r} mismatch: "
            f"got {mapping.get(name)!r}, expected {expected!r}."
        )


def validate_authorization(path: Path) -> dict[str, Any]:
    """Validate a future authorization file entirely before device logic."""
    resolved = path.resolve()
    repository_root = Path(__file__).resolve().parents[2]
    if resolved == repository_root or repository_root in resolved.parents:
        raise HarnessError("Authorization files must remain outside the repository.")
    if not resolved.is_file():
        raise HarnessError(f"Future authorization material is missing: {resolved}")
    try:
        document = json.loads(resolved.read_text(encoding="utf-8"))
    except (OSError, UnicodeError, json.JSONDecodeError) as error:
        raise HarnessError("Future authorization material is unreadable or invalid.") from error
    root = _require_mapping(document, "root")
    _require_exact(root, "schema", AUTHORIZATION_SCHEMA)
    _require_exact(root, "status", AUTHORIZATION_STATUS)
    _require_exact(root, "change", CHANGE_NAME)

    device = _require_mapping(root.get("device"), "device")
    _require_exact(device, "product", "ESP-VoCat")
    _require_exact(device, "pcb_revision", "V1.0")
    _require_exact(device, "chip", "ESP32-S3")
    _require_exact(device, "transport", "USB Serial/JTAG")
    _require_exact(device, "same_device_recovery", True)
    port = device.get("port")
    if not isinstance(port, str) or not PORT_PATTERN.fullmatch(port):
        raise HarnessError("Authorization must contain one exact Windows COM port.")

    artifact = _require_mapping(root.get("artifact"), "artifact")
    artifact_path = Path(str(artifact.get("path", ""))).resolve()
    _require_exact(artifact, "sha256", CANDIDATE_SHA256)
    _require_exact(artifact, "offset", CANDIDATE_OFFSET)
    _require_exact(artifact, "length", CANDIDATE_SIZE)
    _require_exact(artifact, "end_exclusive", CANDIDATE_END_EXCLUSIVE)
    _require_exact(artifact, "erase_start", ERASE_START)
    _require_exact(artifact, "erase_end_exclusive", ERASE_END_EXCLUSIVE)
    _require_exact(artifact, "erase_length", ERASE_LENGTH)
    validate_geometry(
        offset=artifact["offset"],
        length=artifact["length"],
        end_exclusive=artifact["end_exclusive"],
        erase_start=artifact["erase_start"],
        erase_end_exclusive=artifact["erase_end_exclusive"],
        erase_length=artifact["erase_length"],
    )
    artifact_result = validate_artifact(artifact_path)
    manifest_result = validate_manifest(artifact_path.parent / "FIRST_FLASH_PACKAGE_MANIFEST.txt")

    operation = _require_mapping(root.get("operation"), "operation")
    operation_expected = {
        "esptool_version": SUPPORTED_ESPTOOL_VERSION,
        "no_stub": True,
        "compression": False,
        "encrypted": False,
        "erase_all": False,
        "preserve_header": True,
        "flash_mode": "keep",
        "flash_freq": "keep",
        "flash_size": "keep",
        "post_write_md5": True,
        "before": "usb_reset",
        "after": "no_reset",
        **ENFORCED_ATTEMPTS,
    }
    for name, expected in operation_expected.items():
        _require_exact(operation, name, expected)

    observation = _require_mapping(root.get("observation_handoff"), "observation_handoff")
    for name, expected in {
        "separate_operation": True,
        "baud": 115200,
        "window_seconds": 60,
        "startup_deadline_seconds": 15,
    }.items():
        _require_exact(observation, name, expected)

    recovery = _require_mapping(root.get("recovery"), "recovery")
    for name, expected in {
        "rollback_filename": ROLLBACK_FILENAME,
        "rollback_size": ROLLBACK_SIZE,
        "rollback_sha256": ROLLBACK_SHA256,
        "rollback_offset": ROLLBACK_OFFSET,
        "rollback_end_exclusive": ROLLBACK_END_EXCLUSIVE,
        "full_image_sha256": RECOVERY_FULL_IMAGE_SHA256,
        "separate_authorization_required": True,
    }.items():
        _require_exact(recovery, name, expected)

    human = _require_mapping(root.get("human_review"), "human_review")
    statement = human.get("verbatim_statement")
    if not isinstance(statement, str) or len(statement.strip()) < 40:
        raise HarnessError("Authorization lacks a substantive verbatim human statement.")
    _require_exact(human, "risks_acknowledged", True)
    _require_exact(human, "automatic_retry_authorized", False)
    _require_exact(human, "rollback_authorized", False)

    return {
        "authorization_path": str(resolved),
        "port": port,
        "artifact": artifact_result,
        "manifest": manifest_result["manifest"],
        "operation": operation_expected,
        "observation_handoff": dict(observation),
    }


def audit() -> dict[str, Any]:
    before = installed_integrity()
    modules = _import_audited_modules()
    structure = verify_runtime_structure(modules, require_audited_defaults=True)
    filter_result = validate_output_filter()
    with single_attempt_overrides(modules) as enforced:
        runtime = dict(enforced)
    after = installed_integrity()
    if before != after:
        raise HarnessError("Installed esptool integrity changed during audit.")
    return {
        "mode": "audit",
        "device_access": "NONE",
        "serial_open": False,
        "port_enumeration": False,
        "installation": before,
        "audited_defaults": AUDITED_DEFAULTS,
        "enforced_attempts": runtime,
        "structure": structure,
        "sensitive_output_filter": filter_result,
        "installed_package_modified": False,
        "authorization_present": False,
        "device_mode_entered": False,
    }


def self_test() -> dict[str, Any]:
    result = audit()
    if result["enforced_attempts"] != ENFORCED_ATTEMPTS:
        raise HarnessError("Exactly-once assertion failed.")
    return {
        "mode": "self-test",
        "result": "PASS",
        "exactly_once": result["enforced_attempts"],
        "sensitive_output_filter": result["sensitive_output_filter"],
        "serial_open": False,
        "port_enumeration": False,
        "installed_package_modified": False,
    }


def execute_device(authorization_path: Path | None) -> dict[str, Any]:
    """Run the future exact operation only after all fail-closed checks pass."""
    if authorization_path is None:
        raise HarnessError("Future independent authorization material is required.")
    before = installed_integrity()
    authorization = validate_authorization(authorization_path)
    validate_output_filter()
    modules = _import_audited_modules()
    verify_runtime_structure(modules, require_audited_defaults=True)

    artifact = authorization["artifact"]
    port = authorization["port"]
    # Deliberately never display this argv.  It contains no default port and is
    # constructed only from the already validated independent authorization.
    argv = [
        "--chip",
        "esp32s3",
        "--port",
        port,
        "--baud",
        "115200",
        "--before",
        "usb_reset",
        "--after",
        "no_reset",
        "--no-stub",
        "--connect-attempts",
        "1",
        "write_flash",
        "--flash_mode",
        "keep",
        "--flash_freq",
        "keep",
        "--flash_size",
        "keep",
        "--no-compress",
        "--no-progress",
        hex(CANDIDATE_OFFSET),
        artifact["path"],
    ]

    captured_out = io.StringIO()
    captured_err = io.StringIO()
    operation_error: BaseException | None = None
    try:
        with single_attempt_overrides(modules):
            with contextlib.redirect_stdout(captured_out), contextlib.redirect_stderr(
                captured_err
            ):
                modules["esptool"].main(argv=argv)
    except BaseException as error:
        operation_error = error
    finally:
        after = installed_integrity()

    if before != after:
        raise HarnessError("Installed esptool integrity changed during device execution.")
    sanitized = sanitize_sensitive_output(
        captured_out.getvalue() + captured_err.getvalue()
    )
    if sanitized:
        print(sanitized)
    if operation_error is not None:
        raise HarnessError(
            f"Single device attempt stopped: {type(operation_error).__name__}: "
            f"{sanitize_sensitive_output(str(operation_error))}"
        ) from operation_error
    return {
        "result": "single authorized operation completed",
        "installed_package_modified": False,
        "observation_handoff": "REQUIRED AS A SEPARATE OPERATION",
    }


def _parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description=(
            "Fail-closed offline audit and future authorization-gated "
            "single-attempt esptool harness."
        )
    )
    modes = parser.add_mutually_exclusive_group(required=True)
    modes.add_argument("--audit", action="store_true")
    modes.add_argument("--self-test", action="store_true")
    modes.add_argument("--validate-manifest", type=Path)
    modes.add_argument("--validate-artifact", type=Path)
    modes.add_argument("--device", action="store_true")
    parser.add_argument(
        "--authorization",
        type=Path,
        help="Future independent authorization JSON; there is no default.",
    )
    return parser


def main(argv: list[str] | None = None, output: TextIO = sys.stdout) -> int:
    args = _parser().parse_args(argv)
    try:
        if args.audit:
            result = audit()
        elif args.self_test:
            result = self_test()
        elif args.validate_manifest is not None:
            result = validate_manifest(args.validate_manifest)
        elif args.validate_artifact is not None:
            result = {
                "artifact": validate_artifact(args.validate_artifact),
                "geometry": validate_geometry(
                    offset=CANDIDATE_OFFSET,
                    length=CANDIDATE_SIZE,
                    end_exclusive=CANDIDATE_END_EXCLUSIVE,
                    erase_start=ERASE_START,
                    erase_end_exclusive=ERASE_END_EXCLUSIVE,
                    erase_length=ERASE_LENGTH,
                ),
                "device_access": "NONE",
            }
        else:
            result = execute_device(args.authorization)
        print(json.dumps(result, indent=2, sort_keys=True), file=output)
        return 0
    except HarnessError as error:
        print(f"REFUSED: {error}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
