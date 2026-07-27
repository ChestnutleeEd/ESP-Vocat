"""Fail-closed one-reset, one-open startup observer for ESP-VoCat PCB V1.0.

The module is inert unless its explicit execution mode is selected. It never
enumerates ports, never retries an open, never reopens after disconnect, never
writes serial data, and never stores raw serial logs.
"""

from __future__ import annotations

import argparse
import codecs
import ctypes
from ctypes import wintypes
from dataclasses import dataclass
import json
import re
import sys
import time
from typing import Callable, Protocol, TextIO


BAUD_RATE = 115200
OBSERVATION_SECONDS = 60.0
READY_DEADLINE_SECONDS = 15.0
MAX_LINE_CHARS = 512
MAX_LINES = 1000
MAX_TOTAL_BYTES = 1024 * 1024

EXPECTED_LINES = (
    "ESP-VoCat PCB V1.0 Smoke Test Candidate",
    "Compile-time ESP-IDF version: 5.5.4",
    "PCB target: ESP-VoCat PCB V1.0",
    "Host-built candidate",
    "Device execution not yet authorized",
    "PSRAM intentionally disabled",
    "No peripheral initialization",
    "Ready marker: PCB_V1_SMOKE_TEST_CANDIDATE_READY",
)
READY_MARKER = EXPECTED_LINES[-1]

_EXACT_COM_PORT = re.compile(r"COM[1-9][0-9]*", re.IGNORECASE)
_MAC = re.compile(r"(?i)(?<![0-9a-f])(?:[0-9a-f]{2}[:-]){5}[0-9a-f]{2}(?![0-9a-f])")
_SENSITIVE_LINE = re.compile(
    r"(?i)\b(?:base\s+mac|mac\s+address|unique\s+(?:id|identifier)|"
    r"device\s+(?:id|identifier|serial)|chip[_ -]?id|"
    r"(?:secure\s+boot\s+)?key\s*(?:digest|hash)|blk_key)\b"
)
_FATAL = re.compile(
    r"(?i)(?:guru meditation|panic(?:ked)?|watchdog|task wdt|"
    r"allocation fail|alloc failed|malloc(?:\(\))?.*fail|abort\(\)|"
    r"invalid (?:header|image|segment)|no bootable app|"
    r"(?:checksum|hash).*(?:fail|mismatch|error)|"
    r"anti[- ]rollback.*(?:reject|fail|error)|"
    r"secure boot.*(?:reject|fail|error)|"
    r"waiting for download|download mode|download[_ -]boot)"
)
_RESET_BANNER = re.compile(r"(?i)^\s*rst:0x[0-9a-f]+")


class ObservationError(RuntimeError):
    """A fail-closed startup observation error."""


class Clock(Protocol):
    def __call__(self) -> float: ...


class Session(Protocol):
    def reset_once(self) -> None: ...

    def read(self, timeout_seconds: float) -> bytes: ...

    def close(self) -> None: ...


class Backend(Protocol):
    def open_exact_read_only(self, port: str, baud_rate: int) -> Session: ...


@dataclass(frozen=True)
class ObservationResult:
    port: str
    baud_rate: int
    open_attempts: int
    reset_attempts: int
    serial_data_writes: int
    enumeration_attempts: int
    reopen_attempts: int
    observation_seconds: float
    ready_deadline_seconds: float
    ready_seen_seconds: float
    expected_lines_seen: int
    ready_marker_count: int
    sanitized_lines: tuple[str, ...]


def validate_exact_port(port: str) -> str:
    normalized = port.strip().upper()
    if not _EXACT_COM_PORT.fullmatch(normalized):
        raise ObservationError("port must be one exact COM name such as COM7")
    return normalized


def sanitize_line(line: str) -> str:
    if _SENSITIVE_LINE.search(line):
        return "[REDACTED_SENSITIVE_LINE]"
    return _MAC.sub("[REDACTED_MAC]", line)


class _LineAudit:
    def __init__(self) -> None:
        self.expected_index = 0
        self.ready_count = 0
        self.ready_seen_seconds: float | None = None
        self.reset_banner_count = 0
        self.lines: list[str] = []

    def feed(self, raw_line: str, elapsed: float) -> str:
        raw_line = raw_line.rstrip("\r")
        if len(raw_line) > MAX_LINE_CHARS:
            raise ObservationError("serial line exceeded the bounded length")
        if _FATAL.search(raw_line):
            raise ObservationError("fatal startup text observed")
        if _RESET_BANNER.search(raw_line):
            self.reset_banner_count += 1
            if self.reset_banner_count > 1:
                raise ObservationError("repeated reset banner observed")

        if raw_line in EXPECTED_LINES:
            if self.expected_index >= len(EXPECTED_LINES):
                raise ObservationError("expected startup sequence repeated")
            expected = EXPECTED_LINES[self.expected_index]
            if raw_line != expected:
                raise ObservationError("expected startup lines appeared out of order")
            self.expected_index += 1
            if raw_line == READY_MARKER:
                self.ready_count += 1
                self.ready_seen_seconds = elapsed
                if elapsed > READY_DEADLINE_SECONDS:
                    raise ObservationError("ready marker missed the 15-second deadline")

        sanitized = sanitize_line(raw_line)
        self.lines.append(sanitized)
        if len(self.lines) > MAX_LINES:
            raise ObservationError("serial output exceeded the bounded line count")
        return sanitized

    def finish(self) -> None:
        if self.expected_index != len(EXPECTED_LINES):
            raise ObservationError("complete eight-line startup sequence was not observed")
        if self.ready_count != 1 or self.ready_seen_seconds is None:
            raise ObservationError("ready marker did not appear exactly once")


def run_startup_observation(
    port: str,
    backend: Backend,
    *,
    clock: Clock = time.monotonic,
    emit: Callable[[str], None] | None = None,
) -> ObservationResult:
    """Open once, reset once, then read for 60 seconds without retry or reopen."""

    exact_port = validate_exact_port(port)
    open_attempts = 0
    session: Session | None = None
    audit = _LineAudit()
    decoder = codecs.getincrementaldecoder("utf-8")(errors="replace")
    pending = ""
    total_bytes = 0

    try:
        open_attempts += 1
        session = backend.open_exact_read_only(exact_port, BAUD_RATE)
        session.reset_once()
        reset_released_at = clock()

        while True:
            elapsed = clock() - reset_released_at
            if elapsed >= OBSERVATION_SECONDS:
                break
            timeout = min(0.25, OBSERVATION_SECONDS - elapsed)
            try:
                chunk = session.read(timeout)
            except Exception as exc:
                raise ObservationError(
                    "observation handle failed; no reopen or retry is permitted"
                ) from exc

            total_bytes += len(chunk)
            if total_bytes > MAX_TOTAL_BYTES:
                raise ObservationError("serial output exceeded the bounded byte count")
            if chunk:
                pending += decoder.decode(chunk)
                if "\n" not in pending and len(pending) > MAX_LINE_CHARS:
                    raise ObservationError("serial line exceeded the bounded length")
                while "\n" in pending:
                    raw_line, pending = pending.split("\n", 1)
                    sanitized = audit.feed(raw_line, clock() - reset_released_at)
                    if emit is not None:
                        emit(sanitized)

            elapsed = clock() - reset_released_at
            if elapsed >= READY_DEADLINE_SECONDS and audit.ready_count == 0:
                raise ObservationError("ready marker was absent at the 15-second deadline")

        pending += decoder.decode(b"", final=True)
        if pending:
            sanitized = audit.feed(pending, clock() - reset_released_at)
            if emit is not None:
                emit(sanitized)
        audit.finish()
    finally:
        if session is not None:
            session.close()

    assert audit.ready_seen_seconds is not None
    return ObservationResult(
        port=exact_port,
        baud_rate=BAUD_RATE,
        open_attempts=open_attempts,
        reset_attempts=1,
        serial_data_writes=0,
        enumeration_attempts=0,
        reopen_attempts=0,
        observation_seconds=OBSERVATION_SECONDS,
        ready_deadline_seconds=READY_DEADLINE_SECONDS,
        ready_seen_seconds=audit.ready_seen_seconds,
        expected_lines_seen=audit.expected_index,
        ready_marker_count=audit.ready_count,
        sanitized_lines=tuple(audit.lines),
    )


class _DCB(ctypes.Structure):
    _fields_ = [
        ("DCBlength", wintypes.DWORD),
        ("BaudRate", wintypes.DWORD),
        ("Flags", wintypes.DWORD),
        ("wReserved", wintypes.WORD),
        ("XonLim", wintypes.WORD),
        ("XoffLim", wintypes.WORD),
        ("ByteSize", wintypes.BYTE),
        ("Parity", wintypes.BYTE),
        ("StopBits", wintypes.BYTE),
        ("XonChar", ctypes.c_char),
        ("XoffChar", ctypes.c_char),
        ("ErrorChar", ctypes.c_char),
        ("EofChar", ctypes.c_char),
        ("EvtChar", ctypes.c_char),
        ("wReserved1", wintypes.WORD),
    ]


class _COMMTIMEOUTS(ctypes.Structure):
    _fields_ = [
        ("ReadIntervalTimeout", wintypes.DWORD),
        ("ReadTotalTimeoutMultiplier", wintypes.DWORD),
        ("ReadTotalTimeoutConstant", wintypes.DWORD),
        ("WriteTotalTimeoutMultiplier", wintypes.DWORD),
        ("WriteTotalTimeoutConstant", wintypes.DWORD),
    ]


class Win32ReadOnlySession:
    """One Windows COM handle with reset control followed by read-only data flow."""

    _SET_RTS = 3
    _CLR_RTS = 4
    _CLR_DTR = 6
    _PURGE_RXABORT = 0x0002
    _PURGE_RXCLEAR = 0x0008

    def __init__(self, handle: int, kernel32: ctypes.WinDLL) -> None:
        self._handle = handle
        self._kernel32 = kernel32
        self._reset_done = False
        self._closed = False

    def _escape(self, operation: int) -> None:
        if not self._kernel32.EscapeCommFunction(self._handle, operation):
            raise ctypes.WinError(ctypes.get_last_error())

    def reset_once(self) -> None:
        if self._reset_done:
            raise ObservationError("reset may be attempted only once")
        self._reset_done = True
        self._escape(self._CLR_DTR)  # release GPIO0 / normal boot
        self._escape(self._SET_RTS)  # assert reset
        time.sleep(0.2)
        self._escape(self._CLR_RTS)  # release reset
        time.sleep(0.2)

    def read(self, timeout_seconds: float) -> bytes:
        if not self._reset_done or self._closed:
            raise ObservationError("invalid observation session state")
        timeout_ms = max(1, int(timeout_seconds * 1000))
        timeouts = _COMMTIMEOUTS(0xFFFFFFFF, 0, timeout_ms, 0, 0)
        if not self._kernel32.SetCommTimeouts(self._handle, ctypes.byref(timeouts)):
            raise ctypes.WinError(ctypes.get_last_error())
        buffer = ctypes.create_string_buffer(4096)
        received = wintypes.DWORD()
        if not self._kernel32.ReadFile(
            self._handle, buffer, len(buffer), ctypes.byref(received), None
        ):
            raise ctypes.WinError(ctypes.get_last_error())
        return buffer.raw[: received.value]

    def close(self) -> None:
        if not self._closed:
            self._closed = True
            if not self._kernel32.CloseHandle(self._handle):
                raise ctypes.WinError(ctypes.get_last_error())


class Win32ReadOnlyBackend:
    """Exact-name Win32 backend; contains no discovery or alternate-port path."""

    _GENERIC_READ = 0x80000000
    _OPEN_EXISTING = 3
    _INVALID_HANDLE_VALUE = ctypes.c_void_p(-1).value

    def __init__(self) -> None:
        if sys.platform != "win32":
            raise ObservationError("the reviewed production backend is Windows-only")
        self._kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)
        self._kernel32.CreateFileW.argtypes = (
            wintypes.LPCWSTR,
            wintypes.DWORD,
            wintypes.DWORD,
            wintypes.LPVOID,
            wintypes.DWORD,
            wintypes.DWORD,
            wintypes.HANDLE,
        )
        self._kernel32.CreateFileW.restype = wintypes.HANDLE
        self._kernel32.GetCommState.argtypes = (wintypes.HANDLE, ctypes.POINTER(_DCB))
        self._kernel32.GetCommState.restype = wintypes.BOOL
        self._kernel32.SetCommState.argtypes = (wintypes.HANDLE, ctypes.POINTER(_DCB))
        self._kernel32.SetCommState.restype = wintypes.BOOL
        self._kernel32.SetCommTimeouts.argtypes = (
            wintypes.HANDLE,
            ctypes.POINTER(_COMMTIMEOUTS),
        )
        self._kernel32.SetCommTimeouts.restype = wintypes.BOOL
        self._kernel32.EscapeCommFunction.argtypes = (wintypes.HANDLE, wintypes.DWORD)
        self._kernel32.EscapeCommFunction.restype = wintypes.BOOL
        self._kernel32.PurgeComm.argtypes = (wintypes.HANDLE, wintypes.DWORD)
        self._kernel32.PurgeComm.restype = wintypes.BOOL
        self._kernel32.ReadFile.argtypes = (
            wintypes.HANDLE,
            wintypes.LPVOID,
            wintypes.DWORD,
            ctypes.POINTER(wintypes.DWORD),
            wintypes.LPVOID,
        )
        self._kernel32.ReadFile.restype = wintypes.BOOL
        self._kernel32.CloseHandle.argtypes = (wintypes.HANDLE,)
        self._kernel32.CloseHandle.restype = wintypes.BOOL

    def open_exact_read_only(self, port: str, baud_rate: int) -> Win32ReadOnlySession:
        exact_port = validate_exact_port(port)
        device_path = rf"\\.\{exact_port}"
        handle = self._kernel32.CreateFileW(
            device_path,
            self._GENERIC_READ,
            0,
            None,
            self._OPEN_EXISTING,
            0,
            None,
        )
        if handle == self._INVALID_HANDLE_VALUE:
            raise ctypes.WinError(ctypes.get_last_error())

        dcb = _DCB()
        dcb.DCBlength = ctypes.sizeof(_DCB)
        try:
            if not self._kernel32.GetCommState(handle, ctypes.byref(dcb)):
                raise ctypes.WinError(ctypes.get_last_error())
            dcb.BaudRate = baud_rate
            dcb.ByteSize = 8
            dcb.Parity = 0
            dcb.StopBits = 0
            dcb.Flags |= 0x00000001  # fBinary
            dcb.Flags &= ~0x00000002  # fParity
            if not self._kernel32.SetCommState(handle, ctypes.byref(dcb)):
                raise ctypes.WinError(ctypes.get_last_error())
            if not self._kernel32.PurgeComm(
                handle, Win32ReadOnlySession._PURGE_RXABORT | Win32ReadOnlySession._PURGE_RXCLEAR
            ):
                raise ctypes.WinError(ctypes.get_last_error())
        except Exception:
            self._kernel32.CloseHandle(handle)
            raise
        return Win32ReadOnlySession(handle, self._kernel32)


def offline_audit() -> dict[str, object]:
    return {
        "device_access": False,
        "port_enumeration": False,
        "open_attempts": 0,
        "reset_attempts": 0,
        "serial_data_writes": 0,
        "production_policy": {
            "exact_port_only": True,
            "open_attempts": 1,
            "reset_attempts": 1,
            "reopen_attempts": 0,
            "enumeration_attempts": 0,
            "serial_data_writes": 0,
            "duration_seconds": OBSERVATION_SECONDS,
            "ready_deadline_seconds": READY_DEADLINE_SECONDS,
        },
        "expected_lines": EXPECTED_LINES,
    }


def _parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser()
    parser.add_argument("--offline-audit", action="store_true")
    parser.add_argument("--execute-startup-once", action="store_true")
    parser.add_argument("--port")
    return parser


def main(argv: list[str] | None = None, output: TextIO = sys.stdout) -> int:
    args = _parser().parse_args(argv)
    if args.offline_audit:
        if args.execute_startup_once or args.port:
            raise ObservationError("offline audit cannot include a port or device mode")
        output.write(json.dumps(offline_audit(), indent=2) + "\n")
        return 0
    if not args.execute_startup_once:
        raise ObservationError("device mode is not selected")
    if not args.port:
        raise ObservationError("one exact port is required")

    result = run_startup_observation(
        args.port, Win32ReadOnlyBackend(), emit=lambda line: output.write(line + "\n")
    )
    output.write(json.dumps(result.__dict__, indent=2) + "\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
