# PCB V1.0 First Flash Attempt — 2026-07-26

> **STOPPED**
>
> **INCONCLUSIVE**
>
> **RANGE-SCOPE DEVIATION**
>
> **DO NOT CONTINUE WITHOUT NEW AUTHORIZATION**

- Record time: 2026-07-26T23:54:55+08:00
- Device: ESP-VoCat PCB V1.0
- Port: `COM7`
- Result: **STOPPED / INCONCLUSIVE — RANGE-SCOPE DEVIATION**
- Write invocation count: **1**
- Automatic retry: **NONE**
- Serial observation: **NOT PERFORMED**
- Rollback: **NOT PERFORMED / NOT AUTHORIZED**
- Device/Flash authorization: **CONSUMED AND CLOSED / NONE**

## 1. Scope

The user explicitly authorized one App-only First Flash of:

- artifact:
  `D:\ESP-VoCat_First_Flash_Packages\2026-07-26\pcb_v1_first_flash_smoke_test.bin`;
- size: 160832 bytes / `0x00027440`;
- SHA-256:
  `1B72A60DE9C9BB42DE401A58D7772B0525AFBC4DC3D8C85BB550D2D758F99DDC`;
- start: `0x00020000`;
- candidate end-exclusive: `0x00047440`;
- reviewed sector erase end-exclusive: `0x00048000`;
- ROM loader, no stub, no compression, preserved header, one post-write MD5;
- exactly one outer, block, connection, port-open, sync, and reset-reopen
  attempt.

The exact final authorizing clause was:

> 我明确授权：
>
> 对上述精确候选文件、精确地址和精确范围执行一次且仅一次 App-only 写入，并在写入后执行一次启动观察。
>
> 本授权仅适用于本消息中列出的设备、端口、文件、哈希、范围和操作。任何字段变化都会使本授权立即失效。

The authorization excluded automatic retry, rollback, full recovery,
`read_flash`, independent erase, bootloader/table/data-partition writes,
eFuse/security changes, RAM stub, compression, and any substituted port or
artifact.

The one-time external authorization JSON was deleted after the operation. The
authorization is consumed and closed, is not present in the repository or
staging package, and cannot be reused.

## 2. Preflight

Before opening the serial port:

- Windows returned exactly one `COM7` record, status `OK`, with no alternate
  port selection;
- all four immutable 32 MiB recovery files were 33554432 bytes and matched
  SHA-256
  `72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001`;
- both D: and E: staging packages contained exactly the reviewed three files;
- both manifests, candidates, and complete raw `ota_0` rollback artifacts
  validated;
- Python syntax checks passed for the harness and host test;
- 16/16 host tests passed;
- harness audit and self-test passed;
- Python 3.13.9, esptool 4.12.dev3, executable hash, complete source-tree hash,
  six relevant source hashes, source structure, and installation integrity
  matched;
- all six controlled attempt counts were enforced as 1;
- port enumeration remained denied;
- the sensitive-output filter self-test passed;
- the external authorization JSON passed the harness's complete offline
  validation.

No serial port was opened during these checks. The first Windows management
query timed out without returning a result; a second exact-filter query
returned the single `COM7` record. Neither query opened a serial port.

The later authorized esptool connection matched the reviewed target:

- exact port: `COM7`;
- chip: ESP32-S3;
- package: QFN56;
- revision: v0.2;
- USB mode: USB Serial/JTAG;
- identity comparison: **MATCHED the authorized connection target**.

The real base MAC and other unique identifiers were filtered before display
and were not saved.

## 3. Executed device operation

Classification: `WRITE`.

The repository harness was invoked once in device mode with the external
authorization file. It selected exact `COM7`, exact chip `esp32s3`, ROM
loader, `--no-stub`, no compression, preserved Flash header values, no
post-write reset, and the candidate at `0x00020000`.

Sanitized decisive output:

```text
esptool.py v4.12.dev3
Serial port COM7
Connecting...
Chip is ESP32-S3 (QFN56) (revision v0.2)
Features: WiFi, BLE, Embedded PSRAM 16MB (AP_1v8)
Crystal is 40MHz
USB mode: USB-Serial/JTAG
MAC: [REDACTED MAC]
Enabling default SPI flash mode...
Configuring flash size...
Flash will be erased from 0x00020000 to 0x00047fff...
Erasing flash...
Took 0.80s to erase flash block
Writing at 0x00020000... (0 %)
...
Writing at 0x00047400... (100 %)
Wrote 161792 bytes at 0x00020000 in 1.9 seconds (684.8 kbit/s)...
Hash of data verified.
Leaving...
Staying in bootloader.
```

The harness returned exit code 0, reported one authorized operation completed,
and confirmed that the installed package remained unchanged. This proves only
that esptool completed its single invocation and that its MD5 comparison over
the 160832-byte candidate image succeeded.

Reviewed and reported geometry:

| Meaning | Start | End-inclusive | End-exclusive | Length |
|---|---:|---:|---:|---:|
| sector erase envelope | `0x00020000` | `0x00047FFF` | `0x00048000` | `0x00028000` / 163840 bytes |
| candidate image | `0x00020000` | `0x0004743F` | `0x00047440` | `0x00027440` / 160832 bytes |
| esptool transmitted blocks | `0x00020000` | `0x000477FF` | `0x00047800` | `0x00027800` / 161792 bytes |

## 4. Stop-condition trigger

The reported 161792 bytes exceed the authorized 160832-byte candidate by 960
bytes.

Installed esptool 4.12.dev3 source confirms:

- `cmds.py` pads the input to four-byte alignment; this did not change the
  already four-byte-aligned candidate length;
- `flash_begin()` receives `uncsize = 160832`;
- the ROM Flash block size is 1024 bytes;
- the no-stub write loop slices 1024-byte `FLASH_WRITE_SIZE` blocks;
- the final 64-byte candidate block is padded with 960 `0xFF` bytes;
- `flash_block()` sends the complete 1024-byte final block;
- the displayed `bytes_written` therefore becomes 161792;
- post-write MD5 uses `uncsize`, so it covers only the authorized 160832
  candidate bytes, not the 960-byte block padding.

Resulting ranges:

| Meaning | Range |
|---|---|
| authorized candidate bytes | `0x00020000-0x0004743F` |
| additional transmitted `0xFF` padding | `0x00047440-0x000477FF` / 960 bytes / `0x000003C0` |
| remaining erased-only tail | `0x00047800-0x00047FFF` |
| total sector erase envelope | `0x00020000-0x00047FFF` |

Although the padding value was `0xFF` and remained within the disclosed erase
envelope, the authorization explicitly described
`0x00047440-0x00047FFF` as erased but not covered by the candidate. The 960
transmitted bytes therefore trigger the user's stop condition for possible
contact with an unauthorized range.

The whole padding range lies inside the authorized sector erase envelope, but
that does not make it conform to the exact candidate-image authorization. The
separate range `0x00047800-0x00047FFF` was not covered by transmitted data and
was affected only by the preceding sector erase.

`Hash of data verified` proves that the bytes esptool included in its
160832-byte MD5 comparison matched the candidate data. It does not prove that
the preserved original bootloader can start the candidate App, that the USB
Serial/JTAG application console works, that runtime is stable, or that
OpenSpec Task 3.4 is complete.

## 5. Actions after the stop

- no second write or block retry was performed;
- no automatic or manual write retry was performed;
- no startup reset was performed;
- the 60-second serial observation was not performed;
- no observation serial port was opened and no monitor was run;
- no `read_flash` or independent verify read was performed;
- no independent erase, rollback, restore, or recovery was performed;
- no command was issued to modify `otadata`, bootloader, partition table, NVS,
  `ota_1`, assets, or eFuse;
- the last observed device state is **STAYING IN ROM BOOTLOADER**;
- `ota_0` received the one write and corresponding sector erase described
  above;
- boot success, ready marker, runtime stability, and original-firmware behavior
  are all **UNOBSERVED**;
- Level 1 and Level 2 remain separately gated and unauthorized.

No conclusion is made that the device is damaged, and no conclusion is made
that it can boot normally.

## 6. Privacy and evidence limits

The base MAC was filtered in memory before display. No MAC, unique chip ID,
credential, token, NVS value, raw device dump, recovery content, or
authorization JSON was added to the repository. This record contains only
sanitized connection facts and host-source analysis.

No claim is made that bootloader, partition table, NVS, `otadata`, `phy_init`,
`ota_1`, assets, or upper 16 MiB changed. The command and reported erase range
did not target those regions, but no post-write `read_flash` was authorized or
performed.

## 7. Decision

The attempt is **STOPPED / INCONCLUSIVE** and does not satisfy serial
smoke-test acceptance. The write tool reported success for its one invocation,
but the operation failed the exact-range conformance requirement. Further
device work requires a new review and explicit user direction. No rollback is
implied or authorized by this record.

Current safety state:

| Item | State |
|---|---|
| First Flash attempt | `STOPPED / INCONCLUSIVE` |
| Runtime validation | `NOT PERFORMED` |
| Compatibility | `B — PLAUSIBLE BUT NOT PROVEN` |
| Task 3.4 | `NOT COMPLETED` |
| Device state | `STAYING IN ROM BOOTLOADER AT LAST OBSERVED STATE` |
| Device access authorization | `CONSUMED AND CLOSED / NONE` |
| Flash authorization | `CONSUMED AND CLOSED / NONE` |
| Startup-reset authorization | `NONE` |
| Observation authorization | `NONE` |
| Rollback authorization | `NONE` |
| Further automatic action | `NO-GO` |

## 8. Later host-only padding and next-action assessment

A later pure-host review, with no device access, is recorded in
`docs/hardware/pcb-v1-post-flash-padding-and-next-action-assessment.md`.

The verified historical full backup shows:

| Historical range | Length | Non-`0xFF` bytes | SHA-256 |
|---|---:|---:|---|
| padding-corresponding `0x00047440-0x000477FF` | 960 | 960 | `4823FFB7303903B42D1653C1B1F76DF54004658877E57ED6CF5285DB0BB43E0F` |
| erase-only `0x00047800-0x00047FFF` | 2048 | 2048 | `F3F93D0ED3E703DB10A6680DCA1007A57F5A5B2F7ED7422E480029D17B69CCB1` |

Both ranges were inside the valid historical Xiaozhi App image. The recorded
sector erase therefore intended to remove those old bytes before final-block
padding. The later `0xFF` payload requests no additional logical NOR
main-array `1`-to-`0` data-bit change over the erased state, but actual
readback, internal ECC/metadata, wear, disturb, and other physical effects
remain unverified.

The candidate's valid checksum and appended-hash boundary ends exactly at
absolute `0x00047440`; the 960-byte transport padding and 2048-byte erased
tail are not part of the candidate. The standard non-Secure-Boot simple-hash
path ignores those bytes; a Secure Boot v2 enforcement path can hash the
`0xFF` sector alignment and then expect a signature block, with preserved
vendor configuration still unverified. The candidate startup closure has no
intentional partition/NVS/OTA/core-dump write path. Normal startup does
perform volatile MXIC Flash configuration-register initialization, and the
standard second-stage path conditionally clears non-volatile BP bits if an
unexpected protected state is present. Prior normal boots strongly support an
already-clear BP state, but it was not reread, and the exact vendor-dirty
bootloader remains unavailable for source audit.

The host-only recommendation is
**RECOMMEND STARTUP-ONLY AUTHORIZATION REVIEW**. It is not an authorization
and no reset or observation followed. This attempt remains
**STOPPED / INCONCLUSIVE**; Task 3.4 remains **NOT COMPLETED**; all current
device, Flash, startup/observation, and rollback authorization remains
`NONE`.

## 9. Final Host-Only Startup Review

The recommendation was reviewed without device access on 2026-07-27. The
controlling document is
`docs/hardware/pcb-v1-startup-only-authorization-review.md`.

Decision: **READY TO REQUEST STARTUP-ONLY AUTHORIZATION**.

The review is **UNSIGNED / NOT AUTHORIZED / DO NOT EXECUTE**. No port was
enumerated or opened, no reset or observation occurred, and no Flash,
readback, rollback, or restore operation followed. The fail-closed observer
was tested only with host fakes.

This attempt remains **STOPPED / INCONCLUSIVE**. Compatibility remains
**B — PLAUSIBLE BUT NOT PROVEN**. Task 3.4 remains **NOT COMPLETED**.
Device access, startup/observation, Flash, and rollback authorization all
remain `NONE`.
