# ESP-VoCat PCB V1.0 Recovery and First-Flash Readiness

- Assessment date: 2026-07-25
- Device operations performed: none
- Current decision: **NO-GO**

## 1. Current Known Recovery Assets

Repository policy defines two immutable recovery locations:

- Primary: `E:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi`
- Cross-disk: `D:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi`

During the planning audit, the `E:` directory was not present. During the 2026-07-25 Apply preflight, both D: originals were reverified, the exact E: directory was created, and verified copies were created without overwriting either original. D: and E: establish distinct volume paths; no evidence establishes that they are different physical disks.

## 2. Full Backups

| Path | Size | SHA-256 |
|---|---:|---|
| `D:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi\esp-vocat_full_flash_32MB_2026-07-10.bin` | 33554432 bytes | `72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001` |
| `D:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi\esp-vocat_full_flash_32MB_verify_2026-07-10.bin` | 33554432 bytes | `72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001` |
| `E:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi\esp-vocat_full_flash_32MB_2026-07-10.bin` | 33554432 bytes | `72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001` |
| `E:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi\esp-vocat_full_flash_32MB_verify_2026-07-10.bin` | 33554432 bytes | `72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001` |

All four hashes are identical and match repository policy. E: also contains `SHA256SUMS.txt`, which records only file names, sizes, SHA-256 values, backup date, and the warning “Contains private device data; do not commit or share”. No D: original was moved, deleted, renamed, modified, or overwritten.

## 3. Verified Original-Firmware Partition Layout

The partition table was found by full-image scanning and uniquely validated at `0x00008000`; it was not accepted merely because that is a standard ESP-IDF offset.

| Name | Offset | Size | Recovery significance |
|---|---:|---:|---|
| `nvs` | `0x00009000` | `0x00004000` | Private settings/credentials; same-device only |
| `otadata` | `0x0000D000` | `0x00002000` | OTA selection state |
| `phy_init` | `0x0000F000` | `0x00001000` | PHY/device-state caution |
| `ota_0` | `0x00020000` | `0x003F0000` | Valid Xiaozhi 2.2.6 app |
| `ota_1` | `0x00410000` | `0x003F0000` | Erased in the backup |
| `assets` | `0x00800000` | `0x00800000` | Original resource package and raw tail data |

Partitioned content ends at `0x01000000`; the upper 16 MiB is erased in the backup. This does not authorize reuse.

## 4. Bootloader, Partition Table, and App Locations

| Region | Offset/range | Status |
|---|---|---|
| Bootloader candidate | starts `0x00000000`; validated image length `0x00003F80` | Valid ESP32-S3 image |
| Partition table | `0x00008000`; 4 KiB analysis window | Uniquely validated |
| Active-image candidate in backup | `ota_0` at `0x00020000`; partition size `0x003F0000` | Valid Xiaozhi app; backup evidence only |
| Second app slot | `ota_1` at `0x00410000`; size `0x003F0000` | Erased in backup |

No offset in this section is Flash authorization. A future custom image requires an independent compatibility and range review.

## 5. Regions That May Contain User or Device-Unique Data

- `nvs`: likely credentials, network settings, tokens, certificates, settings, and identifiers.
- `phy_init`: may include radio/PHY state or calibration-related data.
- `otadata`: contains boot-selection state.
- `assets`: contains original resources and may include stale undeclared tail data.
- Full-image gaps or application data may contain build or device strings.

No private values were decoded. Only structural metadata and hashes were recorded for sensitive partitions.

## 6. Data That Must Not Be Overwritten or Leaked

- Do not overwrite NVS, PHY, OTA metadata, original assets, partition table, or bootloader during the first custom write unless a later exact review demonstrates necessity and the user separately authorizes the expanded range.
- Do not use the full image on another device.
- Do not print, commit, transmit, or place in reports any NVS values, credentials, tokens, certificates, MAC addresses, or unique identifiers.
- Do not replace the full raw `assets` partition with only logically extracted files.
- Do not modify, rename, patch, or use recovery files as build output.

## 7. Recovery Preconditions

Before any custom Flash:

1. Maintain the verified D: and E: cross-volume recovery redundancy; do not describe it as separate physical-disk redundancy without additional evidence.
2. Recalculate size and SHA-256 for every recovery image.
3. Confirm the image belongs to the exact device being authorized.
4. Confirm exact target chip and PCB V1.0 identity.
5. Resolve the Flash/PSRAM board-configuration evidence gate.
6. Verify the unique partition interpretation and the custom image's compatibility.
7. Prepare a host-only recovery procedure and review all decision points.
8. Confirm the exact current port; do not assume historical COM7.
9. Review the exact recovery image, full range, risk, and observation plan.
10. Obtain a separate explicit authorization before any recovery write.

Any missing item is a stop condition.

## 8. Full Recovery Strategy

Full recovery means restoring the reviewed 32 MiB same-device image over its complete original address range, followed by a separate boot/observation step and verification of original-device behavior.

Principles:

- no preceding full-device erase;
- no automatic retry;
- no port inference;
- no use on another device;
- recovery write and observation are separate;
- actual output is preserved;
- a mismatch in file, hash, size, device, port, or range cancels authorization.

This report intentionally provides no executable recovery command.

## 9. Partition-Level Versus Full Recovery

| Approach | Purpose | Benefits | Risks/limitations |
|---|---|---|---|
| Partition-level recovery | Restore one precisely identified region | Smaller write; may preserve unrelated state | Can leave incompatible bootloader/table/OTA/NVS combinations; requires exact diagnosis |
| Full recovery | Restore the complete same-device original image | Reconstructs the captured bootloader, table, apps, data, assets, and gaps together | Overwrites all captured private/stateful regions; must be same-device and explicitly authorized |

Partition-level recovery is not automatically safer. It is appropriate only when the failure is isolated and compatibility is proven. Full recovery is the reference rollback for an unknown or cross-region failure, but it remains a high-impact `DEVICE WRITE`.

## 10. Recommended First Custom-Firmware Write Range

The preferred minimum is one application image in one exact reviewed application slot while preserving:

- bootloader;
- original partition table;
- NVS;
- OTA metadata unless the chosen boot strategy explicitly and safely requires otherwise;
- PHY data;
- original assets;
- unused tail.

No exact custom image, target slot, offset, or write length has yet been built or approved. The original `ota_0`/`ota_1` offsets are evidence about the backup, not automatic custom-write targets. If compatibility with the preserved boot chain cannot be proven, stop; do not widen the first write.

## 11. Why Flash Cannot Be Automatically Authorized

- The custom smoke-test firmware does not exist yet.
- No custom image hash, effective size, offset, or range has been reviewed.
- The original image headers declare DIO/80 MHz/16 MB while prior repository records say 32 MiB Octal/1.8 V.
- Exact Flash and optional PSRAM configuration are unresolved.
- A host-only recovery rehearsal is complete, but it performed no device action and grants no recovery or Flash authorization.
- The exact current device and port are not reviewed.
- No explicit operation-specific authorization exists.

OpenSpec artifacts can prepare implementation and a command structure; they cannot authorize execution.

## 12. First Flash Go/No-Go Gate

Current result: **NO-GO**.

The gate becomes reviewable only when all of the following are true:

- [ ] exact PCB V1.0 device identified;
- [ ] exact current port identified without assuming COM7;
- [x] D: and E: cross-volume recovery assets present;
- [x] all four full-image files are 33554432 bytes and match the expected SHA-256;
- [x] partition layout remains uniquely verified;
- [ ] Flash/PSRAM configuration conflict resolved;
- [ ] serial-only firmware implemented and host-built with ESP-IDF v5.5.4;
- [ ] source/configuration allowlist passes static review;
- [ ] exact image, hash, offset, and write range pass artifact review;
- [x] host-only recovery rehearsal passes at the host/procedure level only;
- [ ] observation window, stop conditions, and rollback are reviewed;
- [ ] user explicitly authorizes the exact operation.

Future command structures may be discussed only in a non-executable placeholder form:

```text
EXAMPLE ONLY — NOT AUTHORIZED — DO NOT EXECUTE
No executable command is recorded.
```

No executable or copyable Flash or recovery command is provided. Any future packet must remain unauthorized until its exact device, current reviewed port, image, hash, range, risk, and observation plan receive separate explicit user authorization.

## 13. Cross-Volume Copy Record

The Apply preflight performed the following host-computer file operation only:

- source directory: `D:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi`;
- destination directory: `E:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi`;
- source files: the two exact reviewed 32 MiB originals;
- destination behavior: create only when absent; never overwrite a mismatching file;
- post-copy result: both destination files are 33,554,432 bytes and match the expected SHA-256;
- manifest: `E:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi\SHA256SUMS.txt`;
- D: originals: not moved, overwritten, modified, renamed, or deleted.

The D: and E: paths establish cross-volume redundancy. They are not claimed to be different physical disks.

## 14. Host-Only Recovery Rehearsal

- Rehearsal date: 2026-07-25.
- Execution scope: host-only procedure and evidence review.
- Device connection: none.
- Recovery write: not executed and not authorized.
- Tool availability: Espressif Python environment available; `esptool.py` reports version `4.12.dev3`.
- Same-device constraint: the full image is restricted to the exact ESP-VoCat PCB V1.0 unit from which repository records say it was captured; this scope was not revalidated on a live device.
- Reviewed full-image size: `33554432` bytes.
- Reviewed complete range: `0x00000000` through `0x02000000`.
- Reviewed SHA-256: `72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001`.
- Privacy impact: a complete recovery would overwrite NVS, PHY, OTA state, assets, and all captured private/device-specific data; therefore it is same-device only.

The rehearsed future separation is:

1. reverify exact image path, size, and SHA-256;
2. reverify exact same device, PCB V1.0, and user-supplied current port;
3. review the full range, privacy impact, risks, tool/version, and possible reset behavior;
4. obtain separate explicit recovery authorization;
5. execute at most one exact `WRITE` operation, with no preceding erase and no automatic retry;
6. stop after the write result;
7. observe original-device boot as a separate `READ-ONLY` operation;
8. verify required original behavior before any further project hardware work.

Operator judgment points:

- stop on any image, size, hash, device, PCB, port, or range mismatch;
- stop if a command would erase first, alter eFuses/security/voltage, add another image, or retry automatically;
- stop if privacy/same-device scope is uncertain;
- after a failed write, do not widen or retry the operation automatically;
- do not treat a successful tool exit as proof of original-device recovery.

Host-only rehearsal result: **COMPLETE for procedure review only**. It proves recovery inputs and decision points are reviewable. It does not prove that the physical device can be restored, does not authorize a recovery write, and does not change the First Flash result from **NO-GO**.

## 15. Configuration and Device-Read Gate

The Flash/PSRAM decision remains unresolved. The Firmware Implementation Gate is **CLOSED**. The next allowable proposal is the separately authorized read-only evidence collection described in `docs/hardware/pcb-v1-device-readonly-inspection-plan.md`.

The inspection must use `<REVIEWED_PORT>` until the user explicitly supplies and authorizes an exact current port. It must not include `read_flash`, Flash write, erase, recovery, eFuse write, firmware build, monitor, or peripheral activity. Device-read-only authorization, if later granted, will not authorize firmware implementation or First Flash.
