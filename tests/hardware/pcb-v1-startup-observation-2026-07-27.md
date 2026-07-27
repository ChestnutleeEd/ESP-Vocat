# PCB V1.0 Candidate Startup Observation — 2026-07-27

> **STARTUP OBSERVATION: PASS**
>
> **FIRST FLASH ATTEMPT REMAINS STOPPED / INCONCLUSIVE**
>
> **AUTHORIZATION CONSUMED AND CLOSED**
>
> **NO FURTHER DEVICE ACTION AUTHORIZED**

- OpenSpec Change: `prepare-pcb-v1-first-flash-smoke-test`
- Device: exact user-confirmed ESP-VoCat PCB V1.0 unit
- Port: exact user-confirmed `COM7`
- Interface: integrated USB Serial/JTAG
- Serial settings: 115200, 8 data bits, no parity, 1 stop bit
- Observer: committed `tools/startup_observation/startup_observer.py`
- Observer SHA-256:
  `A54F52BEFEF97CABB1B042229F7A2509BC736BC8806CA4A719618394F02037E1`
- Result: **PASS**
- Startup observation: **PASS**
- Runtime validation: **PASS FOR THIS MINIMAL SMOKE TEST**
- First Flash attempt: **STOPPED / INCONCLUSIVE — RANGE-SCOPE DEVIATION**

## 1. Authorization boundary

The user explicitly authorized one and only one startup reset on the exact
current `COM7` endpoint and one read-only observation on the same handle for
at most 60 seconds. The authorization excluded Flash read, Flash write,
Flash erase, readback, eFuse operations, device-identification commands,
rollback, restore, `idf.py`, build, a second reset, a second port open,
automatic retry, port enumeration, and alternate-port following.

The authorization was consumed when the observer opened `COM7` and performed
the reset. It is now closed and cannot be reused.

## 2. Host and repository preflight

Immediately before the device action:

- branch:
  `feat/prepare-pcb-v1-first-flash-smoke-test`;
- HEAD and upstream:
  `ab0dc42f34442f9c4212d965f2ae6698af71293e`;
- ahead/behind: `0/0`;
- worktree: clean;
- firmware worktree: unchanged;
- OpenSpec progress: `42/48`;
- Change: active and not archived;
- strict Change validation: passed;
- exact `COM7` Windows metadata query: one record, status `OK`,
  Availability `2`, USB Serial/JTAG class matched;
- other ports opened by preflight: zero;
- observer offline audit: passed;
- observer host tests: `11/11` passed;
- offline-audit device access, enumeration, opens, resets, and serial writes:
  all zero.

The observer's one exclusive `CreateFileW` call was the only conclusive port
occupancy check. It succeeded before reset, so no other process held the
endpoint in a way that prevented the reviewed exclusive open.

## 3. Executed operation

### Phase A — `REBOOT`

- exact port-open attempts: `1`;
- exact successful opens: `1`;
- COM7 open count: `1`;
- DTR release operations: `1`;
- RTS hard-reset assert/deassert attempts: `1`;
- reset attempts: `1`;
- control-line operations after reset: `0`.

### Phase B — `READ-ONLY`

- same handle used after reset: yes;
- handle remained usable for the complete window: yes;
- observation duration: `60.0` seconds;
- serial data writes: `0`;
- automatic enumeration attempts: `0`;
- enumeration attempts: `0`;
- reopen attempts: `0`;
- automatic retries: `0`;
- alternate-port following: `0`;
- original handle remained valid for the complete observation window: yes;
- destructive USB re-enumeration: not observed;
- raw serial log saved: no;
- retained/displayed lines: sanitized before retention/display.

No second device operation followed.

## 4. Sanitized observation

Normal ROM and ESP-IDF startup text appeared before the candidate lines. It
identified an ESP32-S3 boot path, Octal Flash mode, ESP-IDF v5.5.4 App
startup, and the candidate project name. No MAC, unique chip ID, device
serial, key digest, credential, token, NVS value, or raw device dump was
retained in this record.

The exact expected lines appeared once and in order:

```text
ESP-VoCat PCB V1.0 Smoke Test Candidate
Compile-time ESP-IDF version: 5.5.4
PCB target: ESP-VoCat PCB V1.0
Host-built candidate
Device execution not yet authorized
PSRAM intentionally disabled
No peripheral initialization
Ready marker: PCB_V1_SMOKE_TEST_CANDIDATE_READY
```

The observer reported:

- expected lines seen: `8`;
- ready marker count: `1`;
- internal ready timestamp: `0.001138100000389386` seconds;
- ready deadline: `15.0` seconds.

The internal timestamp begins after `reset_once()` returns. That function
waits 0.2 seconds after RTS deassertion before returning. The exact
reset-release-to-marker instant was therefore not separately measured; the
marker was already readable no later than approximately `0.201` seconds
after reset release (`0.2011381` seconds from the recorded internal timing).
This is unambiguously inside the authorized 15-second deadline without
claiming false timing precision.

## 5. Failure and stability audit

During the complete 60-second window:

- repeated startup: not observed;
- ready-marker repetition: not observed;
- panic or Guru Meditation: not observed;
- watchdog failure: not observed;
- allocation failure: not observed;
- ROM/Bootloader fatal error: not observed;
- invalid image/header/segment: not observed;
- checksum/hash/security rejection: not observed;
- anti-rollback rejection: not observed;
- continuous ROM/download loop: not observed;
- boot loop: not observed;
- USB handle loss: not observed;
- port change: not observed by the same-handle observer;
- evidence of an unauthorized persistent write: not observed.

The observer performs no port enumeration, so it does not independently
query Windows for a re-enumeration event. Operationally, no disruptive USB
re-enumeration was observed: the original handle remained valid for the full
window and no reopen or alternate port was used.

## 6. Acceptance decision

The bounded startup observation is **PASS** because:

1. `COM7` was opened exactly once;
2. one reset was performed;
3. no enumeration, reopen, retry, alternate-port following, or serial input
   occurred;
4. all eight exact lines appeared once and in order well inside 15 seconds;
5. the ready marker appeared exactly once;
6. the full 60-second window completed on the same handle;
7. no listed fatal, reset, stability, security, or persistent-write symptom
   appeared.

This proves only that the preserved boot chain selected and loaded the exact
candidate already present in `ota_0`, the candidate reached `app_main`, its
fixed USB Serial/JTAG output appeared, and no listed failure occurred in the
bounded window.

It does not convert the 2026-07-26 range-deviating First Flash attempt into a
PASS. That attempt remains **STOPPED / INCONCLUSIVE**. It also does not prove
long-term stability, Flash readback, recovery, PSRAM, display, touch, audio,
microphone, motor, network, storage behavior, power control, or product
functionality.

## 7. OpenSpec evidence

This runtime record supplies completion evidence for:

- Task 3.4: the preserved original boot chain demonstrably selected `ota_0`,
  loaded the v5.5.4 DOUT candidate, reached the ordered ready marker, and
  remained stable for the bounded window without a compatibility or
  persistent-write error;
- Task 12.1: exact reviewed serial observation opened once with no serial
  input;
- Task 12.2: expected output and failure conditions were evaluated;
- Task 12.3: the complete 60-second observation finished on the same handle.

Tasks 10.1-10.3, 11.1, 14.2, and 14.3 are not completed by this record.

## 8. Final authorization state

| Authorization | State |
|---|---|
| startup reset | **CONSUMED AND CLOSED / NONE** |
| serial observation | **CONSUMED AND CLOSED / NONE** |
| device access | **NONE** |
| Flash readback | **NONE / NOT PERFORMED** |
| Flash write or erase | **NONE / NOT PERFORMED** |
| eFuse or security operation | **NONE / NOT PERFORMED** |
| rollback or restore | **NONE / NOT PERFORMED** |
| second reset or second open | **NONE / NOT PERFORMED** |

## 9. Synchronized final evidence status

| Item | State |
|---|---|
| Firmware candidate runtime | **PASS — minimal smoke test only** |
| Preserved Bootloader compatibility | **PROVEN FOR THE TESTED CANDIDATE** |
| Task 3.4 | **COMPLETED** |
| OpenSpec | **42/48** |
| First Flash attempt | **STOPPED / INCONCLUSIVE — RANGE-SCOPE DEVIATION** |
| Startup observation | **PASS** |
| Device access authorization | **CONSUMED AND CLOSED / NONE** |
| Startup/observation authorization | **CONSUMED AND CLOSED / NONE** |
| Flash authorization | **NONE** |
| Readback authorization | **NONE** |
| Rollback authorization | **NONE** |
| Restore authorization | **NONE** |

The eight expected lines were complete, ordered, and each appeared once. The
ready marker appeared once, the same handle remained valid for the full
60-second window, and no panic, watchdog, allocation failure, boot loop, ROM
fatal error, Bootloader fatal error, or security rejection was observed.
No `read_flash`, `write_flash`, `erase_flash`, readback, eFuse, rollback, or
restore operation was executed, and no serial input was sent.

The three submission files contain no real MAC, chip-unique ID, device serial,
key digest, credential, token, certificate, Wi-Fi information, raw serial log,
authorization file, executable device command, binary, ELF, map, dump,
staging package, rollback artifact, generated `sdkconfig`, or firmware build
output.

Work stops after host-side documentation and validation. Any later device
action requires a new independent human review and exact authorization.
