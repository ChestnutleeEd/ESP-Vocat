# ESP-VoCat PCB V1.0 Recovery and First-Flash Readiness

- Assessment date: 2026-07-25
- Device operations performed: none
- Current decision: **NO-GO**

## 1. Current Known Recovery Assets

Repository policy defines two immutable recovery locations:

- Primary: `E:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi`
- Cross-disk: `D:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi`

During this audit, the `E:` directory was not present. The `D:` directory was present and contained two expected full-image files. The files were read only for size, hash, and offline structure.

## 2. Full Backups

| Path | Size | SHA-256 |
|---|---:|---|
| `D:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi\esp-vocat_full_flash_32MB_2026-07-10.bin` | 33554432 bytes | `72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001` |
| `D:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi\esp-vocat_full_flash_32MB_verify_2026-07-10.bin` | 33554432 bytes | `72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001` |

The hashes are identical and match repository policy. Because both currently available files are on `D:`, they do not satisfy the intended cross-disk failure isolation.

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

1. Restore two-disk recovery redundancy, including the recorded `E:` location or a newly reviewed equivalent.
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
- The `E:` recovery directory is missing.
- No host-only recovery rehearsal is complete.
- The exact current device and port are not reviewed.
- No explicit operation-specific authorization exists.

OpenSpec artifacts can prepare implementation and a command structure; they cannot authorize execution.

## 12. First Flash Go/No-Go Gate

Current result: **NO-GO**.

The gate becomes reviewable only when all of the following are true:

- [ ] exact PCB V1.0 device identified;
- [ ] exact current port identified without assuming COM7;
- [ ] two-disk recovery assets present;
- [ ] both full images are 33554432 bytes and match the expected SHA-256;
- [ ] partition layout remains uniquely verified;
- [ ] Flash/PSRAM configuration conflict resolved;
- [ ] serial-only firmware implemented and host-built with ESP-IDF v5.5.4;
- [ ] source/configuration allowlist passes static review;
- [ ] exact image, hash, offset, and write range pass artifact review;
- [ ] host-only recovery rehearsal passes;
- [ ] observation window, stop conditions, and rollback are reviewed;
- [ ] user explicitly authorizes the exact operation.

Future command structures may be discussed only in a non-executable placeholder form:

```text
EXAMPLE ONLY — NOT AUTHORIZED — DO NOT EXECUTE
esptool.py --port <REVIEWED_PORT> write_flash <REVIEWED_OFFSET> <REVIEWED_IMAGE>
```

The placeholder is incomplete by design. It contains neither a real port nor a real write offset and grants no Flash authority.
