## Why

The host-only ESP-IDF v5.5.4 `esp32s3` build baseline is complete, but it deliberately established neither a PCB V1.0 board configuration nor authorization to Flash any generated artifact. The next step needs a recovery-first, evidence-gated definition for the smallest custom firmware that can safely test startup and fixed serial logging without touching unverified hardware.

## What Changes

- Establish evidence, recovery-asset, partition-layout, artifact-review, and explicit human-authorization gates for the first PCB V1.0 custom Flash.
- Define a later minimal smoke-test implementation that starts on ESP32-S3 and emits fixed startup and compile-time version logs.
- Permit PSRAM initialization only after its exact PCB V1.0 configuration is supported by reviewed evidence; otherwise build and test with PSRAM initialization disabled.
- Require a board-configuration allowlist and default-deny all unknown peripherals, GPIOs, storage mutation, network, OTA, security configuration, and physical movement.
- Require a separately reviewed and explicitly authorized device-read-only inspection before firmware configuration when offline evidence cannot resolve mandatory Flash/PSRAM inputs.
- Separate host-only implementation/build, artifact inspection, device connection, Flash, serial observation, pass/fail, and rollback into independently reviewed stages.
- Require a separate, exact user authorization for any future Flash operation. A vague instruction such as “continue” or “do the next step” is not authorization.
- Preserve the archived host-build baseline Change and its synchronized main specification unchanged.

## Scope

This Change specifies the later implementation, host-only build, static review, recovery rehearsal, first-Flash authorization, device execution, serial-only observation, and rollback workflow for ESP-VoCat PCB V1.0. The initial smoke-test acceptance surface is limited to ESP32-S3 startup, fixed serial logs, compile-time version information, and optional PSRAM initialization only when the configuration gate is satisfied.

Evidence used by this Change remains explicitly classified:

- `CONFIRMED`: facts directly established by prior recorded device evidence or by this round's host-side verification of immutable backup files and their image structure, within the stated evidence domain.
- `STRONGLY SUPPORTED`: matching source or original-firmware component evidence that does not prove PCB wiring.
- `UNVERIFIED`: current-device state or electrical facts not established by the available evidence and therefore unavailable to implementation.

The 2026-07-26 host-only source audit opens the Firmware Implementation Gate only for a later independent Apply to create the minimal firmware, create a reviewed candidate `sdkconfig.defaults`, configure/build on the host, and perform static/artifact review. It grants no device or Flash authorization.

Apply preflight on 2026-07-25 restored the exact D:/E: cross-volume recovery paths and independently reconfirmed the backup layout and original images. Later authorized read-only evidence exposed the exact VDDSPI fields, and the 2026-07-26 local ESP-IDF v5.5.4 audit mapped Flash, disabled PSRAM, and USB Serial/JTAG to a conservative host-only candidate in `docs/hardware/pcb-v1-esp-idf-configuration-map.md`. The old esptool 3.3 V conflict is resolved as a tool-logic defect, while the eFuse result remains configuration evidence rather than a rail measurement.

Firmware Implementation Gate is **OPEN FOR HOST-ONLY IMPLEMENTATION**. First Flash remains **NO-GO**. App-only compatibility, the actual offset/range, current device/port, and explicit operation authorization remain future gates; no further device access is authorized.

## Non-goals

- Complete desktop-cat behavior, animation, companion-app, asset, reminder, or voice functionality.
- Display, screen-touch, or top-touch initialization in the first serial-only smoke-test revision.
- Motor, microphone, speaker, amplifier, audio codec, source localization, SD, battery-control, or main-power-control work.
- Wi-Fi, Bluetooth, cloud services, OTA, or NVS mutation.
- Secure Boot, Flash Encryption, eFuse writes, security-key changes, Flash-voltage changes, or irreversible configuration.
- Use of unverified GPIOs, generic ESP32-S3 board defaults, or PCB V1.2 definitions.
- Redesign or modification of the original bootloader or partition table.
- Restoration of complete original product functionality as part of this planning round.
- Device connection, serial-port enumeration, serial monitoring, Flash, erase, restore, or firmware build in this planning round.

## Capabilities

### New Capabilities

- `pcb-v1-first-flash-smoke-test`: Defines the evidence-gated, recovery-first, serial-log-only PCB V1.0 smoke-test and its exact first-Flash authorization, observation, stop, and rollback requirements.

### Modified Capabilities

None. In particular, `esp-idf-host-build-baseline` remains unchanged.

## Impact

- **Firmware:** A later Apply may replace the minimal baseline behavior with a PCB V1.0-isolated startup/logging smoke test and reviewed configuration inputs. This proposal itself changes no firmware.
- **Companion application:** No impact.
- **Documentation:** Adds the PCB V1.0 firmware-evidence audit, recovery/readiness report, ESP-IDF configuration map, and this Change's planning artifacts.
- **Assets:** No asset files are read into the repository or changed.
- **Tests:** A later Apply must add host-only configure/build evidence, static safety review, artifact-layout review, and—only after exact authorization—serial observation and rollback evidence.
- **Dependencies:** Reuses the installed ESP-IDF v5.5.4 and OpenSpec 1.6.0; no install or upgrade is allowed.
- **Hardware operations:** The Change models later device connection and a first Flash as gated tasks, but the current proposal authorizes neither. Erase, eFuse, motor, audio, microphone, power-control, and security operations remain prohibited.
