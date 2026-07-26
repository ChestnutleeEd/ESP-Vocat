# PCB V1.0 First-Flash Smoke-Test Host Build

## 1. Purpose

Record the host-only implementation, configure, build, static safety review, and offline artifact review for the minimal ESP-VoCat PCB V1.0 serial smoke-test candidate.

## 2. Scope

The candidate emits fixed identity, compile-time ESP-IDF version, safety-state, and ready-marker text, then remains in a one-second FreeRTOS delay loop. It initializes no project peripheral, storage, network, security, or device-identification feature.

## 3. Safety Boundary

- Host operations only.
- No COM or serial-port enumeration or opening.
- No device connection, reset, query, monitor, Flash, erase, restore, or eFuse operation.
- ESP-IDF-generated binaries are not device-validated and are not approved for Flash.
- Build-generated offsets are metadata only and are not a device-write layout or authorization.

## 4. Source Baseline

- Repository: `D:/ESP-VoCat-Project/custom-vocat`
- Branch: `feat/prepare-pcb-v1-first-flash-smoke-test`
- Starting HEAD and upstream: `c63e010be894483ea692deade8b3084d3be13bdd`
- Starting ahead/behind: `0/0`
- Starting worktree: clean
- Starting firmware: the tracked three-file host-build baseline
- Implementation commit: `017707640d8cf6d4f0ded94ed0726d202a58944a`

## 5. OpenSpec Change

- Change: `prepare-pcb-v1-first-flash-smoke-test`
- Schema: `spec-driven`
- Starting progress: `11/48`
- Starting Change validation: passed
- Starting repository validation: 2 passed, 0 failed
- The Change remains active and is not archived.

## 6. Configuration Evidence

`docs/hardware/pcb-v1-esp-idf-configuration-map.md` was re-read against the local ESP-IDF v5.5.4 source before editing. Its formal candidate is unambiguous:

- target `esp32s3`;
- conservative 16 MB image header/address-space limit on the confirmed 32 MB physical Flash;
- explicit Octal Flash, OPI runtime mode, STR sampling, and 80 MHz;
- expected generated DOUT image header because the image header has no OPI encoding;
- USB Serial/JTAG primary console and no secondary console;
- PSRAM disabled;
- default single-app build partition table only for host review;
- Secure Boot, Flash Encryption, rollback/anti-rollback, and eFuse mutation disabled or absent.

The corresponding symbols and dependencies were independently found in the local v5.5.4 Kconfig files. No target symbol was placed in `sdkconfig.defaults`; target selection remained a separate `idf.py set-target esp32s3` action.

## 7. sdkconfig.defaults

Tracked file: `firmware/sdkconfig.defaults`

```text
CONFIG_ESPTOOLPY_OCT_FLASH=y
CONFIG_ESPTOOLPY_FLASH_MODE_AUTO_DETECT=y
CONFIG_ESPTOOLPY_FLASHMODE_OPI=y
CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE_STR=y
CONFIG_ESPTOOLPY_FLASHFREQ_80M=y
CONFIG_ESPTOOLPY_FLASHSIZE_16MB=y
# CONFIG_ESPTOOLPY_HEADER_FLASHSIZE_UPDATE is not set

# PSRAM support is intentionally excluded from the minimal-build dependency closure.

CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG=y
CONFIG_ESP_CONSOLE_SECONDARY_NONE=y

CONFIG_PARTITION_TABLE_OFFSET=0x8000
# CONFIG_SECURE_BOOT is not set
# CONFIG_SECURE_FLASH_ENC_ENABLED is not set
# CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK is not set
```

The top-level CMake file enables ESP-IDF `MINIMAL_BUILD`, so `esp_psram` and excluded optional components are not in the project Kconfig/build dependency closure. This is why no `CONFIG_SPIRAM`, mode, frequency, heap, or runtime initialization symbol appears in the final generated configuration.

## 8. Toolchain Activation

The existing environment was activated process-locally for every ESP-IDF command by dot-sourcing:

```text
C:\Espressif\tools\Microsoft.v5.5.4.PowerShell_profile.ps1
```

No install, repair, update, upgrade, or tool modification was performed.

## 9. Resolved Tool Paths and Versions

| Item | Resolved value |
|---|---|
| `IDF_PATH` | `D:\esp\v5.5.4\esp-idf` |
| ESP-IDF revision | tag `v5.5.4`, Git `735507283d5b2f9fb363a1901172dbd9e847945d` |
| `IDF_TOOLS_PATH` | `C:\Espressif\tools` |
| `idf.py` | PowerShell alias `Invoke-idfpy`, ESP-IDF `v5.5.4`, script under `D:\esp\v5.5.4\esp-idf\tools` |
| Python | `C:\Espressif\tools\python\v5.5.4\venv\Scripts\python.exe`, `3.13.9` |
| CMake | `C:\Espressif\tools\cmake\3.30.2\bin\cmake.exe`, `3.30.2` |
| Ninja | `C:\Espressif\tools\ninja\1.12.1\ninja.exe`, `1.12.1` |
| esptool | Python package `v4.12.dev3` |

`C:\Espressif` is a directory junction to `E:\Espressif`; CMake may print the canonical `E:` path during a sub-action. This is the same installed tool tree, not a fallback toolchain.

## 10. Static Safety Review

Reviewed tracked firmware inputs:

- `firmware/CMakeLists.txt`
- `firmware/main/CMakeLists.txt`
- `firmware/main/main.c`
- `firmware/sdkconfig.defaults`

Direct application calls are limited to `puts`, `printf`, and `vTaskDelay`. The application object has exactly those three undefined references and defines only `app_main`.

No project source contains a GPIO number, GPIO call, board driver, device-identification query, partition read, Flash read/write/erase, eFuse call, restart call, NVS call, OTA call, network call, filesystem call, or display/touch/audio/motor/power initialization. The fixed logs cannot emit a MAC, chip identifier, credential, token, NVS value, or recovery content.

The final minimal component closure excludes Wi-Fi, Bluetooth, `nvs_flash`, `esp_https_ota`, LCD, touch, SD/MMC, FATFS, SPIFFS, wear levelling, PSRAM, and Wi-Fi provisioning. Core ESP-IDF dependencies still contain generic framework components such as `app_update`, `efuse`, `esp_driver_gpio`, and SoC metadata. Final-ELF symbol review found the core read-only `esp_ota_get_running_partition` and restart support, but no `esp_ota_*` mutation, eFuse-write, Flash-write/erase, NVS, Wi-Fi, or Bluetooth symbol. The project does not call the core restart symbol. This review does not claim that the ESP-IDF framework contains no related strings or support code.

## 11. Configure Command and Result

- Working directory: `D:\ESP-VoCat-Project\custom-vocat\firmware`
- Final command: `idf.py set-target esp32s3`
- Final exit code: `0`
- Result: target `esp32s3`; `Minimal build - ON`; defaults loaded; CMake configure/generate complete.
- Generated outputs: ignored `firmware/sdkconfig`, `firmware/sdkconfig.old`, and `firmware/build/`.
- No port was specified or accessed.

Configure history retained for honesty:

1. The first successful configure used the initial non-minimal CMake input.
2. Its later build showed that default ESP-IDF configuration compiled many unused optional component archives. `MINIMAL_BUILD ON` was added to enforce the dependency boundary.
3. The next configure warned that `SPIRAM` was unknown because `esp_psram` had correctly left the minimal Kconfig closure. The ineffective negative assignment was replaced with a plain explanatory comment; no generated `sdkconfig` was edited.
4. One rerun was mistakenly started from the repository root and exited `2` because that directory has no ESP-IDF `CMakeLists.txt`. It touched no firmware generated state and performed no device operation.
5. The corrected final configure from `firmware/` exited `0` with no unknown, deprecated, unrecognized, or conflict warning.

## 12. Resolved sdkconfig Audit

| Area | Final resolved value |
|---|---|
| Target | `CONFIG_IDF_TARGET="esp32s3"`, `CONFIG_IDF_TARGET_ESP32S3=y` |
| Octal Flash | `CONFIG_ESPTOOLPY_OCT_FLASH=y` |
| Runtime mode | `CONFIG_ESPTOOLPY_FLASHMODE_OPI=y` |
| Generated header mode string | `CONFIG_ESPTOOLPY_FLASHMODE="dout"` |
| Auto-detect consistency path | `CONFIG_ESPTOOLPY_FLASH_MODE_AUTO_DETECT=y` |
| Sampling | `CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE_STR=y` |
| Frequency | `CONFIG_ESPTOOLPY_FLASHFREQ_80M=y`, `"80m"` |
| Header/address-space size | `CONFIG_ESPTOOLPY_FLASHSIZE_16MB=y`, `"16MB"` |
| Header size auto-update | not set |
| PSRAM | component and all `CONFIG_SPIRAM*` symbols absent from minimal closure |
| Primary console | `CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG=y` |
| Secondary console | `CONFIG_ESP_CONSOLE_SECONDARY_NONE=y` |
| UART console | `CONFIG_ESP_CONSOLE_UART_NUM=-1` |
| Partition strategy | default single factory app/no OTA, `partitions_singleapp.csv` |
| Partition-table offset | `0x8000` |
| Secure Boot | not set |
| Flash Encryption | not set |
| App rollback | not set |
| Anti-rollback | unavailable/off because rollback is off |

Final configure logs contain no Kconfig or compiler warning.

## 13. Build Command and Result

- Working directory: `D:\ESP-VoCat-Project\custom-vocat\firmware`
- Final command: `idf.py build`
- Final exit code: `0`
- Ninja project steps: `527`
- Result: `Project build complete.`
- Compiler warnings: none
- App binary: `0x27440` / 160832 bytes
- Default host-build app partition: `0x100000` / 1048576 bytes
- Remaining default partition capacity: `0xD8BC0` / 887744 bytes, 85%
- `idf.py size` exit code: `0`
- Size-tool total image size before BIN padding: 160713 bytes

The earlier non-minimal build also exited `0`, but it is superseded and not the reviewed final artifact.

## 14. Internal esptool Image Generation

Ninja internally invoked esptool.py `v4.12.dev3` to convert the bootloader and application ELF files into local ESP32-S3 images. Those invocations:

- specified no serial port;
- did not enumerate or open a device;
- did not read, write, or erase physical Flash;
- did not access eFuses;
- granted no Flash authorization.

ESP-IDF printed its generic post-build Flash suggestions. No suggested command was executed, adopted, or recorded as an operation packet.

## 15. Artifact Inventory

All paths are relative to `firmware/`. All files in this table are ignored generated outputs and are not tracked.

| Artifact | Size (bytes) | SHA-256 |
|---|---:|---|
| `build/pcb_v1_first_flash_smoke_test.bin` | 160832 | `1B72A60DE9C9BB42DE401A58D7772B0525AFBC4DC3D8C85BB550D2D758F99DDC` |
| `build/pcb_v1_first_flash_smoke_test.elf` | 3028636 | `DF6C33F20E948589634AC4335F2619BD7F97372C53513C509A49ED6E6C3B485E` |
| `build/pcb_v1_first_flash_smoke_test.map` | 2337132 | `42FFE77AC38F70E877004A215B3878366833CBD1D748A7D2F2F57B6E2C0A07E0` |
| `build/bootloader/bootloader.bin` | 20832 | `50B246AAC93AF402001E09A10D16BA6CD53D0EC97A8BD7386B6B52008DCCD8F5` |
| `build/partition_table/partition-table.bin` | 3072 | `7F00B6C042A89B15B0CAC534F82ED988CAF29278FF5700B0C511EB1B5BB7C820` |
| `build/flasher_args.json` | 977 | `0D4A5B6744EEA4483FFEA6B052D7BD8673B042D4C6CA5A8CE7D09457E843A04B` |
| `build/project_description.json` | 100481 | `A9D0DE4F63648A51DA91D7A9A951B02FA9A4EA74B0BB32C02BD44D9397E69C73` |
| `sdkconfig` | 46456 | `C610E1A5A4C84A6F685573DF3C1FD8C466433439B005E7CC7230862496652D3B` |

## 16. Image Header Review

Offline command category: local-file `python -m esptool image_info --version 2`; no port.

| Field | Bootloader | Application |
|---|---|---|
| File length | 20832 | 160832 |
| Target | ESP32-S3 | ESP32-S3 |
| Image version | 1 | 1 |
| Entry point | `0x403C8920` | `0x403752D0` |
| Segment count | 3 | 6 |
| Flash mode | DOUT | DOUT |
| Flash frequency | 80 MHz | 80 MHz |
| Flash size | 16 MB | 16 MB |
| Checksum | `0x8E`, valid | `0x98`, valid |
| Appended validation hash | `708CFA29E9C25FFE08FE69EB41C9935938CB15C187D20DC2D0B559467AC667CB`, valid | `35F99732C55EBAEA00AFE9ECD372D0B0C681E871D175290A763A7455943C50F3`, valid |
| ESP-IDF | v5.5.4 | v5.5.4 |
| Chip revision bounds | v0.0-v0.99 | v0.0-v0.99 |
| Secure version | not applicable | 0 |

The explicit OPI candidate produced the expected DOUT header. This is a build declaration, not proof that the physical device will boot it.

## 17. Build-Generated Layout

**BUILD-GENERATED LAYOUT — NOT REVIEWED FOR DEVICE WRITE — NOT AUTHORIZED**

| Generated item | Offset |
|---|---:|
| Bootloader | `0x00000000` |
| Partition table | `0x00008000` |
| Factory app | `0x00010000` |

Generated partition entries:

- `nvs`: `0x9000`, 24 KiB;
- `phy_init`: `0xF000`, 4 KiB;
- `factory`: `0x10000`, 1 MiB.

No custom partition CSV was created. These values must not be combined with a port or treated as a future write plan.

## 18. Original Layout Comparison

The preserved original layout contains 16 KiB NVS at `0x9000`, `otadata` at `0xD000`, PHY at `0xF000`, and `ota_0` at `0x20000` with capacity `0x3F0000`. The generated default layout omits `otadata`, enlarges NVS to 24 KiB, and places a factory app at `0x10000`.

Therefore the generated partition table and generated app offset are incompatible with the preserved original layout and are rejected for device-write use. The standalone application binary is smaller than the original `ota_0` capacity: `0x3F0000 - 0x27440 = 0x3C8BC0` bytes of geometric margin. This fit calculation does not select or authorize `0x20000` as a write target.

## 19. App-Only Compatibility Assessment

Task 3.4 remains incomplete.

Positive host-only evidence:

- candidate and original images target ESP32-S3;
- device revision v0.2 is within the candidate v0.0-v0.99 bounds;
- candidate app uses ESP-IDF v5.5.4 and the preserved bootloader reports v5.5.3-dirty;
- candidate header retains 80 MHz and 16 MB, with the expected explicit-OPI DOUT mode;
- candidate secure version is 0, matching the recorded original app secure version 0;
- generated Secure Boot, Flash Encryption, rollback, and anti-rollback settings are off;
- the candidate size fits geometrically within the original `ota_0` capacity.

Remaining blockers:

- current live `otadata` selection/state was not read and is not authorized to be read in this round;
- no future app-only offset or byte range has been selected or approved;
- the generated default factory-app subtype/offset differs from the preserved OTA layout;
- the preserved v5.5.3-dirty bootloader has not actually booted this v5.5.4 DOUT candidate;
- image-header compatibility is source-plausible but not device-verified;
- the standalone app-only packet and human review do not exist.

Conclusion: app-only compatibility is **plausible but not proven**. First Flash remains `NO-GO`; do not widen the write to the generated bootloader or partition table.

## 20. Git Ignore Review

Confirmed with `git check-ignore -v --no-index`:

- `firmware/build/` — `.gitignore` line 42;
- `firmware/sdkconfig` — `.gitignore` line 43;
- `firmware/sdkconfig.old` — `.gitignore` line 44.

No `.bin`, `.elf`, `.map`, dump, generated `sdkconfig`, or build output is tracked or staged.

## 21. Device Operations Not Performed

- No serial-port enumeration or opening.
- No device connection, reset, or access.
- No `chip_id`, `flash_id`, eFuse summary, or other device query.
- No `read_flash`, `write_flash`, `erase_flash`, Flash, monitor, restore, or rollback.
- No eFuse, Secure Boot, Flash Encryption, anti-rollback, VDDSPI, or voltage operation.

## 22. Claims Explicitly Not Made

- PCB V1.0 has not run this candidate.
- Flash electrical mode and timing are not runtime-verified.
- PSRAM is not runtime-verified and remains intentionally excluded.
- Display, touch, audio, microphone, motor, network, storage, power control, and GPIOs are untested.
- The original bootloader is not proven to accept the candidate.
- Generated outputs are not approved for Flash.
- This is not a PCB V1.0 hardware pass or a recovery test.

## 23. Task Evidence

Evidence supports completion of firmware Tasks 4.1-4.4, host-only Tasks 5.1-5.3, static-review Tasks 6.1-6.3, and offline image inspection Task 7.1. Task 3.4 and Tasks 7.2-7.4 remain incomplete because no exact future app-only offset/range or unauthorized operation packet is selected.

Final evidence-recording Tasks 15.1-15.4 are supported by the updated hardware/readiness records, this test record, strict validations, scope audits, the two requested commits, and push verification. No device-stage task is completed.

## 24. Go/No-Go Results

- Firmware Implementation Gate: **HOST-ONLY IMPLEMENTATION COMPLETED**
- Host-only configure/build: **PASS**
- Static safety review: **PASS**
- Offline image/header review: **PASS**
- App-only compatibility: **INCOMPLETE / PLAUSIBLE, NOT PROVEN**
- First Flash: **NO-GO**
- Device access authorization: **NONE**
- Flash authorization: **NONE**

## 25. Remaining Blockers

- resolve Task 3.4 with reviewed OTA metadata/selection evidence and preserved-bootloader compatibility;
- select and human-review one exact app-only image, hash, offset, length, end address, and byte range without treating the generated layout as authoritative;
- reverify recovery assets immediately before a future pre-Flash review;
- review the exact physical device and user-supplied current port;
- review observation window, stop conditions, and rollback packet;
- obtain explicit operation-specific Flash authorization.

## 26. Lifecycle Status

The implementation is committed as `017707640d8cf6d4f0ded94ed0726d202a58944a`. Documentation and OpenSpec evidence are recorded in the following requested evidence commit. The Change remains active. No device stage has started.

Final safety state:

```text
Firmware Implementation Gate: HOST-ONLY IMPLEMENTATION COMPLETED
First Flash: NO-GO
Device access authorization: NONE
Flash authorization: NONE
Artifacts: NOT DEVICE-VALIDATED / NOT APPROVED FOR FLASH / NOT A PCB V1.0 HARDWARE PASS
```
