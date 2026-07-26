# PCB V1.0 First Flash Review Package

> **DRAFT**
>
> **HOST-ONLY REVIEW**
>
> **NOT AUTHORIZED**
>
> **DO NOT EXECUTE**

- Package date: 2026-07-26
- OpenSpec Change: `prepare-pcb-v1-first-flash-smoke-test`
- Compatibility classification: **B — PLAUSIBLE BUT NOT PROVEN**
- Task 3.4: **NOT COMPLETED**
- Firmware Implementation Gate: **HOST-ONLY IMPLEMENTATION COMPLETED**
- First Flash: **NO-GO**
- Device access authorization: **NONE**
- Flash authorization: **NONE**
- Flash Authorization Readiness: **NOT READY FOR FLASH AUTHORIZATION**

This package intentionally contains no executable or copyable Flash command.

## 1. Purpose

Collect the current host-reviewed candidate manifest, preserved-region boundary, recovery evidence, unresolved current-state inputs, risks, observation requirements, and future authorization fields for human review before any First Flash proposal.

## 2. Current Safety Status

| Gate | Status |
|---|---|
| Firmware Implementation Gate | `HOST-ONLY IMPLEMENTATION COMPLETED` |
| Host configure/build | completed previously |
| App-only host geometry review | completed |
| Preserved-bootloader compatibility | `PLAUSIBLE BUT NOT PROVEN` |
| Human pre-Flash review | incomplete |
| First Flash | `NO-GO` |
| Device access authorization | `NONE` |
| Flash authorization | `NONE` |

## 3. Exact Device Identity Required

Before any future device operation, the user must explicitly identify and review:

- the exact physical ESP-VoCat unit;
- PCB revision V1.0;
- ESP32-S3 identity;
- confirmation that it is the same device covered by the recovery assets.

Prior device evidence is historical and does not satisfy this operation-specific checkpoint.

## 4. Exact Port Required

Candidate port field: `COM7` — **FUTURE RECONFIRMATION REQUIRED**.

`COM7` is not assumed to be permanent and is not authorized by this package.
It must be reconfirmed immediately before any future operation. Any port change
invalidates a future reviewed packet.

## 5. Candidate Artifact Manifest

| Field | Host-reviewed value |
|---|---|
| repository-relative path | `firmware/build/pcb_v1_first_flash_smoke_test.bin` |
| file state | ignored generated artifact |
| size | `0x00027440` / 160832 bytes |
| target | ESP32-S3 |
| image version | 1 |
| entry point | `0x403752D0` |
| segments | 6 |
| header | DOUT / 80 MHz / 16 MB |
| chip revision bounds | v0.0 through v0.99 |
| secure version | 0 |
| ESP-IDF | v5.5.4 |
| checksum/hash | valid |

`firmware/build/hello_world_build_baseline.bin` is absent and is not the reviewed candidate.

## 6. Candidate App SHA-256

`1B72A60DE9C9BB42DE401A58D7772B0525AFBC4DC3D8C85BB550D2D758F99DDC`

Any path, byte, size, timestamp, or hash change invalidates this package and requires complete host review again.

## 7. Candidate Offset

`0x00020000`

Status:

- `CANDIDATE APP-ONLY RANGE`
- `HOST-REVIEWED`
- `NOT DEVICE-VALIDATED`
- `NOT AUTHORIZED FOR WRITE`

This value is derived from the historical original `ota_0` table entry. It is not a current live-slot selection.

## 8. Candidate Length

`0x00027440` / 160832 bytes.

## 9. Candidate End-Exclusive

`0x00047440`

Last candidate byte: `0x0004743F`.

Sector erase geometry:

- sector size: `0x00001000` / 4096 bytes;
- aligned erase start: `0x00020000` / 131072;
- aligned erase end-exclusive: `0x00048000` / 294912;
- aligned erase length: `0x00028000` / 163840 bytes;
- affected sectors: `0x20-0x47` / 32-71.

The image-byte range, erase envelope, and `ota_0` partition range are distinct.

## 10. Original ota_0 Capacity

- Start: `0x00020000`
- Capacity: `0x003F0000`
- End-exclusive: `0x00410000`
- Candidate remaining space: `0x003C8BC0`

## 11. Preserved Regions

A future app-only proposal with the reviewed erase envelope preserves:

- bootloader;
- partition table;
- NVS;
- both `otadata` sectors;
- `phy_init`;
- all bytes of `ota_1`;
- all bytes of `assets`;
- the upper unused 16 MiB;
- every gap and unrelated region outside the sector envelope
  `0x00020000-0x00047FFF`.

The image range ends at `0x0004743F`, but sector erasure extends through
`0x00047FFF`. The 3008 original bytes at
`0x00047440-0x00047FFF` are not preserved by a future candidate write.

## 12. Regions Explicitly Excluded

The candidate packet excludes:

- bootloader at `0x00000000`;
- partition table at `0x00008000`;
- NVS `0x00009000–0x0000CFFF`;
- `otadata` `0x0000D000–0x0000EFFF`;
- PHY `0x0000F000–0x0000FFFF`;
- `ota_1` `0x00410000–0x007FFFFF`;
- assets `0x00800000–0x00FFFFFF`;
- upper tail `0x01000000–0x01FFFFFF`;
- erase operations;
- every other image and range.

The build-generated factory layout at `0x10000` is excluded and rejected for device-write use.

## 13. Recovery Assets

Rehashed during this host-only review:

| Path | Size |
|---|---:|
| `D:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi\esp-vocat_full_flash_32MB_2026-07-10.bin` | 33554432 |
| `E:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi\esp-vocat_full_flash_32MB_2026-07-10.bin` | 33554432 |

The same-name `verify` copies were also rehashed and matched. D: and E: are
distinct volume paths; separate physical disks are not claimed.

Host-only staging packages:

- `D:\ESP-VoCat_First_Flash_Packages\2026-07-26`
- `E:\ESP-VoCat_First_Flash_Packages\2026-07-26`

Each contains only the candidate, the complete raw `ota_0` rollback image, and
a path-specific manifest. The staged candidate copies retain size 160832 and
SHA-256
`1B72A60DE9C9BB42DE401A58D7772B0525AFBC4DC3D8C85BB550D2D758F99DDC`.
The staged rollback copies are 4128768 bytes with SHA-256
`C8A2FE4AB0F9B7C823F1DCFDFC926682C9DBF1B079056461DC6F18A93A345D0E`.
The staging directories are outside the immutable backup directories.

## 14. Recovery Hashes

Both rehashed files matched:

`72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001`

Recovery remains same-device only because the full image contains private and stateful regions.

## 15. Historical OTA Selection Evidence

The 2026-07-10 backup snapshot contains:

- entry 0: nonempty, `ota_seq=1`, `VALID`, CRC valid;
- entry 1: erased;
- historical selected slot: `ota_0`;
- no `PENDING_VERIFY`, `INVALID`, or `ABORTED` state.

This historical selection explains why `0x00020000` is the only candidate app-only range that would replace the historically selected image without mutating OTA metadata. It does not establish the current selection.

## 16. Current-State Evidence Still Required

- operation-specific device/PCB confirmation for a future First Flash packet;
- operation-specific user-supplied port for a future First Flash packet;
- acknowledgment that the 2026-07-26 snapshot found the current bootloader,
  table, `otadata`, and both OTA header windows byte-identical to the historical
  image;
- acknowledgment that current `otadata` selected `ota_0` and the current
  `ota_1` header window was erased;
- current security/anti-rollback state sufficient for the exact operation;
- operation-time confirmation that the 2026-07-26 snapshot has not become
  stale;
- reconfirmed recovery-file set immediately before authorization;
- exact deployed-bootloader acceptance risk reviewed by the user;
- review of the esptool automatic eFuse/OTP/MAC connection-banner scope
  deviation recorded in the current snapshot report;
- reviewed one-shot write mechanism with no automatic whole-image or block
  retry;
- operation-time reconfirmation of the fixed observation and rollback fields.

No current-state device read is authorized by this package.

## 17. Pre-Flash Checklist

- [x] Candidate artifact exists.
- [x] Candidate size and SHA-256 match the committed build record.
- [x] Candidate image metadata and checksum/hash reviewed offline.
- [x] Candidate geometry fits the historical `ota_0` partition.
- [x] Candidate dependencies do not require OTA, NVS, assets, network, or peripherals.
- [x] Preserved/excluded regions are explicit.
- [x] Historical backup OTA metadata parsed and clearly time-scoped.
- [x] Current bootloader/table/OTA/header snapshot collected and compared.
- [x] Two primary cross-volume recovery files rehashed.
- [x] D:/E: isolated staging packages created and payload hashes matched.
- [x] Exact sector erase envelope reviewed.
- [x] Ancillary connection reads and volatile effects reviewed.
- [x] Exact 60-second observation window host-reviewed.
- [x] Level 1 full-`ota_0` rollback artifact staged and reviewed.
- [ ] Compatibility classification reaches A.
- [ ] Exact current device is reviewed.
- [ ] Exact current port is reviewed.
- [x] Current OTA selection and OTA header-window state are reviewed at the
  2026-07-26 snapshot time.
- [ ] Current security/anti-rollback state is reviewed.
- [x] Exact observation window is host-reviewed.
- [x] Separate Level 1 rollback packet is host-prepared.
- [ ] One-shot no-retry execution mechanism is reviewed.
- [ ] Human pre-Flash review records a final decision.
- [ ] Exact operation receives explicit user authorization.

## 18. Flash Operation Risks

- The 2026-07-26 selected slot matched the historical snapshot, but that
  evidence can become stale before a future write.
- A partial app write could make the selected slot unbootable.
- Historical `ota_1` was erased and may not provide a fallback.
- Vendor `v5.5.3-dirty` bootloader changes are unknown.
- DOUT/OPI behavior has not been tested with the preserved bootloader.
- Current security or anti-rollback state could reject secure version 0.
- USB re-enumeration may change the port.
- A successful host review or tool exit cannot prove runtime startup.
- The ignored artifact can be deleted or replaced without a Git change.

## 19. Observation Plan

Observation remains a separate future `DEVICE READ` operation after, and only
after, an independently authorized write.

- Interface: integrated USB Serial/JTAG at 115200.
- Port: `COM7`, **FUTURE RECONFIRMATION REQUIRED**.
- Write stage remains in the ROM loader after its result.
- The separately opened monitor intentionally performs one startup hard reset.
- Observation lasts 60 seconds from that reset.
- The complete fixed application sequence and ready marker must appear within
  15 seconds, once and in order.
- USB re-enumeration is allowed up to 15 seconds only if the exact reviewed
  port returns.
- No input, logging toggle, display, touch, audio, motor, network, or
  peripheral test is allowed.

Exact expected application output:

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

The application then emits no periodic output.

## 20. Stop Conditions

Stop before any write on:

- device, PCB, port, file, hash, offset, length, end, partition, recovery, or authorization mismatch;
- missing artifact or recovery asset;
- current OTA/security evidence inconsistent with the package;
- any command containing erase, bootloader, partition table, OTA metadata, PHY, assets, upper tail, eFuse, voltage, security, monitor, or unrelated image behavior;
- unresolved one-shot/no-retry enforcement;
- package not explicitly reviewed by the user.

After a future write, stop further device work on:

- tool failure;
- partial or uncertain write;
- missing ready marker after 15 seconds;
- unexpected output;
- reset loop, panic, watchdog, or allocation failure.
- USB disappearance longer than 15 seconds or re-enumeration under a different
  port.

No automatic retry or widened range is allowed.

## 21. Rollback Preconditions

Rollback is not included in First Flash authorization.

Level 1 is the default app-only rollback:

- artifact: `original_xiaozhi_ota_0_full_partition.bin`;
- size: `0x003F0000` / 4128768 bytes;
- SHA-256:
  `C8A2FE4AB0F9B7C823F1DCFDFC926682C9DBF1B079056461DC6F18A93A345D0E`;
- proposed range:
  `0x00020000-0x0040FFFF`, end-exclusive `0x00410000`;
- no preceding erase, unrelated partition, or automatic retry;
- separate authorization and original-firmware boot observation.

The complete raw partition is used because it reconstructs every captured
`ota_0` byte and removes mixed candidate/vendor tail data without guessing the
original image's effective length.

Level 2 full-backup recovery is **NOT AUTHORIZED / HIGHER RISK / SEPARATE
REVIEW REQUIRED** and may be considered only if Level 1 fails.

Original-device recovery has not been executed or verified.

## 22. Explicit User Authorization Fields

Current authorization: **NONE**.

Every field below must later be stated and approved explicitly:

| Required field | Current value |
|---|---|
| exact physical device | not supplied |
| exact PCB revision | historical V1.0 evidence only; operation-specific confirmation required |
| exact current port | `COM7`; future reconfirmation required |
| exact artifact path | candidate recorded; not authorized |
| exact SHA-256 | candidate recorded; not authorized |
| exact offset | candidate recorded; not authorized |
| exact length | candidate recorded; not authorized |
| exact end-exclusive | candidate recorded; not authorized |
| preserved regions | recorded; human review pending |
| recovery assets/hashes | recorded; final revalidation pending |
| observation window | 60 seconds; 15-second startup/re-enumeration deadline |
| stop conditions | host-reviewed |
| rollback plan | Level 1 app-only prepared; separately authorized |
| esptool ancillary reads | reviewed; explicit future authorization required |
| timeout policy | default 3-second command minimum; one connection/open attempt; scaled erase/MD5 timeouts |
| retry policy | no retry required; stock CLI enforcement unresolved |
| acknowledged risks | not acknowledged for an operation |

Generic continuation language is not authorization.

## 23. Prohibited Operations

- Device or COM enumeration/access before new authorization.
- Any Flash, erase, restore, monitor, or eFuse operation.
- Bootloader, partition-table, NVS, OTA metadata, PHY, assets, or upper-tail modification.
- Secure Boot, Flash Encryption, anti-rollback, VDDSPI, voltage, key, JTAG, USB, or download-mode changes.
- Combined write/monitor behavior.
- Automatic retry.
- Use of a historical port assignment.
- Substitution of another artifact, hash, offset, or range.

No executable command is present in this package.

## 24. Go/No-Go Decision

**NO-GO**

Reasons:

- compatibility remains classification B;
- operation-specific device and port remain unreviewed, and current
  security/anti-rollback state is unreviewed;
- vendor `v5.5.3-dirty` compatibility is unproven;
- stock esptool v4.12.dev3 hard-codes whole-image and block retry behavior that
  conflicts with the current no-retry OpenSpec boundary;
- human review is incomplete;
- Device access authorization is `NONE`;
- Flash authorization is `NONE`.

This draft may proceed only to human review. It does not permit device access or execution.

### Current Snapshot Note

The separately authorized 2026-07-26 read-only snapshot is recorded in
`docs/hardware/pcb-v1-current-boot-chain-readonly-snapshot.md`. Its five
explicit Flash ranges matched the historical backup, but esptool's standard
connection path implicitly read eFuse/OTP-derived chip-description and MAC
registers. The MAC was not displayed or retained; the deviation still requires
human review. The snapshot authorization is consumed and closed; device access
authorization is consumed and closed; current device access authorization is
`NONE`. This does not change the
package from `NO-GO`.

## 25. Operation Readiness Source Audit

The controlling detailed audit is
`docs/hardware/pcb-v1-first-flash-operation-readiness.md`.

Key decisions:

- candidate image range:
  `0x00020000-0x0004743F`;
- erase envelope:
  `0x00020000-0x00047FFF`;
- ROM loader / no stub;
- no compression;
- Flash mode/frequency/size header fields kept;
- write stage ends without reset; a separate monitor-start reset begins the
  observation window;
- normal unencrypted write uses the ROM MD5 comparison;
- all security-info, chip/eFuse/OTP/MAC, USB-mode, Flash-ID/SFDP, volatile
  register, and reset side effects require explicit future authorization.

The ancillary authorization scope includes ROM security-info flags,
Flash-encryption count, key-purpose codes, chip ID/API revision, chip
revision/package, embedded Flash/PSRAM features, base MAC, USB-mode register,
volatile watchdog handling, SPI attach, Flash RDID/capacity, possible XMC SFDP
checks, Flash reset commands, volatile Flash parameters, and post-write MD5.
The base MAC and unnecessary unique values must not be retained in Git.

Flash Authorization Readiness is **NOT READY FOR FLASH AUTHORIZATION** because
the stock CLI's automatic retry behavior has no reviewed one-shot enforcement.
Compatibility remains **B — PLAUSIBLE BUT NOT PROVEN**. Task 3.4 remains
**NOT COMPLETED**. First Flash remains **NO-GO**.
