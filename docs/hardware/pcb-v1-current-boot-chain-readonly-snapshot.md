# PCB V1.0 Current Boot-Chain Read-Only Snapshot

- Snapshot date: 2026-07-26
- Device: user-authorized ESP-VoCat PCB V1.0
- Port: `COM7`
- Tool: esptool.py `v4.12.dev3`
- Result: **COMPLETED WITH ANCILLARY READ-SCOPE DEVIATION**
- First Flash: **NO-GO**
- Device access authorization: **CONSUMED AND CLOSED / NONE**
- Flash authorization: **NONE**

## 1. Purpose

Collect the smallest current-device snapshot needed to compare the live bootloader,
partition table, OTA selection metadata, and the first 4 KiB of both OTA app
partitions with the verified 2026-07-10 recovery image.

This snapshot does not test or authorize the candidate app.

## 2. Authorization Boundary

The user explicitly authorized five Flash reads on the ESP-VoCat PCB V1.0 at
`COM7`, using the existing esptool.py `v4.12.dev3`:

| Region | Offset | Length | End-exclusive |
|---|---:|---:|---:|
| bootloader window | `0x00000000` | `0x00008000` | `0x00008000` |
| partition table | `0x00008000` | `0x00001000` | `0x00009000` |
| `otadata` | `0x0000D000` | `0x00002000` | `0x0000F000` |
| `ota_0` header window | `0x00020000` | `0x00001000` | `0x00021000` |
| `ota_1` header window | `0x00410000` | `0x00001000` | `0x00411000` |

The NVS range is `0x00009000-0x0000CFFF`. The partition-table read ends at
the NVS start, and the `otadata` read begins at the NVS end-exclusive. None
of the five explicit Flash reads overlaps NVS.

Not authorized and not performed as explicit operations:

- any other Flash address range;
- complete `ota_0`, complete `ota_1`, assets, PHY, NVS, or full Flash;
- Flash program, erase, restore, merge, monitor, or firmware build;
- an explicit eFuse command or any eFuse write;
- Secure Boot, Flash Encryption, VDDSPI, or device-configuration change.

## 3. Host and Repository Preconditions

- Branch: `feat/prepare-pcb-v1-first-flash-smoke-test`
- Initial local/upstream HEAD:
  `3b9c99f8504cc7a87dd3671f454b83f89f13d82d`
- Initial ahead/behind: `0/0`
- Initial worktree: clean
- Firmware diff: none
- ESP-IDF Python:
  `C:\Espressif\tools\python\v5.5.4\venv\Scripts\python.exe`
- Python: `3.13.9`
- esptool.py: `4.12.dev3`
- Candidate app remained 160832 bytes with SHA-256
  `1B72A60DE9C9BB42DE401A58D7772B0525AFBC4DC3D8C85BB550D2D758F99DDC`.
- D: recovery image remained 33554432 bytes with SHA-256
  `72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001`.

## 4. Temporary Files

All current and historical slices were placed under:

`%TEMP%\custom-vocat-current-chain-readonly-20260726-103133`

No slice was written to the repository or either recovery directory. The
temporary files are not Git inputs and must not be committed.

## 5. Actual Device Operations

Common reviewed arguments were:

- target: `esp32s3`;
- port: `COM7`;
- baud: `115200`;
- ROM loader only: `--no-stub`;
- operation: `read_flash`;
- progress display disabled;
- no monitor, write, erase, restore, merge, or eFuse subcommand.

| # | Classification | Offset | Length | Before | After | Output size | Exit |
|---:|---|---:|---:|---|---|---:|---:|
| 1 | `READ-ONLY` | `0x00000000` | `0x00008000` | `default_reset` | `no_reset` | 32768 | 0 |
| 2 | `READ-ONLY` | `0x00008000` | `0x00001000` | `no_reset` | `no_reset` | 4096 | 0 |
| 3 | `READ-ONLY` | `0x0000D000` | `0x00002000` | `no_reset` | `no_reset` | 8192 | 0 |
| 4 | `READ-ONLY` | `0x00020000` | `0x00001000` | `no_reset` | `no_reset` | 4096 | 0 |
| 5 | `READ-ONLY` + final `REBOOT` | `0x00410000` | `0x00001000` | `no_reset` | `hard_reset` | 4096 | 0 |

The fifth command ended with `Hard resetting via RTS pin...`. No further
device command was executed.

RAM stub upload: **NO**. Every command used `--no-stub`.

## 6. Explicit Flash-Range Audit

All five explicit read ranges matched the authorization exactly. No explicit
Flash read addressed:

- NVS;
- `phy_init`;
- bytes beyond the first 4 KiB of either OTA app;
- assets;
- any gap, tail, or other Flash region.

No unauthorized Flash address range was read.

## 7. Ancillary Connection Read-Scope Deviation

Although the five Flash ranges were exact, local esptool source review after
the operation confirmed that its standard connection path performs additional
register reads before the selected subcommand:

- `esptool/__init__.py:835-841` calls chip description, feature, crystal,
  USB-mode, and MAC reporting for every normal connection;
- ESP32-S3 chip description and feature methods read eFuse register fields;
- `ESP32S3ROM.read_mac()` reads the base MAC from the eFuse/OTP register
  window;
- the no-stub Flash preparation path performs volatile SPI attachment,
  Flash identification/reset handling, and ROM connection-state operations.

These were not separate CLI subcommands and did not read an additional Flash
address range, but the eFuse/OTP-derived description and MAC reads exceeded the
literal “no eFuse read” and “no unnecessary unique identifier” boundary.
The deviation occurred because the esptool connection banner and its internal
connection behavior were not reviewed sufficiently before execution. It is a
real ancillary read-scope deviation, not merely a display or logging issue, and
it was not part of the explicitly authorized read scope.

Privacy containment:

- command output was captured only in process memory;
- only allowlisted, sanitized status lines were emitted;
- MAC values and feature lines were not displayed or written to a file;
- no raw transcript was retained;
- no key, digest, credential, NVS value, or MAC value entered the repository.

This deviation is why the overall result is not labeled a cleanly in-scope
completion. Future use of esptool for a narrowly constrained read must review
or bypass its automatic connection-banner calls before device execution.

## 8. Current-to-Historical Byte Comparison

Historical slices came from the reverified D: recovery image at the exact same
offsets and lengths.

| Region | Current SHA-256 | Historical SHA-256 | Result |
|---|---|---|---|
| bootloader 32 KiB window | `CFE09A91534C7F0D957BB8944822322B63DF69F189D3CEA7D22CFD569AD42CC3` | same | byte-identical |
| partition table 4 KiB | `CE40CFE75056EF74BC052942F8A9EE3DCE8E5E14BA17A6F63685A8FA0D11A23D` | same | byte-identical |
| `otadata` 8 KiB | `8BA3B110139F45443D4F268D1A3373EF99A1718B71D51664531B83EE2D4B91A3` | same | byte-identical |
| `ota_0` first 4 KiB | `BA3C8752F9A5DCE88F3099FA7B07FA81717743D92E619C3532DCD3F20A214711` | same | byte-identical |
| `ota_1` first 4 KiB | `F47A8EC3E9AFF2318D896942282AD4FE37D6391C82914F54A5DA8A37DE1300C6` | same | byte-identical and all `0xFF` |

## 9. Current Bootloader

Offline esptool `image_info --version 2` exited 0:

- image: ESP32-S3 image v1;
- entry: `0x403C8908`;
- segments: 3;
- header: DIO / 80 MHz / 16 MB;
- chip revision: v0.0-v0.99;
- checksum: valid;
- appended validation hash: valid;
- Bootloader version: 1;
- ESP-IDF string: `v5.5.3-dirty`.

The complete authorized 32 KiB bootloader window is byte-identical to the
2026-07-10 recovery snapshot.

## 10. Current Partition Table

The ESP-IDF v5.5.4 partition parser accepted the current table and produced:

| Name | Type/subtype | Offset | Size |
|---|---|---:|---:|
| `nvs` | data/nvs | `0x00009000` | `0x00004000` |
| `otadata` | data/ota | `0x0000D000` | `0x00002000` |
| `phy_init` | data/phy | `0x0000F000` | `0x00001000` |
| `ota_0` | app/ota_0 | `0x00020000` | `0x003F0000` |
| `ota_1` | app/ota_1 | `0x00410000` | `0x003F0000` |
| `assets` | data/spiffs | `0x00800000` | `0x00800000` |

The complete authorized 4 KiB partition-table window is byte-identical to the
historical snapshot.

## 11. Current OTA Metadata

The parser used the ESP-IDF v5.5.4 `esp_ota_select_entry_t` layout and
`esp_rom_crc32_le(UINT32_MAX, &ota_seq, 4)` semantics. In the offline Python
cross-check this corresponds to `zlib.crc32(sequence_bytes, 0xFFFFFFFF)`.

| Entry | Empty | `ota_seq` | State | Stored CRC | Expected CRC | Valid |
|---|---|---:|---|---:|---:|---|
| sector 0 | no | 1 | `VALID` | `0x4743989A` | `0x4743989A` | yes |
| sector 1 | yes | `UINT32_MAX` | `UNDEFINED`/erased | `0xFFFFFFFF` | not an entry | no |

The sole valid and newest entry is sector 0. With two OTA app partitions,
`(ota_seq - 1) % 2` selects `ota_0`.

No `NEW`, `PENDING_VERIFY`, `INVALID`, or `ABORTED` state was found.
The complete current 8 KiB `otadata` is byte-identical to the historical
snapshot.

## 12. Current OTA App Headers

### `ota_0`

The authorized 4 KiB window begins with a structurally valid ESP32-S3 image
header:

- magic: `0xE9`;
- segments declared: 6;
- entry: `0x4037978C`;
- header: DIO / 80 MHz / 16 MB;
- chip ID: 9 / ESP32-S3;
- revision range: v0.0-v0.99;
- appended-digest flag: set;
- app descriptor magic: valid;
- app: `xiaozhi` `2.2.6`;
- ESP-IDF: `v5.5.3-dirty`;
- secure version: 0.

The 4 KiB window is byte-identical to the historical `ota_0` header window.
Because only 4 KiB was authorized, a full-image checksum/hash validation was
not repeated. Offline esptool `image_info` correctly exited 2 when it reached
the truncated first segment; this is a window-size limitation, not an invalid
header result.

### `ota_1`

All 4096 authorized bytes are `0xFF`. No ESP image header is present. The
window is byte-identical to the historical erased `ota_1` header window.

## 13. Host Analysis Corrections

The first offline OTA CRC attempt used the wrong generic zlib initial value and
contradicted the byte-identical, previously validated historical record. That
result was rejected. Local ESP-IDF source was reread, and the calculation was
corrected to the `UINT32_MAX` initial value used by the bootloader. The corrected
stored and expected CRC values match.

No device reread or retry occurred during either host-analysis correction.

## 14. Compatibility Implication

Current-device uncertainty about the following items is now removed for this
snapshot time:

- preserved bootloader bytes;
- preserved partition-table bytes;
- selected OTA entry and state;
- `ota_0` original-image header presence;
- erased `ota_1` header window.

The current boot chain matches the historical snapshot and selects `ota_0`.
Therefore a proposed write to `0x00020000` would target the currently selected
and only observed app image, while `ota_1` has no fallback image header. This
is a material recovery risk, not authorization.

Compatibility remains **B — PLAUSIBLE BUT NOT PROVEN** because:

- vendor modifications in `v5.5.3-dirty` remain unavailable;
- the preserved Bootloader has not actually loaded the v5.5.4 DOUT candidate;
- current security/anti-rollback state was not reviewed in this authorization;
- the ancillary eFuse/OTP read-scope deviation requires explicit review;
- no observation window, First Flash human decision, or operation-specific
  write authorization exists.

Task 3.4 remains incomplete.

## 15. Persistent-Write Audit

- RAM stub uploaded: no.
- Flash program: none.
- Flash erase: none.
- Flash restore: none.
- eFuse write: none.
- Bootloader/table/OTA/NVS/app/configuration mutation: none.
- Persistent device-configuration modification: none.
- Monitor: none.
- Firmware modification or build: none.
- Persistent write identified: **none**.

The operations had transient reset, ROM-loader, register, SPI-attachment, Flash
reset, and connection-state effects. The final hard reset completed.

## 16. Final Safety State

- Current snapshot authorization: **CONSUMED AND CLOSED**
- Further device access authorization: **NONE**
- Device access authorization: **CONSUMED AND CLOSED / NONE**
- Firmware Implementation Gate: **HOST-ONLY IMPLEMENTATION COMPLETED**
- First Flash: **NO-GO**
- Flash authorization: **NONE**
- Candidate app execution: **NOT AUTHORIZED / NOT TESTED**
