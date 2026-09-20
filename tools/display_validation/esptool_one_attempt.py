"""Fail-closed future write entry point for the PCB V1.0 display candidate.

Offline audit mode never enumerates or opens a serial port. Device mode is
unreachable without a fresh external human-authorization document and an
exact reviewed esptool argument vector.
"""

from __future__ import annotations

import argparse
import contextlib
import hashlib
import importlib.util
import io
import json
from pathlib import Path
import subprocess
import sys
from typing import Any, Mapping, TextIO


REPOSITORY_ROOT = Path(__file__).resolve().parents[2]
LEGACY_GUARD_PATH = (
    REPOSITORY_ROOT / "tools" / "first_flash" / "esptool_single_attempt.py"
)
EXPECTED_AUTHORIZATION_PATH = Path(
    r"D:\ESP-VoCat_First_Write_Authorizations\pcb-v1-display-validation-authorization.json"
)
CHANGE_NAME = "validate-pcb-v1-display-backlight-on-hardware"
AUTHORIZATION_SCHEMA = "esp-vocat-display-first-write-authorization-v1"
AUTHORIZATION_STATUS = "EXPLICIT HUMAN FLASH AUTHORIZATION"
BRANCH = "feat/validate-pcb-v1-display-backlight-on-hardware"
UPSTREAM = "origin/feat/validate-pcb-v1-display-backlight-on-hardware"

CANDIDATE = (
    REPOSITORY_ROOT
    / "firmware"
    / "build-backlight-repro-a-20260920-r1"
    / "pcb_v1_minimal_display_smoke_test.bin"
)
CANDIDATE_SIZE = 231360
CANDIDATE_SHA256 = (
    "FEE9C3D1AA77CD0DF1B2C2D55A86B7BEAA2871E5E8443B9D458B10324FF8F362"
)
CANDIDATE_OFFSET = 0x00020000
SEMANTIC_END_EXCLUSIVE = 0x000587C0
TRANSPORT_END_EXCLUSIVE = 0x00058800
TRANSPORT_PADDING_BYTES = 64
ERASE_END_EXCLUSIVE = 0x00059000

FULL_IMAGE_SIZE = 33554432
FULL_IMAGE_SHA256 = (
    "72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001"
)
OTA0_SIZE = 4128768
OTA0_SHA256 = (
    "C8A2FE4AB0F9B7C823F1DCFDFC926682C9DBF1B079056461DC6F18A93A345D0E"
)
RECOVERY_ASSETS = (
    (
        Path(
            r"D:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi"
            r"\esp-vocat_full_flash_32MB_2026-07-10.bin"
        ),
        FULL_IMAGE_SIZE,
        FULL_IMAGE_SHA256,
    ),
    (
        Path(
            r"D:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi"
            r"\esp-vocat_full_flash_32MB_verify_2026-07-10.bin"
        ),
        FULL_IMAGE_SIZE,
        FULL_IMAGE_SHA256,
    ),
    (
        Path(
            r"E:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi"
            r"\esp-vocat_full_flash_32MB_2026-07-10.bin"
        ),
        FULL_IMAGE_SIZE,
        FULL_IMAGE_SHA256,
    ),
    (
        Path(
            r"E:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi"
            r"\esp-vocat_full_flash_32MB_verify_2026-07-10.bin"
        ),
        FULL_IMAGE_SIZE,
        FULL_IMAGE_SHA256,
    ),
    (
        Path(
            r"D:\ESP-VoCat_First_Flash_Packages\2026-07-26"
            r"\original_xiaozhi_ota_0_full_partition.bin"
        ),
        OTA0_SIZE,
        OTA0_SHA256,
    ),
    (
        Path(
            r"E:\ESP-VoCat_First_Flash_Packages\2026-07-26"
            r"\original_xiaozhi_ota_0_full_partition.bin"
        ),
        OTA0_SIZE,
        OTA0_SHA256,
    ),
)

EXPECTED_ESPTOOL_ARGS = (
    "--chip",
    "esp32s3",
    "--port",
    "COM7",
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
    "0x20000",
    str(CANDIDATE),
)


class PacketError(RuntimeError):
    """The reviewed packet does not match the exact fail-closed contract."""


def _sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as source:
        for block in iter(lambda: source.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest().upper()


def _require_file(path: Path, size: int, expected_hash: str) -> dict[str, Any]:
    if not path.is_file():
        raise PacketError(f"Required file is missing: {path}")
    actual_size = path.stat().st_size
    actual_hash = _sha256(path)
    if actual_size != size or actual_hash != expected_hash:
        raise PacketError(
            f"File identity mismatch: {path}; size={actual_size}; sha256={actual_hash}."
        )
    return {"path": str(path), "bytes": actual_size, "sha256": actual_hash}


def _load_legacy_guard():
    spec = importlib.util.spec_from_file_location(
        "esp_vocat_audited_esptool_guard", LEGACY_GUARD_PATH
    )
    if spec is None or spec.loader is None:
        raise PacketError("Unable to load the audited esptool guard.")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def _run_git(*args: str) -> str:
    try:
        completed = subprocess.run(
            ["git", *args],
            cwd=REPOSITORY_ROOT,
            check=True,
            capture_output=True,
            text=True,
            encoding="utf-8",
        )
    except (OSError, subprocess.CalledProcessError) as error:
        raise PacketError(f"Git precondition check failed: {' '.join(args)}") from error
    return completed.stdout.strip()


def _require_mapping(value: Any, name: str) -> Mapping[str, Any]:
    if not isinstance(value, Mapping):
        raise PacketError(f"Authorization field {name!r} must be an object.")
    return value


def _require_exact(mapping: Mapping[str, Any], name: str, expected: Any) -> None:
    if mapping.get(name) != expected:
        raise PacketError(
            f"Authorization field {name!r} mismatch: "
            f"got {mapping.get(name)!r}, expected {expected!r}."
        )


def validate_reviewed_args(args: list[str]) -> None:
    if tuple(args) != EXPECTED_ESPTOOL_ARGS:
        raise PacketError("esptool arguments do not match the exact reviewed packet.")


def validate_local_evidence() -> dict[str, Any]:
    candidate = _require_file(CANDIDATE, CANDIDATE_SIZE, CANDIDATE_SHA256)
    recovery = [
        _require_file(path, size, expected_hash)
        for path, size, expected_hash in RECOVERY_ASSETS
    ]
    guard = _load_legacy_guard()
    installation = guard.installed_integrity()
    modules = guard._import_audited_modules()
    structure = guard.verify_runtime_structure(
        modules, require_audited_defaults=True
    )
    with guard.single_attempt_overrides(modules) as attempts:
        enforced_attempts = dict(attempts)
    return {
        "candidate": candidate,
        "recovery": recovery,
        "installation": installation,
        "structure": structure,
        "enforced_attempts": enforced_attempts,
        "geometry": {
            "offset": CANDIDATE_OFFSET,
            "semantic_end_exclusive": SEMANTIC_END_EXCLUSIVE,
            "transport_end_exclusive": TRANSPORT_END_EXCLUSIVE,
            "transport_padding_bytes": TRANSPORT_PADDING_BYTES,
            "erase_end_exclusive": ERASE_END_EXCLUSIVE,
        },
    }


def validate_authorization(path: Path) -> dict[str, Any]:
    if path.resolve() != EXPECTED_AUTHORIZATION_PATH.resolve():
        raise PacketError("Authorization path does not match the reviewed packet.")
    if REPOSITORY_ROOT == path.resolve() or REPOSITORY_ROOT in path.resolve().parents:
        raise PacketError("Authorization material must remain outside the repository.")
    if not path.is_file():
        raise PacketError("Fresh external human authorization is missing.")
    try:
        root = _require_mapping(json.loads(path.read_text(encoding="utf-8")), "root")
    except (OSError, UnicodeError, json.JSONDecodeError) as error:
        raise PacketError("Authorization material is unreadable or invalid.") from error

    for name, expected in {
        "schema": AUTHORIZATION_SCHEMA,
        "status": AUTHORIZATION_STATUS,
        "change": CHANGE_NAME,
    }.items():
        _require_exact(root, name, expected)

    repository = _require_mapping(root.get("repository"), "repository")
    _require_exact(repository, "branch", BRANCH)
    head = repository.get("head")
    if not isinstance(head, str) or len(head) != 40:
        raise PacketError("Authorization must bind one full 40-character HEAD.")
    _require_exact(repository, "upstream", UPSTREAM)
    _require_exact(repository, "ahead", 0)
    _require_exact(repository, "behind", 0)
    _require_exact(repository, "clean", True)

    device = _require_mapping(root.get("device"), "device")
    for name, expected in {
        "product": "ESP-VoCat",
        "pcb_revision": "V1.0",
        "chip": "ESP32-S3",
        "port": "COM7",
        "transport": "USB Serial/JTAG",
        "flash_bytes": 33554432,
        "vdd_spi": "FORCED_1V8",
        "secure_boot": "DISABLED",
        "flash_encryption": "DISABLED",
        "download_mode": "ENABLED",
    }.items():
        _require_exact(device, name, expected)

    artifact = _require_mapping(root.get("artifact"), "artifact")
    for name, expected in {
        "path": str(CANDIDATE),
        "bytes": CANDIDATE_SIZE,
        "sha256": CANDIDATE_SHA256,
        "offset": CANDIDATE_OFFSET,
        "semantic_end_exclusive": SEMANTIC_END_EXCLUSIVE,
        "transport_end_exclusive": TRANSPORT_END_EXCLUSIVE,
        "transport_padding_bytes": TRANSPORT_PADDING_BYTES,
        "erase_end_exclusive": ERASE_END_EXCLUSIVE,
    }.items():
        _require_exact(artifact, name, expected)

    operation = _require_mapping(root.get("operation"), "operation")
    for name, expected in {
        "esptool_version": "4.12.dev3",
        "baud": 115200,
        "before": "usb_reset",
        "after": "no_reset",
        "no_stub": True,
        "compression": False,
        "encrypted": False,
        "erase_all": False,
        "flash_mode": "keep",
        "flash_frequency": "keep",
        "flash_size": "keep",
        "post_write_md5": True,
        "outer_operation_attempts": 1,
        "block_attempts": 1,
        "connect_attempts": 1,
        "open_port_attempts": 1,
        "sync_attempts_per_connect": 1,
        "reset_reopen_attempts": 1,
    }.items():
        _require_exact(operation, name, expected)

    human = _require_mapping(root.get("human_review"), "human_review")
    statement = human.get("verbatim_statement")
    if not isinstance(statement, str) or len(statement.strip()) < 40:
        raise PacketError("Authorization lacks a substantive human statement.")
    _require_exact(human, "risks_acknowledged", True)
    _require_exact(human, "automatic_retry_authorized", False)
    _require_exact(human, "rollback_authorized", False)
    _require_exact(human, "startup_authorized", False)
    _require_exact(human, "observation_authorized", False)

    if _run_git("branch", "--show-current") != BRANCH:
        raise PacketError("Current branch differs from the authorization.")
    if _run_git("rev-parse", "HEAD") != head:
        raise PacketError("Current HEAD differs from the authorization.")
    if _run_git("rev-parse", "@{u}") != head:
        raise PacketError("Upstream HEAD differs from the authorization.")
    if _run_git("status", "--porcelain=v1"):
        raise PacketError("Working tree is not clean.")
    if _run_git("rev-list", "--left-right", "--count", "@{u}...HEAD") != "0\t0":
        raise PacketError("Branch is not synchronized with upstream.")

    return {"head": head, "authorization": str(path.resolve())}


def execute_device(path: Path, esptool_args: list[str], output: TextIO) -> int:
    validate_reviewed_args(esptool_args)
    validate_authorization(path)
    validate_local_evidence()
    guard = _load_legacy_guard()
    modules = guard._import_audited_modules()
    guard.verify_runtime_structure(modules, require_audited_defaults=True)
    captured_out = io.StringIO()
    captured_err = io.StringIO()
    operation_error: BaseException | None = None
    try:
        with guard.single_attempt_overrides(modules):
            with contextlib.redirect_stdout(captured_out), contextlib.redirect_stderr(
                captured_err
            ):
                modules["esptool"].main(argv=esptool_args)
    except BaseException as error:
        operation_error = error
    sanitized = guard.sanitize_sensitive_output(
        captured_out.getvalue() + captured_err.getvalue()
    )
    if sanitized:
        print(sanitized, file=output, end="" if sanitized.endswith("\n") else "\n")
    if operation_error is not None:
        raise PacketError(
            f"The single authorized write attempt stopped: {type(operation_error).__name__}."
        ) from operation_error
    return 0


def _parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Audit or execute the exact PCB V1.0 display write packet."
    )
    mode = parser.add_mutually_exclusive_group(required=True)
    mode.add_argument("--audit", action="store_true")
    mode.add_argument("--device", action="store_true")
    parser.add_argument("--authorization", type=Path)
    parser.add_argument("esptool_args", nargs=argparse.REMAINDER)
    return parser


def main(argv: list[str] | None = None, output: TextIO = sys.stdout) -> int:
    args = _parser().parse_args(argv)
    reviewed_args = list(args.esptool_args)
    if reviewed_args[:1] == ["--"]:
        reviewed_args = reviewed_args[1:]
    try:
        validate_reviewed_args(reviewed_args)
        if args.audit:
            if args.authorization is not None:
                raise PacketError("Offline audit mode does not accept authorization.")
            result = validate_local_evidence()
            result.update(
                {
                    "mode": "audit",
                    "device_access": "NONE",
                    "serial_open": False,
                    "authorization": "NOT_PRESENT",
                    "reviewed_esptool_args": reviewed_args,
                }
            )
            print(json.dumps(result, indent=2, sort_keys=True), file=output)
            return 0
        if args.authorization is None:
            raise PacketError("Fresh external human authorization is required.")
        return execute_device(args.authorization, reviewed_args, output)
    except PacketError as error:
        print(f"REFUSED: {error}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
