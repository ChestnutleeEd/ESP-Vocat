# ESP-VoCat PCB V1.0 Board Configuration Decision

- Decision date: 2026-07-25
- Target: ESP-VoCat PCB V1.0 only
- Decision scope: host-only evidence review before firmware configuration
- Device access in this decision: none

## 1. Decision Status

**PARTIAL DECISION — FIRMWARE IMPLEMENTATION GATE CLOSED**

The evidence is sufficient to retain `esp32s3` as the compile target, keep PSRAM disabled, and default-deny all peripheral, GPIO, network, storage-mutation, OTA, and security behavior. It is not sufficient to select a tracked PCB V1.0 Flash/PSRAM configuration or prepare a First Flash range.

## 2. Inputs Reviewed

- `PROJECT_CONSTITUTION.md`
- `AGENTS.md`
- `docs/HARDWARE_PROFILE.md`
- `docs/PRODUCT_SPEC_DRAFT.md`
- `docs/DECISION_LOG.md`
- `docs/SYSTEM_ARCHITECTURE_DRAFT.md`
- `docs/hardware/pcb-v1-firmware-evidence-audit.md`
- `docs/hardware/pcb-v1-recovery-and-first-flash-readiness.md`
- `openspec/specs/esp-idf-host-build-baseline/spec.md`
- archived `establish-esp-idf-hello-world-build-baseline` Change
- `tests/build/esp-idf-hello-world-build-baseline.md`
- every artifact in Change `prepare-pcb-v1-first-flash-smoke-test`
- the two exact D: recovery files and their E: copies
- fresh offline partition-table, bootloader, and `ota_0` analysis from one hash-matching D: image
- limited printable-string searches in the effective bootloader and `ota_0` image ranges
- searches of Git-tracked text for traceable raw device-read output

No firmware, generated configuration, device, serial port, NVS content, eFuse, or live hardware state was inspected.

## 3. Flash Capacity Evidence

- Each full recovery file is exactly `33554432` bytes (`0x02000000`) and covers a 32 MiB address range.
- The hardware profile records physical Flash capacity as 32 MiB and classifies it `CONFIRMED`, but the underlying raw device-test transcript is not present in the tracked repository.
- Both original image headers declare `16 MB`.
- A 32 MiB dump proves the captured file range, not the physical chip capacity or the currently safe ESP-IDF Flash-size option.

Current configuration conclusion: physical Flash capacity remains unsupported for a new tracked board configuration.

## 4. Image Header Declarations

The independently parsed original bootloader and `ota_0` headers both declare:

- target: ESP32-S3;
- image format version: 1;
- Flash mode: DIO;
- Flash frequency: 80 MHz;
- Flash size: 16 MB.

These values are `CONFIRMED` image declarations. They are not physical-device measurements and do not resolve the conflict with prior repository records of 32 MiB Octal Flash at 1.8 V.

## 5. Partition Address-Space Usage

The full-image scan found one valid root partition table at `0x00008000`.

| Name | Offset | Size | End |
|---|---:|---:|---:|
| `nvs` | `0x00009000` | `0x00004000` | `0x0000D000` |
| `otadata` | `0x0000D000` | `0x00002000` | `0x0000F000` |
| `phy_init` | `0x0000F000` | `0x00001000` | `0x00010000` |
| `ota_0` | `0x00020000` | `0x003F0000` | `0x00410000` |
| `ota_1` | `0x00410000` | `0x003F0000` | `0x00800000` |
| `assets` | `0x00800000` | `0x00800000` | `0x01000000` |

All entries are within the 32 MiB dump and do not overlap. This confirms the backup-image layout only. It does not authorize a tracked custom partition table, an app offset, or a write.

## 6. Upper 16 MiB Erased-Region Evidence

The region `0x01000000` through `0x02000000` contains zero non-`0xFF` bytes in the verified backup. `ota_1` also contains zero non-`0xFF` bytes.

This is immutable-file evidence about the backup. It does not prove physical Flash electrical configuration, current live contents, or permission to repurpose either region.

## 7. Flash Bus-Mode Evidence

- Original image headers declare DIO.
- The hardware profile records Octal / 8 data lines as prior `CONFIRMED` device evidence, but its underlying raw test output is not tracked.
- The app contains OPI, Octal, QOUT, DOUT, and MSPI strings. Reviewed samples are generic driver messages, symbols, supported-mode names, or error text.
- No `CONFIG_ESPTOOLPY_FLASHMODE` string was found.

Current configuration conclusion: physical Flash bus mode is unresolved. Neither DIO nor Octal is allowed into tracked board configuration in this Apply batch.

## 8. Flash Frequency Evidence

- Both original image headers declare 80 MHz.
- No traceable raw device output or tracked original Kconfig establishes the physical/runtime Flash frequency.
- No `CONFIG_ESPTOOLPY_FLASHFREQ` string was found.

Current configuration conclusion: 80 MHz is a confirmed image declaration only and is not allowed into tracked board configuration.

## 9. Flash Voltage Evidence

- The hardware profile records 1.8 V as prior `CONFIRMED` device evidence.
- The repository does not contain the raw observation or eFuse transcript supporting that classification.
- The app contains one exact printable `1.8V` string, but a compiled string does not establish the active voltage domain.
- Image headers and dump length do not encode proof of physical Flash voltage.

Current configuration conclusion: Flash voltage is unresolved for this Apply batch. No voltage change or voltage-setting configuration is allowed. VDD_SPI and Flash-voltage changes remain prohibited.

## 10. PSRAM Capacity Evidence

- The hardware profile records 16 MiB PSRAM as prior `CONFIRMED` device evidence.
- The tracked repository contains no raw PSRAM-size output.
- The app includes generic PSRAM initialization and diagnostic strings such as a formatted “Found ... MB PSRAM device” message, but no observed value.
- No `CONFIG_SPIRAM` string was found.

Current configuration conclusion: PSRAM capacity is not allowed into tracked configuration.

## 11. PSRAM Mode Evidence

- The app contains `octal_psram`, `SPIRAM`, PSRAM, and MSPI strings.
- These strings show compiled code paths or symbols; they do not prove that an option was enabled, that PSRAM was detected, or that the PCB uses a particular bus mode.
- No raw device-test output, original `sdkconfig`, or equivalent traceable configuration was found.

Current configuration conclusion: PSRAM mode, clock, voltage domain, and ESP-IDF v5.5.4 options are unresolved. The PSRAM gate is closed and PSRAM initialization must remain disabled.

## 12. Conflicts and Ambiguities

1. Original headers declare DIO / 80 MHz / 16 MB, while prior repository records describe 32 MiB Octal Flash at 1.8 V.
2. The prior physical records lack a tracked raw transcript, so they cannot be independently reconciled in this Apply batch.
3. Compiled OPI, Octal, PSRAM, MSPI, and 1.8 V strings are generic capability or diagnostic evidence, not active configuration evidence.
4. The valid backup layout does not prove that an app-only custom image will be compatible with the preserved bootloader and OTA selection state.
5. D: and E: are distinct volume paths. No evidence establishes that they are different physical disks.
6. Current device identity, port, running image, download/reset behavior, eFuse state, Flash identification, and PSRAM detection were not observed.

## 13. Configuration Allowlist

The following values and constraints are the complete allowlist for planning. They do not authorize firmware editing while the implementation gate is closed.

- ESP-IDF compile target: `esp32s3`.
- Product scope label: ESP-VoCat PCB V1.0 only.
- Startup acceptance surface: fixed, privacy-safe serial log text only.
- PSRAM initialization: disabled.
- GPIO references and GPIO APIs: absent.
- Display, touch, audio, microphone, motor, SD, battery, power-control, Wi-Fi, Bluetooth, NVS product use, OTA, and external storage: disabled or absent.
- Secure Boot, Flash Encryption, eFuse mutation, VDD_SPI change, Flash-voltage change, JTAG restriction, USB Serial/JTAG restriction, and download-mode restriction: prohibited.
- Partition behavior: preserve the original backup interpretation; create no tracked custom partition input.

## 14. Configuration Denylist

Until the required evidence is reviewed, do not add:

- physical Flash size, mode, frequency, voltage, manufacturer, or device ID as a tracked board value;
- `DIO`, `QIO`, `OPI`, Octal, 80 MHz, 16 MB, 32 MiB, 1.8 V, or 3.3 V as an inferred physical configuration;
- PSRAM capacity, mode, clock, voltage, pin ownership, or ESP-IDF option;
- `sdkconfig`, `sdkconfig.defaults`, bootloader configuration, custom partition table, or OTA-layout change;
- app offset, target slot, first-write range, or recovery command;
- a real serial port, including historical `COM7`;
- reset/download automation;
- any GPIO, peripheral, network, storage-mutation, security, erase, restore, or physical-movement behavior.

## 15. Firmware-Implementation Gate

**CLOSED**

Firmware implementation, tracked board configuration, configure, and build must not begin until the physical Flash and PSRAM evidence required by this decision is either:

1. resolved by traceable evidence and mapped to exact ESP-IDF v5.5.4 options; or
2. explicitly removed as an implementation dependency through a reviewed design that still establishes a safe boot/Flash configuration.

Disabling PSRAM alone does not resolve the mandatory Flash configuration gate.

## 16. Device-Read-Only Evidence Required

A separately reviewed and explicitly authorized device-read-only inspection should collect only the minimum necessary evidence:

- exact chip family confirmation for the reviewed PCB V1.0 device;
- Flash manufacturer/device identification and detected physical capacity;
- evidence capable of resolving, or explicitly failing to resolve, Flash bus mode and voltage domain;
- PSRAM detected capacity and mode, if a safe read-only source is available;
- read-only eFuse summary fields necessary for voltage-domain, security-state, USB Serial/JTAG, and download-mode review;
- the exact current user-reviewed port without assuming `COM7`;
- reset/download behavior needed for later operation review.

Any MAC address or unnecessary unique identifier emitted by a tool must be redacted from tracked evidence. The inspection plan is in `docs/hardware/pcb-v1-device-readonly-inspection-plan.md`.

## 17. Current Go/No-Go Decision

- Firmware Implementation: **NO-GO / GATE CLOSED**
- First Flash: **NO-GO**
- Device access authorization: **NOT GRANTED**

The only next decision is whether the user explicitly authorizes a separately reviewed, minimum device-read-only inspection packet. Vague continuation language does not authorize device access.

## Decision Table

| Parameter | Candidate value | Evidence source | Evidence classification | Safe to use in tracked configuration | Reason | Additional verification required |
|---|---|---|---|---|---|---|
| IDF target | `esp32s3` | Both original images; prior hardware profile | `CONFIRMED` for image target; prior-recorded `CONFIRMED` for device | Yes, compile target only | Independent images agree and no competing MCU target exists | Reconfirm exact device before any device operation |
| Physical Flash capacity | 32 MiB | Dump length; hardware profile | `CONFIRMED` file range; prior-recorded `CONFIRMED` physical claim without raw transcript | No | File range is not chip-capacity proof | Traceable Flash identification/capacity read |
| Image-declared Flash size | 16 MB | Bootloader and app headers | `CONFIRMED` image declaration | No | Declaration conflicts with prior physical record and is not a measurement | Reconcile with physical Flash evidence and IDF configuration |
| Flash mode | DIO header / Octal prior record | Image headers; hardware profile; generic app strings | `CONFIRMED` declaration / prior-recorded `CONFIRMED` claim / strings only `STRONGLY SUPPORTED` | No | Evidence domains conflict | Minimum device-read evidence plus exact IDF v5.5.4 mapping |
| Flash frequency | 80 MHz | Bootloader and app headers | `CONFIRMED` image declaration | No | No traceable runtime/physical configuration record | Device/config evidence and IDF mapping |
| Flash voltage | 1.8 V prior record | Hardware profile; generic app string | Prior-recorded `CONFIRMED`; app string not configuration evidence | No | Raw supporting record absent; image cannot prove voltage | Read-only voltage-domain/eFuse evidence and reviewed interpretation; never change voltage |
| PSRAM capacity | 16 MiB prior record | Hardware profile; generic formatted app log string | Prior-recorded `CONFIRMED`; string only `STRONGLY SUPPORTED` | No | No observed value or raw transcript | Traceable PSRAM detection output |
| PSRAM mode | Octal candidate | Generic `octal_psram` and MSPI strings | `UNVERIFIED` active configuration | No | Compiled paths do not prove detected hardware | Traceable mode/clock/voltage evidence and IDF mapping |
| Partition table offset | `0x00008000` | Full-dump scan, MD5/terminator/bounds checks, ESP-IDF parser | `CONFIRMED` for backup image | No, except as recovery interpretation | Not a new-board configuration or write authorization | Artifact compatibility review before any write |
| App offset | `0x00020000` for original `ota_0` | Validated backup partition table | `CONFIRMED` for backup image | No | Original offset is not an automatic custom-image target | Custom artifact, bootloader, OTA, and slot review |
| First Flash write range | Unresolved | No custom image exists | `UNVERIFIED` | No | No image, hash, length, slot, or compatibility decision | Implement/build only after gate opens, then inspect exact artifact |
| UART port | `<REVIEWED_PORT>` | No device access; `COM7` is historical only | `UNVERIFIED` current state | No | Ports are not stable identities | Separate explicit device-read-only authorization and review |
| Reset/download method | Unresolved | No current device interaction record | `UNVERIFIED` | No | Method and side effects were not observed | Separate reviewed `REBOOT` plan if a reset/download transition is necessary |
