# PCB V1.0 Minimal Display Smoke Test — Host Build and Artifact Review

## 1. Record status

Initial review date: 2026-07-28

Reproducibility reconciliation date: 2026-09-19

Current result: `PASS — HOST-SIDE DISPLAY ARTIFACT EVIDENCE COMPLETE; READY FOR NEXT GATE`

This current result supersedes the initial artifact-hash conclusion below while preserving the 2026-07-28 implementation history.

Operation category: `WRITE — HOST-ONLY FIRMWARE IMPLEMENTATION AND VALIDATION`

Initial 2026-07-28 result: `PASS — HOST IMPLEMENTATION AND ARTIFACT REVIEW COMPLETE`

This is a host-only implementation, build, static-audit, and artifact-review
record. No device was connected, discovered, enumerated, opened, reset,
monitored, queried, read, written, erased, restored, or otherwise operated.
No eFuse, rollback, recovery execution, device authorization, successor
Change, commit, push, or archive action occurred.

The artifact is:

- `backlight=hard-disabled`;
- `visual=UNVERIFIED`;
- `not-for-visual-validation`;
- `device-execution=NOT_AUTHORIZED`.

Physical display health remains `UNVERIFIED`.

## 2. Controlling baseline

The implementation began only after the requested clean baseline was
confirmed:

| Item | Initial value |
|---|---|
| Branch | `feat/prepare-pcb-v1-minimal-display-smoke-test` |
| Local HEAD | `5394afd180ed47b1145570feac7bc425584c43ea` |
| Upstream HEAD | `5394afd180ed47b1145570feac7bc425584c43ea` |
| Ahead/behind | `0/0` |
| Worktree/staged/firmware diff | clean |
| OpenSpec tasks | 28 checked / 37 unchecked / 65 total |
| Current Change strict validation | pass |
| Full repository strict validation | 3 passed / 0 failed |
| ESP-IDF | v5.5.4 |
| Python | 3.13.9 |

The required constitutional, hardware, product, architecture, decision,
OpenSpec, build-evidence, First Flash, and startup-observation documents were
read before implementation.

The historical result remains exactly:

`STOPPED / INCONCLUSIVE — RANGE-SCOPE DEVIATION`

The later startup observation remains `PASS`; the bounded runtime result
remains `PASS FOR THIS MINIMAL SMOKE TEST`. Neither result is reused as
authority for this artifact.

## 3. Implemented file boundary

Firmware changes are limited to:

- `.gitignore` entries for the dedicated generated build/sdkconfig paths;
- `firmware/CMakeLists.txt`;
- `firmware/main/CMakeLists.txt`;
- `firmware/main/idf_component.yml`;
- `firmware/main/main.c`;
- `firmware/main/pcb_v1_display.h`;
- `firmware/main/pcb_v1_display.c`;
- `firmware/main/pcb_v1_display_test_pattern.h`;
- `firmware/main/pcb_v1_display_test_pattern.c`.

Host evidence and tests are limited to:

- `tests/host/test_pcb_v1_minimal_display.py`;
- `tests/host/pcb_v1_display_host_harness.c`;
- `tests/host/fakes/`;
- `tests/build/pcb-v1-minimal-display-smoke-test-artifact-manifest.json`;
- this record.

The display module owns fail-closed GPIO44 handling, QSPI, panel lifecycle,
the run-once state machine, transfer synchronization, failure handling, and
cleanup. The test-pattern module is a pure RGB565 strip generator with a tiny
source-resident glyph set. `main.c` prints fixed safety metadata, invokes the
state machine once, reports the terminal result, and remains in a one-second
non-busy delay loop.

## 4. Initialization-table factual correction

Implementation revealed a planning fact error without changing any hardware
authority or table byte. The fixed Xiaozhi source contains:

- 184 outer `st77916_lcd_init_cmd_t` command elements;
- 181 non-empty parameter-array initializers;
- 365 total legacy `{0x` initializer tokens.

The previous value 365 was therefore a lexical-token count, not a command
count. `proposal.md`, `design.md`, `spec.md`, and `tasks.md` were minimally
corrected to record both values. `.openspec.yaml` was not changed.

The implementation initializer is the complete
`vendor_specific_init_yysj` from:

- commit `49ac8a6da399f27a9546d4f73640b7f86c24bac6`;
- file `main/boards/esp-vocat/esp_vocat.cc`;
- symbol `vendor_specific_init_yysj`.

Audited results:

| Check | Result |
|---|---|
| Command elements | 184 |
| Legacy initializer tokens | 365 |
| Whitespace-normalized SHA-256 | `e8a1f2ea307b51be59d3daa201bb444b5f5ddc2d8da2931fda6e91579c4522be` |
| Final sequence | `0x21`, `0x11`, final `0x00` with 120 ms delay |
| Component default table used | no |

The host-only table stub used by the native fake-backend harness changes only
the test compilation of the large constant table. Production firmware builds
the complete audited table. It is not a backlight or hardware enable flag.

## 5. Dependency resolution and API

Tracked manifest constraint:

`espressif/esp_lcd_st77916: "==1.0.1"`

Component Manager resolution:

| Item | Value |
|---|---|
| Direct dependency | `espressif/esp_lcd_st77916` |
| Resolved version | 1.0.1 |
| Registry component hash | `5fa0f8b1274576d4484e2b8d9358e2a5d09c721511bef0dce6a55b4206b5f0e9` |
| Repository commit | `6a112f4ddfeaf30ec360567ea9260a39e195c385` |
| Component C source SHA-256 | `A9124AC8A582D32FEE40F60E6E5BE747253ACE5BC458D0E21972A84A416570FF` |
| Component header SHA-256 | `21F444CDC25ED1CEEC379FE91FB5FD09D31EF179A946C9A7BD6F67D2E3599BFA` |
| ESP-IDF dependency | `>5.0.4,!=5.1.1`, resolved 5.5.4 |
| Private helper | `espressif/cmake_utilities` 0.5.3 |

`firmware/dependencies.lock` was generated and is intentionally ignored by
the existing repository policy. It records target `esp32s3`, IDF 5.5.4,
ST77916 1.0.1, the registry hash above, and no BSP. The managed component
manifest contains no LVGL, touch, audio, LEDC, PSRAM, network, motor, SD, or
ESP-VoCat board dependency.

The reviewed v1.0.1 API provides:

- QSPI bus mapping for clock plus D0-D3;
- QSPI panel IO with DC `-1`/`GPIO_NUM_NC`, 32 command bits, 8 parameter
  bits, and quad mode;
- `st77916_vendor_config_t` with explicit table pointer/count and QSPI flag;
- `esp_lcd_new_panel_st77916(...)`.

The implementation overrides the macro's queue depth from 10 to exactly 1
and retains the 40 MHz pixel clock.

## 6. GPIO and panel configuration

The production source uses only this numeric GPIO allowlist:

`{3, 11, 12, 13, 14, 18, 44, 46}`

| Role | Configuration |
|---|---|
| Reset | GPIO3, active low |
| D2 | GPIO11 |
| D3 | GPIO12 |
| D1 | GPIO13 |
| CS | GPIO14 |
| CLK | GPIO18 |
| Backlight gate | GPIO44, output low only |
| D0 | GPIO46 |
| DC | `GPIO_NUM_NC` |
| Host | `SPI2_HOST` |
| LCD clock | 40 MHz |
| Format/order | RGB565, RGB |
| Geometry | 360 × 360 |
| Mirror/swap/gap | false / false / 0,0 |
| Inversion | locked `0x21` table command |

GPIO44 is preloaded low, configured as a digital output with pull-down, set
low again, and read back before QSPI or panel work. Every draw, policy, and
READY transition rechecks low. Failure handling asserts low before and after
cleanup. No source path writes 1 to GPIO44.

There is no LEDC configuration, duty, percentage, fade, brightness API,
NVS brightness access, non-zero constant, runtime enable, compile-time
backlight enable, wrapper alias, or `BACKLIGHT_LOW_ENABLE` state.

## 7. State machine and markers

The reachable run-once sequence is:

1. `BOOT_MARKER`
2. `BACKLIGHT_FORCED_OFF`
3. `QSPI_BUS_INIT`
4. `PANEL_IO_CREATE`
5. `PANEL_RESET`
6. `PANEL_INIT`
7. `DISPLAY_ON`
8. `TEST_PATTERN_DRAW`
9. `BACKLIGHT_POLICY_GATE`
10. `READY`

`FAIL_SAFE` is the terminal failure state and has no second entry marker after
the single originating failure marker.

Each reached state emits one `LCD_SM_ENTER <STATE>`. A failing operation
preserves its `esp_err_t`, emits one
`LCD_SM_FAIL state=<STATE> err=<CODE>`, stops later initialization, performs
safe cleanup when no transaction is in flight, holds GPIO44 low, and returns
`FAIL_SAFE`. There is no automatic retry, reset, restart, or boot loop.
Repeated public invocation returns the stored terminal result without new
hardware actions.

Exact policy and READY markers:

- `DISPLAY_BACKLIGHT_POLICY: DISABLED_NOT_AUTHORIZED`
- `LCD_SM_READY visual=UNVERIFIED backlight=DISABLED_NOT_AUTHORIZED`

READY means only logical completion of the software calls. It does not mean a
visible screen, enabled backlight, correct colors/orientation, stable DMA on
the unit, or a healthy display.

## 8. Rendering and memory lifecycle

The deterministic source-generated pattern contains:

- black, red, green, and blue horizontal regions;
- a one-pixel white border;
- fixed text `ESP-VoCat LCD TEST`;
- a minimal glyph set containing only required characters.

RGB565 bytes are written most-significant byte first. There is no framebuffer,
file, image asset, font library, LVGL, filesystem, PSRAM, or double buffer.

Exactly one strip is allocated with:

`360 × 80 × 2 = 57,600 bytes`

Capabilities are `MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL`; allocation failure
and four-byte alignment are checked. Queue depth is 1. Half-open transfers
are `[0,80)`, `[80,160)`, `[160,240)`, `[240,320)`, and `[320,360)`.
A completion callback/semaphore is required before reuse. If the callback
times out, the buffer, IO, panel, bus, and semaphore are deliberately retained
because a transfer may still be in flight; freeing them would create a
use-after-free risk. GPIO44 still remains low and the machine stays in
`FAIL_SAFE`.

Static size and stack evidence:

| Item | Result |
|---|---|
| DIRAM use | 68,695 / 341,760 bytes; 273,065 bytes reported remaining |
| Static BSS | 2,720 bytes |
| Main task configured stack | 3,584 bytes |
| Largest display-module stack-usage frame | 96 bytes |
| 57,600-byte strip on stack | no |
| PSRAM configuration | not set |

Runtime heap success and actual task stack margin remain device-unverified;
the host evidence shows no static allocation blocker.

## 9. Host tests

Command:

`python -m unittest discover -s tests\host -p "test_*.py" -v`

Final result after reproducibility reconciliation: 55 tests run, 55 passed, 0 failed, 0 skipped under the required Espressif Python 3.13.9. The same 55 tests also passed under pytest when pytest was loaded into that exact interpreter without installing or modifying packages.

The display suite includes the clean-source artifact contract tests. Its native Windows
fake-backend harness compiles the real pattern and state-machine sources and
passes 723 C assertions. It covers:

- GPIO44-low ordering before QSPI/panel work;
- exact allowlist and V1.2 denylist;
- every reachable sequence-state failure injection;
- fail-safe GPIO44-low behavior;
- success ordering and run-once idempotence;
- exact markers and marker cardinality;
- five transfer windows;
- callback-before-reuse behavior;
- timeout retention of in-flight resources;
- reverse cleanup;
- pattern pixels, border, text, bounds, and RGB565 byte order;
- dependency pin;
- table count/token/hash/final sequence;
- absence of LEDC, high output, brightness/fade/NVS, PSRAM, LVGL, touch,
  audio, network, motor, SD, and device commands;
- artifact-manifest hard-disabled fields, ranges, hash, and file length.

The pre-existing Flash helper tests print simulated erase text from their fake
backend. Those lines are unit-test output only; no port or device was accessed.

## 10. Configure and build

Clean configure used a new build directory because repository policy forbids
recursive deletion of the old build directory:

`idf.py -B build-pcb-v1-minimal-display-smoke-test -D SDKCONFIG=sdkconfig.pcb-v1-minimal-display-smoke-test reconfigure`

Configure result: pass. It resolved ESP32-S3, IDF 5.5.4, Python 3.13.9,
ST77916 1.0.1, and generated the reviewed lock.

The first default-parallel build encountered a GCC 14.2.0 internal compiler
error in the unmodified IDF file `esp_lcd/rgb/esp_lcd_panel_rgb.c` during the
IRA pass. It also exposed one project const-qualifier warning, which was
fixed.

A disciplined single-object feedback loop established:

- the same `-Og` IDF object compiles with ccache disabled at `-j1`;
- it also compiles with ccache enabled at `-j1`;
- IDF CMake includes the unused RGB backend because ESP32-S3 advertises the
  RGB LCD capability;
- cache corruption was therefore excluded;
- the practical host-toolchain mitigation is serial compilation, without
  changing IDF, dependency versions, tests, warnings, optimization flags, or
  any hardware policy.

Final build:

`ninja -C build-pcb-v1-minimal-display-smoke-test -j1 all`

Final result: pass. The final rebuild compiled the modified project sources,
linked the ELF, generated the app image, and passed the IDF partition-size
check. Final project-source warnings: none. The parallel GCC ICE remains a
host-toolchain reliability note, not an implementation or safety blocker.

That first artifact build did not enable ESP-IDF reproducible-build mode and is superseded as canonical evidence by the two independent clean builds in section 20.

## 11. Static and link audit

Source, generated config, managed dependency, ELF, map, and size archives were
reviewed.

Required findings:

- GPIO44 output-low code precedes QSPI/panel creation;
- `SPI2_HOST`, 40 MHz, queue depth 1, exact QSPI mapping, policy marker,
  READY marker, `FAIL_SAFE`, internal DMA strip, table hash, and hard-disabled
  artifact metadata are present.

Prohibited production-source findings:

- no GPIO9, GPIO45, GPIO47, or 80 MHz LCD clock;
- no GPIO44-high or equivalent path;
- no LEDC, duty, brightness, fade, NVS display setting, or enable flag;
- no PCB V1.2 BSP, auto-detection, or board-wide init;
- no PSRAM, LVGL, touch, audio, Wi-Fi, Bluetooth, motor, SD, filesystem,
  image asset, full framebuffer, or double buffer;
- no device-access command.

Final allocated ELF symbols contain none of the prohibited service patterns.
The size tool reports no contribution from LEDC, I2S, MCPWM, SD/MMC, SDSPI,
PSRAM, Wi-Fi, Bluetooth, NVS, LVGL, or touch driver archives. The link map
contains generic `LOAD` and linker-script wildcard lines for IDF capability
libraries, and the ELF exposes five absolute `A`-type Wi-Fi ROM aliases from
the ESP32-S3 ROM linker definitions. These are not allocated executable/data
symbols, driver archive contributions, project calls, or initialized
services. They are recorded rather than misrepresented as product behavior.

The Flash header's 80 MHz field is Flash-interface metadata. It is not the LCD
clock; the LCD source and panel IO remain 40 MHz.

## 12. Final app artifact

| Field | Value |
|---|---|
| Canonical filename | `pcb_v1_minimal_display_smoke_test.bin` |
| SHA-256 | `4E66B7EDCD38B5225B00E1DB790CA7DD9C842E765961E8929228105F9C10A05D` |
| File length | 222,000 bytes / `0x36330` |
| Size-tool image size | 221,885 bytes before BIN padding |
| Target | ESP32-S3 |
| Entry point | `0x403754B8` |
| Image segments | 6 |
| Image checksum | valid (`0x71`) |
| Image validation hash | valid, `1635e2fd1dbb2bfaf3639547b3606ab790b08583c0c578e4fcd959ddef503b6b` |
| Project/app version | `pcb_v1_minimal_display_smoke_te` / `5394afd-dirty` |
| Header Flash mode | DOUT |
| Header Flash frequency | 80m |
| Header Flash size | 16MB |

The header fields are build metadata and are not promoted to physical-device
facts. The app version is intentionally dirty because this authorized Apply
produced uncommitted source changes and no commit was authorized.

Segments:

| Index | Length | Load address | File offset | Memory |
|---:|---:|---:|---:|---|
| 0 | `0x0C238` | `0x3C020020` | `0x00000018` | DROM |
| 1 | `0x034CC` | `0x3FC94D00` | `0x0000C258` | internal DRAM |
| 2 | `0x008E4` | `0x40374000` | `0x0000F72C` | internal IRAM |
| 3 | `0x15EB0` | `0x42000020` | `0x00010018` | IROM |
| 4 | `0x10408` | `0x403748E4` | `0x00025ED0` | internal IRAM |
| 5 | `0x00020` | `0x50000000` | `0x000362E0` | RTC data |

## 13. Partition containment

The generated host-build partition table was decoded from its binary:

| Name | Range | Size |
|---|---|---:|
| `nvs` | `[0x00009000,0x0000F000)` | 24 KiB |
| `phy_init` | `[0x0000F000,0x00010000)` | 4 KiB |
| `factory` | `[0x00010000,0x00110000)` | 1 MiB |

The app fits that generated factory partition with `0xC9CD0` bytes (79%)
remaining. The generated layout is nevertheless rejected for device-write use
because it conflicts with the preserved original layout.

The immutable original partition-table backup was decoded again without
modification:

| Name | Range | Size |
|---|---|---:|
| `nvs` | `[0x00009000,0x0000D000)` | 16 KiB |
| `otadata` | `[0x0000D000,0x0000F000)` | 8 KiB |
| `phy_init` | `[0x0000F000,0x00010000)` | 4 KiB |
| `ota_0` | `[0x00020000,0x00410000)` | `0x3F0000` |
| `ota_1` | `[0x00410000,0x00800000)` | `0x3F0000` |
| `assets` | `[0x00800000,0x01000000)` | 8 MiB |

The standalone app geometrically fits original `ota_0`, leaving `0x3B9CD0`
bytes. This is host evidence only and grants no write target or authorization.

## 14. Non-executable candidate range model

Candidate start `0x00020000` is derived from the newly decoded immutable
original partition table, not copied from the generated factory offset.

The three scopes are intentionally separate:

| Scope | Range | Size/detail |
|---|---|---|
| Semantic image | `[0x00020000,0x00056330)` | `0x36330` / 222,000 bytes |
| Expected ROM no-stub transport | `[0x00020000,0x00056400)` | `0x36400`; 217 × 1,024-byte blocks |
| Sector erase envelope | `[0x00020000,0x00057000)` | 55 × 4,096-byte sectors |

The final transport block contains 816 semantic bytes followed by 208 expected
`0xFF` padding bytes. The padding range is
`[0x00056330,0x00056400)`.

This model is non-executable. It contains no device command and cannot be
used as authorization. Any future review must independently confirm the
exact transport behavior rather than infer it solely from file length.

## 15. Recovery evidence

All four immutable 32 MiB recovery images were rehashed without modification:

- two files under
  `E:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi`;
- two files under
  `D:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi`.

Every file is 33,554,432 bytes and every SHA-256 is:

`72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001`

Level 1 remains a separately reviewed complete original `ota_0` restore.
Level 2 remains a separately gated 32 MiB full recovery. They are not
interchangeable. DEC-023, rollback decision, restoration rehearsal, and the
independent Recovery verification Change remain required and unchanged.

## 16. First implementation review

The implementation-completeness review found and corrected:

1. the 365-token/184-command planning-count error;
2. a callback user-context lifetime issue by moving the display context from
   a returned stack frame to persistent static run-once storage;
3. possible in-flight use-after-free by retaining resources on callback
   timeout;
4. repeated checked backlight configuration in failure handling by separating
   one checked initialization from best-effort low retention;
5. implicit DC `-1` by making `GPIO_NUM_NC` explicit;
6. a vendor-config const warning;
7. missing embedded hard-disabled/not-for-visual-validation metadata;
8. an unavailable Windows target in the ESP-IDF clang build by using the
   already installed Visual C++ compiler for the native harness;
9. the GCC parallel-build ICE by establishing and using the serial feedback
   loop described above.

Tests and the firmware build were rerun after these fixes.

## 17. Second adversarial safety review

The adversarial review searched source, tests, config, dependency metadata,
component tree, map, size archives, ELF symbols, artifact metadata, OpenSpec,
Git scope, historical records, and recovery references for:

- direct or indirect GPIO44 high;
- LEDC/non-zero duty/brightness/fade/NVS/enable gates;
- V1.2 GPIOs, 80 MHz LCD clock, GPIO9 power misuse, BSP import, or revision
  detection;
- PSRAM, LVGL, touch, audio, network, motor, SD, filesystem, large stack
  arrays, full/double framebuffer;
- retry/reset/boot-loop behavior;
- incomplete DMA waits, reuse, or frees;
- dependency drift or table mutation;
- READY/host-build visual overclaim;
- First Flash history rewrite or recovery weakening.

No reachable safety violation or blocking implementation issue remains.
Generic IDF capability metadata and ROM aliases were explained separately as
described in the link audit.

## 18. Successor handoff boundary

No successor Change was created. Before a future
`execute-pcb-v1-minimal-display-smoke-test` proposal can exist, a separate
review must require:

1. explicit approval of the exact GPIO, active-high polarity, frequency,
   resolution, raw duty, and percentage for any non-zero backlight;
2. a rebuilt non-zero artifact with fresh dependency, source, static, map,
   symbol, and artifact review;
3. fresh branch, HEAD, candidate hash, semantic/transport/erase ranges, ROM
   block plan, and immutable recovery evidence;
4. fresh exact device identity and current port review, never historical COM7
   or another old identity result;
5. a new exact single-attempt authorization;
6. separate write and startup/display-observation operations;
7. bounded checks for illumination, regions/text, color plausibility,
   orientation, refresh stability, and stop conditions;
8. continued `UNVERIFIED` labels for facts not actually observed;
9. no reuse of consumed First Flash, startup, observation, or recovery
   authority;
10. preservation of DEC-023, the Level 1/Level 2 distinction, restoration
    rehearsal, rollback decision, and independent Recovery verification.

The current hard-disabled artifact is not eligible for visual validation.

## 19. OpenSpec and final boundary

After the initial 2026-07-28 evidence synchronization, the Change contained
65 checked / 0 unchecked / 65 total tasks. Current-Change strict validation
and full repository strict validation passed at that boundary. The Change
remained active and unarchived, allowing the later reproducibility defect to
be reconciled in section 20.

Initial host-only condition, superseded by section 20:

`PASS — HOST IMPLEMENTATION AND ARTIFACT REVIEW COMPLETE`

This reaches the next host-only final-audit boundary only. It does not reach a
device-operation, Flash, non-zero-backlight, visual-validation, recovery, or
archive boundary.

## 20. Artifact reproducibility reconciliation

The 2026-07-28 artifact review had one unresolved defect: its checked-in test opened a specific ignored BIN and compared that local file with one recorded hash. A second ignored build directory already contained a same-size BIN with a different hash, so the test could pass or fail based on stale local state and did not establish a clean-source artifact contract.

Binary comparison found only these substantive differences between the two historical 222,000-byte images:

- app descriptor time `23:18:55` versus `00:05:53`;
- app descriptor date `Jul 28 2026` versus `Jul 29 2026`;
- the dependent 32-byte ELF SHA field;
- the image checksum and appended validation hash.

Both historical generated configurations had `CONFIG_APP_COMPILE_TIME_DATE` enabled and `CONFIG_APP_REPRODUCIBLE_BUILD` unset. Segment geometry, entry point, application version, project name, IDF version, image length, and safety behavior did not supply an alternative explanation. The root cause was therefore embedded build time/date metadata, not source or linker-order drift.

The smallest correction was added to tracked `firmware/sdkconfig.defaults`:

`CONFIG_APP_REPRODUCIBLE_BUILD=y`

Under ESP-IDF v5.5.4 this removes application time/date and path variance, selects path-hiding macros, and makes `CONFIG_APP_COMPILE_TIME_DATE` unavailable. No display GPIO, state-machine, dependency, optimization, partition, backlight, or device policy changed.

Two new build directories and two independently generated sdkconfig files were used. Neither existed before the run; neither consumed the old build directories, BIN, ELF, MAP, or CMake cache. Build B additionally used `CCACHE_DISABLE=1` so its compilation could not obtain objects from ccache.

| Evidence | Clean Build A | Clean Build B |
|---|---|---|
| Build directory | `firmware/build-repro-a-20260919` | `firmware/build-repro-b-20260919` |
| sdkconfig | `firmware/sdkconfig.repro-a-20260919` | `firmware/sdkconfig.repro-b-20260919` |
| Target | `esp32s3` | `esp32s3` |
| Build mode | `ninja -j1` | `CCACHE_DISABLE=1`, `ninja -j1` |
| Resolved sdkconfig SHA-256 | `8F7AE6B6487C58CCAF561985CFDDFC2B7435891FE7F07B59C54160BD6939072B` | same |
| App BIN | 222,000 bytes, `4E66B7EDCD38B5225B00E1DB790CA7DD9C842E765961E8929228105F9C10A05D` | identical |
| App ELF | 3,583,264 bytes, `7D0EC306C0F277EF3A7A19BC1304A4C34C27D620B5BE08AAA472001A4372AA9E` | identical |
| App MAP | 2,591,216 bytes, `4482B6044071ED0CD54278B69C1E63D373861BB007F2D000B4378EE363D4BA6B` | identical |
| Bootloader BIN | 20,784 bytes, `6847BAC793FD36FE314897918A54E089859222CC58B0CDFC02EF696E93B20116` | identical |
| Partition table BIN | 3,072 bytes, `7F00B6C042A89B15B0CAC534F82ED988CAF29278FF5700B0C511EB1B5BB7C820` | identical |

Both application descriptors contain empty time/date fields and the same `5394afd-dirty` application version. The source-input inventory covers the current CMake, sdkconfig defaults, exact component manifest, main integration, and all four display implementation files, including the previously untracked implementation files. Its canonical aggregate SHA-256 is:

`40DC66C7AFD7C23BA750A0DC0E4B8FF885C447F4E7F95AC240EEA2D4D7E33EB4`

`tools/artifact_reproducibility/verify_pcb_v1_display_artifact.py` now supplies two checks:

1. manifest-only verification recomputes the current source inventory and validates the recorded configuration, two-build identities, canonical artifact, safety fields, and authorization boundary without opening any ignored BIN;
2. dynamic verification accepts any two newly produced build directories, requires reproducible resolved configuration, compares BIN/ELF/MAP/bootloader identities, validates empty descriptor time/date, and reconciles the fresh BIN with the canonical manifest.

Actual regression results on 2026-09-19:

- manifest-only artifact contract: PASS;
- dynamic two-clean-build artifact comparison: PASS;
- complete unittest suite under exact Espressif Python 3.13.9: 55/55 PASS;
- complete pytest collection under the same exact interpreter: 55/55 PASS;
- native display harness: 13,680 assertions PASS;
- low-level call inventory: 41 actual points;
- injectable fault matrix: 36/36 PASS, 0 uncovered;
- cleanup matrix: 5 order cases, 4 fault cases, 1 priority case PASS;
- both clean ESP-IDF builds and partition-size checks: PASS;
- `idf.py size`: 221,885-byte effective image, 222,000-byte padded BIN, DIRAM 68,695/341,760 bytes;
- physical display status: `UNVERIFIED`;
- device execution: `NOT_AUTHORIZED` and not performed.

One preliminary unittest invocation used Anaconda Python and produced 16 missing-`serial` environment errors; a preliminary pytest invocation under Anaconda produced 5 intentional exact-interpreter rejections. Neither was represented as a product failure or PASS. Both complete suites were then rerun successfully under the required Espressif interpreter without installing or modifying packages.

The prior `2794...` manifest hash and the other `5E638...` hash are historical non-reproducible outputs and are not canonical. The canonical application hash is `4E66...A05D`; semantic, ROM-transport, padding, and sector-erase ranges remain unchanged because the file length remains 222,000 bytes. The manifest no longer names or requires a specific ignored BIN path.

After reproducibility reconciliation, the Change contains 71 checked / 0
unchecked / 71 total tasks. Current-Change strict validation and full
repository strict validation pass. The Change remains active and unarchived.

Current final host-only condition:

`PASS — HOST-SIDE DISPLAY ARTIFACT EVIDENCE COMPLETE; READY FOR NEXT GATE`
