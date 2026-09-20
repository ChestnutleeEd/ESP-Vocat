## Context

The predecessor `prepare-pcb-v1-minimal-display-smoke-test` is complete at commit `a0b7c726d90448af0f811b7f6c996897e2a16c8a`. Its host tests, native fault harness, two clean builds, and reproducibility contract pass. Its canonical 222,000-byte App image hashes to `4E66B7EDCD38B5225B00E1DB790CA7DD9C842E765961E8929228105F9C10A05D`, but it intentionally keeps GPIO44 low and is explicitly `not-for-visual-validation`. Physical display behavior remains `UNVERIFIED`.

The current firmware initializes ST77916 QSPI, draws the fixed test pattern with a single internal-DMA strip, reaches a hard-disabled backlight policy gate, and settles in a stable terminal loop. Its GPIO44 path preloads low before direction configuration, reasserts low after configuration, checks the level before each draw, and best-effort restores low on every failure path. This is the safety base to extend; it is not replaced wholesale.

The historical First Flash remains `STOPPED / INCONCLUSIVE — RANGE-SCOPE DEVIATION`. The later startup observation remains `PASS`, and the bounded runtime result remains `PASS FOR THIS MINIMAL SMOKE TEST`. All prior device authorizations are consumed/closed or `NONE`.

### Evidence classification

- `CONFIRMED` repository/document facts:
  - the official V1.0 schematic record identifies `LCD_BLK`, AO3400A N-MOS Q2, gate pull-down R5 10 kΩ, LEDK series resistor R4 20 Ω, and GPIO44;
  - the display-module specification records LEDA/LEDK and typical three-parallel-LED backlight values of 3 V, 60 mA, and 0.18 W;
  - fixed historical software uses active-high LEDC at 25 kHz/10-bit;
  - the fixed official ESP-Brookesia V1.0 source uses 2 kHz/10-bit with initial duty zero;
  - ESP-IDF v5.5.4 supports GPIO44 as input/output, maps it as default U0RXD, supports low-speed LEDC from the 80 MHz APB clock, and provides `ledc_set_duty_and_update` and `ledc_stop(..., idle_level)`;
  - the current firmware console uses USB Serial/JTAG rather than UART0;
  - four immutable 32 MiB recovery images and two Level 1 `ota_0` staging copies were rehashed read-only on 2026-09-19 and still match their recorded sizes and hashes.
- `STRONGLY SUPPORTED` PCB V1.0 conclusions:
  - GPIO44 drives the Q2 gate directly as an active-high control;
  - high turns on the low-side LEDK path and low turns it off;
  - R5 biases the backlight off while GPIO44 is undriven;
  - PWM of the GPIO44 gate is the intended software control method.
- `UNVERIFIED` physical facts:
  - actual board population and continuity of Q2/R4/R5/display flex;
  - reset/power-on GPIO44 gate waveform and any visible transient;
  - actual PWM waveform, peak/average LED current, brightness linearity, minimum visible duty, flicker, display health, color, orientation, refresh, and thermal behavior;
  - execution of the documented BOOT-during-power-on procedure on this exact
    unit and the resulting ROM download-mode enumeration.

### Assumptions

- The official V1.0 schematic and two independent V1.0 software precedents are sufficient to select a conservative host-implementation contract without claiming physical proof.
- The existing ST77916, pattern, QSPI, memory, and failure behavior remain unchanged except where the backlight state machine and tests must be extended.
- A fixed sub-1% duty is preferable to dynamic brightness for the first bounded observation.
- No live backup/readback is required merely to preserve the currently installed project candidate: the immutable same-device original recovery image, Level 1 original `ota_0`, committed source, and reproducible predecessor evidence are the controlling recovery assets. Any contrary future decision would require a separate read authorization.

### Expected affected files during a later Apply

- `firmware/main/pcb_v1_display.h`
- `firmware/main/pcb_v1_display.c`
- `firmware/main/main.c`
- `firmware/main/CMakeLists.txt`
- `tests/host/fakes/pcb_v1_display_host_fakes.h` and the smallest needed fake LEDC header surface
- `tests/host/pcb_v1_display_host_harness.c`
- `tests/host/test_pcb_v1_minimal_display.py` or a task-specific successor test file
- a new successor artifact manifest and host-build review under `tests/build/`
- this Change's OpenSpec artifacts and any required task-specific documentation

`pcb_v1_display_test_pattern.c/.h`, ST77916 dependency version, vendor table, partition layout, bootloader, and product modules are not expected to change.

## Goals / Non-Goals

**Goals:**

- Define one exact, auditable, conservative non-zero backlight configuration.
- Prevent an unintended high/full-brightness pulse during boot, pin mux transition, LEDC setup, normal execution, and failure cleanup.
- Enable backlight only after panel initialization, display-on, and all test-pattern transfers complete.
- Preserve single-shot, no-retry, fail-safe behavior and comprehensive host fault injection.
- Produce a new reproducible visual-validation artifact with its own hash and geometry evidence.
- Close recovery, identity, port, operation, authorization, and observation gates before any future physical write.
- Define bounded visual acceptance without claiming results in advance.

**Non-Goals:**

- Dynamic/user brightness, fades, NVS settings, companion control, automatic escalation, or a second duty step.
- Touch, LVGL, PSRAM, audio, microphone, motor, SD, network, filesystem, product UI, or animation.
- PCB V1.2, complete BSP reuse, GPIO9 power control, GPIO45 DC, GPIO47 reset, or 80 MHz LCD clock.
- Device operations during this planning task.
- Full recovery execution, restoration rehearsal, or predecessor archive.
- Proving physical current, brightness, transient, display health, or recovery through host evidence.

## Decisions

### 1. GPIO44 electrical contract

GPIO44 is treated as `active-high` and `MOSFET-driven`, not as a direct LED-current output. It drives the AO3400A gate; Q2 switches LEDK on the low side. Logical/PWM zero is OFF, and logical/PWM high time is ON. `output_invert` must be disabled.

This conclusion is `STRONGLY SUPPORTED — REQUIRES FUTURE DEVICE VALIDATION`. The implementation and artifact must not call it physically confirmed.

Rejected alternatives:

- Active-low: contradicts the recorded N-MOS low-side topology and both V1.0 software precedents.
- Direct constant GPIO high: cannot provide the requested minimum visual exposure and creates an unnecessary 100% duty interval.
- Another GPIO or control IC: unsupported and outside the exact PCB V1.0 allowlist.

### 2. Boot/default-state assessment

Local ESP-IDF v5.5.4 identifies GPIO44 as a general input/output and default `U0RXD`; it is not identified as an ESP32-S3 strapping pin. The current firmware uses USB Serial/JTAG for its console, so no UART0 RX service may retain GPIO44. Static validation must reject a new UART0 dependency.

The external 10 kΩ gate pull-down strongly supports OFF while the pad is high impedance. The actual reset-time waveform remains `UNVERIFIED`. Firmware must therefore configure the low state at the earliest display step and the future visual observation must explicitly watch for an early full-brightness flash.

### 3. Exact initial PWM configuration

The fixed contract is:

| Parameter | Value |
|---|---|
| GPIO | 44 |
| Polarity | active high; no output inversion |
| Driver | ESP-IDF LEDC |
| Speed mode | `LEDC_LOW_SPEED_MODE` |
| Timer/channel | `LEDC_TIMER_0` / `LEDC_CHANNEL_0` |
| Clock | `LEDC_USE_APB_CLK` (80 MHz source in reviewed ESP-IDF source) |
| Frequency | 2,000 Hz |
| Resolution | 10 bits |
| Counter maximum | 1023 |
| Initial/configuration duty | 0 |
| First visual-test raw duty | 10 |
| Approximate duty | `10 / 1023 × 100 = 0.9775%` |
| H-point | 0 |
| Fade | prohibited |
| Runtime duty changes | prohibited |

Rationale:

- Both reviewed V1.0 implementations agree on 10-bit resolution and active-high PWM.
- The fixed official V1.0 source supplies the 2 kHz precedent. It is above visible flicker frequency while requiring fewer MOSFET transitions than 25 kHz.
- At 2 kHz, raw duty 10 provides an approximately 4.89 µs high interval in each 500 µs period. This is a concrete, reproducible, sub-1% first exposure.
- Under the module's typical 60 mA full-on figure, ideal average current would be roughly 0.59 mA. This is an engineering estimate only: PWM does not reduce peak current during the high interval, and no physical current is claimed.
- The value is below historical 10% fallback and 75% product defaults and avoids an arbitrary full-brightness test.

Rejected alternatives:

- 25 kHz/10-bit: historically proven as software, but higher switching frequency is unnecessary for the first static pattern and conflicts with the official 2 kHz precedent.
- Raw duty 1: minimally non-zero but may be below practical visibility and create an inconclusive test.
- 5–10%: more likely visible but provides materially more exposure than needed for the first gate.
- Dynamic ramp/fade: increases states and transient risk without helping first validation.

### 4. Startup state sequence

The successor state graph has exactly these states:

1. `BOOT_MARKER`
2. `BACKLIGHT_FORCED_OFF`
3. `QSPI_BUS_INIT`
4. `PANEL_IO_CREATE`
5. `PANEL_RESET`
6. `PANEL_INIT`
7. `DISPLAY_ON`
8. `TEST_PATTERN_DRAW`
9. `BACKLIGHT_PWM_PREPARE`
10. `BACKLIGHT_LOW_ENABLE`
11. `READY`
12. `FAIL_SAFE`

`BACKLIGHT_FORCED_OFF` retains the existing preload-low → configure output/pull-down → reassert-low → read-low sequence and occurs before every QSPI/panel call.

`BACKLIGHT_PWM_PREPARE` occurs only after the entire pattern transfer completes. It configures timer 0, then channel 0 on GPIO44 with initial duty 0 and h-point 0. Each API result is checked. The implementation must validate that the configured/effective duty is zero before continuing. The R5 pull-down and zero output cover the GPIO-to-LEDC mux transition; host ordering tests must prove no non-zero call occurs during preparation.

`BACKLIGHT_LOW_ENABLE` performs one checked `ledc_set_duty_and_update` to raw duty 10/h-point 0, verifies the reported duty equals 10, and emits one exact marker containing GPIO, polarity, frequency, resolution, raw duty, and approximate percentage. There is no alternate value, retry, fade, ramp, or runtime control.

`READY` retains `visual=UNVERIFIED` and records the exact PWM contract. It does not release display or LEDC resources because the bounded observation needs the static pattern and fixed low backlight to remain active.

### 5. Failure and shutdown ordering

Any failure before LEDC channel ownership uses the existing GPIO-low fail-safe path. Once LEDC may own GPIO44, cleanup performs best-effort shutdown in this order before other resources are released:

1. request duty 0 with a checked update when the channel exists;
2. call `ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0)` so idle output is low;
3. preload the GPIO output latch low;
4. restore GPIO44 as a digital output with pull-down;
5. reassert/read low;
6. release remaining display resources in the already reviewed reverse order.

All available cleanup steps are attempted even if one fails. The original main-flow error retains priority. Cleanup emits no second failure marker and never selects another duty. Timer/channel setup and enable calls are each single-attempt.

There is no firmware-initiated reboot or normal shutdown in this smoke test. On external reset/power loss, the 10 kΩ pull-down strongly supports OFF until firmware reaches `BACKLIGHT_FORCED_OFF`; actual physical transient remains an observation item. A failure that leaves the device unreachable does not trigger automatic rollback.

### 6. Markers and evidence

Success markers must include:

- `LCD_SM_ENTER BACKLIGHT_PWM_PREPARE`
- `DISPLAY_BACKLIGHT_CONFIG gpio=44 polarity=ACTIVE_HIGH frequency_hz=2000 resolution_bits=10 initial_duty=0`
- `LCD_SM_ENTER BACKLIGHT_LOW_ENABLE`
- `DISPLAY_BACKLIGHT_ENABLED raw_duty=10 max_duty=1023 percent=0.98 visual=UNVERIFIED`
- `LCD_SM_READY visual=UNVERIFIED backlight=LOW_FIXED_TEST_ONLY`

Failure remains exactly one `LCD_SM_FAIL state=<STATE> err=<CODE>`. Markers contain no MAC, device identity, credential, NVS value, or recovery data.

### 7. Host validation strategy

The future Apply must extend the fake API and native harness at the real LEDC API boundary. Fault injection must cover timer configuration, channel configuration, zero-duty verification, non-zero update, enabled-duty verification, zero-duty cleanup update, LEDC stop, and GPIO-low restoration. Each injectable call gets a matrix case; APIs without an error channel get call/order/value assertions.

Host acceptance must prove:

- exact 12-state graph and marker cardinality;
- no LEDC/non-zero call before `TEST_PATTERN_DRAW` completes;
- initial channel duty 0, only one permitted non-zero value 10, no fade/dynamic/NVS path;
- exact mode/timer/channel/clock/frequency/resolution/h-point/polarity values;
- error before or after enable prevents READY and drives the shutdown sequence;
- cleanup continues after individual errors and retains the original diagnosis;
- repeated run-once calls cause no new hardware action;
- existing 41-point predecessor inventory is deliberately rebaselined to a documented successor inventory with complete coverage, not silently assumed unchanged;
- all previous display geometry, table, buffer, callback, timeout, denylist, and unrelated-peripheral tests continue to pass.

Static review scans source, generated configuration, component tree, map, symbols, and strings. It rejects duty values other than 0 and 10 in the backlight path, another LEDC timer/channel, output inversion, high-speed mode, another clock/frequency/resolution, fade APIs, NVS brightness, dynamic setters, UART0 ownership, wrapper aliases, GPIO44 direct-high writes, and any new GPIO/peripheral.

### 8. Build and artifact gate

Configure, build, inspect, connect, Flash, observe, and restore remain separate actions. Before any device gate:

- two new build directories independently configure/build the complete intended source with ESP-IDF v5.5.4 and `CONFIG_APP_REPRODUCIBLE_BUILD=y`;
- BIN, ELF, MAP, bootloader, partition table, resolved sdkconfig, source-input digest, toolchain identity, and clean-build hashes are recorded;
- the two App BINs have equal size and SHA-256;
- a new manifest identifies `visual-validation-candidate`, GPIO44 active high, 2 kHz, 10 bits, raw 10/1023, 0.98%, and fail-safe low;
- the new artifact hash is distinct from the predecessor hard-disabled hash;
- semantic range, ROM transport blocks/padding, and sector erase envelope are independently calculated;
- the exact preserved `ota_0` containment and every untouched region are listed;
- no generated build directory, sdkconfig output, cache, or binary is committed.

The predecessor manifest remains immutable historical evidence and is not overwritten or relabeled.

### 9. Recovery and backup gate

Current read-only planning evidence:

- four 33,554,432-byte full images match SHA-256 `72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001`;
- two 4,128,768-byte Level 1 complete original `ota_0` images match SHA-256 `C8A2FE4AB0F9B7C823F1DCFDFC926682C9DBF1B079056461DC6F18A93A345D0E`.

Immediately before any future write review, these exact assets must be rechecked without modification. A mismatch or missing file is a stop condition. The future packet must distinguish:

- Level 1: restore the complete original `ota_0` range `0x00020000-0x00410000` under a separately reviewed Recovery Change;
- Level 2: restore the complete same-device 32 MiB image under an independent escalation gate.

No new live backup is required by the current evidence because the original same-device state is already captured redundantly and prior App-only writes did not target NVS, `otadata`, bootloader, table, PHY, assets, or the upper tail. A live readback would be a separate `READ-ONLY` device operation and must not be added casually.

### 10. Device and authorization gate

Before any future write, all of these must be satisfied in a fresh review:

- exact branch/HEAD and clean source state;
- exact visual-candidate path, size, SHA-256, semantic range, transport range/padding, and erase envelope;
- exact ESP32-S3 / PCB V1.0 identity and current USB Serial/JTAG endpoint; historical COM7 is not reusable;
- download mode enabled and USB Serial/JTAG available, with expected chip identity fields compared without retaining private identifiers;
- a documented manual BOOT/RESET recovery-entry procedure for this board if automatic ROM entry fails; the repository currently lacks an exact button sequence, so this is a device-gate blocker until resolved from authoritative documentation or a separately reviewed nondestructive procedure;
- reverified recovery assets and same-device scope;
- reviewed one-attempt mechanism with no automatic retry and explicit ROM final-block padding;
- exact non-executable command review, followed only later by fresh user authorization for that exact command/port/artifact/ranges;
- write and startup/display observation as separate operations and separate authorizations.

Creating or implementing this Change does not satisfy any of these device gates.

### 11. Visual acceptance and stop conditions

The separately authorized observation may pass only if all of these bounded facts are observed:

- preserved boot chain starts the exact candidate with no reset loop, panic, watchdog, allocation failure, or security rejection;
- stage markers are single, ordered, and agree with the exact PWM contract;
- there is no visible full-brightness flash before the low-enable marker;
- backlight reaches only a visibly low test level without flicker, pulsing, odor, heat, noise, or abnormal current symptom perceptible to the operator;
- the fixed black/RGB/white-border/`ESP-VoCat LCD TEST` pattern appears;
- color regions, text, orientation, and refresh are plausible and stable for the bounded window;
- no repeated initialization or duty change occurs.

Stop immediately without retry or automatic rollback on unexpected brightness, early flash, flicker/pulsing, blank output after ordered READY, wrong/repeating markers, reset loop, panic, watchdog, allocation failure, USB-handle loss, odor, heat, sound, or any command/range/identity mismatch. Hardware danger calls for manual power removal; repository tools must not automate it. Any rollback or restore is deferred to a separately reviewed Recovery Change.

An observation PASS can confirm only the explicitly observed physical facts for the bounded run. It does not verify long-term life, measured current, full brightness range, touch, other peripherals, power cycles, or recovery.

## Risks / Trade-offs

- [Active-high/topology mapping is not measured on this unit] → Use the official V1.0 schematic plus two matching software precedents, fixed sub-1% exposure, and preserve `STRONGLY SUPPORTED` until observation.
- [PWM lowers average exposure but not peak LED current] → Rely on the board's designed resistor/MOSFET path, never claim peak-current reduction, and start at raw 10 only.
- [Raw 10 may be too dim to see] → Treat a blank but otherwise stable run as inconclusive, not as permission to raise duty; any higher duty requires a revised host/artifact gate.
- [GPIO-to-LEDC mux transient] → Preload and hold digital low, configure LEDC channel at duty zero, check ordering, and observe for early flash.
- [GPIO44 default U0RXD interaction] → Keep USB Serial/JTAG console, reject UART0 ownership, and audit generated symbols/configuration.
- [LEDC cleanup call fails] → Attempt zero update, idle-low stop, direct GPIO-low restoration, and remaining cleanup; retain the first failure.
- [Physical test damages recoverability] → Restrict the candidate to reviewed App-only geometry, rehash Level 1/Level 2 assets, require manual download-mode procedure, and keep recovery independent.
- [A lit screen is mistaken for project completion] → Keep touch and restoration milestones separate; preserve DEC-023 and `UNVERIFIED` for unobserved facts.
- [Historical range-scope deviation repeats] → Authorize semantic, transport/padding, and erase ranges independently.

## Migration Plan

1. Implement only the exact LEDC/state/marker changes on the host.
2. Extend fakes, native harness, Python tests, fault matrix, and static guards.
3. Configure and build in two fresh ignored directories; inspect generated dependency/config/map/symbol evidence.
4. Create a new visual-candidate manifest/review and reconcile exact ranges and recovery assets.
5. Stop for a full host diff and OpenSpec review. At this point the Change may become ready for device-gate review, not device execution.
6. Resolve the manual BOOT/RESET download-mode procedure and freshly verify identity/port/recovery prerequisites.
7. Prepare and review, but do not execute, one exact App-only write packet.
8. Obtain fresh explicit user authorization for that exact `WRITE`; perform at most one attempt and stop on the result.
9. Separately review and authorize one startup/display observation (`REBOOT` then `READ-ONLY`).
10. Record actual observations without overclaim. If recovery is needed, stop and create/use an independent Recovery verification Change.

Rollback is not automatic and is not part of implementation. Level 1 and Level 2 recovery retain their separate review and authorization boundaries.

## Open Questions

- The authoritative board procedure is now documented: hold the dedicated
  BOOT button while powering on to enter download mode; RST is the reset
  button, and USB-C is the programming/debugging connection. Physical
  execution on this exact unit remains unverified until observed.
- Is raw duty 10 visibly sufficient on the physical unit? Only a bounded authorized observation can answer; an inconclusive result does not authorize a higher value.
- What reset-time GPIO44/MOSFET waveform occurs on this unit? The external pull-down supports OFF, but only measurement or bounded observation can confirm the absence of a flash.
- Does the physical panel show the expected RGB order/orientation and stable refresh? It remains `UNVERIFIED` until observation.

No open question blocks the exact host-side implementation and validation described above. All device operations remain blocked until their gates close and fresh authorization is obtained.

## Host implementation outcome (2026-09-20)

The bounded host implementation is complete on
`feat/validate-pcb-v1-display-backlight-on-hardware`. It implements the exact
12-state graph, fixed GPIO44 LEDC contract, post-pattern zero-duty preparation,
single raw-10 enable, first-error-preserving fail-safe shutdown, and exact
`visual=UNVERIFIED` markers. It does not add dynamic brightness, fade, NVS,
touch, audio, network, motor, SD, PSRAM, LVGL, or another GPIO path.

The complete unittest and pytest collections each pass 56/56. The native
harness passes 19,256 assertions with 51 production call points, 46 failable
boundaries, a 46/46 fault matrix, and no uncovered point. Two independent
ESP-IDF v5.5.4 builds are byte-identical across App BIN/ELF/MAP, bootloader,
partition table, and resolved sdkconfig. The successor App BIN is 231,360 bytes
with SHA-256
`FEE9C3D1AA77CD0DF1B2C2D55A86B7BEAA2871E5E8443B9D458B10324FF8F362`.
The detailed evidence is recorded in
`tests/build/pcb-v1-display-backlight-validation-artifact-manifest.json` and
`tests/build/pcb-v1-display-backlight-validation-host-build-and-artifact-review.md`.

Official Espressif ESP-VoCat v1.0 documentation now establishes the
board-specific control and connection facts: a dedicated RST button, a
dedicated BOOT button, BOOT held while powering on to enter download mode,
and USB-C for programming download and debugging. It also identifies
ESP32-S3-WROOM-2-N32R16V, 32 MB Flash, 16 MB PSRAM, and LCD_BLK on GPIO44:

- https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp-vocat/user_guide_v1.0.html

This closes the documentation gap in task 6.1. The procedure is
`DOCUMENTED PROCEDURE`, not `PHYSICALLY CONFIRMED ON THIS UNIT`; no physical
BOOT attempt is inferred. Recovery assets were rehashed immediately before
the later device-gate interrogation. Physical display/backlight behavior remains
`UNVERIFIED`.

## Pre-write physical identity gate outcome (2026-09-20)

Repository, candidate, recovery, official-procedure, and endpoint gates passed.
One authorized no-stub esptool 4.12.dev3 `flash_id` interrogation on the
uniquely enumerated `COM7` endpoint confirmed ESP32-S3 QFN56 revision v0.2,
16 MB embedded PSRAM, USB Serial/JTAG, JEDEC `C2:8039`, 32 MB Flash, and octal
Flash mode. It also reported `Flash voltage set by eFuse to 3.3V`.

That voltage report conflicts with the documented
`ESP32-S3-WROOM-2-N32R16V` / `ESP32-S3R16V` 1.8 V expectation and with the
prior hardware profile. Local esptool source confirms that the report is read
from the ESP32-S3 VDD_SPI eFuse fields. The identity gate therefore failed
closed. No second interrogation or device operation followed, and no future
write command was generated. VDD_SPI/eFuse changes remain absolutely
prohibited. Physical display/backlight status remains `UNVERIFIED`.

### Subsequent VDD_SPI reconciliation

The original identity-gate failure remains preserved. The repository's earlier
sanitized minimum eFuse summary already recorded `VDD_SPI_FORCE=1`,
`VDD_SPI_XPD=1`, and `VDD_SPI_TIEH=0`, all readable and then reported
writeable. ESP32-S3 definitions map those fields to a forced, enabled 1.8 V
LDO, with GPIO45 ignored.

Installed esptool.py `v4.12.dev3` returned 3.3 V because its inherited helper
tested whether any bit in the combined FORCE/XPD/TIEH mask was nonzero before
testing the narrower 1.8 V combination. Current upstream code tests FORCE,
then XPD, then TIEH and returns 1.8 V for this exact combination. The old text
is therefore a tool-logic defect, not a conflicting device configuration.

Macronix's official datasheet maps live RDID `C2 80 39` exactly to the
MX25UM25645G 256-Mbit / 32-MiB Octal device with a 1.65-2.0 V supply. Together
with 16 MB embedded `AP_1v8` PSRAM and the WROOM-2 N32R16V documentation, the
reviewed live evidence is consistent with a 1.8 V memory arrangement. Exact
module SKU and the physical rail remain unverified by software-only evidence.

No new device query was needed or performed. The voltage blocker is cleared;
task 7.3 remains open and may resume only as a separate exact write-packet
review. This conclusion does not authorize a Flash write. See
`tests/hardware/pcb-v1-vdd-spi-contradiction-resolution-2026-09-20.md`.

## Exact first-write packet review outcome (2026-09-20)

OpenSpec task 7.3 is complete as a host-only packet review. The exact
candidate, `COM7`, ESP32-S3/PCB V1.0 identity, semantic/transport/erase
geometry, 64-byte ROM padding, preserved partition layout, six recovery
assets, esptool v4.12.dev3 source behavior, security boundary, stop rules, and
one visible future command are recorded in
`docs/hardware/pcb-v1-display-first-write-review-packet.md`.

Stock esptool CLI was not accepted by itself because its whole-operation,
block, sync, and reset retry defaults cannot all be set to one publicly. A
candidate-specific fail-closed entry point now requires the complete reviewed
`write_flash` argument vector, fresh external human authorization, synchronized
Git identity, exact candidate/recovery hashes, and the previously audited
process-local one-attempt overrides. Its offline audit and five packet tests
pass without enumerating or opening a serial port.

No write, erase, readback, reset, monitor, startup, observation, rollback,
restore, or eFuse operation occurred. Task 7.4 and every physical observation
or recovery task remain open. Hardware/display status is `UNVERIFIED`, and
device execution is `NOT AUTHORIZED`.
