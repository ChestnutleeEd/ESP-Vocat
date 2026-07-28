## Why

The existing PCB V1.0 minimal smoke-test firmware can start and has passed its separately authorized bounded startup observation, but it intentionally never initializes the display. A black screen is therefore expected with that firmware and proves neither display health nor display failure; physical screen health remains `UNVERIFIED`.

The next safe increment is a PCB V1.0-only ST77916 QSPI test that can later draw a deterministic pattern without importing the complete board-support package, LVGL, touch, audio, storage, networking, or other peripherals. This Change freezes the public and repository evidence, locks the driver dependency and initialization-table ownership, defines a hard-disabled backlight policy, and makes the future host-only acceptance and artifact audits explicit. That evidence is sufficient for a host-only implementation that keeps GPIO44 low on every reachable path, but it is not sufficient to approve any non-zero backlight output or visual device execution.

The historical First Flash result remains `STOPPED / INCONCLUSIVE — RANGE-SCOPE DEVIATION`. Its later startup observation remains `PASS`, and runtime validation remains `PASS FOR THIS MINIMAL SMOKE TEST`. None of those results authorizes a display firmware write or validates the screen.

## What Changes

- Establish a `STRONGLY SUPPORTED — REQUIRES FUTURE DEVICE VALIDATION` PCB V1.0 display baseline for ST77916, 360 × 360 RGB565 over QSPI on `SPI2_HOST` at no more than 40 MHz.
- Establish the exact display GPIO allowlist `{18, 14, 46, 13, 11, 12, 3, 44}`, with `GPIO_NUM_NC` for DC, and default-deny every other GPIO.
- Establish an explicit PCB V1.2 and unrelated-feature denylist, including GPIO47 reset, GPIO9 LCD power/LCD_EN, GPIO45 DC, 80 MHz, complete BSP reuse, board auto-detection, touch, LVGL, audio, NVS brightness restore, and board-level peripheral initialization.
- Lock the future component dependency to `espressif/esp_lcd_st77916` version `==1.0.1` and require integrity evidence for the resolved source.
- Lock the vendor initialization sequence to the 365-entry table in historical Xiaozhi commit `49ac8a6da399f27a9546d4f73640b7f86c24bac6`, with an extraction/hash audit and no silent edits.
- Define a deterministic eleven-state initialization machine with `BACKLIGHT_POLICY_GATE`, one failure marker, no retry/reset loop, stable `FAIL_SAFE`, and GPIO44 held low on every normal and error path.
- Define one 57,600-byte DMA-capable internal-memory strip buffer, a deterministic RGB test pattern, minimal bitmap text, and no full framebuffer, PSRAM, LVGL, filesystem, image asset, or network.
- Define later, still-unchecked firmware implementation, host configure/build, static audit, artifact review, and successor handoff tasks.
- Record `RESOLVED FOR HOST IMPLEMENTATION — GPIO44 SHALL remain hard-disabled; non-zero backlight output and visual device execution remain NOT AUTHORIZED pending a separately reviewed successor Change.`
- Prohibit LEDC, PWM frequency/resolution, brightness percentages, non-zero duty, fade, NVS brightness restore, dynamic brightness APIs, and hidden compile-time or runtime enable paths from the current implementation.

## Scope

This Change covers evidence freeze, dependency and initialization-source decisions, safety design, a later restricted host-side firmware implementation, a later host-only configure/build, static and artifact review, and preparation of evidence for a separate successor Change.

Planning-round operation classification:

`WRITE — GIT BRANCH AND OPENSPEC DOCUMENTATION ONLY`

Device access, startup/observation, Flash, readback, rollback, and restore authorization are all `NONE`. This classification permits no device or firmware operation.

Every prior First Flash, startup, observation, rollback, and recovery authorization remains consumed/closed or `NONE` and cannot be reused. Any future device action requires a separately reviewed successor and fresh exact authorization.

The later firmware scope is PCB V1.0-only: configure GPIO44 as output low before any QSPI or panel call and retain that level for the complete run, initialize ST77916 QSPI, emit fixed serial stage/policy markers, transfer black/red/green/blue regions with a white border and fixed text `ESP-VoCat LCD TEST`, and enter stable READY/FAIL behavior. The current implementation must not configure LEDC or contain any reachable non-zero backlight path. Suggested future files are:

- `firmware/main/pcb_v1_display.h`
- `firmware/main/pcb_v1_display.c`
- `firmware/main/pcb_v1_display_test_pattern.h`
- `firmware/main/pcb_v1_display_test_pattern.c`

These files are design candidates only and are not created by this proposal round.

## Non-goals

- Port enumeration, device identity queries, reset, monitor, serial access, Flash, readback, erase, eFuse access, rollback, restore, or any authorization package.
- Physical screen illumination, screen-health confirmation, color/orientation/refresh validation, or runtime DMA validation.
- PCB V1.2 support, PCB revision auto-detection, complete V1.0 or V1.2 BSP reuse, or generic ESP32-S3 board configuration.
- GPIO9 power control, LCD_EN, GPIO45 DC, GPIO47 reset, an 80 MHz LCD clock, or any GPIO outside the exact allowlist.
- Touch, LVGL, PSRAM, Wi-Fi, Bluetooth, microphone, speaker, amplifier, audio, motor, SD, images, animation, filesystem, network, product UI, dynamic brightness, or NVS brightness restore.
- LEDC, PWM backlight control, a non-zero duty/percentage, PWM frequency or resolution, fade, a brightness service, or any compile-time/runtime backlight-enable gate.
- Bootloader, partition-table, security, Flash-voltage, recovery-asset, or recovery-policy changes.
- Reuse of any prior First Flash, startup, observation, rollback, or recovery authorization.
- Dependency installation, download, activation, resolution, lockfile mutation, firmware build, generated build artifacts, commit, push, or Change archive in this round.

## Success criteria

This Change is complete only when:

- proposal, design, capability specification, and tasks are mutually consistent and pass strict OpenSpec and Markdown validation;
- the exact dependency and initialization-table source are locked with auditable evidence;
- GPIO allowlist, V1.2 denylist, state machine, strip-buffer design, fail-safe behavior, and host/artifact acceptance are completely specified;
- the hard-disabled decision is recorded consistently: host implementation is permitted, while non-zero backlight output, visual validation, and device execution remain explicitly blocked;
- later firmware implementation, host configure/build, static audits, artifact review, and successor evidence work remain unchecked until actually performed.

Future completion of this Change may include firmware implementation, host configure/build, static validation, and artifact review, and may prove only source/configuration conformance, successful host compilation/linking, and artifact readiness. Its READY result must record `visual=UNVERIFIED` and `backlight=DISABLED_NOT_AUTHORIZED`. It must not claim that the screen lit, the backlight operated, the screen is healthy, colors/orientation are correct, refresh/DMA is stable on the device, or recovery succeeded.

Future host validation must verify GPIO44-low-before-QSPI order, GPIO44-low invariance after every injected state failure, the exact `TEST_PATTERN_DRAW` -> `BACKLIGHT_POLICY_GATE` -> `READY` sequence, exact policy/READY markers, and absence of `ledc_*`, brightness/fade/NVS-display behavior, GPIO44-high output, non-zero duty, wrapper/alias bypass, `BACKLIGHT_LOW_ENABLE`, and compile-time/runtime enable gates. Artifact review must record `backlight=hard-disabled`, `visual=UNVERIFIED`, and `not-for-visual-validation`.

## Capabilities

### New Capabilities

- `pcb-v1-minimal-display-smoke-test`: Defines the evidence-locked, fail-closed, host-prepared PCB V1.0 ST77916 QSPI display smoke test and the boundary to a separately authorized device-execution Change.

### Modified Capabilities

None. The active `pcb-v1-first-flash-smoke-test` Change and the synchronized `esp-idf-host-build-baseline` specification remain unchanged.

## Impact

- **Firmware:** A later Apply may add only the minimal PCB V1.0 display/test-pattern modules and minimal `main.c` integration under the hard-disabled policy. GPIO44 must remain output-low; LEDC and every non-zero path remain prohibited. This proposal round changes no firmware.
- **Dependencies:** A later Apply must add the exact component version `espressif/esp_lcd_st77916: "==1.0.1"` and verify the resolved source; this round changes no manifest or lockfile.
- **Build/configuration:** A later Apply may configure and build with the existing ESP-IDF v5.5.4 only after source changes are reviewed; no configure/build runs in this round.
- **Documentation/tests:** Adds only this OpenSpec Change now. Later work must record host configure/build, static safety, dependency, GPIO, binary, and artifact evidence.
- **Hardware:** No device operation is included or authorized. Physical display outcomes stay `UNVERIFIED`.
- **Recovery:** DEC-023, Level 1 original `ota_0` restore, Level 2 32 MiB full recovery, restoration rehearsal, and independent Recovery verification remain unchanged and cannot be waived by a host build or a future lit screen.
