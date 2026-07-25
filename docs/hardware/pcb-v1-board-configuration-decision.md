# ESP-VoCat PCB V1.0 Board Configuration Decision

- Decision date: 2026-07-25
- Target: ESP-VoCat PCB V1.0 only
- Decision scope: host-side audit of offline evidence plus the two separately authorized read-only device queries
- Device access in this decision: the completed `chip_id` and `flash_id` queries are recorded; no device was accessed while auditing or updating this decision

## 1. Decision Status

**PARTIAL DECISION — FIRMWARE IMPLEMENTATION GATE CLOSED**

The evidence is sufficient to retain `esp32s3` as the compile target and to confirm, in the exact query context, a 32 MB JEDEC capacity code, the ESP32-S3 Octal Flash-type eFuse selector, and embedded 16 MB PSRAM package fields. It remains insufficient to select a complete tracked PCB V1.0 Flash/PSRAM configuration because the installed esptool voltage interpretation conflicts with the prior 1.8 V record and PSRAM mode/clock remain unresolved. PSRAM stays disabled and every peripheral, GPIO, network, storage-mutation, OTA, and security behavior remains default-denied.

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
- the sanitized, previously completed `chip_id` and `flash_id` outputs in `docs/hardware/pcb-v1-device-readonly-inspection-result.md`
- the installed esptool.py `v4.12.dev3` source and local help

No firmware, generated configuration, NVS content, eFuse summary, or private device content was inspected. The prior authorized queries accessed `COM7`, entered the ROM-loader flow without a RAM stub, read identification/eFuse-derived fields, and hard-reset the device. No device was accessed again for this decision update.

## 3. Flash Capacity Evidence

- Each full recovery file is exactly `33554432` bytes (`0x02000000`) and covers a 32 MiB address range.
- The completed `flash_id` query returned manufacturer `0xC2`, device `0x8039`, and `Detected flash size: 32MB`; the installed source maps JEDEC capacity byte `0x39` to `32MB`.
- Both original image headers declare `16 MB`.
- A 32 MiB dump proves the captured file range, not the physical chip capacity or the currently safe ESP-IDF Flash-size option.

Current configuration conclusion: 32 MB is `CONFIRMED` current-device Flash-identification evidence for this exact query context and may be recorded as physical capacity. It does not determine a safe image-header declaration, partition change, or write range.

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
- The completed query printed `Flash type set in eFuse: octal (8 data lines)`.
- In installed source, `ESP32S3ROM.flash_type()` reads bit 9 of `EFUSE_RD_REPEAT_DATA3_REG`; the command maps set to Octal and clear to Quad.
- The app contains OPI, Octal, QOUT, DOUT, and MSPI strings. Reviewed samples are generic driver messages, symbols, supported-mode names, or error text.
- No `CONFIG_ESPTOOLPY_FLASHMODE` string was found.

Current configuration conclusion: the Octal eFuse selector is `CONFIRMED` for the query. It describes a target eFuse/package-interface field, while DIO describes the original image header; the two are different layers. Exact ESP-IDF v5.5.4 mapping remains unresolved, so no Flash-mode value is yet allowed into tracked board configuration.

## 8. Flash Frequency Evidence

- Both original image headers declare 80 MHz.
- No traceable raw device output or tracked original Kconfig establishes the physical/runtime Flash frequency.
- No `CONFIG_ESPTOOLPY_FLASHFREQ` string was found.

Current configuration conclusion: 80 MHz is a confirmed image declaration only and is not allowed into tracked board configuration.

## 9. Flash Voltage Evidence

- The hardware profile records 1.8 V as prior `CONFIRMED` device evidence.
- The repository does not contain the raw observation or eFuse transcript supporting that classification.
- The completed query printed `Flash voltage set by eFuse to 3.3V`.
- For ESP32-S3, installed esptool inherits `ESP32ROM._get_efuse_flash_voltage()`. Its first condition returns `3.3V` when any of `VDD_SPI_FORCE`, `VDD_SPI_XPD`, or `VDD_SPI_TIEH` is nonzero. Because the later 1.8 V and OFF masks are subsets, those eFuse branches are unreachable in this installed implementation.
- The output label `eFuse` proves the method did not use its strapping fallback, but the output does not reveal the individual bit pattern.
- The app contains one exact printable `1.8V` string, but a compiled string does not establish the active voltage domain.
- Image headers and dump length do not encode proof of physical Flash voltage.

Current configuration conclusion: the 3.3 V text is `CONFIRMED` tool output but the physical Flash voltage remains conflicted and `UNVERIFIED`. A minimum read-only eFuse summary is justified but remains `NOT AUTHORIZED`. No voltage change or voltage-setting configuration is allowed. VDD_SPI and Flash-voltage changes remain prohibited.

## 10. PSRAM Capacity Evidence

- Both completed queries printed `Embedded PSRAM 16MB (AP_1v8)`.
- Installed `ESP32S3ROM.get_psram_cap()` decodes eFuse package fields and maps capacity code 3 to embedded 16 MB PSRAM.
- Installed `get_psram_vendor()` maps vendor/variant code 2 to `AP_1v8`.
- The app includes generic PSRAM initialization and diagnostic strings such as a formatted “Found ... MB PSRAM device” message, but no observed value.
- No `CONFIG_SPIRAM` string was found.

Current configuration conclusion: embedded 16 MB PSRAM is `CONFIRMED` package/eFuse evidence for this exact device query. `AP_1v8` is the installed tool's PSRAM vendor/variant label and does not prove external Flash voltage or a unified PCB rail. Capacity may be recorded, but PSRAM initialization remains disallowed.

## 11. PSRAM Mode Evidence

- The app contains `octal_psram`, `SPIRAM`, PSRAM, and MSPI strings.
- These strings show compiled code paths or symbols; they do not prove that an option was enabled, that PSRAM was detected, or that the PCB uses a particular bus mode.
- No PSRAM bus-mode/clock output, original `sdkconfig`, or equivalent traceable active configuration was found.

Current configuration conclusion: PSRAM mode, clock, voltage domain, and ESP-IDF v5.5.4 options are unresolved. The PSRAM gate is closed and PSRAM initialization must remain disabled.

## 12. Conflicts and Ambiguities

1. Original headers declare DIO / 80 MHz / 16 MB, while current device evidence reports a 32 MB JEDEC capacity code and an Octal eFuse selector. These describe image declarations versus device/package identification and can coexist, but their exact ESP-IDF mapping remains unresolved.
2. Installed esptool prints 3.3 V from its eFuse helper, while prior repository records say 1.8 V and the PSRAM feature label is `AP_1v8`. The helper's condition order does not expose or uniquely decode the raw eFuse bit pattern.
3. Compiled OPI, Octal, PSRAM, MSPI, and 1.8 V strings are generic capability or diagnostic evidence, not active configuration evidence.
4. The valid backup layout does not prove that an app-only custom image will be compatible with the preserved bootloader and OTA selection state.
5. D: and E: are distinct volume paths. No evidence establishes that they are different physical disks.
6. Device identity, query-time `COM7`, USB-Serial/JTAG mode, reset behavior, Flash RDID/capacity, Flash-type eFuse selector, and embedded-PSRAM package fields were observed. Current running image, exact voltage-bit pattern, PSRAM runtime mode/clock, and future port identity were not.

## 13. Configuration Allowlist

The following values and constraints are the complete allowlist for planning. They do not authorize firmware editing while the implementation gate is closed.

- ESP-IDF compile target: `esp32s3`.
- Physical Flash capacity evidence: 32 MB for the exact queried device; this adds no partition or write-range permission.
- Embedded PSRAM capacity evidence: 16 MB package/eFuse field; initialization remains disabled.
- Product scope label: ESP-VoCat PCB V1.0 only.
- Startup acceptance surface: fixed, privacy-safe serial log text only.
- PSRAM initialization: disabled.
- GPIO references and GPIO APIs: absent.
- Display, touch, audio, microphone, motor, SD, battery, power-control, Wi-Fi, Bluetooth, NVS product use, OTA, and external storage: disabled or absent.
- Secure Boot, Flash Encryption, eFuse mutation, VDD_SPI change, Flash-voltage change, JTAG restriction, USB Serial/JTAG restriction, and download-mode restriction: prohibited.
- Partition behavior: preserve the original backup interpretation; create no tracked custom partition input.

## 14. Configuration Denylist

Until the required evidence is reviewed, do not add:

- Flash mode, frequency, voltage, manufacturer, or device ID as an active tracked board option;
- `DIO`, `QIO`, `OPI`, Octal, 80 MHz, 16 MB, 1.8 V, or 3.3 V as an inferred complete physical configuration;
- PSRAM mode, clock, voltage, pin ownership, or ESP-IDF option;
- `sdkconfig`, `sdkconfig.defaults`, bootloader configuration, custom partition table, or OTA-layout change;
- app offset, target slot, first-write range, or recovery command;
- `COM7` as a permanent or future serial-port assumption;
- reset/download automation;
- any GPIO, peripheral, network, storage-mutation, security, erase, restore, or physical-movement behavior.

## 15. Firmware-Implementation Gate

**CLOSED**

Firmware implementation, tracked board configuration, configure, and build must not begin until the physical Flash and PSRAM evidence required by this decision is either:

1. resolved by traceable evidence and mapped to exact ESP-IDF v5.5.4 options; or
2. explicitly removed as an implementation dependency through a reviewed design that still establishes a safe boot/Flash configuration.

Disabling PSRAM alone does not resolve the mandatory Flash configuration gate.

## 16. Device-Read-Only Evidence Required

A separately authorized read-only inspection has now confirmed the chip family, query-time port, Flash manufacturer/device/capacity, Flash-type eFuse selector, USB mode, reset behavior, and embedded PSRAM capacity/vendor label. The remaining minimum device evidence is a sanitized read-only eFuse summary exposing the individual VDDSPI voltage fields needed to audit the installed tool's ambiguous 3.3 V interpretation and the required security/download state fields.

That eFuse summary remains **NOT AUTHORIZED**. Any MAC address or unnecessary unique identifier emitted by a tool must be redacted from tracked evidence. The completed result is in `docs/hardware/pcb-v1-device-readonly-inspection-result.md`; the authorization boundary remains defined by `docs/hardware/pcb-v1-device-readonly-inspection-plan.md`.

## 17. Current Go/No-Go Decision

- Firmware Implementation: **NO-GO / GATE CLOSED**
- First Flash: **NO-GO**
- Completed `chip_id`/`flash_id` authorization: **CONSUMED AND CLOSED**
- Further device access authorization: **NOT GRANTED**

The only next device decision is whether the user explicitly authorizes a separately reviewed, sanitized, minimum read-only eFuse-summary packet. Vague continuation language does not authorize device access.

## Decision Table

| Parameter | Candidate value | Evidence source | Evidence classification | Safe to use in tracked configuration | Reason | Additional verification required |
|---|---|---|---|---|---|---|
| IDF target | `esp32s3` | Both original images; prior hardware profile | `CONFIRMED` for image target; prior-recorded `CONFIRMED` for device | Yes, compile target only | Independent images agree and no competing MCU target exists | Reconfirm exact device before any device operation |
| Physical Flash capacity | 32 MB | Current JEDEC RDID `0xC2/0x8039`; esptool capacity map; dump length | `CONFIRMED` current-device identification evidence; independent `CONFIRMED` file range | Yes, capacity record only | Capacity byte `0x39` maps to 32 MB and agrees with dump span | Exact build/header/partition mapping still required |
| Image-declared Flash size | 16 MB | Bootloader and app headers | `CONFIRMED` image declaration | No | Declaration conflicts with prior physical record and is not a measurement | Reconcile with physical Flash evidence and IDF configuration |
| Flash mode | DIO image header / Octal eFuse selector | Image headers; current `flash_id`; installed ESP32-S3 source | Both `CONFIRMED` in different evidence domains | No | Image transfer declaration and eFuse/package interface are different layers | Exact ESP-IDF v5.5.4 mapping and voltage resolution |
| Flash frequency | 80 MHz | Bootloader and app headers | `CONFIRMED` image declaration | No | No traceable runtime/physical configuration record | Device/config evidence and IDF mapping |
| Flash voltage | 3.3 V installed-tool output / 1.8 V prior record | Current `flash_id`; installed voltage helper; hardware profile | Output `CONFIRMED`; physical conclusion `UNVERIFIED` and conflicted | No | Installed helper returns 3.3 V for any nonzero masked eFuse voltage bit and hides the bit pattern | Separately authorized sanitized eFuse summary plus authoritative electrical reconciliation; never change voltage |
| PSRAM capacity | Embedded 16 MB | Current chip features; installed ESP32-S3 package-field decoder | `CONFIRMED` eFuse/package evidence | Yes, capacity record only; initialization remains off | Capacity code 3 maps directly to embedded 16 MB | Exact mode/clock/voltage/options before enablement |
| PSRAM mode | Unresolved; `AP_1v8` is a vendor/variant label | Current chip features; installed source; generic app strings | Label `CONFIRMED`; active bus mode `UNVERIFIED` | No | Neither the label nor compiled strings report active bus mode | Traceable mode/clock evidence and exact IDF mapping |
| Partition table offset | `0x00008000` | Full-dump scan, MD5/terminator/bounds checks, ESP-IDF parser | `CONFIRMED` for backup image | No, except as recovery interpretation | Not a new-board configuration or write authorization | Artifact compatibility review before any write |
| App offset | `0x00020000` for original `ota_0` | Validated backup partition table | `CONFIRMED` for backup image | No | Original offset is not an automatic custom-image target | Custom artifact, bootloader, OTA, and slot review |
| First Flash write range | Unresolved | No custom image exists | `UNVERIFIED` | No | No image, hash, length, slot, or compatibility decision | Implement/build only after gate opens, then inspect exact artifact |
| UART port | `COM7` for the completed queries | User-reviewed port and successful current query | `CONFIRMED` for the recorded event only | No future assumption | Ports are not stable identities | Re-review before any future operation |
| Reset/download method | ROM-loader connection; final RTS hard reset | Direct output and installed source | `CONFIRMED` for the recorded event | No | Connection also made transient register/SPI changes; it grants no future reset permission | Separately review any future connection/reset behavior |
