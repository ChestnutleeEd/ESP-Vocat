# ESP-VoCat PCB V1.0 Hardware Profile
- Document status: Active
- Version: 1.0
- Established: 2026-07-11
- Target device: ESP-VoCat
- Confirmed PCB version: V1.0
This document records confirmed hardware facts, strongly supported reference information and unresolved assumptions.
Do not treat strongly supported or unverified information as equivalent to confirmed physical measurements.
---
## 1. Evidence classification
All hardware information must be classified into one of the following levels.
### CONFIRMED
Confirmed directly through the current physical device, including:
- ROM or boot logs
- esptool output
- eFuse summary
- Flash reads
- running original firmware
- repeatable device behavior
### STRONGLY SUPPORTED
Supported by source code or component documentation that closely matches the current device, but has not yet been electrically verified on this exact unit.
### UNVERIFIED
Not yet confirmed by the physical device or reliable matching source.
Unverified information must not be used for potentially unsafe hardware control.
---
## 2. Confirmed main hardware
| Item | Confirmed value | Evidence level |
|---|---|---|
| Device | ESP-VoCat | CONFIRMED |
| PCB version | V1.0 | CONFIRMED |
| MCU | ESP32-S3 | CONFIRMED |
| MCU package | QFN56 | CONFIRMED |
| Silicon revision | v0.2 | CONFIRMED |
| Crystal | 40 MHz | CONFIRMED |
| Flash capacity | 32 MiB | CONFIRMED |
| Flash interface | Octal / 8 data lines | CONFIRMED |
| Flash voltage | 1.8 V | CONFIRMED |
| Embedded PSRAM | 16 MiB | CONFIRMED |
| USB interface | USB Serial/JTAG | CONFIRMED |
| Current observed serial port | COM7 | CONFIRMED, not permanent |
| Wi-Fi | Supported | CONFIRMED |
| Bluetooth LE | Supported | CONFIRMED |
Important:
- COM7 is not a permanent device identity.
- Software must not hard-code COM7.
- Flash voltage must never be changed.
- Generic ESP32-S3 board defaults must not be assumed.
---
## 3. Confirmed security and eFuse state
| Item | State |
|---|---|
| Secure Boot | Disabled |
| Flash Encryption | Disabled |
| Download mode | Enabled |
| USB Serial/JTAG | Enabled |
| JTAG disable | Not set |
| Security key purposes | No active key purpose identified |
Prohibited actions:
- enabling Secure Boot without a dedicated reviewed migration plan;
- enabling Flash Encryption;
- disabling USB Serial/JTAG;
- disabling download mode;
- changing key purposes;
- changing eFuses;
- changing Flash voltage.
---
## 4. Confirmed Flash layout
Physical Flash address range:
```text
0x00000000 - 0x02000000
```
Total size:
```text
0x02000000 = 32 MiB
```
Confirmed partition layout:
| Name | Type | Offset | Size | Notes |
|---|---:|---:|---:|---|
| bootloader | - | `0x00000000` | to partition table area | Valid ESP32-S3 image |
| partition table | - | `0x00008000` | `0x1000` extracted | Valid |
| nvs | data | `0x00009000` | `0x4000` | Contains credentials and settings |
| otadata | data | `0x0000D000` | `0x2000` | Selects OTA slot |
| phy_init | data | `0x0000F000` | `0x1000` | Special PHY initialization data |
| ota_0 | app | `0x00020000` | `0x3F0000` | Active Xiaozhi firmware |
| ota_1 | app | `0x00410000` | `0x3F0000` | Currently erased |
| assets | data | `0x00800000` | `0x800000` | Animation and resource package |
Partitioned area ends at:
```text
0x01000000
```
The region:
```text
0x01000000 - 0x02000000
```
was confirmed as erased `0xFF` in the original backup.
This unused region must not be repurposed until a new partition strategy has been reviewed and recovery implications are understood.
---
## 5. Confirmed original firmware
Active application:
```text
xiaozhi 2.2.6
```
Build information observed:
```text
Compile date: 2026-07-01
ESP-IDF: v5.5.3-dirty
```
Active OTA slot:
```text
ota_0
```
`ota_1` was confirmed erased in the original image.
The installed firmware identifies the board at boot as:
```text
ESP-VoCat: PCB version V1.0
```
---
## 6. Confirmed recovery image
Primary full-Flash image:
```text
E:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi\esp-vocat_full_flash_32MB_2026-07-10.bin
```
Verification image:
```text
E:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi\esp-vocat_full_flash_32MB_verify_2026-07-10.bin
```
Cross-disk backup directory:
```text
D:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi
```
Confirmed full-image size:
```text
33554432 bytes
```
Confirmed SHA-256 for both independent reads:
```text
72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001
```
Any mismatch from this hash is a stop condition for recovery operations.
---
## 7. Strongly supported peripherals
The following devices are strongly supported by the matching ESP-VoCat reference source and original firmware behavior.
| Function | Probable component | Evidence level |
|---|---|---|
| Audio ADC | ES7210 | STRONGLY SUPPORTED |
| Audio codec / DAC | ES8311 | STRONGLY SUPPORTED |
| Display controller | ST77916 | STRONGLY SUPPORTED |
| Display resolution | 360 × 360 | STRONGLY SUPPORTED |
| Display bus | QSPI | STRONGLY SUPPORTED |
| Screen touch | CST816S | STRONGLY SUPPORTED |
| Screen touch I2C address | `0x15` | STRONGLY SUPPORTED |
| Battery or fuel-related device | I2C address `0x55` | STRONGLY SUPPORTED |
| IMU | BMI270, possibly optional | UNVERIFIED |
| Top petting input | Single-channel capacitive touch | STRONGLY SUPPORTED |
| Rotating base connection | UART | STRONGLY SUPPORTED |
Each peripheral must still be tested independently before formal integration.
---
## 8. Strongly supported PCB V1.0 GPIO map
These assignments are derived from matching ESP-VoCat source code and the confirmed PCB V1.0 branch.
They are not yet all electrically measured on the current unit.
### 8.1 Audio control
| Signal | GPIO | Evidence |
|---|---:|---|
| Codec I2C SDA | GPIO2 | STRONGLY SUPPORTED |
| Codec I2C SCL | GPIO1 | STRONGLY SUPPORTED |
| Codec power | GPIO48 | STRONGLY SUPPORTED |
| I2S MCLK | GPIO42 | STRONGLY SUPPORTED |
| I2S WS | GPIO39 | STRONGLY SUPPORTED |
| I2S BCLK | GPIO40 | STRONGLY SUPPORTED |
| I2S DIN | GPIO15 | STRONGLY SUPPORTED, V1.0 |
| I2S DOUT | GPIO41 | STRONGLY SUPPORTED |
| Amplifier enable | GPIO4 | STRONGLY SUPPORTED, V1.0 |
### 8.2 Display
| Signal | GPIO | Evidence |
|---|---:|---|
| LCD clock | GPIO18 | STRONGLY SUPPORTED |
| LCD chip select | GPIO14 | STRONGLY SUPPORTED |
| LCD data 0 | GPIO46 | STRONGLY SUPPORTED |
| LCD data 1 | GPIO13 | STRONGLY SUPPORTED |
| LCD data 2 | GPIO11 | STRONGLY SUPPORTED |
| LCD data 3 | GPIO12 | STRONGLY SUPPORTED |
| LCD reset | GPIO3 | STRONGLY SUPPORTED, V1.0 |
| LCD backlight | GPIO44 | STRONGLY SUPPORTED |
### 8.3 Touch and interaction
| Signal | GPIO | Evidence |
|---|---:|---|
| BOOT button | GPIO0 | STRONGLY SUPPORTED |
| Top capacitive touch | GPIO7 | STRONGLY SUPPORTED, V1.0 |
| Screen-touch interrupt | GPIO10 | STRONGLY SUPPORTED |
PCB V1.0 top touch appears to expose only one capacitive channel.
Therefore it can likely detect:
- touch start;
- touch duration;
- repeated taps;
- touch release;
- approximate rhythm.
It must not be assumed to detect precise swipe direction across the top surface.
### 8.4 Base and miscellaneous
| Signal | GPIO | Evidence |
|---|---:|---|
| Base UART TX | GPIO6 | STRONGLY SUPPORTED, V1.0 |
| Base UART RX | GPIO5 | STRONGLY SUPPORTED, V1.0 |
| Main power control | GPIO9 | STRONGLY SUPPORTED |
| Status LED | GPIO43 | STRONGLY SUPPORTED |
| SD-related signal | GPIO17 | STRONGLY SUPPORTED |
| SD-related signal | GPIO16 | STRONGLY SUPPORTED |
| SD-related signal | GPIO38 | STRONGLY SUPPORTED |
Important:
Generic base-control examples using TX GPIO5 and RX GPIO4 must not be copied into this PCB V1.0 project without verification.
---
## 9. Reserved and sensitive pins
Until board support has been validated, all pins listed in this document must be treated as reserved.
Do not use them for generic LED blinking, debug output or experimental GPIO control.
Particularly sensitive groups include:
- Flash and PSRAM pins;
- display QSPI pins;
- audio I2S pins;
- codec power and amplifier enable;
- base UART pins;
- main power control;
- LCD reset and backlight;
- BOOT GPIO0.
A generic ESP-IDF blink example must not be flashed using an arbitrary GPIO number.
---
## 10. Original assets
The original `assets` partition was confirmed to contain a valid resource package with approximately 20 files, including:
- EAF animation resources;
- `index.json`.
The full 8 MiB assets partition contains additional stale or undeclared tail data beyond the active package.
Therefore:
- extraction and logical package size are not equivalent to the raw partition;
- restoring only visible active files is not equivalent to restoring the original assets partition;
- the complete raw partition must remain preserved.
---
## 11. NVS handling
The original NVS partition is structurally healthy and contains private configuration data, credentials or tokens.
Rules:
- do not print NVS values into logs;
- do not commit NVS values into Git;
- do not include raw NVS dumps in public repositories;
- do not overwrite original NVS during early tests unless explicitly required;
- test firmware should use a separate reviewed partition layout or deliberately controlled storage strategy.
---
## 12. First safe hardware test boundary
The first PCB V1.0 smoke-test firmware may test only:
1. serial boot output;
2. display initialization;
3. solid-color display test;
4. screen-touch coordinate reporting;
5. top capacitive-touch detection.
The first smoke test must not enable:
- Wi-Fi;
- Bluetooth;
- microphone;
- speaker or amplifier;
- source localization;
- rotating base;
- SD interface;
- battery or power-control writes;
- arbitrary unknown GPIO output.
---
## 13. Unresolved hardware questions
The following items still require controlled tests or stronger documentation:
- exact LCD initialization command sequence;
- exact LCD pixel format and orientation;
- actual screen-touch coordinate orientation;
- top-touch threshold and filtering;
- whether BMI270 is physically installed;
- exact battery/fuel-device model at I2C `0x55`;
- complete base UART packet protocol;
- safe base motion angle and speed limits;
- microphone channel arrangement;
- feasibility and accuracy of sound-direction estimation;
- amplifier power-up and mute sequence;
- power-control behavior of GPIO9;
- SD socket presence and electrical configuration;
- whether all reference GPIO values exactly match this physical unit.
---
## 14. Stop conditions
Development must stop before hardware execution when:
- the target is not confirmed as ESP32-S3;
- PCB version is not confirmed as V1.0;
- Flash voltage appears inconsistent;
- a GPIO conflicts with this profile;
- a source example targets a different PCB revision;
- recovery images cannot be located;
- recovery image hash does not match;
- an operation would modify eFuses;
- an operation would erase the complete Flash without explicit approval;
- motor or power-control behavior is still uncertain;
- a hardware result is being inferred rather than measured.
---
## 15. Update rule
When a hardware fact is verified:
1. record the test method;
2. record the observed result;
3. change its evidence level only when justified;
4. update the document version;
5. add the decision or test record under `docs/decisions` or `tests`;
6. do not silently overwrite previous uncertainty.

## 16. First Flash attempt state

On 2026-07-26 one explicitly authorized App-only write invocation was
performed on the same reviewed ESP-VoCat PCB V1.0 unit at exact `COM7`.
Connection identified ESP32-S3 QFN56 revision v0.2 and USB Serial/JTAG. The
base MAC was filtered before display and was not retained.

The operation:

- used esptool 4.12.dev3, ROM loader, no stub, and no compression;
- erased `0x00020000-0x00047FFF`;
- transmitted the 160832-byte candidate beginning at `0x00020000`;
- also transmitted 960 final-block padding bytes with value `0xFF` over
  `0x00047440-0x000477FF`;
- reported candidate-range MD5 success;
- performed no automatic retry;
- ended in ROM loader.

The 960-byte transmitted padding was not represented in the authorization's
candidate byte range, even though it remained inside the reviewed erase
envelope. This triggered the possible-unauthorized-range stop condition. No
startup reset, serial observation, readback, rollback, or recovery followed.

Exact affected ranges:

- sector erase:
  `0x00020000-0x00047FFF`, end-exclusive `0x00048000`, length
  `0x00028000` / 163840 bytes;
- candidate image:
  `0x00020000-0x0004743F`, end-exclusive `0x00047440`, length
  `0x00027440` / 160832 bytes;
- transmitted ROM blocks:
  `0x00020000-0x000477FF`, end-exclusive `0x00047800`, length
  `0x00027800` / 161792 bytes;
- additional transmitted padding:
  `0x00047440-0x000477FF`, length `0x000003C0` / 960 bytes, value `0xFF`;
- erased but not covered by transmitted data:
  `0x00047800-0x00047FFF`.

Evidence state at the end of the First Flash attempt, before the later
startup observation:

- candidate-byte transmission and candidate-range MD5: **CONFIRMED by the
  one esptool result**;
- final-block `0xFF` transmission semantics: **CONFIRMED by installed
  esptool source and reported byte count**;
- application boot, ready marker, stability, and peripheral behavior:
  **UNVERIFIED / UNOBSERVED**;
- physical contents outside the reported erase/write envelope:
  **not read back in this attempt**;
- Level 1 and Level 2 recovery: **NOT AUTHORIZED / NOT PERFORMED**.

Authorization and execution state at the end of the First Flash attempt:

- First Flash attempt: **STOPPED / INCONCLUSIVE — RANGE-SCOPE DEVIATION**;
- First Flash runtime validation: **NOT PERFORMED**;
- device/Flash authorization: **CONSUMED AND CLOSED / NONE**;
- startup-reset, observation, and rollback authorization: **NONE**;
- last observed device state: **STAYING IN ROM BOOTLOADER**;
- further automatic action: **NO-GO**.

The controlling sanitized record is
`tests/hardware/pcb-v1-first-flash-attempt-2026-07-26.md`.

## 17. Candidate startup observation

On 2026-07-27 the user separately authorized exactly one startup reset and
one bounded read-only observation on exact `COM7`. The committed,
pure-host-tested observer opened the endpoint once, released DTR, performed
one RTS assert/deassert reset, and used the same read-only handle for the
complete 60-second window.

Observed result:

- the preserved boot chain selected and loaded the candidate in `ota_0`;
- all eight fixed candidate lines appeared once and in order;
- the ready marker appeared once well inside the 15-second deadline;
- the same handle remained valid for 60 seconds;
- no repeated startup, panic, watchdog, allocation failure, ROM/Bootloader
  fatal error, security rejection, or boot loop was observed;
- no serial data was sent;
- no port enumeration, reopen, retry, Flash readback, Flash write, erase,
  eFuse operation, rollback, or restore occurred.

Evidence classification:

- preserved-bootloader loading of the exact candidate and bounded serial
  startup stability: **CONFIRMED by the 2026-07-27 observation**;
- ready-marker and fixed-log behavior: **CONFIRMED by the bounded
  observation**;
- absence of listed failures: **CONFIRMED only for the 60-second window**;
- long-term stability, physical tail bytes, recovery, PSRAM, display, touch,
  audio, microphone, motor, network, storage, and other peripheral behavior:
  **UNVERIFIED / NOT TESTED**.

The startup observation is **PASS**, but the 2026-07-26 First Flash attempt
remains **STOPPED / INCONCLUSIVE — RANGE-SCOPE DEVIATION**. The startup and
observation authorization is consumed and closed. No current device, Flash,
readback, rollback, or recovery authorization exists.

The controlling sanitized record is
`tests/hardware/pcb-v1-startup-observation-2026-07-27.md`.

## 18. Synchronized startup-observation closeout

### Verified by the 2026-07-27 observation

- ESP32-S3;
- the preserved original Bootloader loaded the candidate;
- DOUT / 80 MHz / 16 MB candidate App started;
- USB Serial/JTAG output was normal;
- the minimal App ran stably for 60 seconds;
- PSRAM was intentionally disabled for this Smoke Test;
- no peripheral hardware was initialized.

### Not yet verified

- PSRAM actual runtime;
- display;
- screen touch;
- audio;
- microphone;
- motor;
- Wi-Fi;
- Bluetooth;
- other GPIO and peripheral hardware;
- long-term stability;
- behavior after a power cycle;
- rollback execution;
- full recovery execution.

The minimal Smoke Test PASS is not a pass for the complete PCB or all
peripherals.

| Item | State |
|---|---|
| Firmware candidate runtime | **PASS — minimal smoke test only** |
| Runtime validation | **PASS FOR THIS MINIMAL SMOKE TEST** |
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

The observation counts were: COM7 opened once; reset once; serial writes zero;
automatic enumeration, reopen, and retry zero; the original handle remained
valid for the entire 60-second window; and no destructive USB re-enumeration
was observed. The eight expected lines appeared once and in order, and the
ready marker appeared once no later than approximately 0.201 seconds after
reset release.
