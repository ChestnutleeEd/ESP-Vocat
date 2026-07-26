# ESP-VoCat PCB V1.0 Board Configuration Decision

- Decision date: 2026-07-26
- Target: ESP-VoCat PCB V1.0 only
- Decision scope: host-side audit of offline evidence, completed authorized read-only device evidence, and local ESP-IDF v5.5.4 source
- Device access in this decision: completed `chip_id`, `flash_id`, and minimum eFuse-summary results are recorded; no device was accessed during this ESP-IDF mapping audit or update

## 1. Decision Status

**CONFIGURATION DECISION COMPLETE — FIRMWARE IMPLEMENTATION GATE OPEN FOR HOST-ONLY IMPLEMENTATION**

The evidence is sufficient to retain `esp32s3` as the compile target and form an exact conservative host-only candidate: explicit Octal Flash with STR sampling, 80 MHz, a 16 MB image-address-space limit, USB Serial/JTAG console, and PSRAM disabled. The individual VDDSPI eFuse fields resolve the installed esptool 3.3 V interpretation as a tool-logic defect and support a forced, enabled 1.8 V LDO configuration; this remains configuration evidence rather than a PCB voltage measurement. Every peripheral, GPIO, network, storage-mutation, OTA, and security behavior remains default-denied.

The complete source mapping and its limitations are in `docs/hardware/pcb-v1-esp-idf-configuration-map.md`. First Flash remains **NO-GO**, with no Flash authorization and no further device-access authorization.

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
- the sanitized, completed minimum read-only eFuse summary in the same result document
- local clean ESP-IDF v5.5.4 source at `D:\esp\v5.5.4\esp-idf`
- the installed esptool.py `v4.12.dev3` source and local help

No firmware, generated configuration, NVS content, or private device content was inspected. The prior authorized queries accessed the then-reviewed port, entered the ROM-loader flow without a RAM stub, read only approved identification/eFuse fields, and hard-reset the device. No device was accessed again for the ESP-IDF mapping audit or this decision update.

## 3. Flash Capacity Evidence

- Each full recovery file is exactly `33554432` bytes (`0x02000000`) and covers a 32 MiB address range.
- The completed `flash_id` query returned manufacturer `0xC2`, device `0x8039`, and `Detected flash size: 32MB`; the installed source maps JEDEC capacity byte `0x39` to `32MB`.
- Both original image headers declare `16 MB`.
- A 32 MiB dump proves the captured file range, not the physical chip capacity or the currently safe ESP-IDF Flash-size option.

Current configuration conclusion: 32 MB is `CONFIRMED` current-device Flash-identification evidence. ESP-IDF maps it to `CONFIG_ESPTOOLPY_FLASHSIZE_32MB`, but the conservative first host-only candidate keeps `CONFIG_ESPTOOLPY_FLASHSIZE_16MB` because runtime accepts a smaller header on a larger chip and the entire preserved layout ends at 16 MB. Neither choice authorizes a partition change or write range.

## 4. Image Header Declarations

The independently parsed original bootloader and `ota_0` headers both declare:

- target: ESP32-S3;
- image format version: 1;
- Flash mode: DIO;
- Flash frequency: 80 MHz;
- Flash size: 16 MB.

These values are `CONFIRMED` image declarations, not physical-device measurements. ESP-IDF source now explains them as a separate layer from the 32 MB/eight-line/VDDSPI eFuse facts.

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

Current configuration conclusion: the Octal eFuse selector is `CONFIRMED` and maps to `CONFIG_ESPTOOLPY_OCT_FLASH=y`; STR is the conservative sample choice because DTR support is not proven. DIO describes the original image header, while eFuse and later MSPI startup select the physical/runtime Octal path. The explicit-Octal v5.5.4 candidate will encode DOUT in its image header because OPI has no image-header encoding and ROM obtains Octal handling from eFuse.

## 8. Flash Frequency Evidence

- Both original image headers declare 80 MHz.
- No traceable raw device output or tracked original Kconfig establishes the physical/runtime Flash frequency.
- No `CONFIG_ESPTOOLPY_FLASHFREQ` string was found.

Current configuration conclusion: 80 MHz maps to `CONFIG_ESPTOOLPY_FLASHFREQ_80M=y` and is allowed for the host-only candidate because both original headers declare it and it is the ESP32-S3 target default. This is not a physical timing validation.

## 9. Flash Voltage Evidence

- The hardware profile records 1.8 V as prior `CONFIRMED` device evidence.
- The repository does not contain the raw observation or eFuse transcript supporting that classification.
- The completed query printed `Flash voltage set by eFuse to 3.3V`.
- For ESP32-S3, installed esptool inherits `ESP32ROM._get_efuse_flash_voltage()`. Its first condition returns `3.3V` when any of `VDD_SPI_FORCE`, `VDD_SPI_XPD`, or `VDD_SPI_TIEH` is nonzero. Because the later 1.8 V and OFF masks are subsets, those eFuse branches are unreachable in this installed implementation.
- The output label `eFuse` proves the method did not use its strapping fallback, but the output does not reveal the individual bit pattern.
- The app contains one exact printable `1.8V` string, but a compiled string does not establish the active voltage domain.
- Image headers and dump length do not encode proof of physical Flash voltage.

The later minimum read-only summary recorded `VDD_SPI_FORCE=1`, `VDD_SPI_XPD=1`, and `VDD_SPI_TIEH=0`. ESP32-S3 definitions map this forced, enabled combination to the 1.8 V LDO. The historical 3.3 V text remains `CONFIRMED` tool output, but its early branch conflicts with the independently decoded fields and is not a final physical-voltage conclusion. The 1.8 V result is eFuse configuration evidence, not a PCB-rail measurement. No voltage change or voltage-setting configuration is allowed; VDD_SPI and Flash-voltage changes remain prohibited.

## 10. PSRAM Capacity Evidence

- Both completed queries printed `Embedded PSRAM 16MB (AP_1v8)`.
- Installed `ESP32S3ROM.get_psram_cap()` decodes eFuse package fields and maps capacity code 3 to embedded 16 MB PSRAM.
- Installed `get_psram_vendor()` maps vendor/variant code 2 to `AP_1v8`.
- The app includes generic PSRAM initialization and diagnostic strings such as a formatted “Found ... MB PSRAM device” message, but no observed value.
- No `CONFIG_SPIRAM` string was found.

Current configuration conclusion: embedded 16 MB PSRAM is `CONFIRMED` package/eFuse evidence. ESP-IDF has no “16 MB Octal” capacity Kconfig; the Octal driver reads runtime density. `AP_1v8` remains a vendor/variant label and does not prove bus mode, clock, routing, runtime success, external Flash voltage, or a unified PCB rail. Capacity may be recorded, but PSRAM initialization remains disabled for the first candidate.

## 11. PSRAM Mode Evidence

- The app contains `octal_psram`, `SPIRAM`, PSRAM, and MSPI strings.
- These strings show compiled code paths or symbols; they do not prove that an option was enabled, that PSRAM was detected, or that the PCB uses a particular bus mode.
- No PSRAM bus-mode/clock output, original `sdkconfig`, or equivalent traceable active configuration was found.

Current configuration conclusion: `CONFIG_SPIRAM`, `CONFIG_SPIRAM_MODE_OCT`, and `CONFIG_SPIRAM_SPEED_*` are mapped, but enabled mode/clock/routing remain unresolved. The serial-only smoke test does not depend on PSRAM, and ESP-IDF explicitly supports Flash operation with PSRAM disabled. `CONFIG_SPIRAM` therefore remains off in the first candidate.

## 12. Conflicts and Ambiguities

1. Original headers declare DIO / 80 MHz / 16 MB, while device evidence reports a 32 MB JEDEC capacity code and an Octal eFuse selector. ESP-IDF source now resolves these as image-address-space/boot declarations versus physical/package/runtime layers.
2. Installed esptool's historical 3.3 V output is explained by its early branch. The independent FORCE/XPD/TIEH fields support a forced 1.8 V LDO configuration but do not constitute a PCB measurement.
3. Compiled OPI, Octal, PSRAM, MSPI, and 1.8 V strings are generic capability or diagnostic evidence, not active configuration evidence.
4. The valid backup layout does not prove that an app-only custom image will be compatible with the preserved bootloader and OTA selection state.
5. D: and E: are distinct volume paths. No evidence establishes that they are different physical disks.
6. Device identity, query-time port, USB-Serial/JTAG mode, reset behavior, Flash RDID/capacity, Flash-type eFuse selector, embedded-PSRAM package fields, and the exact VDDSPI eFuse bits were observed. Current running image, PCB rail measurement, PSRAM runtime mode/clock, and future port identity were not.

## 13. Configuration Allowlist

The following values and constraints are the complete allowlist for a future, separate host-only implementation task. They authorize no current firmware edit, device access, or Flash.

- ESP-IDF compile target: `esp32s3`.
- Physical Flash capacity evidence: 32 MB for the exact queried device; this adds no partition or write-range permission.
- Host-only Flash candidate: `CONFIG_ESPTOOLPY_OCT_FLASH=y`, OPI runtime, STR sampling, 80 MHz, and a 16 MB header/address-space limit.
- Console candidate: integrated USB Serial/JTAG, with no custom UART GPIO.
- Embedded PSRAM capacity evidence: 16 MB package/eFuse field; initialization remains disabled.
- Product scope label: ESP-VoCat PCB V1.0 only.
- Startup acceptance surface: fixed, privacy-safe serial log text only.
- PSRAM initialization: disabled.
- GPIO references and GPIO APIs: absent.
- Display, touch, audio, microphone, motor, SD, battery, power-control, Wi-Fi, Bluetooth, NVS product use, OTA, and external storage: disabled or absent.
- Secure Boot, Flash Encryption, eFuse mutation, VDD_SPI change, Flash-voltage change, JTAG restriction, USB Serial/JTAG restriction, and download-mode restriction: prohibited.
- Partition behavior: preserve the original backup interpretation; create no tracked custom partition input.

## 14. Configuration Denylist

Do not add:

- DTR sampling, 120 MHz Flash, or an enabled PSRAM mode/clock;
- any voltage setting, VDDSPI API, manufacturer/device-ID dependency, or a claim that a header value is a physical measurement;
- bootloader configuration, custom partition table, or OTA-layout change;
- app offset, target slot, first-write range, or recovery command;
- `COM7` as a permanent or future serial-port assumption;
- reset/download automation;
- any GPIO, peripheral, network, storage-mutation, security, erase, restore, or physical-movement behavior.

## 15. Firmware-Implementation Gate

**OPEN FOR HOST-ONLY IMPLEMENTATION**

A later independent Apply task may modify only the minimal serial-only firmware, create a reviewed candidate `sdkconfig.defaults`, configure/build on the host, and perform static/artifact review. The candidate must follow `docs/hardware/pcb-v1-esp-idf-configuration-map.md`.

This gate does not mean Flash-ready, hardware-validated, First Flash `GO`, or permission to connect to a device. Partition and actual write offset/range remain separate gates.

## 16. Device-Read-Only Evidence Status

The separately authorized read-only evidence confirmed the chip family, query-time port, Flash manufacturer/device/capacity, Flash-type selector, USB mode, reset behavior, embedded PSRAM capacity/vendor label, and minimum VDDSPI fields. The completed sanitized result is in `docs/hardware/pcb-v1-device-readonly-inspection-result.md`.

That authorization was consumed and closed. No further device evidence is required for host-only implementation, and no further device access is authorized. Any future device operation requires a new, exact scope.

## 17. Current Go/No-Go Decision

- Firmware Implementation: **HOST-ONLY IMPLEMENTATION COMPLETED**
- First Flash: **NO-GO**
- Completed `chip_id`/`flash_id` authorization: **CONSUMED AND CLOSED**
- Further device access authorization: **NOT GRANTED**

The next allowed work is host-only compatibility and pre-Flash evidence preparation that does not select or authorize a device-write range. It grants no device authority. Vague continuation language does not authorize device access or Flash.

## Decision Table

| Parameter | Candidate value | Evidence source | Evidence classification | Safe to use in tracked configuration | Reason | Additional verification required |
|---|---|---|---|---|---|---|
| IDF target | `esp32s3` | Both original images; prior hardware profile | `CONFIRMED` for image target; prior-recorded `CONFIRMED` for device | Yes, compile target only | Independent images agree and no competing MCU target exists | Reconfirm exact device before any device operation |
| Physical Flash capacity | 32 MB | Current JEDEC RDID `0xC2/0x8039`; esptool capacity map; dump length | `CONFIRMED` current-device identification evidence; independent `CONFIRMED` file range | Capacity record yes; host-only header candidate remains 16 MB | Capacity maps to `CONFIG_ESPTOOLPY_FLASHSIZE_32MB`; a smaller header is legal on a larger chip | Review built artifact before changing the 16 MB candidate |
| Image-declared Flash size | 16 MB | Original headers; ESP-IDF runtime size check | `CONFIRMED` image declaration and source-supported conservative limit | Yes, host-only candidate | Covers the preserved layout and avoids upper-tail/32-bit-address assumptions | Inspect generated header and partition bounds |
| Flash mode | DIO original header / Octal eFuse selector / OPI STR candidate | Image headers; `FLASH_TYPE=1`; ESP-IDF v5.5.4 Kconfig/MSPI source | Inputs `CONFIRMED`; candidate source-mapped | Yes, host-only candidate | Header and physical/runtime layers differ; explicit OPI emits DOUT header | Inspect generated header and preserved-bootloader compatibility |
| Flash frequency | 80 MHz | Original headers; ESP32-S3 target Kconfig | `CONFIRMED` image declaration; source-supported candidate | Yes, host-only candidate | Exact symbol is `CONFIG_ESPTOOLPY_FLASHFREQ_80M` | Build/artifact review; no physical timing claim |
| Flash voltage | Forced 1.8 V LDO eFuse configuration | `FORCE=1`, `XPD=1`, `TIEH=0`; ESP32-S3 eFuse definitions | `CONFIRMED` eFuse configuration; physical rail not measured | No setting is required or allowed | Historical 3.3 V tool output is an explained early-branch defect | Preserve state; never modify or trial-Flash voltage |
| PSRAM capacity | Embedded 16 MB | Package/eFuse fields; ESP-IDF runtime density decoder | `CONFIRMED` eFuse/package evidence | Capacity record only; initialization remains off | No 16 MB Kconfig exists; capacity is runtime-detected after enablement | Mode/clock/routing/runtime review before later enablement |
| PSRAM mode | Disabled for first candidate; `AP_1v8` is a vendor/variant label | Current package fields; ESP-IDF Kconfig/driver source | Label `CONFIRMED`; active mode/clock `UNVERIFIED` | Yes, disabled only | Minimal serial program has no PSRAM dependency | Keep `CONFIG_SPIRAM` off |
| Partition table offset | `0x00008000` | Full-dump scan, MD5/terminator/bounds checks, ESP-IDF parser | `CONFIRMED` for backup image | No, except as recovery interpretation | Not a new-board configuration or write authorization | Artifact compatibility review before any write |
| App offset | `0x00020000` for original `ota_0` | Validated backup partition table | `CONFIRMED` for backup image | No | Original offset is not an automatic custom-image target | Custom artifact, bootloader, OTA, and slot review |
| First Flash write range | Unresolved | No custom image exists | `UNVERIFIED` | No | No image, hash, length, slot, or compatibility decision | Implement/build only after gate opens, then inspect exact artifact |
| UART port | `COM7` for the completed queries | User-reviewed port and successful current query | `CONFIRMED` for the recorded event only | No future assumption | Ports are not stable identities | Re-review before any future operation |
| Reset/download method | ROM-loader connection; final RTS hard reset | Direct output and installed source | `CONFIRMED` for the recorded event | No | Connection also made transient register/SPI changes; it grants no future reset permission | Separately review any future connection/reset behavior |

## 18. Host-Only Implementation Verification

The approved candidate was implemented and built on the host on 2026-07-26 with ESP-IDF v5.5.4. The final generated configuration resolved to Octal Flash, OPI runtime, STR, DOUT header string, 80 MHz, 16 MB, USB Serial/JTAG primary console, no secondary/UART console, and no PSRAM component. Secure Boot, Flash Encryption, rollback, and anti-rollback remained off.

The final app is 160832 bytes with SHA-256 `1B72A60DE9C9BB42DE401A58D7772B0525AFBC4DC3D8C85BB550D2D758F99DDC`. Offline inspection reports ESP32-S3 image v1, DOUT / 80 MHz / 16 MB, valid checksum/hash, revision bounds v0.0-v0.99, and secure version 0.

The generated default layout is a host-build single-factory-app layout with app offset `0x10000`; it is not compatible as a partition-table replacement for the preserved OTA layout and is not authorized for device write. Task 3.4 remains incomplete because exact future `otadata`, offset/range, and preserved-bootloader runtime compatibility are unresolved.

Firmware Implementation Gate: **HOST-ONLY IMPLEMENTATION COMPLETED**. First Flash remains **NO-GO**. Device and Flash authorization remain **NONE**.
