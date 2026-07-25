# ESP-VoCat PCB V1.0 Firmware Evidence Audit

- Audit date: 2026-07-26
- Audit mode: offline evidence plus host-side audit of completed authorized read-only device evidence and local ESP-IDF v5.5.4 source
- Target evidence domain: ESP-VoCat PCB V1.0
- Device access: `chip_id`, `flash_id`, and a minimum eFuse summary were previously completed under separate authorization; no device was accessed during this ESP-IDF source audit/update

## 1. Executive Summary

Two 32 MiB full-Flash files were independently reverified in the immutable `D:` recovery directory. Both are `33554432` bytes and both match the repository-expected SHA-256 `72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001`. On 2026-07-25 the exact `E:` recovery directory was created, the two verified files were copied without overwriting either D: original, and all four files were rehashed successfully. This establishes redundancy across the D: and E: volume paths; it does not prove that the volumes are on different physical disks.

Offline scanning of one hash-matching image found 47 raw partition-magic occurrences but only one complete, valid partition-table candidate. It is at `0x00008000`, contains six bounded, non-overlapping entries, and was independently accepted by ESP-IDF's `gen_esp32part.py`. The bootloader and active `ota_0` application are valid ESP32-S3 images with valid checksums and validation hashes.

Both image headers declare `DIO`, `80 MHz`, and `16 MB`. These remain image declarations rather than measurements of physical Flash capacity, interface, or voltage.

A later, separately authorized `chip_id`/`flash_id` inspection completed with exit code 0 and no RAM stub. It directly reported ESP32-S3 QFN56 revision v0.2, embedded 16 MB PSRAM (`AP_1v8`), USB-Serial/JTAG, Flash RDID manufacturer `0xC2` / device `0x8039`, detected 32 MB capacity, an Octal/8-line Flash-type eFuse selector, and the historical line `Flash voltage set by eFuse to 3.3V`. A separate minimum read-only eFuse summary recorded `VDD_SPI_FORCE=1`, `VDD_SPI_XPD=1`, and `VDD_SPI_TIEH=0`. All emitted MAC lines were redacted. Installed-source audit shows that esptool's 3.3 V helper enters its broad branch too early, while the ESP32-S3 eFuse definitions map the independent bit combination to a forced, enabled 1.8 V LDO. The voltage interpretation conflict is resolved as a tool-logic defect; the 1.8 V conclusion remains configuration evidence, not a PCB-rail measurement.

Current First Flash assessment: **NO-GO**.

## 2. Scope

This audit covers:

- Git-tracked project safety, hardware, product, architecture, decision, test, host-build, and archived OpenSpec records;
- immutable recovery-file existence, size, timestamp, and SHA-256;
- offline Flash-image structure, partition-table, bootloader, application metadata, and a strict allowlist of component strings;
- evidence classification and blockers for a future serial-log-only PCB V1.0 smoke test.

It records the already completed, authorized connection/query results but performs no new device access. It does not cover Flash read/write/erase/restore, firmware build, live electrical measurement, current running firmware, or peripheral tests.

## 3. Safety Boundary

- A minimal port-identification step, the later `chip_id`/`flash_id` queries, and the minimum eFuse summary were separately authorized and completed before this host-side audit; the historical port is confirmed only for those recorded events.
- No COM port was enumerated or opened again during this audit/update.
- No `idf.py` command was run.
- No device-side command was rerun during this audit/update.
- The completed identification queries used the authorized port, `--no-stub`, and hard-reset behavior. The later eFuse operation was read-only `summary`. None contained Flash read/write/erase/restore or any eFuse write/protect/voltage operation.
- D: recovery originals were not moved, renamed, modified, overwritten, deleted, or used as output. Verified copies were created only at the exact reviewed E: paths.
- Original planning-round slices were written under the following historical path (the username was rendered incorrectly in that earlier record):
  `C:\Users\栗旭阳\AppData\Local\Temp\custom-vocat-pcb-v1-analysis-20260725-203936`
- Fresh Apply slices were written under:
  `C:\Users\栗旭阳\AppData\Local\Temp\custom-vocat-pcb-v1-apply-preflight-20260725-211303`
- Extracted content was not executed.
- NVS values, account data, Wi-Fi configuration, certificates, tokens, MAC addresses, and unique identifiers were not decoded or printed.
- Firmware source, generated `sdkconfig`, bootloader configuration, and partition inputs were not changed.

## 4. Sources Inspected

Repository sources:

- `AGENTS.md`
- `PROJECT_CONSTITUTION.md`
- `.gitignore`
- `docs/HARDWARE_PROFILE.md`
- `docs/PRODUCT_SPEC_DRAFT.md`
- `docs/DECISION_LOG.md`
- `docs/SYSTEM_ARCHITECTURE_DRAFT.md`
- `openspec/specs/esp-idf-host-build-baseline/spec.md`
- `openspec/changes/archive/2026-07-25-establish-esp-idf-hello-world-build-baseline/**`
- `tests/build/esp-idf-hello-world-build-baseline.md`
- the three tracked files `firmware/CMakeLists.txt`, `firmware/main/CMakeLists.txt`, and `firmware/main/main.c`

Repository inventory results:

- `tools/` is absent.
- `docs/decisions/` is absent; the decision record is `docs/DECISION_LOG.md`.
- The host baseline records a successful host-only build and explicitly states that its generated artifacts have no Flash authorization.
- No PCB V1.2 board values were found for reuse; PCB V1.2 references are safety warnings against mixing revisions.

Offline commands used against temporary file slices:

```text
& 'C:\Espressif\tools\python\v5.5.4\venv\Scripts\python.exe' 'D:\esp\v5.5.4\esp-idf\components\partition_table\gen_esp32part.py' 'C:\Users\栗旭阳\AppData\Local\Temp\custom-vocat-pcb-v1-analysis-20260725-203936\partition-table-0x00008000.bin'
& 'C:\Espressif\tools\python\v5.5.4\venv\Scripts\python.exe' -m esptool image_info --version 2 'C:\Users\栗旭阳\AppData\Local\Temp\custom-vocat-pcb-v1-analysis-20260725-203936\bootloader-candidate.bin'
& 'C:\Espressif\tools\python\v5.5.4\venv\Scripts\python.exe' -m esptool image_info --version 2 'C:\Users\栗旭阳\AppData\Local\Temp\custom-vocat-pcb-v1-analysis-20260725-203936\app-ota_0-0x00020000.bin'
```

These were offline file parsers. They specified no port and performed no device operation.

## 5. Flash Backup Identification

The exact directories came from repository-controlled documents, so no broad disk scan was performed.

| Recorded location | Result |
|---|---|
| `E:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi` | Created during Apply preflight; two verified copies and `SHA256SUMS.txt` present |
| `D:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi` | Present; two 32 MiB candidates found |

Located files:

| Path | Size | Last modified |
|---|---:|---|
| `D:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi\esp-vocat_full_flash_32MB_2026-07-10.bin` | 33554432 bytes | 2026-07-10 22:48:47 +08:00 |
| `D:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi\esp-vocat_full_flash_32MB_verify_2026-07-10.bin` | 33554432 bytes | 2026-07-10 23:08:20 +08:00 |

The two D: originals and two E: copies all have the expected size and SHA-256. D: and E: are verified as distinct volume paths only; physical-disk independence was not established.

## 6. Hash Verification

| File | SHA-256 | Expected match |
|---|---|---|
| `esp-vocat_full_flash_32MB_2026-07-10.bin` | `72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001` | Yes |
| `esp-vocat_full_flash_32MB_verify_2026-07-10.bin` | `72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001` | Yes |

The two hashes are identical and match `PROJECT_CONSTITUTION.md`, `AGENTS.md`, and `docs/HARDWARE_PROFILE.md`.

## 7. Partition Table Analysis

Method:

1. Scan the entire 32 MiB file for entry magic without assuming `0x8000`.
2. Parse 32-byte entries.
3. Require printable unique labels, known type classes, nonzero aligned ranges, bounds within the 32 MiB file, no overlap, and a valid terminator/MD5 marker.
4. Deduplicate candidates that begin inside another entry sequence.
5. Cross-check the unique result with ESP-IDF v5.5.4 `gen_esp32part.py`.

Results:

- Raw `0x50AA` magic occurrences: 47.
- Fully validated root candidates: 1.
- Confirmed backup-image partition-table offset: `0x00008000`.
- Extracted temporary 4 KiB table SHA-256:
  `CE40CFE75056EF74BC052942F8A9EE3DCE8E5E14BA17A6F63685A8FA0D11A23D`.
- MD5 marker and terminator were structurally accepted.
- No partition crosses `0x02000000`.
- No partitions overlap.

| Name | Type | Subtype | Offset | Size | Flags | State |
|---|---|---|---:|---:|---:|---|
| `nvs` | data (`0x01`) | nvs (`0x02`) | `0x00009000` | `0x00004000` (16 KiB) | `0x0` | Non-erased; privacy-protected |
| `otadata` | data (`0x01`) | ota (`0x00`) | `0x0000D000` | `0x00002000` (8 KiB) | `0x0` | Non-erased |
| `phy_init` | data (`0x01`) | phy (`0x01`) | `0x0000F000` | `0x00001000` (4 KiB) | `0x0` | Non-erased; device-data caution |
| `ota_0` | app (`0x00`) | ota_0 (`0x10`) | `0x00020000` | `0x003F0000` (4032 KiB) | `0x0` | Valid app image |
| `ota_1` | app (`0x00`) | ota_1 (`0x11`) | `0x00410000` | `0x003F0000` (4032 KiB) | `0x0` | Entire partition erased (`0xFF`) |
| `assets` | data (`0x01`) | spiffs (`0x82`) | `0x00800000` | `0x00800000` (8 MiB) | `0x0` | Non-erased |

The partitioned area ends at `0x01000000`. The entire `0x01000000–0x02000000` tail is `0xFF`. This is backup-image evidence only and does not authorize repurposing the tail.

Sensitive-region metadata recorded without content:

- `nvs`: offset `0x00009000`, size `0x00004000`, SHA-256
  `37820DDB1464096388107940C528CCD8ED0261FE792B10178D21DA7B40696D0B`.
- `phy_init`: offset `0x0000F000`, size `0x00001000`, SHA-256
  `5C7C203E450AEEA0CBF765536D255D56AF8247576F54F5D8339C1CDA6B79024F`.

No values from either region were decoded.

## 8. Bootloader Image Analysis

- Candidate range inspected: `0x00000000–0x00008000`.
- Temporary slice size: 32768 bytes.
- Validated effective image length: 16256 bytes (`0x00003F80`).
- Detected target: ESP32-S3.
- Image version: 1.
- Entry point: `0x403C8908`.
- Segment count: 3.
- Checksum: valid.
- Validation hash: valid.
- Bootloader version: 1.
- ESP-IDF: `v5.5.3-dirty`.
- Compile time: `Mar 25 2026 18:04:01`.
- Header declaration: DIO, 80 MHz, 16 MB.

This establishes a valid plaintext-parsable ESP32-S3 bootloader in the backup. It does not establish the current device's eFuse/security state or physical Flash interface.

## 9. Application Image Analysis

- Partition: `ota_0`.
- Partition range: `0x00020000–0x00410000`.
- Temporary partition slice size: 4128768 bytes (`0x003F0000`).
- Validated effective image length: 2615808 bytes (`0x0027EA00`).
- Partition SHA-256:
  `C8A2FE4AB0F9B7C823F1DCFDFC926682C9DBF1B079056461DC6F18A93A345D0E`.
- Detected target: ESP32-S3.
- Image version: 1.
- Entry point: `0x4037978C`.
- Segment count: 6.
- Checksum: valid.
- Validation hash: valid.
- Project name: `xiaozhi`.
- App version: `2.2.6`.
- Compile time: `Jul 1 2026 10:20:26`.
- ESP-IDF: `v5.5.3-dirty`.
- Secure version field: 0.
- Header declaration: DIO, 80 MHz, 16 MB.

The secure-version field is application metadata. It does not confirm eFuse state, Secure Boot state, or Flash Encryption state.

## 10. Flash Header Declarations

| Image | Mode | Frequency | Size declaration |
|---|---|---|---|
| Bootloader | DIO | 80 MHz | 16 MB |
| `ota_0` app | DIO | 80 MHz | 16 MB |

Interpretation:

- The two images agree with each other.
- The declarations cover the image build/header, not physical Flash electrical configuration.
- The declarations do not establish 1.8 V or 3.3 V.
- A 32 MiB backup file does not by itself prove the installed chip's voltage or interface.
- Repository prior device records say 32 MiB, Octal, 1.8 V. The mismatch with 16 MB/DIO headers is a configuration ambiguity, not proof that either record is false.
- No `sdkconfig.defaults` may be generated from these header fields alone.

## 11. Strings and Component Evidence

Only strict allowlisted tokens were searched in bootloader and `ota_0`; no adjacent bytes or arbitrary strings were emitted.

Observed in the application:

- `ESP-VoCat`: present.
- `PCB version V1.0`: present once.
- `ST77916`: present.
- `ES7210`: present.
- `ES8311`: present.
- `BMI270`: present.
- `esp_lcd_touch`: present.
- `Octal`, `OPI`, and `PSRAM`: present.
- `xiaozhi` and `v5.5.3-dirty`: present and consistent with image metadata.

`CST816S`/`cst816` was not found by the strict token search. Absence of a token is not proof that the controller is absent or unused.

Interpretation:

- Component names prove only that related code/configuration strings exist in the original app.
- They do not prove component population, PCB wiring, GPIO assignments, voltage, bus mode, or safe initialization.
- `BMI270` remains physically `UNVERIFIED`.
- Display, touch, audio, and PSRAM strings are not implementation authorization.

## 12. Privacy and Secret-Handling Notes

The backup contains a non-erased NVS partition and other state-bearing regions. These may contain network configuration, credentials, tokens, certificates, calibration data, device identity, or user settings.

Protection applied:

- no NVS decoding;
- no complete string dump;
- no certificate, token, SSID, password, MAC, account, or unique-ID output;
- no raw binary or partition slice placed in the repository;
- only partition existence, offset, size, and SHA-256 recorded for sensitive areas;
- no attempt to decrypt, crack, or bypass protection.

The app and bootloader were plaintext-parsable. That fact does not establish current Flash Encryption eFuse state.

## 13. Evidence Classification Matrix

| Fact ID | Hardware area | Claim | Evidence source | Exact observed evidence | Classification | Implementation usability | Conflict status | Missing evidence | Required next verification |
|---|---|---|---|---|---|---|---|---|---|
| F-001 | Target identity | Development target is ESP-VoCat PCB V1.0 | Constitution/profile | Both explicitly record PCB V1.0 | CONFIRMED (prior repository device record; not revalidated) | Scope isolation only | None | Current live identity | Exact-device read-only identity check after authorization |
| F-002 | Physical chip | MCU is ESP32-S3 | Current sanitized `chip_id`/`flash_id`; prior profile; image metadata | Current query reports ESP32-S3 QFN56 revision v0.2; both images report chip ID 9 | CONFIRMED current-device and image-target evidence | Compile target usable; exact identity must still be rechecked before a future operation | None | PCB electrical properties | Retain operation-specific identity review |
| F-003 | Backup assets | Two same-hash 32 MiB originals exist on `D:` and two verified copies exist on `E:` | Host file metadata | Four files, 33554432 bytes, expected SHA-256 | CONFIRMED host-file fact across two volume paths | Recovery input candidate | Physical-disk independence not established | Exact same-device scope still relies on prior records | Rehash before any future recovery review |
| F-004 | Flash image size | Backup covers 32 MiB address space | Host file metadata | File length `0x02000000` | CONFIRMED backup-file fact | Layout analysis only | Header says 16 MB | Current live content equality | Keep separate from the independently confirmed 32 MB RDID capacity |
| F-005 | Physical Flash capacity | Device has 32 MB Flash | Current JEDEC RDID `0xC2/0x8039`; installed capacity map; dump length | Capacity byte `0x39` maps to 32 MB | CONFIRMED current-device identification evidence | Maps to `CONFIG_ESPTOOLPY_FLASHSIZE_32MB`; no partition/write range follows | Original headers declare 16 MB | Built-artifact compatibility | Keep conservative 16 MB first candidate and inspect artifact |
| F-006 | Physical Flash bus | Flash-type eFuse selector is Octal/8-line | Current `flash_id`; ESP32-S3 eFuse/IDF source | eFuse bit 9 is set; explicit Octal maps to `CONFIG_ESPTOOLPY_OCT_FLASH` | CONFIRMED eFuse-setting evidence; source-mapped candidate | OPI STR usable for host-only candidate | Original headers declare DIO | Generated header and preserved-bootloader acceptance | Inspect built DOUT header and artifact |
| F-007 | Flash voltage | Forced 1.8 V LDO eFuse configuration; physical rail not measured | `FORCE=1`, `XPD=1`, `TIEH=0`; ESP32-S3 definitions; historical tool output | Individual bits map to forced/enabled 1.8 V; old 3.3 V helper enters a broad branch early | CONFIRMED eFuse configuration; physical rail UNVERIFIED | Safety boundary only; no setting required or allowed | Historical 3.3 V text explained as tool defect | Physical instrumentation only if separately specified | Preserve state; never alter voltage |
| F-008 | PSRAM | Device package fields decode as embedded 16 MB `AP_1v8` | Current features output; installed ESP32-S3 package-field decoder | Capacity code 3 and vendor/variant code 2 | CONFIRMED package/eFuse evidence; active mode UNVERIFIED | Capacity record only; initialization remains disabled | Exact mode/options unresolved | Mode, clock, active voltage, IDF config | Obtain traceable mode/options evidence or keep disabled |
| F-009 | Partition table | Original image table is at `0x8000` | Full-image scan and ESP-IDF parser | One of 47 magic hits uniquely passes validation | CONFIRMED for this backup image | Recovery/layout review | None | Current live Flash equality | Rehash recovery before use; no live inference |
| F-010 | Original layout | Six named partitions are bounded/non-overlapping | Parsed table | `nvs`, `otadata`, `phy_init`, `ota_0`, `ota_1`, `assets` | CONFIRMED for backup image | Preserve-only planning | None | Custom image compatibility | Artifact-layout review |
| F-011 | Original app | `ota_0` holds valid Xiaozhi 2.2.6 | `image_info` | Valid checksum/hash; project/app metadata | CONFIRMED for backup image | Recovery identification | None | Current running slot/state | Future controlled serial/read evidence |
| F-012 | `ota_1` | Original `ota_1` is erased | Full partition scan | Entire 4032 KiB is `0xFF` | CONFIRMED for backup image | No automatic write authorization | Current state unknown | Current live content | Do not assume during Flash review |
| F-013 | Upper tail | `0x01000000–0x02000000` is erased | Full-image scan | Entire 16 MiB tail is `0xFF` | CONFIRMED for backup image | Must not be repurposed | None | Reviewed future partition strategy | Separate partition-design Change |
| F-014 | Image header | Boot/app declare DIO, 80 MHz, 16 MB | `esptool image_info`; ESP-IDF header/runtime source | Matching values in both headers; source separates header and runtime Octal layers | CONFIRMED image declaration; source-mapped relationship | Supports conservative 80 MHz/16 MB candidate; DIO is not a physical bus claim | No direct conflict | Generated candidate header | Inspect artifact before any write |
| F-015 | Display component | Original app includes ST77916 strings | Allowlist scan | `ST77916` token present | STRONGLY SUPPORTED original-firmware config | Not usable in first smoke test | Wiring not proven | Schematic/measurement/init sequence | Separate display Change and test |
| F-016 | Screen touch | Profile says CST816S; strict app token absent | Hardware profile and allowlist scan | Profile `STRONGLY SUPPORTED`; no `cst816` token found | STRONGLY SUPPORTED from reference only | Not usable in first smoke test | Evidence sources incomplete | Physical model, address, pins, orientation | Separate touch Change and test |
| F-017 | Audio ADC/DAC | Original app includes ES7210/ES8311 strings | Allowlist scan/profile | Both tokens present | STRONGLY SUPPORTED | Prohibited in first smoke test | Wiring/power sequence unproven | Exact bus/pins/power behavior | Separate audio Change |
| F-018 | IMU | Original app includes BMI270 strings | Allowlist scan/profile | `BMI270` token present; profile says possibly optional | UNVERIFIED physical population | Not usable | Population unknown | Physical presence and wiring | Controlled future probe after evidence |
| F-019 | GPIO map | Profile lists PCB V1.0 source-derived pins | Hardware profile | Values explicitly marked `STRONGLY SUPPORTED` | STRONGLY SUPPORTED | No GPIO allowed in initial smoke | No electrical measurement record | Per-signal verification | Separate peripheral tests |
| F-020 | PCB V1.2 | V1.2 must not be mixed | Constitution/AGENTS/spec | Only prohibition references found | CONFIRMED process boundary | Mandatory | None | None | Continue exact-revision review |
| F-021 | eFuse/security | Minimum Flash/PSRAM/VDDSPI fields observed; unrelated state not queried | Sanitized `flash_id` and minimum eFuse summary | Flash type, package fields, FORCE/XPD/TIEH recorded; no key digest or unique ID retained | CONFIRMED for reported fields; all unrelated state UNVERIFIED | Cannot authorize mutation or infer unreported security state | Historical voltage label explained | Secure Boot/Flash Encryption/download/JTAG state remains outside this gate | Keep security features disabled; never write eFuse |
| F-022 | Current port | Queries completed on COM7 | User-reviewed port and successful query | `Serial port COM7` in both sanitized outputs | CONFIRMED for the recorded event only | Not reusable as a stable identity | Port may change | Future-operation port | Re-review before any future operation |
| F-023 | Current running state | Device was hard-reset after each query | Direct output; installed reset source | Final RTS hard reset occurred | CONFIRMED reset event; running image UNVERIFIED | No firmware-state claim | None | Post-reset running firmware | Controlled future observation only if authorized |
| F-024 | Recovery privacy | NVS contains private/stateful data | Non-erased NVS plus profile | NVS exists; content deliberately not decoded | CONFIRMED sensitive-region existence | Same-device recovery only | None | No content inspection needed | Maintain privacy controls |

## 14. Confirmed Facts

Confirmed within this audit's host/file domains:

- Two `D:` files exist, each exactly 32 MiB, and share the expected SHA-256.
- The backup contains one uniquely validated partition table at `0x00008000`.
- All six entries are bounded and non-overlapping.
- `ota_0` contains a valid ESP32-S3 `xiaozhi` 2.2.6 app image.
- The bootloader is a valid ESP32-S3 image.
- Both image headers declare DIO, 80 MHz, and 16 MB.
- `ota_1` and the upper 16 MiB tail are erased in this backup.
- NVS exists and is non-erased; its contents were protected.
- The completed queries identified ESP32-S3 QFN56 revision v0.2, USB-Serial/JTAG, Flash RDID `0xC2/0x8039`, 32 MB detected capacity, the Octal Flash-type eFuse selector, and embedded 16 MB PSRAM package fields.
- The minimum eFuse summary recorded FORCE/XPD/TIEH as `1/1/0`, supporting a forced 1.8 V LDO configuration.
- All authorized queries exited 0 and ended with a hard reset; the identification queries used no RAM stub.

Repository-recorded prior device facts were not revalidated and retain that qualification.

## 15. Strongly Supported Facts

- The original firmware includes ST77916, ES7210, ES8311, and BMI270 component strings.
- Matching reference/profile evidence supports a PCB V1.0 GPIO map, display, screen touch, top touch, audio codecs, and base UART.
- These facts are discovery inputs only and are not usable in the first serial-only smoke test.

## 16. Unverified Facts

- Current running firmware and live partition contents.
- Secure Boot, Flash Encryption, download-mode, and JTAG state. USB-Serial/JTAG was observed only in the completed query context.
- Physical Flash voltage by instrument measurement; the eFuse configuration is not that measurement.
- Enabled PSRAM mode, clock, routing, timing, and runtime behavior.
- All PCB wiring and GPIO assignments on this exact unit.
- Physical presence of BMI270.
- Safe display/touch/audio/motor/power initialization.
- Successful restore behavior.

## 17. Conflicts and Ambiguities

1. **Recovery redundancy:** the exact D: and E: volume paths now contain matching recovery files. This is cross-volume evidence, not proof of separate physical disks.
2. **Flash declaration versus current device evidence:** both original image headers declare DIO/80 MHz/16 MB; current RDID/eFuse evidence reports 32 MB and Octal. ESP-IDF source resolves these as different layers, while generated-artifact/bootloader compatibility remains to be checked.
3. **Voltage evidence:** the individual eFuse bits support a forced 1.8 V LDO configuration and explain the historical 3.3 V line as a tool defect; physical rail voltage remains unmeasured.
4. **Touch evidence:** reference/profile evidence names CST816S, but the exact token was absent from the limited app scan. Neither presence nor absence would prove PCB wiring.
5. **BMI270:** compiled component strings do not prove physical population.

## 18. First-Flash Blockers

- Implement and host-build the serial-only smoke-test firmware in a later Apply.
- Review the exact built image, DOUT header expectation, hash, effective size, preserved-bootloader/table/OTA compatibility, offset, and write range.
- Complete a host-only recovery rehearsal.
- Re-review exact device identity and current port before any future operation; the completed `COM7` observation is not a permanent port assignment.
- Obtain explicit authorization for the exact Flash operation.

## 19. Recovery Assets

- Available originals: two same-hash 32 MiB files under
  `D:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi`.
- Available cross-volume copies: two same-hash 32 MiB files under
  `E:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi`.
- The E: directory also contains `SHA256SUMS.txt` with file names, sizes, SHA-256 values, backup date, and a private-data warning.
- No D: original was overwritten, moved, or deleted.
- Expected and observed full-image SHA-256:
  `72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001`.
- Assets remain immutable and outside Git.

## 20. Go/No-Go Assessment

**NO-GO for First Flash.**

The offline evidence, sanitized device record, and local ESP-IDF source are sufficient to select a conservative host-only configuration with PSRAM disabled. They are not sufficient to authorize a device write. Cross-volume redundancy is established, but preserved-boot-chain/artifact compatibility, unbuilt artifacts, future-operation port/device review, exact image/hash/offset/range, and explicit Flash authorization remain stop conditions.

## 21. Recommended Next Change

Cross-volume recovery redundancy, authorized read-only evidence, and the ESP-IDF v5.5.4 configuration map are complete. Firmware Implementation Gate is **OPEN FOR HOST-ONLY IMPLEMENTATION**. The next allowed step is a separate minimal firmware/configuration/configure/build/static-review Apply task using PSRAM disabled. No further device access or Flash is authorized, and display, touch, audio, motor, network, NVS, security, and power behavior remain separate future Changes.

## 22. Apply-Time Independent Revalidation

This section records the independent Apply preflight performed later on 2026-07-25.

### Repository and OpenSpec gate

- Branch: `feat/prepare-pcb-v1-first-flash-smoke-test`.
- Starting HEAD, upstream reference, and remote branch: `55ef92f2835a9695467f02f4d5a24c6acdea2794`.
- Starting ahead/behind: `0/0`.
- Starting worktree: clean.
- OpenSpec: 1.6.0.
- Change status: `spec-driven`, planning artifacts complete, Apply state ready.
- Starting tasks: `0/48`.
- Change strict validation: passed.
- Full-repository strict validation: 2 passed, 0 failed.
- The archived host-build Change, synchronized host-build main spec, and three tracked firmware files were unchanged before Apply.

### Fresh partition scan

The fresh scan used the expected-hash D: original and a newly created temporary directory. It did not reuse prior slices.

- Raw `0x50AA` magic hits: 47.
- Valid root candidates: 1.
- Five additional hits were continuation entries within the valid table.
- Forty-one hits failed printable/structured label validation.
- Unique valid root: `0x00008000`.
- Entry MD5: valid.
- `0xFF` terminator: valid.
- ESP-IDF v5.5.4 partition parser: accepted.
- Bounds violations: 0.
- Partition overlaps: 0.
- `ota_1` non-`0xFF` byte count: 0 of 4,128,768.
- `0x01000000` through `0x02000000` non-`0xFF` byte count: 0 of 16,777,216.

### Fresh image analysis

Bootloader:

- target: ESP32-S3;
- image format version: 1;
- entry point: `0x403C8908`;
- segment count: 3;
- effective image length: 16,256 bytes;
- checksum: `0x09`, valid;
- appended SHA-256: present and valid;
- ESP-IDF: `v5.5.3-dirty`;
- compile time: `Mar 25 2026 18:04:01`;
- header declaration: DIO, 80 MHz, 16 MB.

`ota_0` application:

- target: ESP32-S3;
- image format version: 1;
- entry point: `0x4037978C`;
- segment count: 6;
- effective image length: 2,615,808 bytes;
- checksum: `0x8E`, valid;
- appended SHA-256: present and valid;
- project/version: `xiaozhi` 2.2.6;
- compile time: `Jul 1 2026 10:20:26`;
- ESP-IDF: `v5.5.3-dirty`;
- header declaration: DIO, 80 MHz, 16 MB.

### Limited printable-string search

- Bootloader effective range: none of the requested Flash/PSRAM/configuration keywords was found.
- App effective range: OPI, Octal, QOUT, DOUT, PSRAM, SPIRAM, MSPI, Flash-size/model text, and `1.8V` were present.
- Reviewed matches were generic driver messages, symbols, mode names, diagnostic format strings, or error text.
- `CONFIG_SPIRAM`, `CONFIG_ESPTOOLPY`, `CONFIG_ESPTOOLPY_FLASHSIZE`, `CONFIG_ESPTOOLPY_FLASHMODE`, and `CONFIG_ESPTOOLPY_FLASHFREQ` were not found.
- Presence does not prove a Kconfig option or physical configuration; absence does not prove a feature is unavailable.
- A Git-tracked-text search found prior summary claims but no raw `flash_id`, chip-identification, Flash manufacturer/device-ID, capacity, or observed PSRAM-size transcript.

The detailed board decision is recorded in `docs/hardware/pcb-v1-board-configuration-decision.md`, and the exact ESP-IDF mapping is recorded in `docs/hardware/pcb-v1-esp-idf-configuration-map.md`. The Firmware Implementation Gate is **OPEN FOR HOST-ONLY IMPLEMENTATION** with PSRAM disabled; First Flash remains **NO-GO**.

## 23. Authorized Device-Query Evidence and Installed-Source Audit

The identification queries were explicitly limited to `chip_id` and `flash_id` on the then-reviewed port. Both exited 0, used esptool.py `v4.12.dev3` with `--no-stub`, printed sanitized identification evidence, and ended with `Hard resetting via RTS pin...`. The separately authorized minimum eFuse operation was read-only `summary`, exited 0, and ended with a successful hard reset. No monitor, Flash read/program/erase, eFuse write/protect/voltage operation, restore, build, or firmware Flash was executed. MAC was replaced with `<REDACTED>`.

Installed-source audit established:

- `Embedded PSRAM 16MB (AP_1v8)` comes from ESP32-S3 package/eFuse fields. `AP_1v8` is the source label for PSRAM vendor/variant code 2; it does not describe external Flash voltage or PSRAM bus mode.
- Octal/8-line comes from ESP32-S3 eFuse repeat-data register bit 9 and is not the original image header's DIO field.
- manufacturer `0xC2`, device `0x8039`, and 32 MB are derived from JEDEC RDID, with capacity byte `0x39` mapped to 32 MB.
- ESP32-S3 inherits the generic voltage helper. In the installed source its first branch returns 3.3 V for any nonzero value among `VDD_SPI_FORCE`, `VDD_SPI_XPD`, and `VDD_SPI_TIEH`; the later eFuse 1.8 V/OFF branches cannot be reached. The observed `eFuse` source label therefore does not reveal a unique raw bit pattern or establish the physical Flash rail.
- `--no-stub` prevented RAM-stub upload. The common ROM-loader path still performed volatile watchdog/control-register changes, SPI attachment, Flash reset commands `0x66`/`0x99`, and a final hard reset. No persistent Flash/eFuse/NVS write path was found for the completed operations.

The exact sanitized output, source paths/functions, limitations, and conflict matrix are preserved in `docs/hardware/pcb-v1-device-readonly-inspection-result.md`. The completed minimum summary resolves the old 3.3 V interpretation at the bit/source level without claiming a measured PCB rail. Its authorization is consumed and closed; no further device access is authorized.

## 24. ESP-IDF v5.5.4 Configuration Mapping Result

The clean local ESP-IDF tree at `D:\esp\v5.5.4\esp-idf` is tag `v5.5.4`, revision `735507283d5b2f9fb363a1901172dbd9e847945d`. Host-only source audit established:

- physical 32 MB maps to `CONFIG_ESPTOOLPY_FLASHSIZE_32MB`; a 16 MB image header is legal on a detected larger chip and remains the conservative first candidate because the preserved layout ends at 16 MB;
- explicit Octal Flash maps to `CONFIG_ESPTOOLPY_OCT_FLASH=y` and `CONFIG_ESPTOOLPY_FLASHMODE_OPI=y`; STR is selected because DTR capability is not proven;
- explicit OPI generates a DOUT image-header mode because the header has no OPI encoding and ROM obtains Octal handling from eFuse;
- the original DIO header and physical/runtime Octal operation are different layers and can coexist through the eFuse/MSPI startup path;
- 80 MHz maps to `CONFIG_ESPTOOLPY_FLASHFREQ_80M=y` and is supported by both original headers and the ESP32-S3 target default;
- PSRAM enable/mode/frequency symbols are mapped, but no 16 MB capacity Kconfig exists; runtime density is detected after initialization, so `CONFIG_SPIRAM` remains off;
- the integrated console maps to `CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG=y` and needs no custom UART GPIO;
- no ESP32-S3 tracked configuration is required or allowed to set VDDSPI.

The complete source table, allowlist, denylist, candidate values, and limitations are in `docs/hardware/pcb-v1-esp-idf-configuration-map.md`.

Firmware Implementation Gate: **OPEN FOR HOST-ONLY IMPLEMENTATION**.

First Flash: **NO-GO**. No Flash authorization or further device-access authorization exists. The generated app artifact, preserved bootloader/table/OTA compatibility, exact offset/range, and operation packet remain blocked future reviews.
