# ESP-VoCat PCB V1.0 Device Read-Only Inspection Result

- Inspection date: 2026-07-25
- Evidence-audit date: 2026-07-25
- Device: user-confirmed ESP-VoCat PCB V1.0
- Reviewed port for these two completed operations: `COM7`
- Tool: esptool.py `v4.12.dev3`
- Result status: **COMPLETED WITH AN UNRESOLVED FLASH-VOLTAGE CONFLICT**
- Firmware Implementation Gate: **CLOSED**
- First Flash: **NO-GO**

## 1. Authorization Scope

The user explicitly authorized one `chip_id` query and one `flash_id` query against the ESP-VoCat PCB V1.0 on `COM7`, using the already-installed esptool. The authorization acknowledged that connection could enter download mode and reset the device.

The authorization did not include `read_flash`, `write_flash`, `erase_flash`, restore, an eFuse summary, any eFuse write, monitor, firmware build, firmware Flash, or any unlisted device command. It also did not authorize a First Flash.

No additional device access was performed while creating this evidence record or auditing the installed source.

## 2. Operation Record

| Operation | Classification | Port | ROM/stub mode | Exit code | Post-operation behavior |
|---|---|---|---|---:|---|
| chip identification | `READ-ONLY` with connection/reset side effects | `COM7` | ROM loader; `--no-stub` | 0 | RTS hard reset |
| Flash identification | `READ-ONLY` with connection/reset and volatile SPI side effects | `COM7` | ROM loader; `--no-stub` | 0 | RTS hard reset |

The two command lines are not reproduced here because future device access requires a new, operation-specific authorization. The command categories, exact options relevant to the audit, and sanitized outputs are preserved.

## 3. Sanitized Original Output

### 3.1 chip identification

The following are the exact relevant output lines preserved from the completed command. The MAC values emitted by the tool were replaced before they entered this record.

```text
esptool.py v4.12.dev3
Serial port COM7
Connecting...
Detecting chip type... ESP32-S3
Chip is ESP32-S3 (QFN56) (revision v0.2)
Features: WiFi, BLE, Embedded PSRAM 16MB (AP_1v8)
Crystal is 40MHz
USB mode: USB-Serial/JTAG
MAC: <REDACTED>
Enabling default SPI flash mode...
Warning: ESP32-S3 has no Chip ID. Reading MAC instead.
MAC: <REDACTED>
Hard resetting via RTS pin...
```

- Exit code: `0`.
- Stub upload: none.
- Monitor: not run.

### 3.2 Flash identification

```text
esptool.py v4.12.dev3
Serial port COM7
Connecting...
Detecting chip type... ESP32-S3
Chip is ESP32-S3 (QFN56) (revision v0.2)
Features: WiFi, BLE, Embedded PSRAM 16MB (AP_1v8)
Crystal is 40MHz
USB mode: USB-Serial/JTAG
MAC: <REDACTED>
Enabling default SPI flash mode...
Manufacturer: c2
Device: 8039
Detected flash size: 32MB
Flash type set in eFuse: octal (8 data lines)
Flash voltage set by eFuse to 3.3V
Hard resetting via RTS pin...
```

- Exit code: `0`.
- Stub upload: none.
- Monitor: not run.

## 4. Privacy Handling

- All emitted MAC lines are stored only as `MAC: <REDACTED>`.
- No MAC value, serial number, credential, token, certificate, NVS value, or other unnecessary unique identifier is present.
- No NVS, PHY, OTA metadata, assets, Flash contents, eFuse summary, or arbitrary memory was read.
- No raw binary, dump, partition slice, or device-unique transcript was added to Git.

## 5. Evidence-Type Separation

1. **Tool direct output:** the exact sanitized lines in Section 3 and both exit codes.
2. **Tool source interpretation:** how the installed esptool version derives each line, documented in Sections 6 and 7.
3. **Inference:** conclusions that combine direct output with the audited source, explicitly labeled as such.
4. **Not yet verified:** physical electrical conclusions, active original Kconfig, PSRAM runtime mode/clock, and the actual eFuse bit pattern hidden behind the installed tool's voltage label.

## 6. Installed Tool Identity

- Python:
  `C:\Espressif\tools\python\v5.5.4\venv\Scripts\python.exe`
- Console executable:
  `C:\Espressif\tools\python\v5.5.4\venv\Scripts\esptool.exe`
- Python package:
  `C:\Espressif\tools\python\v5.5.4\venv\Lib\site-packages\esptool`
- Package entry:
  `C:\Espressif\tools\python\v5.5.4\venv\Lib\site-packages\esptool\__init__.py`
- Distribution metadata:
  `C:\Espressif\tools\python\v5.5.4\venv\Lib\site-packages\esptool-4.12.dev3.dist-info`
- Installed version: `4.12.dev3`
- Console entry point:
  `esptool.py = esptool.__init__:_main`

Only local imports, local help, file reads, and source hashes were used during this audit. The Python environment and installed package were not modified.

## 7. Installed-Source Evidence

| File | Class/function | Relevant logic | Meaning | Limitation |
|---|---|---|---|---|
| `esptool/targets/esp32s3.py:211-244` | `ESP32S3ROM.get_psram_cap`, `get_psram_vendor`, `get_chip_features` | PSRAM capacity is assembled from two bits in eFuse block 1 word 4 plus a high bit in word 5. Capacity code 3 maps to `Embedded PSRAM 16MB`. The two-bit vendor field in word 4 maps value 2 to `AP_1v8`. | The printed feature is an ESP32-S3 package/eFuse-derived embedded-PSRAM capacity and vendor/variant label. `AP_1v8` is the installed tool's label for PSRAM vendor-field value 2 and denotes the 1.8 V variant within that PSRAM label. | It does not describe external Flash voltage, a unified PCB supply, PSRAM bus mode, clock, runtime initialization success, or PCB wiring. The source does not further expand the `AP` abbreviation. |
| `esptool/targets/esp32s3.py:75-76,307-313` | `ESP32S3ROM.flash_type` | Reads bit 9 of `EFUSE_RD_REPEAT_DATA3_REG`; clear returns 0 and set returns 1. | This is an ESP32-S3 eFuse-derived Flash-type selector, independent of the application image header. | It does not report the image's current DIO/QIO header field, runtime command mode, safe ESP-IDF options, frequency, voltage, or PCB routing. |
| `esptool/cmds.py:1310-1317` | `flash_id` | Maps Flash type 0 to `quad (4 data lines)` and 1 to `octal (8 data lines)`, then calls `get_flash_voltage`. | The observed Octal line means the installed tool read the ESP32-S3 Flash-type eFuse bit as set. | It does not by itself establish all configuration inputs required for a safe build or write. |
| `esptool/targets/esp32s3.py:122-125,290-296` | ESP32-S3 voltage constants, `_get_rtc_cntl_flash_voltage`, `override_vddsdio` | Defines `VDD_SPI_XPD`, `VDD_SPI_TIEH`, and `VDD_SPI_FORCE` in the ESP32-S3 eFuse register. ESP32-S3 returns no RTC-control voltage override and rejects ROM voltage override. | The generic inherited voltage method is applicable to ESP32-S3 using these target-specific fields, but no voltage override was performed. | It does not expose the three raw eFuse bits in the command output. |
| `esptool/targets/esp32.py:330-365` | `ESP32ROM._get_efuse_flash_voltage`, `get_flash_voltage` inherited by ESP32-S3 | Reads the eFuse voltage register first; if no eFuse result is returned, it falls back to the VDDSPI strapping bit. The installed code's first eFuse condition is `efuse & (FORCE \| XPD \| TIEH)` and returns `3.3V`. Later branches would return `1.8V` and `OFF`; no eFuse result falls back to strap (`1.8V` if set, otherwise `3.3V`). | The exact printed source label `eFuse` shows that this code did not use the strapping fallback for the observed line. | In this installed version, any one or more of the three named eFuse bits satisfies the first condition. Therefore its later `1.8V` and `OFF` eFuse branches are unreachable. The `3.3V` text proves only that the helper saw a nonzero masked value, not which bit pattern was present or that the physical Flash rail is conclusively 3.3 V. |
| `esptool/loader.py:1061-1066`; `esptool/cmds.py:49-73,1300-1307` | `ESPLoader.flash_id`, `detect_flash_id` | Sends JEDEC RDID `0x9F`, prints manufacturer and device bytes, and maps capacity byte `0x39` to `32MB`. | Manufacturer `0xC2`, device `0x8039`, and the capacity-code mapping are direct current-device identification evidence in this query context. | The tool's mapping does not identify PCB routing, voltage, image header settings, or future port identity. |
| `esptool/__init__.py:843-873` | connection flow | A stub is launched only when `--no-stub` is false. | Because both commands used `--no-stub`, no RAM flasher stub was uploaded or executed. | ROM-loader connection still has volatile/reset side effects. |
| `esptool/__init__.py:916-932,986-1030` | ROM Flash preparation | With `--no-stub`, the tool attaches default SPI Flash. Its common flow checks RDID and sends Flash reset-enable/reset commands `0x66` and `0x99`. | These steps prepare/reset the live SPI interface and Flash state; they are not Flash program or erase commands. Manufacturer `0xC2` is not the XMC `0x20` branch targeted by the optional XMC startup sequence. | This means the completed operation was read-only with volatile device side effects, not a mathematically side-effect-free observation. |
| `esptool/targets/esp32s3.py:331-353,365-387`; `esptool/__init__.py:1094-1121` | connect/hard-reset flow | USB-Serial/JTAG connection disables watchdogs through volatile register writes; ESP32-S3 hard reset clears a force-download control bit and then resets. | Explains the connection/reset behavior and the final `Hard resetting via RTS pin...` line. | These are transient register/reset operations. They are not persistent Flash/eFuse writes, but they do modify live state until reset. |

## 8. Flash-Voltage Logic Audit

For ESP32-S3, the installed source follows this path:

1. `ESP32S3ROM._get_rtc_cntl_flash_voltage()` returns `None`.
2. The inherited method reads the ESP32-S3 eFuse voltage register.
3. Under the exact installed condition order:
   - any nonzero value among `VDD_SPI_FORCE`, `VDD_SPI_XPD`, or `VDD_SPI_TIEH` returns `3.3V`;
   - the written `1.8V` and `OFF` eFuse branches cannot be reached because their masks are subsets of the first condition;
   - only when all three masked bits are zero does the function read the strapping register;
   - strapping bit set prints `1.8V`; clear prints `3.3V`.
4. The observed line named `eFuse`, so the fallback strapping path was not selected.

This implementation detail is tool-version-specific and materially limits the conclusion. The exact raw eFuse bit pattern was not printed and will not be reconstructed or guessed. Download mode and a strapping pin can matter to the fallback path, but the observed source label was `eFuse`. The original firmware image header has no role in this method.

Conclusion: `Flash voltage set by eFuse to 3.3V` is a confirmed esptool output line and a confirmed result of this installed implementation. It is **not** sufficient evidence that the physical PCB V1.0 Flash rail is conclusively 3.3 V.

## 9. Stub and Persistent-Write Audit

- `--no-stub` bypassed `run_stub`; no RAM stub was uploaded.
- The `flash_id` operation used ROM-loader register access and JEDEC RDID.
- The common esptool flow performed volatile SPI attachment, watchdog/control-register changes, Flash reset commands, and a final hard reset.
- No Flash program command, Flash erase command, `read_flash`, eFuse burn/write, NVS mutation, bootloader write, partition-table write, or configuration write was dispatched.
- No persistent write path was found for either completed command.

The correct description is therefore: **read-only identification with transient connection/reset/SPI-register side effects and no identified persistent write**.

## 10. Conflict Explanation Matrix

| Observation | Direct source | What it proves | What it does not prove | Confidence | Conflict | Required resolution |
|---|---|---|---|---|---|---|
| Embedded PSRAM 16 MB (`AP_1v8`) | Sanitized `chip_id`/`flash_id` output; ESP32-S3 eFuse package fields | The installed tool decoded embedded PSRAM capacity code 3 and vendor/variant code 2 on this query | PSRAM runtime mode, clock, initialization, PCB-wide voltage, or external Flash voltage | `CONFIRMED` for tool/eFuse package-field decoding | Mode is still unresolved | Map exact package evidence to ESP-IDF v5.5.4 PSRAM options; do not enable yet |
| Manufacturer `0xC2` | JEDEC RDID via `flash_id` | Current queried Flash returned manufacturer byte `0xC2` | Exact part number, voltage, PCB wiring, or image mode | `CONFIRMED` for query | None by itself | Retain with tool/version/query context |
| Device `0x8039` | JEDEC RDID via `flash_id` | Current queried Flash returned device bytes `0x8039` | Exact complete part number or voltage | `CONFIRMED` for query | Header declares only 16 MB | Retain raw ID and capacity mapping context |
| Detected Flash size 32 MB | Capacity byte `0x39` mapped by installed esptool | The queried JEDEC capacity code maps to 32 MB in this tool | Safe image-header size, partition design, or voltage | `CONFIRMED` current-device identification evidence | Original headers declare 16 MB | Reconcile as physical capacity versus image declaration; do not widen write range |
| Octal / 8 data lines | ESP32-S3 eFuse Flash-type bit set | The installed tool read the target Flash-type eFuse selector as Octal | Current app header mode, safe ESP-IDF mode/frequency, or exact PCB routing | `CONFIRMED` eFuse-setting evidence | Original headers declare DIO | Treat eFuse/package interface and image transfer declaration as different layers |
| Flash voltage 3.3 V | Installed inherited voltage helper | This version returned 3.3 V from a nonzero masked eFuse value and printed source `eFuse` | Exact raw bit pattern or conclusive physical Flash rail | `CONFIRMED` output; `UNVERIFIED` physical conclusion | Conflicts with `AP_1v8` interpretation scope and prior 1.8 V record | Separately authorize a sanitized minimum eFuse summary; then reconcile with authoritative board evidence without changing voltage |
| Original image DIO / 80 MHz / 16 MB | Bootloader/app headers | What both original images declare | Physical Flash capacity, eFuse Flash type, voltage, or PCB routing | `CONFIRMED` image declaration | DIO/16 MB differs from Octal/32 MB evidence | Preserve layer distinction; map bootloader/app compatibility before any build/write |
| Original backup covers 32 MiB | Verified file length | Captured file covers `0x00000000-0x02000000` | Physical capacity by itself, electrical mode, or current live content | `CONFIRMED` immutable-file fact | Consistent with current detected 32 MB | Keep as independent corroboration only |
| Upper 16 MiB is erased | Offline byte count | That backup region contains only `0xFF` | Physical chip size, unused safety, or permission to repurpose | `CONFIRMED` backup fact | Header declares 16 MB while dump/query cover 32 MB | Do not repurpose; retain original partition boundary |
| Prior repository 1.8 V record | Hardware profile/constitution history | Repository previously classified 1.8 V as confirmed | The underlying raw measurement or exact current eFuse bits | Prior-recorded `CONFIRMED`; trace missing | Directly conflicts with current installed-tool 3.3 V text | Obtain bit-level read-only evidence and authoritative electrical documentation; never trial-flash or alter voltage |

The DIO and Octal observations are not mutually exclusive statements about the same field: DIO is an image-header transfer declaration, while the Octal line is derived from an ESP32-S3 eFuse Flash-type selector. Likewise, the detected 32 MB capacity is closer to physical chip-identification evidence than an image-declared 16 MB field, but it remains tied to this exact tool, command, device, and query context.

## 11. Current Evidence Decisions

| Parameter | Current evidence result | Configuration decision |
|---|---|---|
| Device target | ESP32-S3 QFN56 revision v0.2; current query `CONFIRMED` | `esp32s3` compile target remains allowed |
| Query-time port | `COM7`; current operation `CONFIRMED` | Historical evidence only for future operations; must be re-reviewed |
| Physical Flash capacity | 32 MB; current RDID evidence `CONFIRMED` | Capacity may be recorded, but no write range follows from it |
| Flash bus type | Octal/8-line eFuse selector `CONFIRMED` | Do not enter tracked board config until exact ESP-IDF mapping and voltage conflict are resolved |
| Flash frequency | 80 MHz image declaration only | Unresolved as a board setting |
| Flash voltage | 3.3 V tool output confirmed; physical conclusion conflicted/`UNVERIFIED` | No voltage setting or change allowed |
| PSRAM capacity | Embedded 16 MB eFuse/package decode `CONFIRMED` | Capacity may be recorded; initialization remains disabled |
| PSRAM voltage/variant | `AP_1v8` eFuse/package label `CONFIRMED` as a label | Does not resolve Flash voltage or PSRAM mode |
| PSRAM mode/clock | Not reported | Unresolved; initialization remains disabled |

## 12. Stop Decision and Required Next Evidence

- Firmware Implementation Gate: **CLOSED**.
- First Flash: **NO-GO**.
- Flash authorization: **NOT GRANTED**.
- eFuse summary: **NOT AUTHORIZED**.

A later, separately reviewed minimum read-only eFuse summary is justified to expose the individual `VDD_SPI_FORCE`, `VDD_SPI_XPD`, and `VDD_SPI_TIEH` states hidden by this installed esptool implementation. It must redact MAC and unrelated unique fields, perform no eFuse write, and stop after the approved fields are recorded. Even that summary may still require reconciliation with authoritative PCB/schematic or electrical evidence; it must not be treated as permission to change voltage.

The only next device decision is whether the user explicitly authorizes that separately reviewed, sanitized, read-only eFuse-summary packet. Vague continuation language is not authorization.
