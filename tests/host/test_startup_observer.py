from __future__ import annotations

import importlib.util
from pathlib import Path
import sys
import unittest


ROOT = Path(__file__).resolve().parents[2]
MODULE_PATH = ROOT / "tools" / "startup_observation" / "startup_observer.py"
SPEC = importlib.util.spec_from_file_location("startup_observer", MODULE_PATH)
assert SPEC is not None and SPEC.loader is not None
observer = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = observer
SPEC.loader.exec_module(observer)


class FakeClock:
    def __init__(self) -> None:
        self.value = 0.0

    def __call__(self) -> float:
        return self.value

    def advance(self, seconds: float) -> None:
        self.value += seconds


class FakeSession:
    def __init__(self, clock: FakeClock, events: list[tuple[float, object]]) -> None:
        self.clock = clock
        self.events = list(events)
        self.reset_count = 0
        self.read_count = 0
        self.close_count = 0

    def reset_once(self) -> None:
        self.reset_count += 1
        if self.reset_count > 1:
            raise AssertionError("reset called more than once")

    def read(self, timeout_seconds: float) -> bytes:
        self.read_count += 1
        if self.events:
            delay, value = self.events.pop(0)
            self.clock.advance(delay)
            if isinstance(value, Exception):
                raise value
            return value
        self.clock.advance(timeout_seconds)
        return b""

    def close(self) -> None:
        self.close_count += 1


class FakeBackend:
    def __init__(self, session: FakeSession) -> None:
        self.session = session
        self.open_count = 0
        self.opened_port = None
        self.opened_baud = None

    def open_exact_read_only(self, port: str, baud_rate: int) -> FakeSession:
        self.open_count += 1
        if self.open_count > 1:
            raise AssertionError("port opened more than once")
        self.opened_port = port
        self.opened_baud = baud_rate
        return self.session


def startup_bytes() -> bytes:
    return ("\r\n".join(observer.EXPECTED_LINES) + "\r\n").encode()


class StartupObserverTests(unittest.TestCase):
    def run_fake(self, events: list[tuple[float, object]]):
        clock = FakeClock()
        session = FakeSession(clock, events)
        backend = FakeBackend(session)
        result = observer.run_startup_observation("com7", backend, clock=clock)
        return result, backend, session

    def test_success_is_one_open_one_reset_zero_writes_and_full_window(self) -> None:
        result, backend, session = self.run_fake([(1.0, startup_bytes())])
        self.assertEqual(backend.open_count, 1)
        self.assertEqual(backend.opened_port, "COM7")
        self.assertEqual(backend.opened_baud, 115200)
        self.assertEqual(session.reset_count, 1)
        self.assertEqual(session.close_count, 1)
        self.assertEqual(result.open_attempts, 1)
        self.assertEqual(result.reset_attempts, 1)
        self.assertEqual(result.serial_data_writes, 0)
        self.assertEqual(result.enumeration_attempts, 0)
        self.assertEqual(result.reopen_attempts, 0)
        self.assertEqual(result.observation_seconds, 60.0)
        self.assertEqual(result.ready_marker_count, 1)
        self.assertEqual(result.expected_lines_seen, 8)
        self.assertLessEqual(result.ready_seen_seconds, 15.0)

    def test_exact_port_is_required(self) -> None:
        for value in ("", "auto", "*", "COM0", "COM7,COM8", r"\\.\COM7"):
            with self.subTest(value=value):
                with self.assertRaises(observer.ObservationError):
                    observer.validate_exact_port(value)

    def test_missing_ready_marker_stops_at_deadline_without_reopen(self) -> None:
        clock = FakeClock()
        session = FakeSession(clock, [(1.0, b"ordinary boot text\r\n")])
        backend = FakeBackend(session)
        with self.assertRaisesRegex(observer.ObservationError, "15-second"):
            observer.run_startup_observation("COM7", backend, clock=clock)
        self.assertEqual(backend.open_count, 1)
        self.assertEqual(session.reset_count, 1)
        self.assertEqual(session.close_count, 1)

    def test_disconnect_stops_without_reopen(self) -> None:
        clock = FakeClock()
        session = FakeSession(clock, [(0.5, OSError("USB disappeared"))])
        backend = FakeBackend(session)
        with self.assertRaisesRegex(observer.ObservationError, "no reopen"):
            observer.run_startup_observation("COM7", backend, clock=clock)
        self.assertEqual(backend.open_count, 1)
        self.assertEqual(session.reset_count, 1)
        self.assertEqual(session.close_count, 1)

    def test_panic_is_a_stop_condition(self) -> None:
        with self.assertRaisesRegex(observer.ObservationError, "fatal startup"):
            self.run_fake([(0.5, b"Guru Meditation Error: Core 0 panic'ed\r\n")])

    def test_repeated_startup_sequence_is_rejected(self) -> None:
        with self.assertRaisesRegex(observer.ObservationError, "repeated"):
            self.run_fake([(1.0, startup_bytes() + startup_bytes())])

    def test_expected_lines_must_be_ordered(self) -> None:
        swapped = list(observer.EXPECTED_LINES)
        swapped[1], swapped[2] = swapped[2], swapped[1]
        data = ("\n".join(swapped) + "\n").encode()
        with self.assertRaisesRegex(observer.ObservationError, "out of order"):
            self.run_fake([(1.0, data)])

    def test_unbounded_line_is_rejected(self) -> None:
        data = b"x" * (observer.MAX_LINE_CHARS + 1)
        with self.assertRaisesRegex(observer.ObservationError, "bounded length"):
            self.run_fake([(0.5, data)])

    def test_sensitive_output_is_redacted_before_retention(self) -> None:
        sensitive = (
            b"MAC: aa:bb:cc:dd:ee:ff\r\n"
            b"device serial: private-value\r\n"
            + startup_bytes()
        )
        result, _, _ = self.run_fake([(1.0, sensitive)])
        retained = "\n".join(result.sanitized_lines)
        self.assertNotIn("aa:bb:cc:dd:ee:ff", retained.lower())
        self.assertNotIn("private-value", retained)
        self.assertIn("[REDACTED_MAC]", retained)
        self.assertIn("[REDACTED_SENSITIVE_LINE]", retained)

    def test_source_contains_no_enumeration_or_serial_data_write_path(self) -> None:
        source = MODULE_PATH.read_text(encoding="utf-8")
        self.assertNotIn("list_ports", source)
        self.assertNotIn("serial.tools", source)
        self.assertNotIn("WriteFile", source)
        self.assertNotIn("GENERIC_WRITE", source)

    def test_offline_audit_is_device_free(self) -> None:
        audit = observer.offline_audit()
        self.assertFalse(audit["device_access"])
        self.assertFalse(audit["port_enumeration"])
        self.assertEqual(audit["open_attempts"], 0)
        self.assertEqual(audit["reset_attempts"], 0)
        self.assertEqual(audit["serial_data_writes"], 0)
        policy = audit["production_policy"]
        self.assertEqual(policy["open_attempts"], 1)
        self.assertEqual(policy["reset_attempts"], 1)
        self.assertEqual(policy["reopen_attempts"], 0)


if __name__ == "__main__":
    unittest.main()
