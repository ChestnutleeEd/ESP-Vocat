# PCB V1.0 Display Validation — First Controlled Startup Observation

Date: 2026-09-20

Change: `validate-pcb-v1-display-backlight-on-hardware`

> **RESULT: STARTUP FAIL — READY ABSENT AT 15-SECOND DEADLINE**
>
> **NO RETRY OR RECOVERY PERFORMED**
>
> **PHYSICAL DISPLAY OBSERVATION: USER REPORT REQUIRED**
>
> **HARDWARE/DISPLAY STATUS: UNVERIFIED**

## 1. Authorization and host gate

The user separately authorized exactly one controlled startup of the already
written display-validation candidate followed by one bounded serial/log
observation on exact `COM7`. The authorization explicitly excluded a second
reset, Flash write/erase, restore, rollback, eFuse operation, brightness or
timing change, and automatic corrective action.

Immediately before startup:

- branch: `feat/validate-pcb-v1-display-backlight-on-hardware`;
- HEAD: `491d750f48e1540e765abbe88fe332c5edb91dd6`;
- upstream: synchronized `0/0`;
- tracked worktree: clean;
- prior write evidence: present and records one successful guarded write plus
  zero additional attempts;
- candidate SHA-256:
  `FEE9C3D1AA77CD0DF1B2C2D55A86B7BEAA2871E5E8443B9D458B10324FF8F362`;
- write authorization: `CONSUMED HUMAN FLASH AUTHORIZATION`, and a host-only
  guard check rejected reuse;
- exact `COM7` metadata: `VID_303A/PID_1001`, MI_00, status OK;
- conflicting Python/esptool/observer processes: zero.

## 2. Expected production evidence

The production source and host tests define an ordered 22-line success
sequence. It starts with:

- `ESP-VoCat PCB V1.0 Minimal Display Smoke Test`;
- `Compile-time ESP-IDF version: 5.5.4`;
- PCB/candidate/PSRAM/artifact-policy markers.

It then requires one ordered entry for each state from `BOOT_MARKER` through
`READY`, including:

- `LCD_SM_ENTER BACKLIGHT_FORCED_OFF`;
- `LCD_SM_ENTER PANEL_INIT`;
- `LCD_SM_ENTER TEST_PATTERN_DRAW`;
- `LCD_SM_ENTER BACKLIGHT_PWM_PREPARE`;
- `DISPLAY_BACKLIGHT_CONFIG gpio=44 polarity=ACTIVE_HIGH frequency_hz=2000 resolution_bits=10 initial_duty=0`;
- `LCD_SM_ENTER BACKLIGHT_LOW_ENABLE`;
- `DISPLAY_BACKLIGHT_ENABLED raw_duty=10 max_duty=1023 percent=0.98 visual=UNVERIFIED`;
- `LCD_SM_READY visual=UNVERIFIED backlight=LOW_FIXED_TEST_ONLY`;
- `LCD_TEST_RESULT READY visual=UNVERIFIED backlight=LOW_FIXED_TEST_ONLY`.

Stop conditions included `LCD_SM_FAIL`, `LCD_TEST_RESULT FAIL`, panic, Guru
Meditation, watchdog, brownout, display/SPI/panel/LEDC failure text, repeated
reset banners, handle loss, excess output, and READY absence after 15 seconds.

## 3. Observation mechanism

The repository's reviewed `tools/startup_observation/startup_observer.py`
provided the Win32 backend and bounds:

- exact port only;
- one read-only handle open;
- one reset;
- zero serial data writes;
- no enumeration, reopen, or retry;
- READY deadline 15 seconds;
- full success window 60 seconds.

Because that tool's expected lines still targeted the predecessor serial-only
candidate, a repository-external wrapper substituted only the exact current
production markers and added the current display failure markers. It did not
alter the backend or repository/firmware.

External wrapper:

`D:\ESP-VoCat_Startup_Observations\pcb-v1-display-startup-observer.py`

SHA-256:

`11460F641FE3E6D4AAB0B369377192688CE6BD587A66767B416DB560A8AC0F1E`

Its device-free self-test passed 22 ordered expected markers, one READY marker,
and five fatal-marker cases. The unchanged repository observer's 11 host tests
also passed.

## 4. Exact startup action

Timestamp/context:

- start: `2026-09-20T22:17:08.1414157+08:00`;
- end: `2026-09-20T22:17:23.9135078+08:00`;
- endpoint: exact `COM7`;
- baud: 115200;
- handle: opened once for `GENERIC_READ`, no sharing;
- GPIO0/DTR: released for normal boot;
- reset: RTS asserted once for 200 ms and released once;
- post-reset settle: 200 ms;
- serial writes: zero.

The reset call returned successfully and the read loop remained active until
the first defined stop condition. There was no second reset or open.

## 5. Captured serial result

No device serial line was emitted to or retained by the observer before the
15-second READY deadline. Therefore:

- reset reason: not captured / `UNVERIFIED`;
- bootloader-to-App transition: not captured / `UNVERIFIED`;
- App start: not proven;
- display initialization: not proven;
- pattern transfer: not proven;
- GPIO44 fail-dark sequence: not proven on hardware;
- LEDC duty-zero preparation: not proven on hardware;
- duty-10 enable: not proven on hardware;
- READY: absent from captured output;
- panic/watchdog/brownout/reset-loop text: none captured, but silence cannot
  prove their physical absence;
- final runtime state: unknown.

The controlling failure was:

`ObservationError: ready marker was absent at the 15-second deadline`

Sanitized external log:

`D:\ESP-VoCat_Startup_Observations\pcb-v1-display-startup-attempt-1-20260920.log`

- bytes: `737`;
- SHA-256:
  `2AB19B4B49160AE1D1BF79665F9BFFB4F42C91FED4E3184FDF602EA65430A9C4`;
- retained device serial lines: zero;
- content: host traceback identifying only the READY-deadline failure.

## 6. Stop and operation counts

The observation handle was closed at the first failure. No further physical
interaction occurred.

| Operation | Count |
|---|---:|
| controlled startup resets | 1 |
| serial monitor/observation sessions | 1 |
| serial data writes | 0 |
| port enumeration | 0 |
| port reopen/retry | 0 |
| additional reset/startup attempts | 0 |
| Flash writes | 0 |
| Flash erases | 0 |
| Flash readback | 0 |
| restore/rollback | 0 |
| eFuse/security/voltage writes | 0 |
| brightness/timing/panel/SPI changes | 0 |

## 7. Physical observation boundary

Codex has no camera or other machine-visible view of the physical display.
The user must report only the current display illumination, test-pattern
visibility, stability/flicker, and obvious color/corruption/orientation state.
No button press, reset, reconnection, power cycle, command, or brightness
change is requested or authorized.

Until that current-state report is recorded, illumination, GPIO44 PWM,
pattern/pixels, color, orientation, refresh stability, reset-time transient,
and application runtime all remain `UNVERIFIED`.

`FAIL — FIRST CONTROLLED STARTUP FAILED; NO RETRY OR RECOVERY PERFORMED`
