# PCB V1.0 First Flash Human Authorization Review

> **FINAL HOST-ONLY REVIEW**
>
> **UNSIGNED**
>
> **NOT AUTHORIZED**
>
> **DO NOT EXECUTE**

- Review date: 2026-07-26
- OpenSpec Change: `prepare-pcb-v1-first-flash-smoke-test`
- Firmware Implementation Gate: **HOST-ONLY IMPLEMENTATION COMPLETED**
- Flash Authorization Readiness: **READY FOR EXPLICIT HUMAN FLASH AUTHORIZATION**
- Human Authorization Request Readiness: **READY TO REQUEST EXPLICIT USER FLASH AUTHORIZATION**
- Compatibility: **B — PLAUSIBLE BUT NOT PROVEN**
- Task 3.4: **NOT COMPLETED**
- First Flash: **NO-GO**
- Device access authorization: **NONE**
- Flash authorization: **NONE**

This record is not authorization. It contains no executable Flash, monitor,
rollback, or recovery command and no executor-readable authorization document,
token, signature, JSON, TOML, or YAML.

## 1. Executive Summary

Every host-side condition for asking the user for one future exact app-only
Flash authorization passed. The repository, D:, and E: candidate copies are
byte-identical. The D:/E: complete raw `ota_0` rollback artifacts are
byte-identical and match the reviewed range in their corresponding verified
32 MiB backups. Both three-file staging packages validate and are isolated
from the immutable backups.

The candidate image and its larger sector erase envelope remain inside
original `ota_0`. The envelope would erase 3008 original bytes beyond the
candidate. The exact-version harness passed syntax checks, 16 host tests,
audit, self-test, and D:/E: manifest/artifact validation with zero real serial
opens and zero port enumerations. Installed esptool remained unchanged.

No host-side blocker remains. Compatibility is still unproven, Task 3.4 is
still incomplete, and no device or Flash authority exists.

## 2. Scope

This is a pure-host final review of repository state, artifacts, manifests,
geometry, harness controls, ancillary reads, volatile effects, observation,
stop criteria, and recovery boundaries. It did not enumerate or open a COM
port, access/reset/query a device, run `idf.py`, configure, build, Flash,
monitor, erase, restore, or rollback. It did not modify firmware, binaries,
manifests, backups, Python, pyserial, ESP-IDF, or installed esptool.

## 3. Current Safety State

| State | Result |
|---|---|
| Final host review | `PASS` |
| Flash Authorization Readiness | `READY FOR EXPLICIT HUMAN FLASH AUTHORIZATION` |
| Human Authorization Request Readiness | `READY TO REQUEST EXPLICIT USER FLASH AUTHORIZATION` |
| Compatibility | `B — PLAUSIBLE BUT NOT PROVEN` |
| Task 3.4 | `NOT COMPLETED` |
| First Flash | `NO-GO` |
| Device access authorization | `NONE` |
| Flash authorization | `NONE` |
| Level 1/Level 2 authorization | `NONE` |

## 4. Repository Baseline

- Branch: `feat/prepare-pcb-v1-first-flash-smoke-test`.
- Initial local/upstream HEAD:
  `040351db627f5d4d3660f50694fcccefdbbd3049`.
- Initial ahead/behind: `0/0`.
- Initial worktree: clean.
- Firmware tracked state: unchanged.
- Change: active and not archived.

## 5. OpenSpec Baseline

- Schema: `spec-driven`.
- Initial progress: `33/48`.
- Change strict validation: passed.
- Repository-wide strict validation: 2 passed, 0 failed.
- No pending Task specifically represents this final host review.
- This review adds evidence to already completed Tasks 8.1-8.3 and 15.1-15.4.
- No new Task is completed. Tasks 3.4, 9.1-9.2, device work, runtime
  acceptance, rollback execution, and recovery validation remain unchecked.

## 6. Candidate Artifact Verification

All copies are 160832 bytes with SHA-256
`1B72A60DE9C9BB42DE401A58D7772B0525AFBC4DC3D8C85BB550D2D758F99DDC`.
Streaming byte comparisons passed.

| Copy | Absolute path |
|---|---|
| repository artifact | `D:\ESP-VoCat-Project\custom-vocat\firmware\build\pcb_v1_first_flash_smoke_test.bin` |
| D: staging | `D:\ESP-VoCat_First_Flash_Packages\2026-07-26\pcb_v1_first_flash_smoke_test.bin` |
| E: staging | `E:\ESP-VoCat_First_Flash_Packages\2026-07-26\pcb_v1_first_flash_smoke_test.bin` |

Offline metadata: ESP32-S3, image v1, entry `0x403752D0`, 6 segments,
DOUT/80 MHz/16 MB, secure version 0, checksum `0x98` valid, appended hash
`35F99732C55EBAEA00AFE9ECD372D0B0C681E871D175290A763A7455943C50F3`
valid, ESP-IDF v5.5.4. The repository artifact remains Git-ignored.

## 7. Rollback Artifact Verification

Both copies are 4128768 bytes with SHA-256
`C8A2FE4AB0F9B7C823F1DCFDFC926682C9DBF1B079056461DC6F18A93A345D0E`.
They are byte-identical.

- D: rollback equals the D: full backup at offset `0x00020000`, length
  `0x003F0000`.
- E: rollback equals the E: full backup at the same range.
- Both full backups remain 33554432 bytes with SHA-256
  `72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001`.
- The rollback contains no data outside original `ota_0` and is not in Git.

## 8. Cross-Volume Package Verification

Each package root contains exactly the candidate, complete raw `ota_0`
rollback, and `FIRST_FLASH_PACKAGE_MANIFEST.txt`. Both manifests validate and
state `HOST-ONLY STAGING`, `NOT AUTHORIZED`, `DO NOT EXECUTE`, exact
candidate/rollback metadata, geometry, preserved/excluded regions, and both
authorizations as `NONE`.

The manifests differ only in their path-specific D:/E: package-root field.
D: and E: are distinct volume paths; different physical disks are not proven.
Staging is outside the immutable backup roots and is not a replacement for
the full backups.

## 9. Candidate Image Geometry

| Field | Hex | Decimal |
|---|---:|---:|
| start | `0x00020000` | 131072 |
| length | `0x00027440` | 160832 |
| end-exclusive | `0x00047440` | 291904 |
| last byte | `0x0004743F` | 291903 |

## 10. Erase Envelope

| Field | Hex | Decimal |
|---|---:|---:|
| sector size | `0x00001000` | 4096 |
| aligned start | `0x00020000` | 131072 |
| aligned end-exclusive | `0x00048000` | 294912 |
| aligned length | `0x00028000` | 163840 |
| first sector | `0x20` | 32 |
| last sector | `0x47` | 71 |

## 11. Partition Containment

Original `ota_0` starts at `0x00020000`, has size `0x003F0000`, and ends
exclusively at `0x00410000`. Both image and erase ranges are wholly inside it.
The envelope does not touch bootloader, partition table, NVS, `otadata`,
`phy_init`, `ota_1`, assets, or the upper 16 MiB.

## 12. Preserved and Erased Bytes

The image covers `0x00020000-0x0004743F`. The erase envelope also erases
`0x00047440-0x00047FFF`: exactly 3008 original bytes not replaced by the
candidate. Remaining `ota_0` from `0x00048000` through `0x0040FFFF` is outside
the envelope and preserved, as are all other partitions and regions.

## 13. Single-Attempt Harness Verification

Harness:
`D:\ESP-VoCat-Project\custom-vocat\tools\first_flash\esptool_single_attempt.py`.
Tests:
`D:\ESP-VoCat-Project\custom-vocat\tests\host\test_esptool_single_attempt.py`.

- Syntax: 2/2 files passed.
- Unit tests: 16/16 passed.
- Audit before/after: passed.
- Self-test: passed.
- D:/E: manifest and candidate validation: passed.
- Missing authorization and artifact/hash/range/envelope mismatch: refused.
- Outer synthetic failure: one `flash_begin()` call.
- Block synthetic failure: one `check_command()` call.
- Sync synthetic failure: one `sync()` call.
- Real serial opens: 0; port enumerations: 0.

## 14. Retry Controls

| Path | Stock | Enforced |
|---|---:|---:|
| outer operation | 2 | 1 |
| failed block | 3 | 1 |
| connection | 7 | 1 |
| initial port open | 1 | 1 |
| sync per connection | 5 | 1 |
| reset reopen | 3 | 1 |

The overrides are process-local and restored on exit. The first failure
propagates without a second operation or failed-block transmission.

## 15. Version and Structure Guards

Verified: Python 3.13.9 at
`C:\Espressif\tools\python\v5.5.4\venv\Scripts\python.exe` (canonical E:
target), esptool `4.12.dev3`, canonical paths, 28 Python sources/30 tree
entries, source-tree digest
`7BA6E754C34E202DEB9EF5C6E24376CDA93F24BD4DDB3B70E392BBA6CD5C7610`,
console hash
`A1F90587C195E8C824FE30D80B2D6062CE244CD346118502F08AFDC4496F8AF4`,
six source hashes, function signatures, constants, retry-loop markers,
package roots, geometry, and absence of a config override.

## 16. Installed esptool Integrity

Before/after integrity records were identical. Source tree, executable, paths,
version, counts, and critical source hashes were unchanged. Installed package
modified: **NO**.

## 17. Ancillary Reads

Future authorization must include ROM security information; chip ID/API/eco;
eFuse/OTP-derived revision and package; embedded Flash/PSRAM feature/vendor
information; security state and `flash_crypt_cnt`; key-purpose codes; base MAC;
USB-mode register; Flash RDID/capacity; XMC SFDP when required; and post-write
ROM MD5. These are not persistent Flash/eFuse writes, but they are device
reads requiring explicit scope.

## 18. Volatile Connection Effects

Future effects may include ROM download mode, DTR/RTS or USB reset, USB
Serial/JTAG re-enumeration, SPI attach, Flash reset commands, volatile
watchdog/Flash-parameter register changes, one MD5 operation, remaining in ROM
loader after write, and a later separate observation-start reset. They are
volatile but require authorization.

## 19. Sensitive-Output Handling

The base MAC is read internally. Raw stdout/stderr must remain in memory,
undergo filtering before display, and never be saved. MACs, unique IDs, key
digests, credentials, tokens, and unnecessary security values must not enter a
report, log, staging, or Git. The filter self-test passed; filtering does not
eliminate the underlying ancillary read.

## 20. Future Exact Operation Parameters

This table is non-executable and not authorization.

| Parameter | Reviewed future value |
|---|---|
| exact device | ESP-VoCat PCB V1.0, ESP32-S3, same recovery-covered unit |
| expected port | `COM7` |
| port status | `FUTURE RECONFIRMATION REQUIRED` |
| proposed exact candidate | `D:\ESP-VoCat_First_Flash_Packages\2026-07-26\pcb_v1_first_flash_smoke_test.bin` |
| SHA-256 | `1B72A60DE9C9BB42DE401A58D7772B0525AFBC4DC3D8C85BB550D2D758F99DDC` |
| offset/length | `0x00020000` / `0x00027440` |
| end-exclusive/last byte | `0x00047440` / `0x0004743F` |
| erase envelope | `0x00020000-0x00047FFF`; end-exclusive `0x00048000` |
| esptool/Python | `4.12.dev3`; audited Python 3.13.9 interpreter above |
| harness | repository absolute path in Section 13 |
| loader/compression/header | ROM/no-stub; no compression; preserve DOUT/80 MHz/16 MB |
| write reset | explicit USB reset before connection; no reset after write |
| observation reset | separate open, then one intentional hard reset |
| verification | one post-write ROM MD5 over exact range |
| observation | USB Serial/JTAG, 115200, 60 seconds |
| ready deadline | 15 seconds |
| rollback | Level 1 in Section 24; Level 2 separately gated |

The E: candidate is a verified mirror, not an authorized substitution.

## 21. Observation Plan

Open the future reconfirmed USB Serial/JTAG port at 115200 as a separate
operation, intentionally reset once, observe 60 seconds, and require all fixed
text plus the marker within 15 seconds. The same exact port may disappear for
no more than 15 seconds and must return unchanged. Send no input and retain no
raw monitor log.

Expected source-derived output:

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

The application should then emit no periodic text.

## 22. Success Criteria

The eight lines must appear once, in order, within 15 seconds; the session must
remain observable for 60 seconds; no repeated startup, panic, watchdog,
allocation failure, reset loop, invalid image, ROM/bootloader error, or
continuous download-mode output may occur; and USB must remain available or
return on the same port within 15 seconds. This proves serial startup only.

## 23. Stop Conditions

Stop before writing on any device, PCB, port, path, filename, size, hash,
offset, length, end, envelope, recovery, package, tool, harness, authorization,
ancillary-scope, reset-policy, observation, or risk mismatch.

Stop afterward on unexpected re-enumeration, changed port, uncertain/partial/
failed/repeated write, any automatic retry, missing marker after 15 seconds,
unexpected/repeated output, panic, watchdog, restart loop, ROM/bootloader/
image/checksum/anti-rollback error, USB absence over 15 seconds, or evidence of
an unapproved range. A stop condition authorizes neither retry nor rollback.

## 24. Level 1 Rollback

- Artifact: `original_xiaozhi_ota_0_full_partition.bin`.
- Proposed D: path:
  `D:\ESP-VoCat_First_Flash_Packages\2026-07-26\original_xiaozhi_ota_0_full_partition.bin`.
- Offset: `0x00020000`.
- Length: `0x003F0000` / 4128768.
- End-exclusive: `0x00410000`.
- SHA-256:
  `C8A2FE4AB0F9B7C823F1DCFDFC926682C9DBF1B079056461DC6F18A93A345D0E`.

Level 1 is an independent operation with fresh checks and its own explicit
authorization. First Flash authorization cannot authorize it.

## 25. Level 2 Recovery

Level 2 full-backup recovery is **NOT AUTHORIZED / HIGHER RISK / SEPARATE
REVIEW REQUIRED**. It may be considered only if Level 1 cannot restore
original boot behavior. No Level 2 command exists here.

## 26. Compatibility Residual Risk

Compatibility remains **B — PLAUSIBLE BUT NOT PROVEN**: the preserved
bootloader is vendor-modified `v5.5.3-dirty`; its changes are unavailable; it
has not started this v5.5.4 DOUT app; snapshot evidence is not runtime proof;
current security/anti-rollback state remains unresolved; and `ota_1` has no
observed fallback header. First Flash may temporarily or permanently prevent
boot. Recovery readiness does not prove physical recovery success.

## 27. Task 3.4 Status

Task 3.4 remains **NOT COMPLETED**. Its original criterion is to decide whether
one app-only write can be compatible with the preserved bootloader/layout
without widening to bootloader, partition table, OTA layout, or unused tail.
Host evidence proves geometry and finds no definite incompatibility, but only
an actual preserved-bootloader start can close runtime compatibility. No
wider write is proposed.

## 28. Human Authorization Requirements

The future user must explicitly confirm: exact device; exact port; candidate
filename and absolute path; SHA-256; offset; length; end-exclusive; erase
envelope; erasure of 3008 trailing original bytes; preserved regions;
ancillary eFuse/OTP/MAC-derived reads; internal base-MAC read and filtering;
ROM/no-stub; no compression; one-attempt policy for all six controlled paths;
post-write MD5; reset policies; USB re-enumeration risk; 60-second observation;
15-second marker deadline; all stop conditions; no automatic Level 1 or Level
2 authority; compatibility class B; possible temporary/permanent boot loss;
and once-only authority for this exact candidate app-only write.

Any changed field or vague continuation language is invalid.

## 29. Authorization Statement Template

> **UNSIGNED / NOT AUTHORIZED**
>
> I, [human name — NOT FILLED], confirm exact device [NOT FILLED], ESP-VoCat
> PCB V1.0, current exact port [NOT FILLED], candidate filename [NOT FILLED],
> absolute path [NOT FILLED], SHA-256 [NOT FILLED], offset [NOT FILLED],
> length [NOT FILLED], end-exclusive [NOT FILLED], and erase envelope
> [NOT FILLED]. I acknowledge that 3008 original trailing bytes will be erased
> and that the listed outside regions are intended to remain preserved. I
> authorize the disclosed ROM-security, chip/API, eFuse/OTP-derived
> revision/package/Flash/PSRAM/security/key-purpose/base-MAC, USB-mode,
> Flash-RDID/capacity, possible XMC-SFDP, volatile-register, reset, and MD5
> effects. I understand the base MAC is read internally but must be filtered
> before display or retention. I approve ROM/no-stub, no compression,
> preserved header, and one attempt each for outer write, block, connection,
> port open, sync, and reset reopen. I accept the reset policy, USB
> re-enumeration risk, 60-second observation, 15-second ready-marker deadline,
> and all stop conditions. I understand Level 1 and Level 2 are not authorized
> here, compatibility remains B — PLAUSIBLE BUT NOT PROVEN, and the device may
> temporarily or permanently fail to boot. Subject to every blank being filled
> and revalidated, I authorize once and only once this exact candidate
> app-only write.
>
> Human signature: [UNSIGNED]
>
> Date/time and timezone: [NOT FILLED]

This is unfilled human-readable text, not a harness-accepted authorization.

## 30. Final Readiness Decision

**READY TO REQUEST EXPLICIT USER FLASH AUTHORIZATION**

All candidate, rollback, manifest, geometry, harness, installed-integrity,
ancillary-read, observation, stop, rollback, and OpenSpec conditions passed.
No host-side blocker remains. This permits only asking the user for a future
exact decision. It creates no authority. Compatibility remains class B, Task
3.4 remains incomplete, First Flash remains `NO-GO`, and both authorizations
remain `NONE`.

## 31. Claims Explicitly Not Made

No device or port was accessed; no Flash/monitor/erase/restore/rollback ran; no
valid authorization material was created; the candidate has not booted; the
preserved bootloader is not proven compatible; no electrical, PSRAM,
peripheral, product, or recovery behavior is verified; cross-volume staging
does not prove physical-disk independence; staging does not replace backups.

## 32. Source References

- `PROJECT_CONSTITUTION.md`
- `docs/HARDWARE_PROFILE.md`
- `docs/PRODUCT_SPEC_DRAFT.md`
- `docs/hardware/pcb-v1-first-flash-operation-readiness.md`
- `docs/hardware/pcb-v1-first-flash-review-package.md`
- `docs/hardware/pcb-v1-esptool-single-attempt-execution-mechanism.md`
- `docs/hardware/pcb-v1-current-boot-chain-readonly-snapshot.md`
- `docs/hardware/pcb-v1-app-only-compatibility-assessment.md`
- `tests/build/pcb-v1-first-flash-smoke-test-host-build.md`
- `tests/host/test_esptool_single_attempt.py`
- `tools/first_flash/esptool_single_attempt.py`
- `firmware/main/main.c`
- all four OpenSpec artifacts for this Change
- D:/E: staged package manifests and artifacts
- D:/E: immutable full backups
- audited local Python 3.13.9 and esptool v4.12.dev3 installation
