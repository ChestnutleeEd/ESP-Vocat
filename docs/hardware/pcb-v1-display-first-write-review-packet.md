# PCB V1.0 Display Validation — Exact First-Write Review Packet

Date: 2026-09-20

Change: `validate-pcb-v1-display-backlight-on-hardware`

> **RESULT: PASS — PACKET REVIEWED, NOT EXECUTED**
>
> **DEVICE EXECUTION: NOT AUTHORIZED**
>
> **HARDWARE/DISPLAY STATUS: UNVERIFIED**

## 1. Scope and command classification

This record completes only OpenSpec task 7.3: construction and offline review
of one exact future App-only write packet. It does not authorize or perform
that packet.

Future command classification: `WRITE`.

- What it would do: connect once to exact `COM7` as ESP32-S3, use the ROM
  loader without a stub or compression, sector-erase only the range required
  by one App image at `0x00020000`, transmit that image plus the reviewed final
  ROM-block padding, perform the built-in plaintext MD5 check, and stay in the
  ROM bootloader.
- Why it would be necessary: install the fixed low-duty GPIO44 display
  validation candidate for a later, separately authorized observation.
- Persistent effect: replace the reviewed prefix of preserved `ota_0`; no
  bootloader, partition-table, NVS, OTA metadata, PHY, second OTA slot, assets,
  upper-Flash, eFuse, security, or voltage write is included.
- Success judgment: one invocation returns success, reports only the reviewed
  erase/write geometry, and reports `Hash of data verified.` for the semantic
  plaintext candidate range.
- Stop result: any connection, chip, security, erase, write, MD5, transport,
  reset, or geometry anomaly closes the attempt with no retry or follow-up.

No `write_flash`, `erase_flash`, `erase_region`, `read_flash`, monitor, reset,
startup, display observation, rollback, restore, or eFuse command was executed
while preparing this record.

## 2. Git baseline and source identity

Initial reviewed baseline:

- branch: `feat/validate-pcb-v1-display-backlight-on-hardware`;
- HEAD: `a809c6f45c51682c20c0eb3d0e99ea7df33ac2c9`;
- upstream: `origin/feat/validate-pcb-v1-display-backlight-on-hardware`;
- initial ahead/behind: `0/1` in upstream-first order, meaning local was ahead
  by one commit;
- worktree: clean.

The existing VDD_SPI evidence commit was pushed without creating another
commit. Local and upstream HEAD then both equaled `a809c6f45c51682c20c0eb3d0e99ea7df33ac2c9`,
ahead/behind was `0/0`, and the worktree remained clean.

The latest firmware-changing commit is
`019e98f7b367b45f2b2d4a52aef0757930a8aad8`. There is no firmware diff from
that commit through the reviewed baseline. All nine manifest source inputs
match their recorded sizes and SHA-256 values. Their independently recomputed
aggregate is:

`E9928883A0E81503537BC5444A21358FEABF69FFFD5D200527A4177050FDDD32`

## 3. Exact candidate

Canonical path:

`D:\ESP-VoCat-Project\custom-vocat\firmware\build-backlight-repro-a-20260920-r1\pcb_v1_minimal_display_smoke_test.bin`

- bytes: `231360` / `0x000387C0`;
- SHA-256:
  `FEE9C3D1AA77CD0DF1B2C2D55A86B7BEAA2871E5E8443B9D458B10324FF8F362`.

The independent Build B copy has the same size and SHA-256. The candidate
matches the committed successor manifest, the nine recorded source inputs,
and the host-validated source state. The predecessor remains hard-disabled,
`not-for-visual-validation`, and distinct at SHA-256
`4E66B7EDCD38B5225B00E1DB790CA7DD9C842E765961E8929228105F9C10A05D`.

## 4. Recovery gate rehash

All six established assets were rehashed again from their exact offline paths:

| Asset group | Copies | Bytes each | SHA-256 | Result |
|---|---:|---:|---|---|
| immutable same-device 32 MiB full image | 4 | 33554432 | `72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001` | PASS |
| complete original `ota_0` | 2 | 4128768 | `C8A2FE4AB0F9B7C823F1DCFDFC926682C9DBF1B079056461DC6F18A93A345D0E` | PASS |

The exact paths remain those recorded in
`tests/hardware/pcb-v1-pre-write-physical-identity-gate-2026-09-20.md`.
No recovery asset was modified. Level 1 `ota_0` restore and Level 2 full-image
restore remain separate future `WRITE` operations and are not authorized by
this packet.

## 5. esptool audit and selected mechanism

Reviewed invocation environment:

- Python path used by the future packet:
  `C:\Espressif\tools\python\v5.5.4\venv\Scripts\python.exe`;
- canonical resolved Python/package volume: `E:\Espressif\tools\python\v5.5.4`;
- Python: `3.13.9`;
- esptool: `4.12.dev3` / CLI banner `v4.12.dev3`;
- installed source-tree SHA-256:
  `7BA6E754C34E202DEB9EF5C6E24376CDA93F24BD4DDB3B70E392BBA6CD5C7610`;
- esptool console executable SHA-256:
  `A1F90587C195E8C824FE30D80B2D6062CE244CD346118502F08AFDC4496F8AF4`.

The ESP-IDF v5.5.4 component script at
`D:\esp\v5.5.4\esp-idf\components\esptool_py\esptool\esptool.py` reports the
same `4.12.dev3`. No other installed Espressif Python environment or stable
esptool installation was found. No tool was installed, upgraded, or silently
substituted.

The known VDD_SPI defect is limited to the inherited `flash_id`
`get_flash_voltage()` display/decoding decision order. `write_flash()` does
not call that helper and does not branch on its text. The defect therefore
does not change write address, erase calculation, image bytes, ROM block
padding, security checks, or post-write MD5 semantics.

Stock CLI alone is rejected for this packet because its audited defaults are:

- whole write attempts: 2;
- per-block attempts: 3;
- connection attempts: 7;
- port-open attempts: 1;
- sync transmissions per connection: 5;
- reset/reopen attempts: 3.

Only connection attempts have a direct CLI limit. The reviewed packet
therefore uses
`tools/display_validation/esptool_one_attempt.py`, which validates the exact
visible esptool argument vector and reuses the already audited process-local
override mechanism to force every listed count to one. It also requires a
fresh external authorization document, exact synchronized Git identity,
candidate identity, and all six recovery hashes before importing device
execution logic. Unknown tool source, altered arguments, absent authorization,
or evidence mismatch fails closed before a serial open.

## 6. Independent geometry calculation

Inputs used for the independent calculation:

- start: `0x00020000`;
- candidate length: `231360` / `0x000387C0`;
- ROM no-stub block size: `1024` / `0x400`;
- Flash sector size: `4096` / `0x1000`.

Results:

| Scope | Range / value |
|---|---|
| semantic payload | `[0x00020000,0x000587C0)` |
| ROM block count | 226 |
| transport | `[0x00020000,0x00058800)` / 231424 bytes |
| final transport block | `[0x00058400,0x00058800)` |
| final semantic bytes in that block | 960 |
| final `0xFF` transport padding | 64 bytes at `[0x000587C0,0x00058800)` |
| sector erase envelope | `[0x00020000,0x00059000)` |
| erased byte count | 233472 / `0x00039000` |
| affected sectors | decimal 32 through 88 inclusive / `0x20` through `0x58` |
| erased tail after semantic payload | 2112 bytes |
| erased-only tail after transported data | 2048 bytes |

All ranges are contained in preserved original
`ota_0 = [0x00020000,0x00410000)`. The operation does not overlap:

- bootloader `[0x00000000,0x00008000)`;
- partition table `[0x00008000,0x00009000)`;
- NVS `[0x00009000,0x0000D000)`;
- `otadata` `[0x0000D000,0x0000F000)`;
- PHY data `[0x0000F000,0x00010000)`;
- `ota_1` `[0x00410000,0x00800000)`;
- assets `[0x00800000,0x01000000)`;
- upper Flash `[0x01000000,0x02000000)`.

The remaining `ota_0` begins at `0x00059000` and is untouched by the reviewed
erase envelope.

## 7. Offline image compatibility

Extended `image_info` inspection with the exact installed tool reports:

- ESP32-S3 image version 1, chip ID 9;
- revision range v0.0 through v0.99, containing observed silicon v0.2;
- six bounded segments and entry point `0x403754D4`;
- valid checksum `0xDE`;
- valid validation hash
  `b1f1cff8ac37531bfd8698d64a3eae798c7faddec457a3943c16062b0207e59c`;
- DOUT, 80 MHz, 16 MB header metadata;
- ESP-IDF v5.5.4, secure version 0, 64 KiB MMU page size.

The complete original `ota_0` is also a valid ESP32-S3 image and records DIO,
80 MHz, 16 MB. The candidate's DOUT setting is intentional, is unchanged from
the predecessor minimal candidate, and the preserved boot chain previously
loaded that DOUT candidate in the bounded 2026-07-27 startup observation.
This is compatibility evidence, not physical display validation.

The candidate starts exactly at preserved `ota_0` offset `0x00020000`; no
generated factory partition table is used or written. `--flash_mode keep`,
`--flash_freq keep`, and `--flash_size keep` are explicit. At an App offset,
installed esptool does not rewrite image header bytes; only bootloader-offset
images are eligible for that update path.

Available security evidence is sufficient for packet review because the
repository's controlling profile records Secure Boot and Flash Encryption
disabled, download mode enabled, USB Serial/JTAG enabled, the preserved
bootloader already started the earlier plaintext DOUT App, and no eFuse or
security mutation has occurred. The future authorization must bind those
states. The exact command contains no `--force`, encryption option, eFuse
operation, or security bypass; installed esptool performs live chip/revision
and security-derived checks before erase/write. Any security rejection or
contradiction is an immediate stop, not permission to override it.

## 8. Frozen future transport policy

| Field | Exact policy |
|---|---|
| chip | `esp32s3` |
| port | `COM7`; no scan or fallback |
| baud | 115200 |
| before | `usb_reset` for reviewed USB Serial/JTAG transport |
| after | `no_reset`; remain in ROM bootloader |
| outer/block/connect/open/sync/reset-reopen attempts | 1 each |
| loader | ROM only, `--no-stub` |
| compression | disabled, explicit `--no-compress` |
| progress | disabled to reduce output noise |
| Flash header flags | mode/frequency/size `keep` |
| encryption / force | absent |
| write offset | `0x20000` |
| images | exactly one App BIN |
| verification | built-in one semantic-range plaintext ROM MD5 comparison; no separate readback or second command |
| post-write action | stop; no monitor, startup, observation, rollback, or restore |

`--verify` is not included. In this installed version the plaintext
`write_flash()` path unconditionally requests and compares the ROM MD5 over
the semantic image length; a separate verification command would widen the
operation sequence without improving the reviewed one-command boundary.

## 9. Exact future write packet

`NOT AUTHORIZED — REVIEW ONLY`

```powershell
& 'C:\Espressif\tools\python\v5.5.4\venv\Scripts\python.exe' 'D:\ESP-VoCat-Project\custom-vocat\tools\display_validation\esptool_one_attempt.py' --device --authorization 'D:\ESP-VoCat_First_Write_Authorizations\pcb-v1-display-validation-authorization.json' -- --chip esp32s3 --port COM7 --baud 115200 --before usb_reset --after no_reset --no-stub --connect-attempts 1 write_flash --flash_mode keep --flash_freq keep --flash_size keep --no-compress --no-progress 0x20000 'D:\ESP-VoCat-Project\custom-vocat\firmware\build-backlight-repro-a-20260920-r1\pcb_v1_minimal_display_smoke_test.bin'
```

This is the only reviewed future write command. It has no alternative port,
baud, candidate, fallback, retry command, second image, monitor, erase command,
or recovery command.

The external authorization path is exact, not a placeholder. The file does
not exist in this review and must not be created until the future task obtains
fresh explicit human authorization. Until then, device mode refuses before
execution. Authorization material is forbidden inside the repository.

## 10. Future execution contract

Immediately before any future execution, the operator and guard must recheck:

1. exact branch, full HEAD, synchronized upstream `0/0`, and clean tree;
2. candidate path, 231360-byte size, SHA-256, and unchanged manifest source
   inputs;
3. all four full-image and both complete `ota_0` recovery paths, sizes, and
   hashes;
4. fresh exact `COM7` endpoint identity as this PCB V1.0 ESP32-S3 over USB
   Serial/JTAG, with 32 MiB Flash and VDD_SPI forced 1.8 V evidence;
5. Secure Boot disabled, Flash Encryption disabled, download mode enabled,
   and no new security contradiction;
6. exact esptool/Python/source hashes and every attempt count equal to one;
7. an external JSON authorization at the exact path with schema
   `esp-vocat-display-first-write-authorization-v1`, status
   `EXPLICIT HUMAN FLASH AUTHORIZATION`, the fresh full HEAD, exact packet
   fields, substantive verbatim user statement, risk acknowledgement, and
   explicit denial of retry, rollback, startup, and observation authority.

Only the command in section 9 may mutate Flash. If it reports connection
failure, unexpected chip/port, security rejection, erase/write error, MD5
mismatch, reset/transport anomaly, or any geometry deviation: stop. Do not
retry, change baud, change port, erase, restore, start the application, or
observe the display under the consumed write authority.

Successful return also stops the write task. `after=no_reset` leaves the
device in the ROM bootloader. Any later startup is a separate `REBOOT` gate;
serial/display observation is a separate `READ-ONLY` gate. Neither is
authorized by this packet.

## 11. Offline validation results

- candidate and recovery rehash: PASS;
- exact CLI help/source audit: PASS;
- candidate and original `ota_0` extended image inspection: PASS;
- independent geometry calculation: PASS;
- legacy retry-guard audit/self-test: PASS;
- legacy guard tests: 16/16 PASS;
- display packet tests: 5/5 PASS;
- complete host unittest collection: 61/61 PASS;
- complete host pytest collection: 61/61 PASS;
- display packet offline audit: PASS, serial open zero, device access none;
- exact visible esptool argument vector: PASS;
- enforced attempt counts: 1/1/1/1/1/1.

One preliminary `python -m unittest tests\\host\\...` invocation failed before
collection because the Windows path was interpreted as a Python module. Zero
tests ran in that invocation. Running the test file directly under the exact
interpreter then passed all 16 legacy guard tests. The fake test's printed
erase text was simulated host output and did not access hardware.

## 12. Disposition

OpenSpec task 7.3 is complete as: exact packet constructed and reviewed, but
not executed. Tasks 7.4, 7.5, all startup/display observation tasks, and all
recovery execution tasks remain open. Hardware/display status remains
`UNVERIFIED`; device execution remains `NOT AUTHORIZED`.

`PASS — EXACT FIRST-WRITE PACKET REVIEWED; EXECUTION REQUIRES FRESH EXPLICIT AUTHORIZATION`
