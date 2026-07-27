# PCB V1.0 First Flash Operation Readiness

> **HOST-ONLY REVIEW**
>
> **NOT AUTHORIZED**
>
> **DO NOT EXECUTE**

- Review date: 2026-07-26
- OpenSpec Change: `prepare-pcb-v1-first-flash-smoke-test`
- Device access performed in this round: **NONE**
- Flash operation performed in this round: **NONE**
- Compatibility: **B — PLAUSIBLE BUT NOT PROVEN**
- Task 3.4: **NOT COMPLETED**
- First Flash: **NO-GO**
- Device access authorization: **NONE**
- Flash authorization: **NONE**
- Flash Authorization Readiness:
  **READY FOR EXPLICIT HUMAN FLASH AUTHORIZATION**

This document contains reviewed fields and source findings, not an executable
device command.

## 1. Executive Summary

The existing ignored application artifact was independently rehashed and
inspected without rebuilding. Two complete `ota_0` slices were independently
extracted from the two verified D: full backups. They matched and were staged,
together with the candidate, in isolated D: and E: package directories outside
the immutable recovery directories.

The candidate image bytes occupy `0x00020000-0x0004743F`. The sector erase
envelope is larger: `0x00020000-0x00047FFF`. Both ranges remain wholly inside
the preserved `ota_0` partition, but the envelope would erase 3008 original
bytes after the candidate image. This distinction is material and is included
in the rollback plan.

All host-side artifact, geometry, ancillary-read, header-preservation,
observation, rollback, and single-attempt execution inputs are now reviewed.
Stock esptool v4.12.dev3 still defaults to two whole-operation attempts and
three attempts for a failed data block, but the repository-owned fail-closed
harness now locks the exact audited installation and forces whole-operation,
block, connection, port-open, sync, and reset-reopen counts to one before any
future serial open. Sixteen pure-host tests passed, including actual one-call
assertions for synthetic outer, block, and sync failures. The installed
esptool package remained unchanged.

## 2. Current Authorization Status

| Item | Status |
|---|---|
| Host-only staging | completed |
| Device access authorization | `NONE` |
| Flash authorization | `NONE` |
| First Flash | `NO-GO` |
| Level 1 rollback authorization | `NONE` |
| Level 2 recovery authorization | `NONE` |
| Operation packet | host-reviewed, non-executable |

The historical `COM7` value is recorded only as a future input requiring
operation-time reconfirmation. It is not authorized by this document.

## 3. Evidence Baseline

- Repository branch:
  `feat/prepare-pcb-v1-first-flash-smoke-test`.
- Pre-write repository and upstream HEAD:
  `8aa5f3c36a9158d12aef8b1c9615a1e3df1858fe`.
- Pre-write ahead/behind: `0/0`.
- Pre-write worktree: clean.
- Initial OpenSpec progress: `30/48`.
- Initial Change strict validation: passed.
- Initial repository-wide strict validation: 2 passed, 0 failed.
- The current boot-chain snapshot remains byte-identical to the historical
  bootloader, partition table, `otadata`, and OTA header windows.
- The snapshot's ancillary read-scope deviation remains explicitly recorded
  and is not weakened by this review.

## 4. Candidate Artifact Manifest

| Field | Reviewed value |
|---|---|
| source | `firmware/build/pcb_v1_first_flash_smoke_test.bin` |
| size | `0x00027440` / 160832 bytes |
| SHA-256 | `1B72A60DE9C9BB42DE401A58D7772B0525AFBC4DC3D8C85BB550D2D758F99DDC` |
| target | ESP32-S3 |
| image version | 1 |
| entry | `0x403752D0` / 1077367504 |
| segments | 6 |
| header | DOUT / 80 MHz / 16 MB |
| secure version | 0 |
| checksum | valid |
| appended hash | valid |

The candidate remained Git-ignored. No configure or build operation ran.

## 5. Rollback Artifact Manifest

| Field | Reviewed value |
|---|---|
| filename | `original_xiaozhi_ota_0_full_partition.bin` |
| source ranges | two independent D: full-backup extractions |
| source offset | `0x00020000` / 131072 |
| source length | `0x003F0000` / 4128768 bytes |
| end-exclusive | `0x00410000` / 4259840 |
| SHA-256 | `C8A2FE4AB0F9B7C823F1DCFDFC926682C9DBF1B079056461DC6F18A93A345D0E` |
| source full-backup SHA-256 | `72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001` |

The two independent D: extractions matched in size and SHA-256 before either
was copied into a staging package.

## 6. Cross-Volume Package Verification

Package roots:

- `D:\ESP-VoCat_First_Flash_Packages\2026-07-26`
- `E:\ESP-VoCat_First_Flash_Packages\2026-07-26`

Each directory contains exactly:

1. `pcb_v1_first_flash_smoke_test.bin`
2. `original_xiaozhi_ota_0_full_partition.bin`
3. `FIRST_FLASH_PACKAGE_MANIFEST.txt`

The D: and E: candidate copies match in size and SHA-256. The D: and E:
rollback copies match in size and SHA-256. The manifests are path-specific
because their package-root fields differ; their safety state and binary
metadata are equivalent.

D: and E: are distinct volume paths. This establishes cross-volume staging
redundancy but does not prove separate physical disks. The staging directories
are isolated from the original backup directories. They are not long-term
trusted recovery sources; the immutable full backups remain the highest-level
recovery assets.

Package creation timestamp: `2026-07-26T20:39:02+08:00`.

## 7. Candidate Image Range

| Field | Hexadecimal | Decimal |
|---|---:|---:|
| write start | `0x00020000` | 131072 |
| image length | `0x00027440` | 160832 |
| end-exclusive | `0x00047440` | 291904 |
| last image byte | `0x0004743F` | 291903 |
| containing partition remaining capacity | `0x003C8BC0` | 3967936 |

This is the image-byte range, not the erase envelope.

## 8. Sector Erase Envelope

The ESP32-S3/esptool sector size is `0x00001000` / 4096 bytes.

| Field | Hexadecimal | Decimal |
|---|---:|---:|
| aligned erase start | `0x00020000` | 131072 |
| aligned erase end-exclusive | `0x00048000` | 294912 |
| aligned erase length | `0x00028000` | 163840 |
| first affected sector | `0x20` | 32 |
| last affected sector | `0x47` | 71 |

The image length is four-byte aligned, so the no-stub path adds no end padding
beyond its ordinary four-byte preparation. The final sector nevertheless
extends 3008 bytes past the image:
`0x00047440-0x00047FFF` / 291904-294911. Those original `ota_0` bytes would be
erased by a future write.

## 9. Partition Containment

The three relevant nested ranges are:

| Range | Start | End-exclusive | Length |
|---|---:|---:|---:|
| image bytes | `0x00020000` / 131072 | `0x00047440` / 291904 | `0x00027440` / 160832 |
| erase envelope | `0x00020000` / 131072 | `0x00048000` / 294912 | `0x00028000` / 163840 |
| original `ota_0` | `0x00020000` / 131072 | `0x00410000` / 4259840 | `0x003F0000` / 4128768 |

Both candidate ranges are wholly inside `ota_0`. They do not overlap NVS,
`otadata`, `phy_init`, `ota_1`, assets, the bootloader, or the partition table,
and they do not reach the upper 16 MiB.

## 10. Preserved Regions

A candidate write with the reviewed envelope preserves:

- bootloader/pre-table area:
  `0x00000000-0x00007FFF` / 0-32767;
- partition table:
  `0x00008000-0x00008FFF` / 32768-36863;
- NVS:
  `0x00009000-0x0000CFFF` / 36864-53247;
- `otadata`:
  `0x0000D000-0x0000EFFF` / 53248-61439;
- `phy_init`:
  `0x0000F000-0x0000FFFF` / 61440-65535;
- gap before `ota_0`:
  `0x00010000-0x0001FFFF` / 65536-131071;
- `ota_0` after the erase envelope:
  `0x00048000-0x0040FFFF` / 294912-4259839;
- `ota_1`:
  `0x00410000-0x007FFFFF` / 4259840-8388607;
- assets:
  `0x00800000-0x00FFFFFF` / 8388608-16777215;
- upper 16 MiB:
  `0x01000000-0x01FFFFFF` / 16777216-33554431.

## 11. Excluded Regions

The candidate operation excludes:

- full-device erase and full-image restore;
- bootloader and partition-table writes;
- NVS, `otadata`, `phy_init`, `ota_1`, and assets writes;
- upper-16-MiB access;
- eFuse, voltage, key, security, JTAG, USB, or download-mode changes;
- every image other than the staged candidate;
- monitor or rollback in the same operation.

## 12. esptool Connection Side Effects

The normal v4.12.dev3 connection path is not a narrow file write alone. Source
review found:

1. Serial-port open, DTR/RTS reset sequencing, ROM synchronization, and
   connection attempts.
2. The ROM security-info response, including security flags,
   `flash_crypt_cnt`, key-purpose codes, chip ID, and API/eco version.
3. ESP32-S3 eFuse-derived chip revision and package description.
4. eFuse-derived embedded Flash capacity/vendor and PSRAM capacity/vendor
   feature strings.
5. Base MAC reads from the eFuse/OTP register window.
6. A UART-device register read to identify USB Serial/JTAG or USB-OTG mode.
7. ESP32-S3 post-connect USB Serial/JTAG handling that may perform volatile
   watchdog-control register writes.
8. No-stub SPI attachment, Flash RDID, possible XMC SFDP/startup checks, Flash
   reset-enable/reset commands, and volatile ROM Flash-parameter setup.
9. Pre-write eFuse/security-derived checks for Secure Boot, chip revision,
   manual encrypted-download disable, and Flash Encryption state.
10. Post-write ROM MD5 calculation across the written byte range.
11. The selected post-operation reset behavior.

These operations do not by themselves write persistent Flash or eFuses, but
they exceed a literal authorization that mentions only the application bytes.

## 13. Ancillary Reads Requiring Future Authorization

A future authorization must explicitly include:

- exact-port open and any local port/PID enumeration needed by the observation
  tool;
- DTR/RTS entry-reset and observation-reset effects;
- ROM synchronization and security-info fields;
- chip ID, revision, package, and API/eco information;
- Secure Boot, Secure Download, Flash Encryption, encrypted-download-disable,
  JTAG/USB-disable, and key-purpose-derived status;
- embedded Flash/PSRAM capability and vendor fields;
- base MAC eFuse/OTP read and its privacy handling;
- USB-mode register read and volatile watchdog-control writes;
- Flash RDID, capacity detection, optional XMC SFDP checks, Flash reset
  commands, SPI attach, and volatile Flash parameter setup;
- post-write MD5 of the exact authorized range;
- final stay-in-bootloader state, subsequent monitor reset, and USB
  re-enumeration risk.

MAC and unnecessary unique values must not enter the repository or package.

## 14. Stub Decision

Decision: **ROM loader / no stub**.

- `--no-stub` is supported for ESP32-S3 `write_flash`.
- It avoids uploading and executing the RAM flasher stub.
- It is slower and transfers uncompressed 1 KiB write blocks.
- The ROM path erases the region up front; the stub normally manages erasing
  while writing.
- ROM MD5 remains available for ESP32-S3.
- Fewer RAM-stage dependencies do not prove greater hardware stability.
- It does not avoid security-info, eFuse/OTP/MAC, Flash-ID, SPI-attach, Flash
  reset, or volatile register behavior.

## 15. Compression Decision

Decision: **no compression**.

This is the default when no-stub is selected. It is slower but avoids the
compressed-transfer path and its additional behavior. The future operation
must not enable compression.

## 16. Header-Preservation Decision

Decision: **preserve the candidate header exactly**.

The future parameter values for Flash mode, frequency, and size remain
`keep`. Independently, v4.12.dev3 `_update_image_flash_params()` returns
without modifying an image unless its address equals the chip's bootloader
offset. The candidate address `0x00020000` is not the ESP32-S3 bootloader
offset `0x00000000`; therefore its DOUT / 80 MHz / 16 MB header is not
rewritten by this path.

No bootloader image is part of the packet.

## 17. Reset Decision

Write-stage decision:

- before connection: explicit `usb_reset` entry for the reviewed integrated
  USB Serial/JTAG transport;
- after the write result: `no_reset`, leaving the device in the ROM loader;
- no monitor is combined with the write.

The explicit USB reset selection avoids esptool's PID-discovery port
enumeration. The single-attempt harness also replaces both esptool port-list
entry points with fail-closed denials. A future authorization must state this
reset policy exactly.

Observation-stage decision:

- open the exact reconfirmed USB Serial/JTAG port at 115200;
- intentionally perform one monitor-start hard reset after the port is open;
- observe for the fixed window;
- do not send input or trigger another reset.

ESP-IDF Monitor source sets RTS/DTR low before opening, raises them after
opening, and resets by default unless no-reset is selected. It may wait and
reopen the same port if USB disappears. It does not perform the esptool chip
banner/eFuse/MAC reads, but it enumerates local serial ports to obtain the PID
used for reset selection.

## 18. Verification Decision

The future verification policy is:

1. rehash the staged candidate before authorization;
2. revalidate the exact size, address, image metadata, and envelope;
3. rely on v4.12.dev3's unconditional post-write ROM MD5 comparison for an
   unencrypted normal write;
4. preserve the actual MD5 result;
5. do not add a separate `verify_flash` read without separate review.

The `--verify` option exists in the v4.12.dev3 parser, but the reviewed
`write_flash()` implementation does not branch on it; the unconditional MD5
comparison is the effective write verification. Tool success proves only that
the transmitted range matched at verification time.

Timeout policy:

- no repository, user-home, or AppData esptool configuration file was present
  during this review;
- normal command timeout: 3 seconds;
- ROM region-erase timeout scaling: 30 seconds per MB, with the normal
  3-second minimum;
- MD5 timeout scaling: 8 seconds per MB, with the normal 3-second minimum;
- serial write timeout: 10 seconds;
- future connection attempts: exactly one;
- future serial-port open attempts: exactly one;
- any timeout is a stop condition and does not authorize a new invocation.

These timeout values must be rechecked if any esptool configuration file
appears before authorization.

## 18A. Single-Attempt Harness Decision

Selected mechanism: **B — process-local fail-closed override**.

Tracked harness:

`tools/first_flash/esptool_single_attempt.py`

Pure-host tests:

`tests/host/test_esptool_single_attempt.py`

Audit report:

`docs/hardware/pcb-v1-esptool-single-attempt-execution-mechanism.md`

The harness supports only the exact audited Python 3.13.9 / esptool
v4.12.dev3 installation and canonical package path. It validates the console
executable hash, complete Python source-tree digest, relevant source-file
hashes, retry-loop markers, function signatures, default constants, and
absence of an esptool config override. Unknown versions or structures are
rejected before device logic.

Effective attempts:

| Controlled path | Stock value | Harness value |
|---|---:|---:|
| whole write operation | 2 | 1 |
| failed data block | 3 | 1 |
| connection | 7 | 1 |
| initial port open | 1 | 1 |
| sync transmission per connection | 5 | 1 |
| reset reopen/reset sequence | 3 | 1 |

The process-local changes are restored on exit and the installed package is
rehashed afterward. No site-packages or executable file is modified.

No authorization file exists in this review. Device mode refuses without a
future external, independent authorization document containing the exact
device, port, artifact/hash/range/envelope, operation policy, recovery fields,
observation handoff, verbatim human statement, and acknowledged risks.

## 19. Observation Plan

- Interface: integrated USB Serial/JTAG console.
- Port field: `COM7`, **FUTURE RECONFIRMATION REQUIRED**.
- Baud: 115200.
- Window: 60 seconds from the intentional observation-start reset.
- Startup deadline: all fixed application lines and the ready marker within
  15 seconds.
- Expected frequency: one ordered startup sequence; no periodic application
  output afterward.
- USB disappearance/re-enumeration allowance: up to 15 seconds for the exact
  reviewed port to return; a different port invalidates the packet.
- Logging: no monitor file logging and no toggle-logging action. Only a
  sanitized observation record may enter Git.
- Input: none.
- End: close observation at 60 seconds if all criteria remain satisfied.

## 20. Expected Firmware Output

The exact application lines from `firmware/main/main.c`, in order, are:

```text
ESP-VoCat PCB V1.0 Smoke Test Candidate
Compile-time ESP-IDF version: 5.5.4
PCB target: ESP-VoCat PCB V1.0
Host-built candidate
Device execution not yet authorized
PSRAM intentionally disabled
No peripheral initialization
Ready marker: PCB_V1_SMOKE_TEST_CANDIDATE_READY
```

ROM and preserved-bootloader lines may precede them. The application then
enters a one-second delayed loop and emits no periodic line.

## 21. Success Criteria

Serial-only success requires:

- the exact candidate identity, version, target, disabled-state, and ready
  lines appear once and in order within 15 seconds;
- the session remains observable for the full 60 seconds;
- no repeated startup sequence, reset, panic, watchdog, allocation failure,
  invalid-image report, bootloader error, or ROM download-loop output occurs;
- the USB Serial/JTAG port remains available or returns on the same reviewed
  identity within the 15-second allowance.

Success would prove only bounded serial startup.

## 22. Stop Conditions

Stop before writing on any device, PCB, port, file, size, hash, offset, range,
envelope, partition, backup, package, tool-version, parameter, or authorization
mismatch.

Stop during or after a future operation on:

- any esptool failure or uncertain result;
- any automatic whole-image or block retry path;
- partial/uncertain write;
- missing application output or ready marker after 15 seconds;
- unexpected application text or repeated startup;
- panic, watchdog, memory-allocation failure, invalid image, checksum/hash
  error, anti-rollback rejection, or continuous ROM/download-mode output;
- USB disappearance longer than 15 seconds or reappearance under another
  port;
- any attempt to widen the write, erase all Flash, or start recovery without
  separate authorization.

## 23. App-Only Rollback Plan

Level 1 is the default rollback proposal:

- artifact:
  `original_xiaozhi_ota_0_full_partition.bin`;
- proposed start:
  `0x00020000` / 131072;
- proposed length:
  `0x003F0000` / 4128768;
- end-exclusive:
  `0x00410000` / 4259840;
- erase envelope:
  exactly `0x00020000-0x0040FFFF`;
- affected sectors:
  `0x20-0x40F` / 32-1039;
- expected SHA-256:
  `C8A2FE4AB0F9B7C823F1DCFDFC926682C9DBF1B079056461DC6F18A93A345D0E`.

Restoring the complete raw partition is more auditable than restoring only
the original app's effective image length: it reconstructs every captured
byte in `ota_0`, removes the candidate and all mixed/stale tail bytes, and
requires no inference about the vendor image's logical end.

Level 1 requires a fresh exact-port review, its own write authorization,
pre-write SHA-256 verification, revalidation of the same single-attempt
harness and installed-tool integrity, post-write MD5, a controlled reset, and
separate original-firmware boot observation.
It does not restore any other partition.

## 24. Full-Recovery Escalation

Level 2 is:

- **NOT AUTHORIZED**
- **HIGHER RISK**
- **SEPARATE REVIEW REQUIRED**

It may be considered only if Level 1 cannot restore original boot behavior and
a new independent review concludes that a cross-region failure requires the
same-device 32 MiB recovery image. It would overwrite private and stateful
regions, including NVS, OTA metadata, PHY data, and assets. No Level 2 command
is present here.

## 25. Residual Compatibility Risk

Compatibility remains **B — PLAUSIBLE BUT NOT PROVEN** because:

- the preserved bootloader is vendor-modified `v5.5.3-dirty`;
- those modifications are unavailable for source comparison;
- that exact bootloader has not started this v5.5.4 DOUT candidate;
- a current snapshot is not runtime compatibility proof;
- `ota_1` had no fallback image header at snapshot time.

## 26. Task 3.4 Status

Task 3.4 remains **NOT COMPLETED**. Only an actual preserved-bootloader start
of the candidate can close its remaining runtime-compatibility blocker.

## 27. Human Review Checklist

- [x] Candidate staged and cross-volume verified.
- [x] Full raw `ota_0` rollback staged and cross-volume verified.
- [x] Image bytes, sector envelope, and partition range distinguished.
- [x] All affected ranges contained in `ota_0`.
- [x] Ancillary reads and volatile connection effects identified.
- [x] Stub, compression, header, reset, verification, timeout, and observation
  policies reviewed.
- [x] Level 1 and Level 2 recovery scopes separated.
- [x] A reviewed execution mechanism enforces one whole-image attempt and one
  block attempt without modifying the installed esptool package.
- [ ] Exact device and `COM7` reconfirmed for the operation.
- [ ] Exact packet explicitly authorized.

## 28. Exact Future Authorization Fields

Any future authorization must state:

- exact physical ESP-VoCat unit and same-device recovery scope;
- PCB V1.0 and ESP32-S3;
- current exact port, with `COM7` reconfirmed rather than assumed;
- esptool v4.12.dev3 and ROM/no-stub policy;
- no-compression policy;
- repository harness version/source/integrity guards;
- exact staged candidate path, size, and SHA-256;
- image start, length, end-exclusive, last byte, and sector envelope;
- preserved and excluded regions;
- header mode/frequency/size preservation;
- explicit USB-reset/no-post-reset policy and separate monitor-start reset;
- built-in MD5 verification scope;
- all ancillary security/eFuse/OTP/MAC/Flash-ID/SFDP/register reads and
  volatile effects in Sections 12-13;
- privacy handling for MAC and security-derived output;
- 60-second observation window and 15-second startup/re-enumeration deadlines;
- stop conditions;
- reviewed one-shot enforcement with no whole-image or block retry;
- Level 1 rollback availability and its separate-authorization requirement;
- risks acknowledged, including selected-slot loss and compatibility class B.

Generic continuation language is not authorization.

## 29. Go/No-Go Decision

Flash Authorization Readiness:
**READY FOR EXPLICIT HUMAN FLASH AUTHORIZATION**.

Reason: the exact-version, exact-source process-local harness forces all
reviewed operation, block, connection, open, sync, and reset retry paths to
one, refuses missing authorization and artifact/range/hash mismatches before
serial logic, denies port enumeration, filters sensitive connection output,
and passed all 16 pure-host tests. The installed esptool package and executable
remained unchanged. No unresolved host execution-parameter blocker remains.

This readiness state does not create an authorization packet, select a current
port, grant device access, or change the First Flash decision.

Independent of readiness:

- Compatibility: **B — PLAUSIBLE BUT NOT PROVEN**
- Task 3.4: **NOT COMPLETED**
- First Flash: **NO-GO**
- Device access authorization: **NONE**
- Flash authorization: **NONE**

## 30. Claims Explicitly Not Made

- The candidate has not run on PCB V1.0.
- The preserved bootloader is not proven to accept it.
- No Flash electrical behavior is verified.
- No PSRAM or peripheral behavior is verified.
- No write, erase, monitor, rollback, or restore was executed.
- No physical recovery behavior was tested.
- Staging redundancy is not physical-disk independence.
- Staging packages are not replacements for the immutable full backups.

## 31. Source References

Repository:

- `PROJECT_CONSTITUTION.md`
- `docs/HARDWARE_PROFILE.md`
- `docs/hardware/pcb-v1-current-boot-chain-readonly-snapshot.md`
- `docs/hardware/pcb-v1-app-only-compatibility-assessment.md`
- `docs/hardware/pcb-v1-esptool-single-attempt-execution-mechanism.md`
- `docs/hardware/pcb-v1-first-flash-review-package.md`
- `docs/hardware/pcb-v1-recovery-and-first-flash-readiness.md`
- `tests/build/pcb-v1-first-flash-smoke-test-host-build.md`
- `tests/host/test_esptool_single_attempt.py`
- `tools/first_flash/esptool_single_attempt.py`
- `firmware/main/main.c`
- `firmware/sdkconfig`

Local esptool v4.12.dev3:

- `esptool/__init__.py`: connection banner, stub selection, SPI/Flash
  preparation, and post-operation reset.
- `esptool/cmds.py`: `_update_image_flash_params()` and `write_flash()`.
- `esptool/loader.py`: sector/write constants, connection, ROM Flash begin,
  MD5, retry, SPI attach, and Flash parameters.
- `esptool/targets/esp32.py`: ESP32-family erase size and eFuse/security
  helpers.
- `esptool/targets/esp32s3.py`: chip description/features/MAC, security
  checks, USB mode, and volatile watchdog handling.

Local ESP-IDF v5.5.4:

- `components/bootloader_support/include/esp_flash_partitions.h`
- `components/app_update/otatool.py`
- `components/bootloader_support/src/bootloader_utility.c`
- `components/bootloader_support/src/esp_image_format.c`

Local ESP-IDF Monitor:

- `esp_idf_monitor/base/argument_parser.py`
- `esp_idf_monitor/base/serial_reader.py`
- `esp_idf_monitor/base/reset.py`
- `esp_idf_monitor/base/logger.py`

## 32. Final Host-Only Human Authorization Review

The final pure-host review is recorded in
`docs/hardware/pcb-v1-first-flash-human-authorization-review.md`.
It reverified the candidate, rollback provenance, exact package contents,
manifests, geometry, containment, single-attempt harness, 16 host tests,
installed-tool integrity, ancillary reads, volatile effects, observation,
stop criteria, and both recovery levels.

Decision: **READY TO REQUEST EXPLICIT USER FLASH AUTHORIZATION**.
The record remains **UNSIGNED / NOT AUTHORIZED / DO NOT EXECUTE**. It creates
no authority. Compatibility remains **B — PLAUSIBLE BUT NOT PROVEN**; Task 3.4
remains **NOT COMPLETED**; First Flash remains **NO-GO**; device access and
Flash authorization remain `NONE`.

## 33. Post-Authorization Execution Update

On 2026-07-26 the user supplied an explicit one-operation authorization
matching the reviewed D: candidate, `COM7`, image geometry, erase envelope,
attempt controls, ancillary reads, observation plan, and recovery boundary.
All pre-open artifact, backup, package, harness, version, source-integrity,
privacy-filter, and attempt-count checks passed.

One device-mode harness invocation then:

- identified ESP32-S3 QFN56 revision v0.2 on exact `COM7` over USB
  Serial/JTAG;
- erased `0x00020000-0x00047FFF`;
- reported `Wrote 161792 bytes at 0x00020000`;
- reported `Hash of data verified`;
- performed no automatic retry;
- remained in the ROM loader.

The 161792-byte count is 960 bytes larger than the 160832-byte candidate.
Installed esptool 4.12.dev3 source confirms that the no-stub path pads the
final 64-byte candidate block to the 1024-byte ROM write size with 960
`0xFF` bytes and sends the full block. The additional transmitted range is
`0x00047440-0x000477FF`. The post-write MD5 uses the unpadded 160832-byte
length, so it verifies only the authorized candidate range.

This contradicts the earlier host-only statement that no end padding would be
written beyond ordinary four-byte preparation. It also conflicts with the
authorization's distinction between candidate-covered bytes and the
erased-only trailing region. The user's possible-unauthorized-range stop
condition therefore triggered.

Current decision:

- operation-tool result: **SUCCESS REPORTED FOR ONE INVOCATION**;
- authorization conformance: **FAILED**;
- First Flash acceptance:
  **STOPPED / INCONCLUSIVE — RANGE-SCOPE DEVIATION**;
- startup reset and 60-second observation: **NOT PERFORMED**;
- automatic retry: **NONE**;
- rollback/recovery: **NOT AUTHORIZED / NOT PERFORMED**;
- last known device state: **STAYING IN ROM BOOTLOADER**;
- device/Flash authorization: **CONSUMED AND CLOSED / NONE**;
- startup-reset, observation, and rollback authorization: **NONE**;
- further automatic action: **NO-GO**.

The one-time external authorization JSON was deleted after the invocation and
cannot be reused. No startup reset, observation serial open, monitor,
`read_flash`, independent verify read, second write, independent erase,
rollback, restore, or eFuse operation followed.

The complete sanitized record and source analysis are in
`tests/hardware/pcb-v1-first-flash-attempt-2026-07-26.md`. Historical
host-only readiness statements above remain evidence of their review-time
state; this section supersedes their current operation status.

## 34. Host-Only Post-Attempt Padding and Next-Action Assessment

The pure-host assessment in
`docs/hardware/pcb-v1-post-flash-padding-and-next-action-assessment.md`
rehashed the verified historical full backup, audited the installed esptool
v4.12.dev3 ROM-block path, parsed the candidate boundary, reviewed the
MX25UM25645G programming/ECC semantics, and inspected candidate startup
closure and mutation symbols.

The historical 960-byte padding-corresponding range and 2048-byte erase-only
tail contained 960 and 2048 non-`0xFF` bytes respectively, all inside the
valid historical Xiaozhi App image. The preceding sector erase, not the later
`0xFF` data value, is the operation that intended to remove those historical
bytes. The 960-byte `0xFF` payload requests no additional logical NOR
main-array `1`-to-`0` data-bit transition over the erased state. Actual tail
readback, internal ECC/metadata, wear, disturb, and other physical effects
remain unverified.

The candidate's valid image, checksum, and appended hash end exactly at
`0x00047440`; the standard non-Secure-Boot simple-hash path does not include
the padding or erase-only tail. A Secure Boot v2 signature-enforcement path
can hash the `0xFF` sector-alignment bytes and then expect a signature block;
the preserved vendor configuration remains unverified.
The candidate has no OTA/NVS/PHY/network/filesystem/core-dump or raw-Flash
mutation path. Normal Octal-Flash startup does issue documented Flash
configuration-register commands for volatile ODS and STR OPI state; the
physical effect of a combined register write carrying unchanged non-volatile
protection fields is not proven and must be disclosed in any future review.
Standard second-stage initialization also conditionally clears non-volatile
BP bits if protection is unexpectedly set; prior normal boots strongly support
that this state is already clear, but it was not reread in this review.
The exact `v5.5.3-dirty` vendor bootloader also remains unavailable for source
audit.

Decision:

- **RECOMMEND STARTUP-ONLY AUTHORIZATION REVIEW**;
- no authorization is created by that recommendation;
- the device remains frozen until a new explicit user decision;
- immediate Level 1 rollback is not recommended as the next technical action;
- First Flash remains **STOPPED / INCONCLUSIVE**;
- Compatibility remains **B — PLAUSIBLE BUT NOT PROVEN**;
- Task 3.4 remains **NOT COMPLETED**;
- device, Flash, startup/observation, and rollback authorization remain
  `NONE`.
