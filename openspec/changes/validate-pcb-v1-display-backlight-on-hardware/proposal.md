## Why

The completed host-side display Change at baseline commit `a0b7c726d90448af0f811b7f6c996897e2a16c8a` reproducibly builds and exercises the PCB V1.0 ST77916 path, but its artifact intentionally holds GPIO44 low and cannot validate illumination or pixels on the physical display. A separately reviewable successor is required to introduce the smallest controlled non-zero backlight path, preserve recovery and evidence boundaries, and define every gate that must close before a future physical write.

The PCB V1.0 schematic evidence strongly supports an active-high GPIO44 gate driving an AO3400A N-channel MOSFET low-side switch for LEDK, with a 10 kΩ gate pull-down and 20 Ω series resistor. This is `CONFIRMED` document content and `STRONGLY SUPPORTED` hardware mapping, not physical confirmation on the current unit. Physical illumination, display health, color, orientation, refresh, PWM behavior, and reset-time transients remain `UNVERIFIED`.

## What Changes

- Define GPIO44 as an active-high, MOSFET-driven backlight gate for this PCB V1.0-only validation candidate, while preserving the evidence classification above.
- Select a conservative first-test LEDC contract: low-speed mode, 2,000 Hz, 10-bit resolution, raw duty `10` of `1023` (approximately `0.98%`), no fade, no stored brightness, and no runtime adjustment.
- Require GPIO44 to be preloaded and configured low before LEDC ownership, remain at duty zero throughout panel initialization and test-pattern transfer, and reach the single approved non-zero duty only after all prior display states pass.
- Require fail-safe duty zero and GPIO-low shutdown on every error and cleanup path, with no retry, fallback duty, automatic reset, or full-brightness transient.
- Extend host tests, fakes, the native fault matrix, static scans, build review, and reproducibility evidence for the new LEDC state and every failure boundary before any device write can be reviewed.
- Require a fresh, reproducible visual-validation artifact with a new canonical hash, exact GPIO44/PWM metadata, independent semantic/transport/erase ranges, and explicit separation from the predecessor hard-disabled hash `4E66B7EDCD38B5225B00E1DB790CA7DD9C842E765961E8929228105F9C10A05D`.
- Define fresh device-identity, current-port, recovery, backup, exact-command, authorization, one-attempt write, separate observation, visual acceptance, and stop/rollback gates.
- Preserve the historical First Flash result `STOPPED / INCONCLUSIVE — RANGE-SCOPE DEVIATION`, startup observation `PASS`, and runtime result `PASS FOR THIS MINIMAL SMOKE TEST`.
- Keep physical display status `UNVERIFIED` until a separately authorized bounded hardware observation supplies evidence.

## Scope

This Change covers repository evidence, a future minimal host-side implementation of the exact non-zero backlight contract, host tests and static audits, two independent clean builds, artifact/range/recovery review, and the planning gates for one future controlled App-only write followed by a separate bounded startup/display observation.

The operation performed while creating these artifacts is:

`WRITE — REPOSITORY OPENSPEC DOCUMENTATION ONLY`

No device operation is authorized by this proposal. Future operations remain individually classified and gated: identity/port inspection is `READ-ONLY` with separately reviewed connection/reset effects; startup is `REBOOT`; App programming is `WRITE`; observation is `READ-ONLY`; recovery is a distinct future `WRITE`. No `ERASE` or `IRREVERSIBLE` operation is in scope.

## Non-goals

- No device enumeration, serial/COM access, reset, monitor, esptool execution, Flash write, readback, erase, rollback, or restore during this planning task.
- No executable Flash or recovery command and no authorization package in the planning artifacts.
- No 100% backlight default, product brightness service, fade, NVS brightness restore, dynamic duty, companion control, or user setting.
- No change to ST77916 geometry, QSPI mapping, initialization table, test pattern, touch, LVGL, PSRAM, audio, microphone, motor, SD, network, storage, power control, partition layout, bootloader, security, eFuses, or Flash voltage.
- No PCB V1.2 support, board auto-detection, complete BSP reuse, GPIO9 LCD power, GPIO45 DC, GPIO47 reset, or 80 MHz LCD clock.
- No claim that host tests or a successful build prove physical safety, display health, illumination, color, orientation, refresh stability, or recovery.
- No archival of the predecessor or this Change, and no weakening of DEC-023 or the independent recovery-verification milestone.

## Capabilities

### New Capabilities

- `pcb-v1-display-backlight-hardware-validation`: Defines the exact low-duty active-high GPIO44 contract, host/artifact prerequisites, separately authorized device gate, bounded visual acceptance, and recovery boundaries for the first PCB V1.0 display illumination test.

### Modified Capabilities

None. The predecessor `pcb-v1-minimal-display-smoke-test` remains a completed hard-disabled host artifact and is not retroactively changed into a visual-validation candidate.

## Impact

- **Firmware:** A later Apply may minimally revise `pcb_v1_display.h/.c`, `main.c`, and component requirements to add one fixed LEDC backlight state. The pattern generator and unrelated firmware remain unchanged unless a task-specific reason is documented.
- **Host tests:** A later Apply must update the fake API surface, native harness, Python source/static checks, full fault matrix, and marker/state assertions.
- **Dependencies/build:** ESP-IDF v5.5.4 remains fixed. The existing exact ST77916 v1.0.1 dependency remains unchanged; LEDC is added only through the ESP-IDF driver dependency required by the implementation.
- **Artifact evidence:** A later Apply must create a new manifest and review for the visual candidate and must not overwrite or relabel the predecessor evidence.
- **Recovery:** The four immutable 32 MiB images and two Level 1 `ota_0` staging copies were rehashed read-only during this planning task and matched their recorded sizes/hashes. They remain immutable and outside build output.
- **Companion application/assets:** No impact.
- **Hardware:** Physical behavior remains `UNVERIFIED`; creating this Change grants no device or Flash authorization.
