# PCB V1.0 App-Only Compatibility Assessment

- Assessment date: 2026-07-26
- OpenSpec Change: `prepare-pcb-v1-first-flash-smoke-test`
- Scope: host-only compatibility review
- Device access: none
- Compatibility classification: **B — PLAUSIBLE BUT NOT PROVEN**
- First Flash: **NO-GO**
- Device access authorization: **NONE**
- Flash authorization: **NONE**

## 1. Executive Summary

The preserved ESP32-S3 boot chain, original OTA partition layout, historical OTA metadata, and the existing ESP-IDF v5.5.4 smoke-test app were reviewed together without rebuilding firmware or accessing a device.

One app-only candidate range is geometrically and structurally coherent with the historical `ota_0` partition: the 160832-byte app can occupy `0x00020000` through `0x0004743F`, leaving `0x003C8BC0` bytes in the original `0x003F0000` slot. The app is a normal ESP32-S3 image v1, has valid checksum/hash, uses supported segment types and 64 KiB mapping alignment at that offset, and does not encode or directly depend on a factory subtype, OTA API, NVS, assets, network, or peripheral service.

The historical 2026-07-10 `otadata` snapshot contains one valid entry: `ota_seq=1`, state `VALID`, valid CRC, selecting `ota_0`; its second entry is erased. No `INVALID`, `ABORTED`, or `PENDING_VERIFY` state is present in that snapshot.

Compatibility is nevertheless **PLAUSIBLE BUT NOT PROVEN**. The current device's OTA selection and security state were not read, the locally available ESP-IDF checkout does not contain v5.5.3 source for comparison, the preserved bootloader identifies itself as vendor-modified `v5.5.3-dirty`, and that exact binary has never loaded this v5.5.4 DOUT candidate. Task 3.4 remains incomplete.

## 2. Scope

This assessment:

- revalidates the existing ignored candidate app without configure or build;
- minimally parses historical OTA metadata from a verified full backup;
- compares the preserved bootloader, partition table, OTA selection semantics, and candidate app;
- calculates a candidate app-only range;
- records compatibility evidence and residual risks.

It does not select a live device, current port, executable command, or authorized write. It does not inspect current Flash, current `otadata`, eFuses, serial output, or runtime behavior.

## 3. Safety Boundary

- Host-only and offline.
- No COM enumeration or opening.
- No device connection, reset, query, monitor, Flash, erase, restore, or eFuse operation.
- No `idf.py`, reconfigure, or build operation.
- No firmware or generated configuration change.
- Backup files remained immutable.
- Temporary slices were created only under `%TEMP%` and were not copied into Git.
- No NVS content or private recovery value was decoded or printed.

## 4. Evidence Inputs

- Verified recovery image:
  `D:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi\esp-vocat_full_flash_32MB_2026-07-10.bin`
- Independently rehashed cross-volume copy:
  `E:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi\esp-vocat_full_flash_32MB_2026-07-10.bin`
- Both files: 33554432 bytes; SHA-256
  `72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001`.
- Original bootloader, partition table, `otadata`, and `ota_0` slices extracted into a new `%TEMP%` directory.
- Existing candidate:
  `firmware/build/pcb_v1_first_flash_smoke_test.bin`.
- Candidate size: 160832 bytes (`0x00027440`).
- Candidate SHA-256:
  `1B72A60DE9C9BB42DE401A58D7772B0525AFBC4DC3D8C85BB550D2D758F99DDC`.
- Existing `sdkconfig`, ELF, map, component metadata, and project source.
- Local ESP-IDF v5.5.4 source at `D:\esp\v5.5.4\esp-idf`.
- Local esptool.py v4.12.dev3.

The stale path `firmware/build/hello_world_build_baseline.bin` does not exist in the current build directory and was not used. The repository build record and matching artifact identify the candidate by the `pcb_v1_first_flash_smoke_test.bin` name.

## 5. Original Boot Chain

The historical boot chain consists of:

1. ESP32-S3 ROM first-stage loader;
2. preserved second-stage bootloader at `0x00000000`;
3. preserved partition table at `0x00008000`;
4. standard two-sector OTA selection metadata at `0x0000D000`;
5. selected app partition from `ota_0`/`ota_1`.

The preserved bootloader is a valid ESP32-S3 image v1:

- effective image length: 16256 bytes (`0x00003F80`);
- entry point: `0x403C8908`;
- segments: 3;
- header: DIO / 80 MHz / 16 MB;
- checksum: `0x09`, valid;
- appended hash:
  `9E023028163589AD2863913F4C31E3B76DC1AB92BB5ED7378D8564622963B361`, valid;
- ESP-IDF string: `v5.5.3-dirty`;
- chip revision bounds: v0.0 through v0.99.

## 6. Original Partition Layout

| Region | Offset | Size | End-exclusive |
|---|---:|---:|---:|
| bootloader | `0x00000000` | effective `0x00003F80` | `0x00003F80` |
| partition table | `0x00008000` | `0x00001000` window | `0x00009000` |
| `nvs` | `0x00009000` | `0x00004000` | `0x0000D000` |
| `otadata` | `0x0000D000` | `0x00002000` | `0x0000F000` |
| `phy_init` | `0x0000F000` | `0x00001000` | `0x00010000` |
| `ota_0` | `0x00020000` | `0x003F0000` | `0x00410000` |
| `ota_1` | `0x00410000` | `0x003F0000` | `0x00800000` |
| `assets` | `0x00800000` | `0x00800000` | `0x01000000` |

The table was accepted by the ESP-IDF v5.5.4 partition parser. The build-generated factory layout is a separate host-build artifact and is rejected as a device-write layout.

## 7. Historical OTA Metadata

The 8192-byte historical `otadata` slice was parsed using the v5.5.4 `esp_ota_select_entry_t` definition and an equivalent implementation of `esp_rom_crc32_le(UINT32_MAX, &ota_seq, 4)`.

| Entry | Empty | `ota_seq` | State | CRC | Bootloader-valid |
|---|---|---:|---|---|---|
| sector 0 | no | 1 | `VALID` | valid | yes |
| sector 1 | yes | `UINT32_MAX` | `UNDEFINED`/erased | not a valid entry | no |

With two OTA app partitions, the source formula `(ota_seq - 1) % app_count` maps sequence 1 to `ota_0`. Entry 0 is the sole and therefore newest valid entry.

Historical rollback-state result:

- no `NEW`;
- no `PENDING_VERIFY`;
- no `INVALID`;
- no `ABORTED`;
- selected historical state is `VALID`.

This is an immutable-file fact for the 2026-07-10 backup snapshot. It is not the current device state and cannot authorize a present write.

## 8. Candidate App Metadata

- Path: `firmware/build/pcb_v1_first_flash_smoke_test.bin`
- Size: 160832 bytes (`0x00027440`)
- SHA-256:
  `1B72A60DE9C9BB42DE401A58D7772B0525AFBC4DC3D8C85BB550D2D758F99DDC`
- Target: ESP32-S3
- Image version: 1
- Entry point: `0x403752D0`
- Segments: 6
- Header: DOUT / 80 MHz / 16 MB
- Chip revision: v0.0 through v0.99
- eFuse block revision: v0.0 through v1.99
- MMU page size: 64 KiB
- Secure version: 0
- ESP-IDF: v5.5.4
- Checksum: `0x98`, valid
- Appended validation hash:
  `35F99732C55EBAEA00AFE9ECD372D0B0C681E871D175290A763A7455943C50F3`, valid

Segment load addresses:

| Segment | Load address | Type |
|---:|---:|---|
| 0 | `0x3C010020` | DROM |
| 1 | `0x3FC91E00` | internal DRAM |
| 2 | `0x40374000` | internal IRAM |
| 3 | `0x42000020` | IROM |
| 4 | `0x40379B8C` | internal IRAM |
| 5 | `0x50000000` | RTC data |

Both mapped segments satisfy the candidate partition offset's 64 KiB mapping-alignment rule.

## 9. Image Header Compatibility

ESP-IDF documents that the second-stage bootloader reads Flash mode, frequency, and size from the selected app header, allowing an app update to change these settings. The image header has DIO/DOUT but no OPI encoding.

The candidate DOUT field is therefore a standard ESP32 image-header value, not a new image format. The ESP32-S3 Octal eFuse and runtime MSPI path are separate from this header field. The preserved bootloader and candidate agree on:

- ESP32-S3 image format v1;
- 80 MHz;
- 16 MB address-space declaration;
- 64 KiB MMU page size assumptions;
- supported chip revision bounds.

The DIO-to-DOUT difference is source-explainable and not an automatic incompatibility. It remains runtime-unverified for this exact vendor-modified bootloader.

## 10. ESP-IDF Patch-Level Compatibility

The local v5.5.4 bootloader documentation explicitly states that an existing bootloader supports apps built from newer ESP-IDF releases because OTA normally updates apps without replacing the bootloader. It also requires testing with the exact deployed bootloader binary.

Only v5.5.4 is available locally; the checkout is a one-commit shallow tree and contains no v5.5.3 tag or source. Therefore:

- the common v5.5 image format and documented forward-app compatibility strongly support the pairing;
- no local source diff can prove equivalence between v5.5.3 and v5.5.4;
- the meaning of the original `-dirty` vendor changes cannot be recovered from the binary metadata;
- exact acceptance by the preserved bootloader is unverified.

## 11. Chip Revision Compatibility

Both the original bootloader and candidate app target chip ID 9, ESP32-S3. The candidate accepts revisions v0.0 through v0.99, which contains the repository-recorded device revision v0.2. The candidate eFuse block range v0.0 through v1.99 also contains the recorded block revision range.

This is header/range compatibility, not a current-device identity check.

## 12. Secure Version and Anti-Rollback

The original `ota_0` app and candidate app both declare secure version 0. The historical OTA entry is `VALID` and contains no invalid/aborted/pending state.

The candidate build has rollback and anti-rollback disabled. No lower candidate secure version is visible relative to the original app.

Residual uncertainty remains because the preserved bootloader's exact anti-rollback Kconfig and the current device eFuse secure version were not read. A secure-version-0 match in two app descriptors does not prove that an anti-rollback eFuse gate is absent.

## 13. Secure Boot and Flash Encryption Evidence

Historical visible evidence:

- the bootloader and original app are plaintext-parsable;
- both have ordinary valid appended SHA-256 hashes;
- the expected Secure Boot v2 signature-block location after the bootloader image does not contain the v2 signature magic;
- no Secure Boot signature structure was identified;
- repository hardware records classify Secure Boot and Flash Encryption as disabled.

Limitations:

- no current security eFuse query was authorized or performed;
- plaintext backup bytes and absent signature magic describe the historical recovery image, not necessarily current device state;
- no conclusion about vendor security configuration is inferred beyond the visible evidence.

## 14. Partition Subtype Compatibility

An ESP32 app image does not encode `factory`, `ota_0`, or `ota_1` as an image-header subtype. The subtype and offset are partition-table metadata used by the bootloader's selection logic.

The candidate source contains no factory/OTA label dependency. The bootloader's generic image loader validates and loads the selected app partition using the partition offset and size. Because `0x00020000` is 64 KiB aligned and the candidate's mapped segments retain correct physical/virtual alignment there, the image is structurally suitable for an OTA app partition.

This does not authorize choosing the current live slot.

## 15. App Dependency Closure

Direct application undefined references are exactly:

- `printf`;
- `puts`;
- `vTaskDelay`.

The final minimal component closure excludes:

- `nvs_flash`;
- Wi-Fi and Bluetooth;
- `esp_https_ota`;
- LCD and touch components;
- PSRAM;
- SD/MMC, FATFS, SPIFFS, and wear levelling;
- I2C/SPI peripheral driver components.

The final ELF contains the core read-only helper `esp_ota_get_running_partition`, inherited through framework dependencies, but the application does not call it. No OTA mutation, NVS, Wi-Fi, Bluetooth, Flash-write/erase, eFuse-write, filesystem, assets, or peripheral symbol is directly referenced by the application object.

The candidate:

- does not require a factory partition;
- does not read or mutate `otadata`;
- does not initialize or mutate NVS;
- does not call OTA APIs;
- does not use `assets` or a filesystem;
- does not use `phy_init`, Wi-Fi, or BLE;
- does not use custom partition labels.

## 16. Candidate App-Only Geometry

**CANDIDATE APP-ONLY RANGE — HOST-REVIEWED — NOT DEVICE-VALIDATED — NOT AUTHORIZED FOR WRITE**

| Field | Value |
|---|---:|
| candidate offset | `0x00020000` |
| candidate length | `0x00027440` |
| end-exclusive | `0x00047440` |
| last written byte | `0x0004743F` |
| original `ota_0` capacity | `0x003F0000` |
| original `ota_0` end-exclusive | `0x00410000` |
| remaining capacity | `0x003C8BC0` |

The range has no overlap with the partition table, NVS, `otadata`, PHY, `ota_1`, assets, or upper unused tail. Geometry alone is not boot compatibility or write authorization.

## 17. Original Bootloader Residual Risks

- `v5.5.3-dirty` vendor modifications are unknown.
- The exact deployed bootloader has not loaded the candidate.
- The current OTA entry and selected slot are unknown.
- The current live contents of `ota_0`/`ota_1` are unknown.
- Current Secure Boot, Flash Encryption, and anti-rollback/eFuse state were not re-read.
- DOUT/OPI behavior is source-supported but not runtime-tested on this exact boot chain.
- A partial app write could leave the historically selected slot unbootable while the historical second slot was erased.
- The ignored artifact may be deleted or replaced; any path, size, or hash change invalidates this review.

## 18. Task 3.4 Acceptance Review

Original task:

> **[READ-ONLY]** Decide whether one app-only write can be compatible with the preserved original bootloader and partition layout; stop rather than proposing a bootloader, partition-table, OTA-layout, or unused-tail change.

Operation category: `READ-ONLY`.

Evidence now available:

- exact preserved bootloader and partition metadata;
- historical standard OTA entry and selected historical slot;
- exact candidate app metadata, segments, size, hash, revision bounds, secure version, and range;
- source explanation for image loading, app-header Flash settings, OTA slot mapping, and version compatibility;
- direct dependency-closure review.

Acceptance not fully met:

- vendor `v5.5.3-dirty` modifications remain unknown;
- exact preserved-bootloader runtime acceptance is untested;
- current OTA/security state is unknown;
- no exact current device/port review exists.

No bootloader, partition-table, OTA-layout, or unused-tail change is proposed. Task 3.4 remains unchecked.

## 19. Compatibility Decision

**B — PLAUSIBLE BUT NOT PROVEN**

Host evidence supports a standard app image at the historical `ota_0` geometry and identifies no explicit format, revision, secure-version, subtype, dependency, or overlap incompatibility.

The evidence is insufficient for classification A because current state and the vendor-modified preserved bootloader remain unverified. It is not classification C because no definite incompatibility was found.

## 20. Remaining Blockers

- current exact PCB V1.0 identity and user-supplied current port;
- current OTA selection/state and live target-slot contents;
- current security/anti-rollback evidence sufficient for the exact operation;
- exact preserved-bootloader runtime acceptance of the candidate;
- human review of the draft First Flash package;
- exact observation window;
- exact rollback packet and separate rollback authorization;
- explicit operation-specific First Flash authorization.

## 21. Claims Explicitly Not Made

- The candidate is not Flash-ready or device-validated.
- The preserved bootloader is not proven to boot the candidate.
- The 2026-07-10 OTA metadata is not current state.
- No current slot was selected.
- No write, erase, restore, monitor, eFuse, or serial operation occurred.
- No PCB V1.0 hardware behavior passed.
- No recovery behavior was tested.

## 22. Source References

Repository:

- `PROJECT_CONSTITUTION.md`
- `docs/HARDWARE_PROFILE.md`
- `docs/hardware/pcb-v1-firmware-evidence-audit.md`
- `docs/hardware/pcb-v1-esp-idf-configuration-map.md`
- `docs/hardware/pcb-v1-board-configuration-decision.md`
- `docs/hardware/pcb-v1-recovery-and-first-flash-readiness.md`
- `tests/build/pcb-v1-first-flash-smoke-test-host-build.md`
- `openspec/changes/prepare-pcb-v1-first-flash-smoke-test/tasks.md`

Local ESP-IDF v5.5.4:

- `components/bootloader_support/include/esp_flash_partitions.h`
- `components/bootloader_support/src/bootloader_common_loader.c`
- `components/bootloader_support/src/bootloader_utility.c`
- `components/bootloader_support/src/esp_image_format.c`
- `components/bootloader_support/bootloader_flash/src/bootloader_flash_config_esp32s3.c`
- `components/spi_flash/esp_flash_spi_init.c`
- `docs/en/api-guides/bootloader.rst`
- `docs/en/api-guides/startup.rst`
- `components/esp_rom/include/esp_rom_crc.h`
- `components/esp_rom/linux/esp_rom_crc.c`

Offline tools:

- esptool.py v4.12.dev3 `image_info --version 2`, with no port;
- ESP-IDF v5.5.4 `gen_esp32part.py`;
- a transient in-memory OTA parser implementing the documented struct, state enum, CRC, and selection formula.
