# ESP-VoCat PCB V1.0 ESP-IDF Configuration Map

- Audit date: 2026-07-26
- Target: ESP-VoCat PCB V1.0 only
- Framework source: `D:\esp\v5.5.4\esp-idf`
- Framework revision: `v5.5.4` / Git `735507283d5b2f9fb363a1901172dbd9e847945d`
- Installed esptool source: `v4.12.dev3`
- Device access during this audit: none

## 1. Executive Summary

The committed device evidence and local ESP-IDF v5.5.4 source now support one exact, conservative configuration candidate for a future host-only serial smoke-test implementation. The key distinction is between physical/package evidence, image-header declarations, and the runtime MSPI mode:

- the queried Flash is physically identified as 32 MB and `FLASH_TYPE=1` selects eight data lines;
- the original bootloader and app headers declare DIO, 80 MHz, and 16 MB;
- ESP-IDF v5.5.4 can place a conservative non-OPI value in the image header while the ROM and later startup stages use the Flash-type eFuse and MSPI logic to enter Octal operation;
- the explicit ESP-IDF Octal choice maps to an OPI runtime configuration but deliberately emits `dout` in the image header because that header has no OPI encoding and the ROM derives Octal handling from eFuse;
- the physical 32 MB capacity does not require the first smoke test to expose or use the upper 16 MB;
- the embedded 16 MB PSRAM is package/eFuse evidence, but its active clock, routing, and complete runtime configuration are not proven; the minimal program can safely leave `CONFIG_SPIRAM` disabled;
- the integrated USB Serial/JTAG controller has an exact console Kconfig choice and requires no application-selected GPIO.

The Firmware Implementation Gate is therefore **OPEN FOR HOST-ONLY IMPLEMENTATION**. This permits only a later, separate task to edit firmware, create a reviewed candidate `sdkconfig.defaults`, configure, build, and statically inspect artifacts without a device. It is not a Flash-readiness decision.

First Flash remains **NO-GO**. There is no Flash authorization and no further device-access authorization. The exact app artifact, original bootloader compatibility, OTA selection state, app offset, byte range, and write packet remain separate human gates.

## 2. Scope

This document maps already committed PCB V1.0 evidence to local ESP-IDF v5.5.4 Kconfig, CMake, Python, bootloader, MSPI, Flash, PSRAM, partition, and console source. It forms a configuration candidate only.

It does not:

- create or modify firmware, `sdkconfig`, `sdkconfig.defaults`, or a partition table;
- run `idf.py`, configure, build, Flash, monitor, erase, restore, or any device command;
- select an executable app offset or write range;
- verify physical voltage, signal integrity, PCB routing, runtime PSRAM, or a generated binary;
- authorize device access or Flash.

## 3. Safety Boundary

- No COM port was enumerated or opened.
- No device was connected, reset, queried, or otherwise accessed.
- No eFuse read or write was performed.
- No Flash, NVS, bootloader, partition table, firmware, `sdkconfig`, Secure Boot state, or Flash Encryption state was changed.
- VDDSPI must not be changed by firmware, bootloader configuration, a host tool, or trial Flashing.
- Secure Boot, Flash Encryption, anti-rollback, eFuse mutation, unknown GPIO, network, OTA, NVS mutation, and all unverified peripherals remain denied.
- All image and partition values below are host-side candidates or historical declarations, never write authorization.

## 4. Evidence Inputs

The mapping independently reuses the committed, sanitized evidence in:

- `docs/hardware/pcb-v1-device-readonly-inspection-result.md`;
- `docs/hardware/pcb-v1-board-configuration-decision.md`;
- `docs/hardware/pcb-v1-firmware-evidence-audit.md`;
- `docs/hardware/pcb-v1-recovery-and-first-flash-readiness.md`;
- `docs/hardware/pcb-v1-device-readonly-inspection-plan.md`.

Confirmed inputs relevant to this map are:

- ESP32-S3, QFN56, revision v0.2, 40 MHz crystal, USB-Serial/JTAG;
- external Flash RDID manufacturer `0xC2`, device `0x8039`, detected capacity 32 MB;
- `FLASH_TYPE=1`, decoded as eight data lines;
- package eFuse fields `FLASH_CAP=None` and `FLASH_VENDOR=None`; these do not negate the external JEDEC Flash;
- `VDD_SPI_FORCE=1`, `VDD_SPI_XPD=1`, `VDD_SPI_TIEH=0`;
- embedded PSRAM capacity 16 MB, `PSRAM_VENDOR=AP_1v8`, `PSRAM_CAP_3=0`;
- original ESP32-S3 bootloader and app headers: DIO, 80 MHz, 16 MB;
- original bootloader/app ESP-IDF version: `v5.5.3-dirty`;
- original partition table at `0x8000`, `ota_0` at `0x20000`, all partitions ending at `0x1000000`;
- erased `ota_1` and erased upper range `0x01000000-0x02000000`.

The eFuse query exited 0, ended in a successful hard reset, performed no persistent write, and retained no real MAC or unique identifier in the repository.

## 5. ESP-IDF Source Version

`D:\esp\v5.5.4\esp-idf` exists, is a clean detached Git worktree, resolves to tag `v5.5.4`, and has revision `735507283d5b2f9fb363a1901172dbd9e847945d`.

The relevant implementation is under `components/spi_flash`; there is no `components/esp_flash` directory in this checkout. The target-specific Flash-frequency Kconfig is included from `components/spi_flash/esp32s3/Kconfig.flash_freq`. The root has `Kconfig` but no root `Kconfig.projbuild`; component `Kconfig.projbuild` files supply project options.

The environment was not activated, `idf.py` was not run, and neither ESP-IDF nor installed esptool was modified.

## 6. Flash Capacity Mapping

The exact physical-capacity Kconfig choice is:

```text
CONFIG_ESPTOOLPY_FLASHSIZE_32MB=y
CONFIG_ESPTOOLPY_FLASHSIZE="32MB"
```

`components/esptool_py/Kconfig.projbuild:112-145` defines sizes 1, 2, 4, 8, 16, 32, 64, and 128 MB, with 2 MB as the generic default. There is no target-specific dependency on the 32 MB choice.

`components/esptool_py/project_include.cmake:28-38,124-138` passes `CONFIG_ESPTOOLPY_FLASHSIZE` to esptool `elf2image` for both the app and, under `BOOTLOADER_BUILD`, the second-stage bootloader. The local esptool target inherits `FLASH_SIZES["32MB"] = 0x50` from `esptool/targets/esp32.py:81-90`; the high nibble is stored with the frequency nibble in byte 3. `components/bootloader_support/include/esp_app_format.h:62-86` defines the matching `ESP_IMAGE_FLASH_SIZE_32MB` header value.

The same value is passed to partition generation by `components/partition_table/CMakeLists.txt:43-45,70-77`, so a partition end beyond the configured size is rejected at build time.

The ESP32-S3 second-stage bootloader does read the Flash JEDEC ID: `bootloader_flash_hardware_init()` calls `bootloader_flash_update_id()` in `components/bootloader_support/bootloader_flash/src/bootloader_flash_config_esp32s3.c:38-47,339-350`, which uses RDID through `bootloader_read_flash_id()` in `components/bootloader_support/bootloader_flash/src/bootloader_flash.c:888-892`. That ID is used for vendor/startup handling. It does not automatically replace the configured capacity: `update_flash_config()` in the ESP32-S3 file decodes the image-header size and passes that size to the ROM Flash parameters. Physical-ID detection and usable image-header size are therefore distinct.

The runtime check is in `components/spi_flash/esp_flash_spi_init.c:576-594`:

- a detected physical chip smaller than the header is an error;
- a detected chip larger than the header produces a warning and the header size is used;
- a header larger than 16 MB triggers a 32-bit-address feature check;
- the usable default Flash size is set from the image header.

Therefore, a 16 MB header on a detected 32 MB device is legal and intentionally limits normal Flash APIs to 16 MB. It does not change the physical capacity. `components/bootloader_support/src/flash_partitions.c:18-31` validates partitions against that configured size.

A 32 MB declaration would raise the configured Flash/API address bound, permit partition validation above 16 MB, and activate the >16 MB/32-bit-address checks in the relevant bootloader/runtime configuration. It does not allocate an OTA slot, change the partition table, write the upper region, or make the MMU map that region by itself; MMU mappings follow selected image segments, and OTA behavior follows the partition table plus `otadata`. Any actual access above 16 MB would still require compatible bootloader/cache/address support and an explicitly reviewed partition or API operation.

For the minimal host-only candidate, use:

```text
CONFIG_ESPTOOLPY_FLASHSIZE_16MB=y
CONFIG_ESPTOOLPY_FLASHSIZE="16MB"
CONFIG_ESPTOOLPY_HEADER_FLASHSIZE_UPDATE=n
```

This matches both original headers, contains the entire preserved original partition address space, avoids making the erased upper 16 MB part of the first candidate, and does not require 32-bit-address operation. It does not deny the confirmed physical 32 MB fact. A 32 MB header remains a mapped, valid later alternative, but it must be assessed together with the built artifact and preserved bootloader before any write packet.

`CONFIG_ESPTOOLPY_HEADER_FLASHSIZE_UPDATE` defaults off. It may rewrite a bootloader header during a Flash operation and invalidate its appended digest, so it is denied for this workflow.

## 7. Flash Bus Mode Mapping

The exact explicit Octal Kconfig path is:

```text
CONFIG_ESPTOOLPY_OCT_FLASH=y
CONFIG_ESPTOOLPY_FLASH_MODE_AUTO_DETECT=y
CONFIG_ESPTOOLPY_FLASHMODE_OPI=y
CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE_STR=y
```

`components/esptool_py/Kconfig.projbuild:13-74` defines:

- `CONFIG_ESPTOOLPY_OCT_FLASH`, dependent on `SOC_SPI_MEM_SUPPORT_FLASH_OPI_MODE`, default off;
- the `CONFIG_ESPTOOLPY_FLASHMODE_*` choice, default DIO unless Octal is enabled and default OPI when it is;
- STR and DTR sampling choices, with DTR available only when explicit Octal is enabled.

`components/efuse/esp32s3/esp_efuse_table.csv:175` defines `FLASH_TYPE` as four data lines for 0 and eight data lines for 1. `components/bootloader_support/bootloader_flash/src/bootloader_flash.c:1022-1028` calls `efuse_ll_get_flash_type()`, and `components/hal/esp32s3/include/hal/efuse_ll.h:33-36` reads the `flash_type` field. This is the source mapping from committed `FLASH_TYPE=1` evidence to Octal hardware handling.

STR is selected explicitly because the device evidence establishes eight data lines but not DTR sampling support. `docs/en/api-guides/flash_psram_config.rst:64-72` states that Octal Flash may support STR, DTR, or both depending on vendor/model. Selecting DTR from `FLASH_TYPE=1` alone would be a guess.

An alternate auto-detect path exists:

```text
CONFIG_ESPTOOLPY_FLASH_MODE_AUTO_DETECT=y
CONFIG_ESPTOOLPY_FLASHMODE_DIO=y
CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE_STR=y
```

`components/esptool_py/Kconfig.projbuild:18-37` and `components/spi_flash/esp_flash_spi_init.c:473-517` switch a non-OPI compile-time mode to OPI STR when the eFuse identifies an Octal Flash. This explains how DIO header evidence and runtime Octal operation can coexist. However, the Kconfig help says not to rely on auto-detect when the hardware is known to be Octal because some features still depend on `CONFIG_ESPTOOLPY_OCT_FLASH`. It is explanatory evidence, not the preferred new candidate.

Wrong bus/sample configuration can fail before the second-stage bootloader, cause the explicit-Octal/eFuse consistency check to abort, leave the Flash in an unreadable command mode, or produce invalid instruction/data reads and reset/panic loops. `docs/en/api-guides/flash_psram_config.rst:231-257` specifically records that ROM depends on `FLASH_TYPE` to reset an Octal Flash into a boot-readable mode; this device's committed value is 1. No eFuse change is needed or allowed.

## 8. Image Header DIO Explanation

`components/bootloader_support/include/esp_app_format.h:39-49,81-107` defines the image header `spi_mode` as a boot-readable mode field with QIO, QOUT, DIO, DOUT, FAST_READ, and SLOW_READ values. It has no OPI enum.

`docs/en/api-guides/bootloader.rst:48-53` describes the stages:

1. the first-stage ROM loader reads the second-stage bootloader header and loads it under conservative conditions;
2. the second-stage bootloader selects an app and reconfigures Flash from the selected app header;
3. application startup performs later Flash/MSPI initialization.

For explicit OPI, `components/esptool_py/Kconfig.projbuild:76-89` deliberately generates the string `dout`, not `opi`: first/second bootloader header handling does not encode OPI, and ROM obtains the Flash mode from eFuse. Local esptool maps `dout` to header value 3 in `esptool/cmds.py:75,304-318`.

For a non-explicit DIO configuration with auto-detect, `components/spi_flash/esp_flash_spi_init.c:473-494` reads the Octal eFuse and changes runtime configuration to OPI STR. `components/esp_system/port/cpu_start.c:570-602` initializes the Flash/MSPI state and maps Flash after those checks.

Consequently:

- the original DIO header is a boot-stage transfer declaration, not evidence that the physical Flash has only two data lines;
- `FLASH_TYPE=1` is physical/package-interface evidence, not an image-header value;
- the two can legally coexist;
- the preferred explicit-Octal host-only candidate will generate a **DOUT** header while configuring OPI STR at runtime.

The DOUT candidate must be confirmed in the built image and tested for compatibility with the preserved original bootloader before any First Flash packet. No write is authorized by this explanation.

## 9. Flash Frequency Mapping

For ESP32-S3, `components/spi_flash/esp32s3/Kconfig.flash_freq:1-24` defines:

- `CONFIG_ESPTOOLPY_FLASHFREQ_120M`;
- `CONFIG_ESPTOOLPY_FLASHFREQ_80M`;
- `CONFIG_ESPTOOLPY_FLASHFREQ_40M`;
- `CONFIG_ESPTOOLPY_FLASHFREQ_20M`.

The target default is 80 MHz. The 120 MHz option has additional high-performance/Octal and STR/experimental dependencies and explicit temperature-related crash warnings.

The exact 80 MHz candidate is:

```text
CONFIG_ESPTOOLPY_FLASHFREQ_80M=y
CONFIG_ESPTOOLPY_FLASHFREQ="80m"
```

`components/esptool_py/Kconfig.projbuild:93-109` maps both the 80 and 120 MHz runtime choices to the boot-header string `80m`, because boot frequency may be capped at 80 MHz. The local esptool mapping in `esptool/targets/esp32.py:92-97` encodes `80m` as nibble `0xF`. `components/bootloader_support/bootloader_flash/src/bootloader_flash_config_esp32s3.c:183-199` decodes `ESP_IMAGE_SPI_SPEED_DIV_1` as 80 MHz.

Both original headers declare 80 MHz and ESP-IDF v5.5.4 selects 80 MHz by default for ESP32-S3. This provides device-image and source support for 80 MHz. There is no stronger project evidence for changing to 40 or 20 MHz, so a lower frequency would also be a guess. The candidate is not a physical timing validation.

Boot-stage and later runtime frequency can differ: the ROM first uses the second-stage bootloader header under its supported boot conditions, the second-stage bootloader reads the selected app header, and application MSPI timing applies the configured runtime choice. The clearest source example is the 120 MHz choice mapping to an 80 MHz header string while later runtime code may select 120 MHz. This project does not use that path; both header and runtime candidate remain 80 MHz.

An unsupported frequency/sample combination can prevent bootloader or app reads, fail timing tuning, corrupt instruction/data fetches, or cause intermittent temperature-sensitive crashes. The v5.5.4 source explicitly warns about 120 MHz conditions. Selecting 40 MHz merely because it sounds conservative is not evidence-based here and could create a different untested Flash/MSPI combination.

## 10. VDDSPI and Flash Voltage

The committed bit-level evidence is:

```text
VDD_SPI_FORCE = 1
VDD_SPI_XPD   = 1
VDD_SPI_TIEH  = 0
```

`components/efuse/esp32s3/esp_efuse_table.csv:146-148` defines:

- XPD as the SPI-regulator power-up signal;
- TIEH 0, when forced, as connection to the 1.8 V LDO and TIEH 1 as connection to `VDD3P3_RTC_IO`;
- FORCE as using eFuse configuration for VDDSPI.

The combined fields therefore support a forced, enabled 1.8 V VDDSPI LDO configuration. This is bit-level configuration evidence, not a PCB-rail instrument measurement.

The earlier esptool v4.12.dev3 line `Flash voltage set by eFuse: 3.3 V` is retained as historical direct output. Its inherited `_get_efuse_flash_voltage()` checks whether any of FORCE, XPD, or TIEH is nonzero before its narrower branches, so this unit's nonzero FORCE/XPD combination enters the 3.3 V branch early. That implementation disagrees with the independently decoded fields and cannot establish physical voltage.

ESP-IDF v5.5.4 has no ESP32-S3 board Kconfig that should be used to burn or “set” the Flash voltage for this candidate. `rtc_vddsdio_set_config()` in `components/esp_hw_support/port/esp32s3/rtc_init.c:237-247` can alter live VDDSDIO control registers and is forbidden. `CONFIG_BOOTLOADER_VDDSDIO_BOOST` exists in `components/bootloader/Kconfig.projbuild:128-146`, but the reviewed boost call is used by ESP32-specific boot paths, not the ESP32-S3 candidate, and must remain unused.

Required policy:

- preserve the existing eFuse/hardware configuration;
- never burn or alter VDDSPI eFuses;
- never invoke `set_flash_voltage`;
- never trial-Flash different voltage settings;
- never call a live VDDSDIO-changing API;
- keep `CONFIG_SECURE_BOOT=n` and `CONFIG_SECURE_FLASH_ENC_ENABLED=n`.

Secure Boot and Flash Encryption do not resolve voltage. Their boot paths can burn eFuses or rewrite/encrypt Flash on first boot and are separately prohibited.

## 11. PSRAM Capacity Mapping

The exact enable symbol is:

```text
CONFIG_SPIRAM=y
```

`components/esp_psram/esp32s3/Kconfig.spiram:1-6` defines it and defaults it off.

There is no ESP32-S3 Kconfig symbol meaning “16 MB Octal PSRAM”. The `ESPPSRAM16`, `ESPPSRAM32`, and `ESPPSRAM64` type names in `components/esp_psram/esp32s3/Kconfig.spiram:22-39` are chip-family selections and some depend on Quad mode; they must not be equated to the committed 16 MB byte capacity.

For Octal PSRAM, `components/esp_psram/esp32s3/esp_psram_impl_octal.c:365-375` reads mode registers at runtime and maps MR2 density code `0x5` to 16 MB. `components/esp_psram/system_layer/esp_psram.c:200-229` records the detected physical size. Capacity is therefore runtime-detected after PSRAM initialization rather than fixed by a 16 MB Kconfig value.

## 12. PSRAM Vendor and Voltage Variant

The committed `PSRAM_VENDOR=AP_1v8` value is an eFuse/package vendor and voltage-variant label.

`components/esp_psram/esp32s3/esp_psram_impl_octal.c:37-38,365-375` recognizes AP vendor ID `0x0D` and UNILC `0x1A`, warns on an unrecognized runtime ID, and independently decodes density. The package label does not prove:

- the active PSRAM bus mode;
- the selected PSRAM frequency;
- successful initialization;
- package-to-PCB routing;
- a common Flash/PSRAM rail measurement;
- all timing parameters.

It must not be used as a substitute for a complete PSRAM configuration.

## 13. PSRAM Bus Mode Mapping

`components/esp_psram/esp32s3/Kconfig.spiram:11-20` defines:

```text
CONFIG_SPIRAM_MODE_QUAD=y
CONFIG_SPIRAM_MODE_OCT=y
```

Quad is the generic default; Octal is the exact Octal-PSRAM symbol. When Octal is selected, `components/esp_psram/CMakeLists.txt:20-35` selects the Octal implementation. That implementation initializes PSRAM pins and mode registers, checks whether the chip is connected in the expected line mode, and returns `ESP_ERR_NOT_SUPPORTED` for a missing/wrong-line-mode device.

The committed package fields do not independently prove the active bus mode or PCB routing. No PSRAM mode is therefore selected for the first candidate.

The reviewed normal initialization path is application startup: `components/esp_system/port/cpu_start.c:610-628` calls the chip and memory initialization functions when their Kconfig gates are enabled. Failure aborts by default; `CONFIG_SPIRAM_IGNORE_NOTFOUND=y` can continue only under its documented dependencies. The first candidate avoids both the initialization and failure-policy ambiguity by compiling PSRAM support out. The second-stage bootloader's required Flash startup does not make PSRAM a dependency of the serial-only application.

## 14. PSRAM Frequency Mapping

`components/esp_psram/esp32s3/Kconfig.spiram:83-133` defines:

```text
CONFIG_SPIRAM_SPEED_120M=y
CONFIG_SPIRAM_SPEED_80M=y
CONFIG_SPIRAM_SPEED_40M=y
CONFIG_SPIRAM_SPEED=<120|80|40>
```

The generic default is 40 MHz. Octal 120 MHz is experimental and carries timing/temperature warnings. `docs/en/api-guides/flash_psram_config.rst:64-72,81-149` explains that Flash and PSRAM share an internal clock and gives supported F8R8 combinations. PSRAM disabled is compatible with the 80 MHz STR Flash group.

No committed evidence identifies the original PSRAM runtime frequency. Therefore no enabled-PSRAM clock is a verified candidate. The first configuration keeps PSRAM disabled.

## 15. Partition and Image Layout

`components/partition_table/Kconfig.projbuild:153-165` defines:

```text
CONFIG_PARTITION_TABLE_OFFSET=0x8000
```

The default is `0x8000`, must be a multiple of `0x1000`, and must agree with the bootloader. The second-stage bootloader reads the table from that offset. An app offset comes from the selected partition-table entry, not from the Flash-size Kconfig.

The committed original table is:

| Entry | Offset | End |
|---|---:|---:|
| `nvs` | `0x9000` | `0xD000` |
| `otadata` | `0xD000` | `0xF000` |
| `phy_init` | `0xF000` | `0x10000` |
| `ota_0` | `0x20000` | `0x410000` |
| `ota_1` | `0x410000` | `0x800000` |
| `assets` | `0x800000` | `0x1000000` |

`docs/en/api-guides/startup.rst:72-89` shows that the second-stage bootloader reads the table, consults `otadata`, selects an app, maps its segments, verifies it, and transfers control.

A future host-only build does not need to adopt or rewrite this original table. Build-time default partition generation is not a device write plan. No tracked partition CSV is allowed in the minimal implementation, and no candidate app offset is authorized in this audit.

## 16. Original Bootloader Compatibility

The original bootloader is ESP32-S3 and reports ESP-IDF `v5.5.3-dirty`; a future candidate app would use v5.5.4. `docs/en/api-guides/bootloader.rst:19-28` says an existing bootloader supports apps built from newer ESP-IDF releases, but it also requires testing OTA/app compatibility with the same deployed bootloader.

Source compatibility is plausible, not proven. A generated app must still be checked for:

- ESP32-S3 chip ID and revision bounds in `esp_image_header_t`;
- image-header mode, frequency, and size;
- segment addresses, MMU page assumptions, checksum, hash, and partition fit;
- app subtype and the selected `otadata` slot;
- secure version and anti-rollback state;
- Secure Boot and Flash Encryption expectations;
- dependencies on bootloader configuration and partition contents.

`components/bootloader_support/src/bootloader_common_loader.c:38-66,120-144` enforces chip ID and revision bounds. `components/esp_app_format/esp_app_desc.c:47-50` makes app secure version 0 unless anti-rollback configuration overrides it. Those values do not exist for a custom artifact yet.

## 17. App-Only Future Write Assessment

One app-only write can be compatible in principle with the preserved bootloader and original partition layout, because the ESP-IDF bootloader is designed to boot newer application images from app partitions. That is only a conditional architecture assessment.

The exact operation is not yet proven because no custom app binary, SHA-256, effective length, segment map, image header, partition-fit calculation, active OTA-slot interpretation, or write range exists. The original `ota_0` offset `0x20000` is evidence, not an automatic target. The erased `ota_1` and upper 16 MB are not permission to repurpose them.

OpenSpec Task 3.4 remains incomplete until the built app and preserved original bootloader/table/OTA state are reviewed together. The stop rule is to reject the candidate rather than propose a new bootloader, partition table, OTA layout, or unused-tail change.

## 18. USB Serial/JTAG Console Mapping

The exact primary-console choice is:

```text
CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG=y
CONFIG_ESP_CONSOLE_SECONDARY_NONE=y
```

`components/esp_system/Kconfig:263-323` defines the primary console choices. USB Serial/JTAG depends on `SOC_USB_SERIAL_JTAG_SUPPORTED`; it derives `CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG_ENABLED=y` and selects `CONFIG_USJ_ENABLE_USB_SERIAL_JTAG`.

`components/bootloader_support/src/bootloader_console.c:133-149` maps second-stage bootloader console output to the ROM USB serial device. `components/esp_vfs_console/vfs_console.c:60-69` opens `/dev/usbserjtag` for application standard I/O.

This uses the integrated ESP32-S3 USB Serial/JTAG controller, not an external USB-UART chip and not an application-selected GPIO. UART default/custom and USB CDC are not selected. Initial ROM output remains controlled by ROM/eFuse/strap behavior and is not fully governed by the app Kconfig.

The committed query-time USB-Serial/JTAG observation supports the controller choice for a future host-only build. Selecting it redirects second-stage/app console I/O but does not itself authorize or control ROM download-mode entry, host reset signaling, or a device connection. The integrated CDC/JTAG interface may be observed or re-enumerated differently across reset/boot stages, so it does not guarantee future port numbering or log visibility.

## 19. Required Kconfig Symbols

The proposed host-only implementation must resolve to:

```text
CONFIG_IDF_TARGET="esp32s3"
CONFIG_IDF_TARGET_ESP32S3=y

CONFIG_ESPTOOLPY_OCT_FLASH=y
CONFIG_ESPTOOLPY_FLASHMODE_OPI=y
CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE_STR=y
CONFIG_ESPTOOLPY_FLASHFREQ_80M=y
CONFIG_ESPTOOLPY_FLASHFREQ="80m"
CONFIG_ESPTOOLPY_FLASHSIZE_16MB=y
CONFIG_ESPTOOLPY_FLASHSIZE="16MB"
# CONFIG_ESPTOOLPY_HEADER_FLASHSIZE_UPDATE is not set

# CONFIG_SPIRAM is not set

CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG=y
CONFIG_ESP_CONSOLE_SECONDARY_NONE=y

CONFIG_PARTITION_TABLE_OFFSET=0x8000
# CONFIG_SECURE_BOOT is not set
# CONFIG_SECURE_FLASH_ENC_ENABLED is not set
# CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK is not set
```

`Kconfig:68-96` maps the target string `esp32s3` to `CONFIG_IDF_TARGET_ESP32S3=y`.

These are candidate resolved values, not a created configuration file. A later implementation task must verify the generated full configuration and dependency closure before build.

## 20. Candidate Kconfig Values

### A. Required and verified

- `IDF_TARGET=esp32s3` / `CONFIG_IDF_TARGET_ESP32S3=y`.
- `CONFIG_ESPTOOLPY_OCT_FLASH=y`.
- `CONFIG_ESPTOOLPY_FLASH_MODE_AUTO_DETECT=y`.
- `CONFIG_ESPTOOLPY_FLASHMODE_OPI=y`.
- `CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE_STR=y`.
- `CONFIG_ESPTOOLPY_FLASHFREQ_80M=y`.
- `CONFIG_ESPTOOLPY_FLASHSIZE_16MB=y` for the conservative first candidate.
- `CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG=y`.
- `CONFIG_ESP_CONSOLE_SECONDARY_NONE=y`.
- `CONFIG_PARTITION_TABLE_OFFSET=0x8000`.

### B. Safe to leave at default

- `CONFIG_ESPTOOLPY_HEADER_FLASHSIZE_UPDATE=n`.
- `CONFIG_ESPTOOLPY_FLASH_MODE_AUTO_DETECT=y` remains the target default and adds an eFuse/runtime consistency check around the explicit Octal selection.
- Secure Boot and Flash Encryption defaults remain off.
- app rollback and anti-rollback defaults remain off.
- no custom partition table is supplied; generated build layout is not treated as a future write layout.

### C. Explicitly disabled for first Smoke Test

- `CONFIG_SPIRAM=n`;
- no Wi-Fi or Bluetooth component/API initialization;
- no NVS initialization or write API;
- no OTA component/API or update path;
- no display, touch, audio, microphone, motor, SD, battery-control, power-control, LED, or arbitrary GPIO dependency/API;
- no network stack use;
- no Secure Boot, Flash Encryption, anti-rollback, eFuse mutation, or VDDSPI API;
- no UART-custom GPIO console.

Some features do not have one safe global Kconfig “off” switch. Their absence must be enforced by component dependency and source/API review rather than an invented symbol.

### D. Unresolved and blocked

- enabled PSRAM mode/frequency/timing/routing and runtime success;
- whether a 32 MB header is compatible with the preserved original bootloader without unnecessary 32-bit-address behavior;
- the built app's exact header, segments, hash, size, secure version, and revision bounds;
- active `otadata` selection and the only safe future app offset/range;
- ROM-log visibility and future host port identity;
- physical VDDSPI rail measurement;
- all peripheral pinout and behavior.

### E. Forbidden

- any eFuse write, protection, key, security, download-mode, JTAG, or USB restriction;
- Secure Boot or Flash Encryption enablement;
- `set_flash_voltage`, trial voltage changes, or VDDSPI live-register changes;
- Flash-size auto-update during a Flash operation;
- unknown GPIO or unverified peripheral initialization;
- NVS mutation, OTA, network, device identity logging, or recovery-data access;
- suppressing an initialization/configuration error and continuing as if verified.

### Comprehensive evidence-to-configuration table

| Hardware fact | Direct evidence | Confidence | ESP-IDF symbol | Candidate value | Dependencies | Source file | Source logic | Safe for host-only implementation | Safe for future Flash | Remaining risk | Required verification |
|---|---|---|---|---|---|---|---|---|---|---|---|
| ESP32-S3 target | Device result plus both original image chip IDs | `CONFIRMED` | `CONFIG_IDF_TARGET`, `CONFIG_IDF_TARGET_ESP32S3` | `"esp32s3"`, `y` | `IDF_TARGET=esp32s3` | `Kconfig:68-96` | Target string selects ESP32-S3/Xtensa | Yes | Conditional | Built image revision bounds | Inspect app header |
| Physical Flash is 32 MB | RDID `0xC2/0x8039`; 32 MiB backup | `CONFIRMED` | `CONFIG_ESPTOOLPY_FLASHSIZE_32MB` | Mapped, not first candidate | None beyond image build | `components/esptool_py/Kconfig.projbuild:112-145` | Choice yields `"32MB"` and header size code | Yes as evidence/mapped alternative | Not yet | Preserved bootloader 32-bit-address behavior | Build and compare artifact before selecting |
| Preserved address space ends at 16 MB | Both original headers and partition end `0x1000000` | `CONFIRMED` | `CONFIG_ESPTOOLPY_FLASHSIZE_16MB` | `y` | Partition ranges must fit | Same Kconfig; `components/spi_flash/esp_flash_spi_init.c:576-594` | Larger physical chip with smaller header is accepted and capped | Yes | Conditional | Generated app still must fit original slot | Inspect header, size, and partition fit |
| Flash has eight data lines | `FLASH_TYPE=1` | `CONFIRMED` eFuse configuration | `CONFIG_ESPTOOLPY_OCT_FLASH` | `y` | `SOC_SPI_MEM_SUPPORT_FLASH_OPI_MODE` | `components/esptool_py/Kconfig.projbuild:13-16`; `components/efuse/esp32s3/esp_efuse_table.csv:175` | Explicit Octal path; eFuse 1 means eight lines | Yes | Conditional | Generated header/preserved bootloader behavior | Inspect DOUT header and app boot compatibility |
| Runtime Flash mode must match eFuse | Same Flash-type field | `CONFIRMED` input/source mapping | `CONFIG_ESPTOOLPY_FLASH_MODE_AUTO_DETECT` | `y` | OPI-capable SoC | `components/esptool_py/Kconfig.projbuild:18-37`; `components/spi_flash/esp_flash_spi_init.c:473-494` | Checks eFuse and selects matching runtime path | Yes | Conditional | Auto-detect is not full physical validation | Inspect generated config and startup logic |
| Octal runtime mode | `FLASH_TYPE=1`; IDF explicit-Octal design | `CONFIRMED` input; source-mapped candidate | `CONFIG_ESPTOOLPY_FLASHMODE_OPI` | `y` | `CONFIG_ESPTOOLPY_OCT_FLASH=y` | `components/esptool_py/Kconfig.projbuild:39-89` | OPI choice emits `dout` header; ROM uses eFuse | Yes | Conditional | Original bootloader acceptance untested | Offline artifact/bootloader review |
| DTR is not proven | No device/model evidence for sampling edge mode | `UNVERIFIED` for DTR | `CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE_STR` | `y` | STR is available with Octal | Same Kconfig `:64-74`; local Flash/PSRAM guide `:64-72` | STR avoids assuming vendor/model DTR support | Yes | Conditional | Electrical timing untested | Host artifact review then separately authorized runtime test |
| Original Flash frequency is 80 MHz | Both original headers | `CONFIRMED` image declaration | `CONFIG_ESPTOOLPY_FLASHFREQ_80M` | `y` | None | `components/spi_flash/esp32s3/Kconfig.flash_freq:1-24` | ESP32-S3 default; generated header string is `80m` | Yes | Conditional | Physical timing not measured | Inspect header; runtime only after future authorization |
| Forced 1.8 V VDDSPI LDO | FORCE/XPD/TIEH `1/1/0` | `CONFIRMED` eFuse configuration | No setting symbol required | Preserve; no firmware action | Existing eFuse/hardware state | `components/efuse/esp32s3/esp_efuse_table.csv:146-148` | FORCE uses eFuse, XPD powers regulator, TIEH 0 selects 1.8 V LDO | Yes by non-action | Conditional | No PCB instrument measurement | Never alter; measurement only in a separate electrical test |
| Embedded PSRAM capacity 16 MB | `PSRAM_CAP=16M`, `PSRAM_CAP_3=0` | `CONFIRMED` package/eFuse evidence | No 16 MB capacity symbol | No capacity setting | PSRAM would have to be enabled first | `components/esp_psram/esp32s3/esp_psram_impl_octal.c:365-375` | MR2 density `0x5` is detected as 16 MB at runtime | Yes as evidence only | No enabled claim | Runtime density not observed | Keep disabled; later exact PSRAM review |
| PSRAM vendor variant is `AP_1v8` | Sanitized eFuse summary | `CONFIRMED` label only | No vendor/voltage Kconfig | No setting | None | Same driver `:37-38,365-375` | Driver recognizes AP runtime vendor ID separately from density | Yes as evidence only | No enabled claim | Label does not prove mode/clock/routing | Later PSRAM mode/routing evidence |
| PSRAM can be omitted | Serial-only behavior has no external-RAM need; IDF combination table | Source-supported | `CONFIG_SPIRAM` | `n` | None | `components/esp_psram/esp32s3/Kconfig.spiram:1-9`; guide `:106-149` | Enable defaults off; PSRAM-disabled group is compatible with Flash groups | Yes | Yes for a PSRAM-free app, still subject to artifact review | Linked code may accidentally require PSRAM | Inspect generated config, link metadata, and source |
| If PSRAM were Octal | Package evidence does not prove active mode | `UNVERIFIED` | `CONFIG_SPIRAM_MODE_OCT` | Blocked/not set | `CONFIG_SPIRAM=y` | `components/esp_psram/esp32s3/Kconfig.spiram:11-20` | Selects Octal driver | No enabled candidate | No | Mode/routing unknown | Separate evidence and implementation task |
| PSRAM frequency | No active clock evidence | `UNVERIFIED` | `CONFIG_SPIRAM_SPEED_40M/80M/120M` | Blocked/not set | `CONFIG_SPIRAM=y`; 120 Octal experimental | Same Kconfig `:83-133` | Choice maps to integer 40/80/120 | No enabled candidate | No | Clock/timing/runtime unknown | Separate evidence and runtime plan |
| Original partition table offset | Validated table at `0x8000` | `CONFIRMED` backup fact | `CONFIG_PARTITION_TABLE_OFFSET` | `0x8000` | Multiple of `0x1000`; bootloader agreement | `components/partition_table/Kconfig.projbuild:153-165` | Bootloader reads table at configured offset | Yes as preserved/default input | Conditional | Host build table is not a write plan | Compare generated outputs; do not create custom CSV now |
| Original `ota_0` starts `0x20000` | Validated original table | `CONFIRMED` backup fact | No generic app-offset symbol | No write target selected | Partition table and `otadata` selection | `docs/en/api-guides/startup.rst:72-89` | Bootloader selects app from table and OTA data | Yes as evidence only | No, pending Task 3.4 | Active slot and artifact compatibility unknown | Built app/table/OTA/range review |
| Integrated USB Serial/JTAG is present | Query-time USB mode | `CONFIRMED` for recorded event | `CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG` | `y` | `SOC_USB_SERIAL_JTAG_SUPPORTED` | `components/esp_system/Kconfig:263-323` | Selects integrated console and USB Serial/JTAG driver | Yes | Conditional | ROM log/future port behavior unverified | Inspect generated config; device use needs new authorization |
| No secondary/UART GPIO console | Minimal no-GPIO boundary | Required safety constraint | `CONFIG_ESP_CONSOLE_SECONDARY_NONE` | `y` | Primary USB Serial/JTAG makes secondary USB unavailable | Same Kconfig `:296-316` | Leaves no secondary UART/custom GPIO path | Yes | Conditional | Boot ROM output remains separate | Static config/source review |
| Secure Boot remains off | Project prohibition; no need for serial test | Required safety constraint | `CONFIG_SECURE_BOOT` | `n` | Default n | `components/bootloader/Kconfig.projbuild:606-650` | Enabling can permanently restrict boot/JTAG | Yes | Yes only while off | Generated dependency drift | Inspect generated config |
| Flash Encryption remains off | Project prohibition; no need for serial test | Required safety constraint | `CONFIG_SECURE_FLASH_ENC_ENABLED` | `n` | Default n | Same Kconfig `:822-889` | Enabling encrypts Flash and may burn eFuses on first boot | Yes | Yes only while off | Generated dependency drift | Inspect generated config |
| Anti-rollback remains off | No authorized security/eFuse mutation | Required safety constraint | `CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK` | `n` | Depends on app rollback if enabled | `components/bootloader/Kconfig.app_rollback:15-30` | Enabled path compares/writes secure-version state | Yes | Yes only while off | Original secure-version state not queried | Inspect generated app secure version and config |

## 21. Configuration Allowlist

Only the symbols and negative feature constraints in Sections 19 and 20 are allowed for the next host-only implementation task. The implementation may emit fixed privacy-safe text and enter a bounded non-busy idle state. It may not initialize PSRAM or any external peripheral.

The 32 MB physical capacity may be recorded in logs or documentation only if clearly labelled as committed evidence; the firmware must not probe the device, expose unique identifiers, or access the upper 16 MB.

## 22. Configuration Denylist

The next host-only task must not add:

- `CONFIG_SPIRAM=y` or any enabled PSRAM mode/speed;
- `CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE_DTR=y`;
- `CONFIG_ESPTOOLPY_FLASHFREQ_120M=y`;
- `CONFIG_ESPTOOLPY_HEADER_FLASHSIZE_UPDATE=y`;
- `CONFIG_SECURE_BOOT=y`;
- `CONFIG_SECURE_FLASH_ENC_ENABLED=y`;
- `CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK=y`;
- custom GPIO, UART pins, partition CSV, bootloader component, OTA layout, write offset, or write range;
- Wi-Fi, Bluetooth, NVS, OTA, display, touch, audio, microphone, motor, SD, battery, or power-control initialization.

## 23. Forbidden APIs and Operations

Forbidden firmware/API paths include:

- `rtc_vddsdio_set_config()` and any VDDSPI/Flash-voltage register write;
- `esp_efuse_write_field_blob()`, `esp_efuse_write_reg()`, `esp_efuse_set_write_protect()`, `esp_efuse_batch_write_begin()`, and `esp_efuse_batch_write_commit()`;
- Secure Boot or Flash Encryption first-boot enable paths;
- NVS write/erase/commit APIs;
- OTA begin/write/end/set-boot APIs;
- Wi-Fi/Bluetooth start or provisioning APIs;
- GPIO direction/level configuration for any unreviewed pin;
- display, touch, audio, microphone, motor, SD, battery, or power-control driver initialization.

Forbidden host/device operations remain Flash, monitor, erase, restore, eFuse operations, device queries, and port access unless a later task grants an exact new scope.

## 24. Host-Only Implementation Candidate

A later independent Apply task may:

1. restate the serial-only scope and exact changed files;
2. create the minimal ESP32-S3 application and candidate `sdkconfig.defaults` using the allowlist;
3. configure and build locally with ESP-IDF v5.5.4;
4. inspect generated `sdkconfig`, linked components, app image, map, partition outputs, and warnings;
5. stop before any device or Flash action.

The application must only print fixed identity/version/target/disabled-feature/ready text through USB Serial/JTAG and then idle without a busy loop. It must not read MAC, eFuse, chip ID, Flash ID, NVS, network state, or any peripheral.

## 25. Items Still Unresolved

- Exact built app compatibility with the original v5.5.3-dirty bootloader.
- The safe future app-only target slot, offset, length, end address, and OTA metadata implications.
- Whether the explicit-Octal candidate's DOUT header is accepted by the preserved bootloader exactly as expected.
- Whether a later 32 MB header provides any benefit without introducing 32-bit-address compatibility work.
- Enabled PSRAM settings and runtime behavior.
- Physical rail voltage by instrumentation.
- All non-console hardware and GPIOs.
- Future port identity and device-operation reset/log behavior.

None of these prevents host-only implementation because the candidate avoids PSRAM, peripherals, unknown GPIO, partition changes, and device access. All remain blockers for First Flash.

## 26. Firmware Implementation Gate Decision

**OPEN FOR HOST-ONLY IMPLEMENTATION**

All opening conditions are satisfied:

- ESP32-S3 is confirmed and exactly mapped;
- both 32 MB physical capacity and the conservative 16 MB header candidate are source-mapped;
- the explicit Octal symbol and STR sampling choice are exact;
- DIO/DOUT image-header semantics and runtime Octal transition are source-explained;
- 80 MHz has original-image and target-Kconfig support;
- firmware need not and must not change VDDSPI;
- the minimal program uses no unknown GPIO;
- USB Serial/JTAG console has an exact Kconfig and implementation path;
- PSRAM can be safely disabled independently of Octal Flash;
- no host-only candidate value depends on guessing;
- partition and write range remain independent gates.

This decision authorizes no current edit beyond this documentation round. Future firmware work requires a separate Apply task.

## 27. First Flash Gate Decision

**NO-GO**

There is no Flash authorization and no further device-access authorization. First Flash remains blocked on implementation, host-only configure/build, static safety review, generated artifact inspection, Task 3.4 compatibility resolution, exact offset/range/hash review, recovery/observation packet review, current device/port review, and explicit operation-specific user authorization.

## 28. Source References

Primary ESP-IDF v5.5.4 references:

- `Kconfig:68-96` — target string and ESP32-S3 selection.
- `components/esptool_py/Kconfig.projbuild:13-157` — Octal, auto-detect, mode, sample, frequency string, Flash size, and header-size update.
- `components/spi_flash/esp32s3/Kconfig.flash_freq:1-24` — ESP32-S3 Flash frequencies.
- `components/esptool_py/project_include.cmake:28-60,124-138` — `elf2image` propagation.
- `components/bootloader_support/include/esp_app_format.h:39-107` — image-header mode/frequency/size fields.
- `components/bootloader_support/bootloader_flash/src/bootloader_flash.c:1022-1028` — Octal eFuse query.
- `components/hal/esp32s3/include/hal/efuse_ll.h:33-36` — low-level `flash_type` read.
- `components/efuse/esp32s3/esp_efuse_table.csv:146-148,175` — VDDSPI and Flash-type definitions.
- `components/spi_flash/esp_flash_spi_init.c:473-517,576-594` — runtime mode auto-detect and size checks.
- `components/esp_system/port/cpu_start.c:570-628` — Flash/MSPI and optional PSRAM startup.
- `components/bootloader/Kconfig.projbuild:92-157,606-650,822-889` — >16 MB bootloader support and forbidden security features.
- `components/partition_table/Kconfig.projbuild:153-165` — table offset.
- `components/partition_table/CMakeLists.txt:43-77` — size/offset propagation and validation.
- `components/bootloader_support/src/flash_partitions.c:18-31` — runtime partition bound check.
- `components/esp_psram/esp32s3/Kconfig.spiram:1-144` — PSRAM enable, mode, type, and speed.
- `components/esp_psram/Kconfig.spiram.common:5-86` — startup, failure, heap-use, and memory-test options.
- `components/esp_psram/esp32s3/esp_psram_impl_octal.c:338-391` — Octal device check, vendor, density, and tuning.
- `components/esp_psram/system_layer/esp_psram.c:200-229,409-466,584-592` — detected size, mapping, and heap integration.
- `components/esp_system/Kconfig:263-364` — console choices and USB Serial/JTAG dependencies.
- `components/bootloader_support/src/bootloader_console.c:133-149` — bootloader USB console.
- `components/esp_vfs_console/vfs_console.c:60-69` — app USB Serial/JTAG console.
- `components/bootloader_support/src/bootloader_common_loader.c:38-66,120-144` — chip/revision checks.
- `components/esp_app_format/esp_app_desc.c:47-50` — secure-version default.
- `docs/en/api-guides/bootloader.rst:19-28,48-53` — bootloader compatibility and header stages.
- `docs/en/api-guides/startup.rst:69-89` — partition selection, MMU mapping, and app load.
- `docs/en/api-guides/flash_psram_config.rst:38-76,81-149,231-257` — Flash/PSRAM configuration, valid combinations, and `FLASH_TYPE` ROM role.

Installed esptool v4.12.dev3 references:

- `esptool/cmds.py:75,263-325` — header mode mapping and optional bootloader-header mutation;
- `esptool/targets/esp32.py:81-97` — inherited Flash-size and frequency encodings;
- `esptool/targets/esp32s3.py:15-18,39,75-76` — ESP32-S3 target identity, bootloader offset, and Flash-type bit.

## 29. Limitations

This is a source-and-document audit. No configuration file or binary was generated, so generated dependency closure, binary header contents, linked components, segment layout, image size, partition fit, and bootloader acceptance remain untested. No electrical measurement or runtime observation was performed.

The local source proves available mechanisms and conditional behavior, not that the original firmware used a particular Kconfig. The original DIO header can be reconciled with Octal hardware through the documented layer model, but the original `sdkconfig` remains unknown. `AP_1v8` remains a package/vendor variant label, not proof of every PSRAM parameter. Physical 32 MB capacity remains separate from the conservative 16 MB image-address-space candidate.

No MAC, chip-unique identifier, key digest, credential, token, certificate, network configuration, executable device command, or private recovery content is included.

## 30. Host-Only Candidate Verification

The 2026-07-26 host-only implementation used `firmware/sdkconfig.defaults` plus `idf.py set-target esp32s3` under ESP-IDF v5.5.4. Final resolved values matched the candidate:

- explicit Octal Flash, OPI runtime, STR, and DOUT header string;
- 80 MHz and 16 MB;
- USB Serial/JTAG primary console and no secondary/UART console;
- default single factory-app build table at host-build offset `0x8000`;
- Secure Boot, Flash Encryption, rollback, and anti-rollback off;
- PSRAM support absent from the minimal dependency closure.

Offline image review confirmed that both generated bootloader and app headers are DOUT / 80 MHz / 16 MB with valid checksums and appended hashes. The app is 160832 bytes and has secure version 0.

The generated default factory-app offset is `0x10000`, not the original `ota_0` offset `0x20000`. The generated bootloader/table/app offsets are build metadata only and are **NOT REVIEWED FOR DEVICE WRITE** and **NOT AUTHORIZED**. Task 3.4 remains incomplete because current `otadata`, exact future offset/range approval, and actual preserved-bootloader acceptance are unresolved.

Firmware Implementation Gate: **HOST-ONLY IMPLEMENTATION COMPLETED**.

First Flash: **NO-GO**. Device access authorization: **NONE**. Flash authorization: **NONE**.
