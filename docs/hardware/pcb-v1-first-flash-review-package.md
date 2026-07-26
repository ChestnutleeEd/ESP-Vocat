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
- Device access authorization: **CONSUMED AND CLOSED / NONE**
- Flash authorization: **NONE**

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
| Device access authorization | `CONSUMED AND CLOSED / NONE` |
| Flash authorization | `NONE` |

## 3. Exact Device Identity Required

Before any future device operation, the user must explicitly identify and review:

- the exact physical ESP-VoCat unit;
- PCB revision V1.0;
- ESP32-S3 identity;
- confirmation that it is the same device covered by the recovery assets.

Prior device evidence is historical and does not satisfy this operation-specific checkpoint.

## 4. Exact Port Required

Current port: **NOT PROVIDED / NOT REVIEWED**.

The historical COM assignment from earlier read-only work is not assumed, suggested, or authorized for this package. A user-supplied current port must be reviewed later. Any port change invalidates a future reviewed packet.

## 5. Candidate Artifact Manifest

| Field | Host-reviewed value |
|---|---|
| repository-relative path | `firmware/build/pcb_v1_first_flash_smoke_test.bin` |
| file state | ignored generated artifact |
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

## 10. Original ota_0 Capacity

- Start: `0x00020000`
- Capacity: `0x003F0000`
- End-exclusive: `0x00410000`
- Candidate remaining space: `0x003C8BC0`

## 11. Preserved Regions

A future app-only proposal must preserve:

- bootloader;
- partition table;
- NVS;
- both `otadata` sectors;
- `phy_init`;
- all bytes of `ota_1`;
- all bytes of `assets`;
- the upper unused 16 MiB;
- every gap and unrelated region outside the exact candidate app range.

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

The same-name `verify` copies remain recorded recovery assets, but this round's minimal rehash covered the two primary cross-volume copies above. D: and E: are distinct volume paths; separate physical disks are not claimed.

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
- exact observation duration;
- complete rollback packet.

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
- [ ] Compatibility classification reaches A.
- [ ] Exact current device is reviewed.
- [ ] Exact current port is reviewed.
- [x] Current OTA selection and OTA header-window state are reviewed at the
  2026-07-26 snapshot time.
- [ ] Current security/anti-rollback state is reviewed.
- [ ] Exact observation window is approved.
- [ ] Separate rollback packet is approved.
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

Observation remains a separate future `DEVICE READ` operation after, and only after, an independently authorized write.

Required future fields:

- exact console interface and settings;
- exact observation duration: **UNRESOLVED**;
- expected identity/version/safety/ready-marker lines;
- capture of silence, unexpected output, reset, panic, watchdog, and allocation failures;
- no device-control input;
- close the observation at the approved time;
- no display, touch, audio, motor, network, or peripheral test.

## 20. Stop Conditions

Stop before any write on:

- device, PCB, port, file, hash, offset, length, end, partition, recovery, or authorization mismatch;
- missing artifact or recovery asset;
- current OTA/security evidence inconsistent with the package;
- any command containing erase, bootloader, partition table, OTA metadata, PHY, assets, upper tail, eFuse, voltage, security, monitor, or unrelated image behavior;
- compatibility remaining below classification A;
- package not explicitly reviewed by the user.

After a future write, stop further device work on:

- tool failure;
- partial or uncertain write;
- silence past the approved timeout;
- unexpected output;
- reset loop, panic, watchdog, or allocation failure.

No automatic retry or widened range is allowed.

## 21. Rollback Preconditions

Rollback is not included in First Flash authorization. A separate future rollback packet must contain:

- same exact device and current port;
- exact full-image path, 33554432-byte size, and expected SHA-256;
- full range and privacy impact;
- no preceding erase;
- one operation only and no automatic retry;
- separate observation plan;
- explicit rollback authorization.

Original-device recovery has not been executed or verified.

## 22. Explicit User Authorization Fields

Current authorization: **NONE**.

Every field below must later be stated and approved explicitly:

| Required field | Current value |
|---|---|
| exact physical device | not supplied |
| exact PCB revision | historical V1.0 evidence only; operation-specific confirmation required |
| exact current port | not supplied |
| exact artifact path | candidate recorded; not authorized |
| exact SHA-256 | candidate recorded; not authorized |
| exact offset | candidate recorded; not authorized |
| exact length | candidate recorded; not authorized |
| exact end-exclusive | candidate recorded; not authorized |
| preserved regions | recorded; human review pending |
| recovery assets/hashes | recorded; final revalidation pending |
| observation window | unresolved |
| stop conditions | draft |
| rollback plan | separate packet required |
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
- observation duration is unresolved;
- human review is incomplete;
- Device access authorization is `CONSUMED AND CLOSED / NONE`;
- Flash authorization is `NONE`.

This draft may proceed only to human review. It does not permit device access or execution.

### Current Snapshot Note

The separately authorized 2026-07-26 read-only snapshot is recorded in
`docs/hardware/pcb-v1-current-boot-chain-readonly-snapshot.md`. Its five
explicit Flash ranges matched the historical backup, but esptool's standard
connection path implicitly read eFuse/OTP-derived chip-description and MAC
registers. The MAC was not displayed or retained; the deviation still requires
human review. The snapshot authorization is consumed and closed; device access
authorization is now `CONSUMED AND CLOSED / NONE`. This does not change the
package from `NO-GO`.
