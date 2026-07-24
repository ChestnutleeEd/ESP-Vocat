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
