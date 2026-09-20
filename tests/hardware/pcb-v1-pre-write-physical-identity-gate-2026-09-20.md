# PCB V1.0 Pre-Write Physical Identity Gate — 2026-09-20

> **RESULT: FAIL — PRE-WRITE SAFETY GATE NOT SATISFIED**
>
> **NO FLASH WRITE WAS AUTHORIZED OR PERFORMED**
>
> **HARDWARE/DISPLAY STATUS: UNVERIFIED**

- Change: `validate-pcb-v1-display-backlight-on-hardware`
- Branch: `feat/validate-pcb-v1-display-backlight-on-hardware`
- HEAD: `019e98f7b367b45f2b2d4a52aef0757930a8aad8`
- Upstream: synchronized, ahead/behind `0/0`
- Initial worktree: clean

## 1. Repository and candidate gate

Both independent build trees and all manifest source inputs matched the
committed successor evidence. The canonical candidate used for this review was:

`D:\ESP-VoCat-Project\custom-vocat\firmware\build-backlight-repro-a-20260920-r1\pcb_v1_minimal_display_smoke_test.bin`

- size: 231,360 bytes;
- SHA-256:
  `FEE9C3D1AA77CD0DF1B2C2D55A86B7BEAA2871E5E8443B9D458B10324FF8F362`;
- semantic range: `[0x00020000,0x000587C0)`;
- expected ROM no-stub transport range: `[0x00020000,0x00058800)`;
- expected sector erase envelope: `[0x00020000,0x00059000)`.

No firmware source or build output changed during this gate.

## 2. Immediate recovery-asset rehash

All six established assets passed immediately before endpoint enumeration:

| Asset | Bytes | SHA-256 | Result |
|---|---:|---|---|
| `D:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi\esp-vocat_full_flash_32MB_2026-07-10.bin` | 33554432 | `72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001` | PASS |
| `D:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi\esp-vocat_full_flash_32MB_verify_2026-07-10.bin` | 33554432 | `72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001` | PASS |
| `E:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi\esp-vocat_full_flash_32MB_2026-07-10.bin` | 33554432 | `72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001` | PASS |
| `E:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi\esp-vocat_full_flash_32MB_verify_2026-07-10.bin` | 33554432 | `72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001` | PASS |
| `D:\ESP-VoCat_First_Flash_Packages\2026-07-26\original_xiaozhi_ota_0_full_partition.bin` | 4128768 | `C8A2FE4AB0F9B7C823F1DCFDFC926682C9DBF1B079056461DC6F18A93A345D0E` | PASS |
| `E:\ESP-VoCat_First_Flash_Packages\2026-07-26\original_xiaozhi_ota_0_full_partition.bin` | 4128768 | `C8A2FE4AB0F9B7C823F1DCFDFC926682C9DBF1B079056461DC6F18A93A345D0E` | PASS |

No recovery asset was modified.

## 3. Official PCB V1.0 procedure

Espressif's official ESP-VoCat v1.0 user guide documents:

- a dedicated RST button for resetting the main board;
- a dedicated BOOT button;
- hold BOOT while powering on to enter download mode;
- USB-C for power, programming download, and debugging, using a USB data
  cable for application development;
- `ESP32-S3-WROOM-2-N32R16V`, 32 MB Flash, and 16 MB PSRAM;
- `LCD_BLK` on GPIO44.

Source:
`https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp-vocat/user_guide_v1.0.html`

This is `DOCUMENTED PROCEDURE`. It was not physically attempted, so
`PHYSICALLY CONFIRMED ON THIS UNIT` remains false. No BOOT+RST combination is
invented or required by this record.

## 4. Windows endpoint enumeration

Windows metadata enumeration opened no port and selected no endpoint by
history. It found:

- `COM7`: `USB Serial Device`, status OK, `VID_303A/PID_1001`, MI_00;
- sibling interface: `USB JTAG/serial debug unit`, status OK,
  `VID_303A/PID_1001`, MI_02;
- parent: one Espressif USB composite device;
- COM3 through COM6: Bluetooth serial endpoints, excluded by metadata.

The USB instance suffix/serial was redacted and not retained. `COM7` was the
only defensible ESP32-S3 USB Serial/JTAG candidate.

A first host-only formatting pass collided with PowerShell's read-only `$PID`
variable and printed an invalid process-ID value in the temporary PID column.
It opened no port. One corrected Windows metadata query then produced the
values above. This was not an esptool attempt or serial probe.

## 5. Single controlled esptool interrogation

Classification: `READ-ONLY`.

Installed tool: esptool 4.12.dev3 under the ESP-IDF v5.5.4 Python environment.

Exactly one endpoint and one esptool device invocation were used:

- endpoint: `COM7` only;
- chip argument: `esp32s3`;
- transport: ROM loader, `--no-stub`, 115200 baud;
- connection attempts: one;
- before: `default_reset`;
- after: `no_reset`;
- command: `flash_id`;
- exit code: 0.

The automatic handshake performed the authorized transient reset required to
enter the ROM loader. The command ended with `Staying in bootloader.` No
application monitoring or later reset occurred.

Sanitized observations:

| Field | Observed |
|---|---|
| chip | ESP32-S3, QFN56 |
| revision | v0.2 |
| features | Wi-Fi, BLE, embedded PSRAM 16 MB (`AP_1v8`) |
| crystal | 40 MHz |
| USB mode | USB Serial/JTAG |
| Flash manufacturer | `0xC2` |
| Flash device | `0x8039` |
| detected Flash size | 32 MB |
| Flash type eFuse | octal / 8 data lines |
| Flash voltage eFuse | **3.3 V** |
| MAC | redacted before display and not retained |

The command did not print a complete Secure Boot/Flash Encryption summary.
No second `get_security_info` query was permitted or attempted. Successful ROM
loader access and Flash identification showed no immediate access rejection,
but this run does not newly prove the complete security state.

## 6. Identity comparison and stop decision

Matching:

- ESP32-S3 family, QFN56 revision v0.2;
- USB Serial/JTAG endpoint;
- 16 MB embedded PSRAM feature;
- 32 MB Flash geometry;
- octal Flash interface;
- prior same-device endpoint/history and recovery geometry.

Material contradiction:

- official board/module evidence identifies
  `ESP32-S3-WROOM-2-N32R16V` / `ESP32-S3R16V`, whose documented SPI voltage is
  1.8 V;
- fresh esptool reported VDD_SPI eFuse voltage 3.3 V;
- local esptool source confirms this report is decoded from ESP32-S3 VDD_SPI
  eFuse fields.

The exact module suffix is not software-proven by `flash_id`. The voltage
contradiction is unresolved and triggers the project's Flash-voltage and
identity stop conditions. No VDD_SPI/eFuse/strap change is authorized or
recommended.

Official module reference:
`https://documentation.espressif.com/esp32-s3-wroom-2_datasheet_en.html`

## 7. Device interactions and authorization closure

Device-touching operations performed:

1. one esptool connection to exact `COM7`;
2. one automatic ROM-loader reset/handshake;
3. read-only chip/interface/features banner reads;
4. read-only JEDEC Flash ID/size, Flash-type eFuse, and VDD_SPI eFuse reads.

Persistent writes/erases: **zero**.

No stub upload, Flash write, Flash erase, Flash readback, eFuse write, security
change, VDD_SPI change, partition change, NVS/OTA modification, application
monitor, display test, GPIO44 test, rollback, or restore occurred. The device
was last observed staying in ROM bootloader.

## 8. Proposed future write

**NOT PREPARED — IDENTITY GATE FAILED.**

The reviewed host-only geometry remains documented, but the condition for
constructing an exact write command was not met. No copyable or executable
write command is included in this record.

## 9. OpenSpec effect

Newly resolved with evidence:

- task 5.7: immediate six-asset recovery rehash;
- task 6.1: authoritative PCB V1.0 BOOT/RST/USB-C procedure;
- task 7.1: fresh current endpoint and minimum identity;
- task 7.2: fresh comparison and fail-closed stop on mismatch.

Write, startup/display observation, visual validation, recovery, and physical
closeout tasks remain open. Hardware/display status remains `UNVERIFIED`.

## 10. Final disposition

`FAIL — PRE-WRITE SAFETY GATE NOT SATISFIED`

## 11. Subsequent VDD_SPI resolution

The `FAIL` above is preserved as the original fail-closed result. A later
host-only reconciliation located the already-recorded minimum eFuse summary
for this PCB V1.0 unit:

```text
VDD_SPI_FORCE = 1
VDD_SPI_XPD   = 1
VDD_SPI_TIEH  = 0
```

Installed esptool.py `v4.12.dev3` checks a broad three-bit mask before its
narrower branches, so nonzero FORCE/XPD incorrectly entered its `3.3V` branch.
The individual fields and ESP32-S3 definitions instead select a forced,
enabled 1.8 V LDO. No new device query was needed or performed.

The original gate history is not rewritten. Its voltage blocker is superseded
by the successor evidence in
`tests/hardware/pcb-v1-vdd-spi-contradiction-resolution-2026-09-20.md`, whose
result is:

`PASS — VDD_SPI CONTRADICTION RESOLVED; PRE-WRITE IDENTITY GATE MAY BE RE-EVALUATED`
