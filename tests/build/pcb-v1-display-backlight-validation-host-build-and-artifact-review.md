# PCB V1.0 Low-Duty Backlight Candidate Host Build and Artifact Review

Date: 2026-09-20

Change: `validate-pcb-v1-display-backlight-on-hardware`

Branch: `feat/validate-pcb-v1-display-backlight-on-hardware`

Build baseline: `a0b7c726d90448af0f811b7f6c996897e2a16c8a`

## 1. Outcome and scope

The host-side implementation and artifact gate pass. The candidate is ready for
a later device-gate review, subject to every still-open gate below. It is not
authorized for device execution.

- Artifact role: `visual-validation-candidate`.
- Backlight policy: `LOW_FIXED_TEST_ONLY`.
- Physical display and backlight status: `UNVERIFIED`.
- Device execution: `NOT_AUTHORIZED` and not performed.
- No COM/serial port was opened or enumerated, and no device-facing esptool,
  reset, write, erase, monitor, readback, or firmware execution occurred.
- The predecessor artifact and its `not-for-visual-validation` status remain
  unchanged.

The only implementation delta is the fixed GPIO44 LEDC candidate, its
fail-safe shutdown, exact markers, host fakes/tests, and reproducibility
evidence. Touch, LVGL, PSRAM, audio, network, motor, SD, NVS brightness,
dynamic brightness, fades, and product behavior remain outside scope.

## 2. Exact backlight contract

GPIO44 is treated as a strongly supported, active-high AO3400A low-side LEDK
gate with an external 10 kOhm pull-down. This topology is not promoted to a
physically confirmed fact.

| Parameter | Fixed value |
|---|---|
| GPIO | 44 |
| Driver | ESP-IDF LEDC |
| Mode / clock | low-speed / APB |
| Timer / channel | 0 / 0 |
| Frequency / resolution | 2,000 Hz / 10 bits |
| Initial duty / h-point | 0 / 0 |
| Validation duty | 10 of 1023 (reported as 0.98%) |
| Polarity | active high, no inversion |
| Fade / dynamic control / NVS | absent |

The existing direct-GPIO preload-low, output/pull-down configuration,
reassert-low, and read-low sequence still precedes all QSPI, panel, and LEDC
calls. LEDC is prepared at verified duty zero only after all five test-pattern
transfers complete. One non-zero update selects raw duty 10, then one duty read
verifies it. No alternate non-zero value, retry, ramp, setter, or runtime input
exists.

On failure after LEDC channel ownership, cleanup attempts duty zero, idle-low
`ledc_stop`, direct GPIO44 latch-low/output/pull-down restore, reassert/read
low, and then the pre-existing reverse display cleanup. All available cleanup
steps continue after a cleanup error while the original failure retains
priority.

## 3. State and marker contract

The state graph contains exactly 12 states:

1. `BOOT_MARKER`
2. `BACKLIGHT_FORCED_OFF`
3. `QSPI_BUS_INIT`
4. `PANEL_IO_CREATE`
5. `PANEL_RESET`
6. `PANEL_INIT`
7. `DISPLAY_ON`
8. `TEST_PATTERN_DRAW`
9. `BACKLIGHT_PWM_PREPARE`
10. `BACKLIGHT_LOW_ENABLE`
11. `READY`
12. `FAIL_SAFE`

Success uses the exact configuration, enable, and terminal markers specified
by the Change, including
`LCD_SM_READY visual=UNVERIFIED backlight=LOW_FIXED_TEST_ONLY`. Failure still
emits exactly one `LCD_SM_FAIL`, with no reset, retry, or fallback.

## 4. Host-test results

The approved interpreter was
`C:\Espressif\tools\python\v5.5.4\venv\Scripts\python.exe` (Python 3.13.9).

| Check | Result |
|---|---|
| Complete unittest collection | 56/56 PASS |
| Complete pytest collection | 56/56 PASS |
| Native compiled harness | 19,256 assertions PASS |
| Production low-level inventory | 51 call points |
| Failable/injectable boundaries | 46 |
| Fault matrix | 46/46 PASS, 0 uncovered |
| Cleanup matrix | 5 order, 10 fault, 1 priority cases PASS |

The native harness compiles the real display and test-pattern sources against
the smallest ESP-IDF-compatible fake surface. It proves exact success ordering,
values and marker cardinality, failure-state cutoff, first-error priority,
complete cleanup continuation, modeled backlight-off failure termination, and
run-once idempotence. Every predecessor pattern/table/QSPI/DMA/callback,
timeout, cleanup, denylist, and artifact test remains in the complete suites.

A first direct `python -m pytest` attempt under the approved interpreter
reported that pytest was not installed in that venv. No test ran in that
attempt. The passing pytest collection used the same exact interpreter while
loading the existing Anaconda pytest package through `PYTHONPATH`; no package
was installed or modified. A preliminary unittest module-name invocation also
failed because `tests` is not a Python package; the correct file/discovery
invocations then passed. These environment outcomes are not hidden or counted
as test failures.

The two existing Flash-helper tests print simulated erase text through fakes.
That text is host-test output only and did not contact hardware.

## 5. Independent clean builds

Two fresh ignored build directories and two fresh ignored generated sdkconfig
files were used with ESP-IDF v5.5.4, target ESP32-S3, CMake 3.30.2, Ninja
1.12.1, and Xtensa GCC 14.2.0 (`esp-14.2.0_20260121`). Both builds ran with
serial Ninja `-j1`; Build B additionally set `CCACHE_DISABLE=1`.

- Build A: `firmware/build-backlight-repro-a-20260920-r1`
- Build B: `firmware/build-backlight-repro-b-20260920-r1`

Both configure and build phases passed. An earlier `export.ps1` attempt failed
before configuration because that wrapper corrupted the non-ASCII Windows user
path. A first direct Build A path also stopped before configuration because the
tool path was incomplete. The authoritative `-r1` builds used explicit local
ESP-IDF/tool paths and passed; neither preliminary attempt performed a device
operation.

## 6. Byte-identical output identities

Every compared output is byte-identical between Builds A and B:

| Output | Bytes | SHA-256 |
|---|---:|---|
| App BIN | 231,360 | `FEE9C3D1AA77CD0DF1B2C2D55A86B7BEAA2871E5E8443B9D458B10324FF8F362` |
| App ELF | 3,679,252 | `53648E19C03173F4AD049875BECD8AB814F6523AD9588ED500DE3F302FA8FE3A` |
| App MAP | 2,644,596 | `9C4250E4FBEDFD78AF6CA089681197458B00545FA72150C7C4793BAE0BDAA1DA` |
| Bootloader BIN | 20,784 | `6847BAC793FD36FE314897918A54E089859222CC58B0CDFC02EF696E93B20116` |
| Partition-table BIN | 3,072 | `7F00B6C042A89B15B0CAC534F82ED988CAF29278FF5700B0C511EB1B5BB7C820` |
| Resolved sdkconfig header | 37,381 | `8F7AE6B6487C58CCAF561985CFDDFC2B7435891FE7F07B59C54160BD6939072B` |

The canonical filename remains the ESP-IDF project output
`pcb_v1_minimal_display_smoke_test.bin`; its successor role is established by
the new manifest and hash, not by relabeling the predecessor. The source-input
aggregate is
`E9928883A0E81503537BC5444A21358FEABF69FFFD5D200527A4177050FDDD32`.

The predecessor hard-disabled hash is
`4E66B7EDCD38B5225B00E1DB790CA7DD9C842E765961E8929228105F9C10A05D`.
It differs from the successor and remains `not-for-visual-validation`.

## 7. Image inspection

Host-only `esptool image_info` inspection reported:

- ESP32-S3 image v1, entry point `0x403754D4`, six segments;
- DOUT, 80 MHz, 16 MB header metadata;
- app version `a0b7c72-dirty`, ESP-IDF v5.5.4, empty compile date/time;
- checksum `0xDE` valid;
- validation hash
  `b1f1cff8ac37531bfd8698d64a3eae798c7faddec457a3943c16062b0207e59c`
  valid;
- secure version 0 and 64 KiB MMU page size.

Segments are DROM `0x0CB48` at `0x3C020020`, DRAM `0x034A8` at
`0x3FC95000`, IROM `0x17780` at `0x42000020`, DRAM `0x0008C` at
`0x3FC984A8`, IRAM `0x10F34` at `0x40374000`, and RTC data `0x00020` at
`0x50000000`. Header values are build metadata, not physical-device evidence.

## 8. Configuration, dependency, map, and size audit

Both resolved configurations retain reproducible-build mode, no compile
date/time, USB Serial/JTAG console ownership, UART console number `-1`,
partition-table offset `0x8000`, no PSRAM, no Secure Boot enable, and no Flash
Encryption enable. The ST77916 dependency remains exactly 1.0.1 with registry
component hash
`5fa0f8b1274576d4484e2b8d9358e2a5d09c721511bef0dce6a55b4206b5f0e9`.
Only `esp_driver_ledc` was added to the main component requirements.

The final component-size report contains the intended application, ST77916,
ESP LCD, and LEDC contributions. It contains no allocated Wi-Fi, Bluetooth,
NVS, I2S, SD/MMC, filesystem, LVGL, or touch-driver contribution. Generic ROM
linker aliases for Wi-Fi/UART and discarded archive sections are not project
calls or enabled services. The linked LEDC driver object contains private fade
implementation helpers, but project source and final call references use only
timer/channel configuration, fixed duty/update/read, and stop; no fade API is
called or exposed.

`idf.py size`/the Ninja size target reported 231,213 effective image bytes,
148,166 used Flash bytes, 69,439 used static D/IRAM bytes, and 272,321 bytes
remaining static D/IRAM. Static IRAM uses the target's full 16,384-byte IRAM
allocation. The configured main-task stack is 3,584 bytes; the bounded path
uses the pre-existing heap DMA strip and introduces no framebuffer or large
stack allocation. Runtime stack high-water remains unmeasured because device
execution is prohibited.

The vendor table remains 184 commands / 365 legacy initializer tokens with
normalized SHA-256
`e8a1f2ea307b51be59d3daa201bb444b5f5ddc2d8da2931fda6e91579c4522be`.
Pattern geometry, five half-open windows, RGB565 byte order, QSPI mapping,
single DMA strip, callback/timeout contract, and GPIO allowlist are unchanged.

## 9. Candidate range model

This is a host-only, non-executable model. It grants no target, port, command,
or authorization.

| Scope | Range | Detail |
|---|---|---|
| Semantic image | `[0x00020000,0x000587C0)` | `0x387C0` / 231,360 bytes |
| Expected ROM no-stub transport | `[0x00020000,0x00058800)` | 226 x 1,024-byte blocks |
| Final transport block | `[0x00058400,0x00058800)` | 960 semantic + 64 `0xFF` padding bytes |
| Sector erase envelope | `[0x00020000,0x00059000)` | 57 x 4,096-byte sectors |

All three ranges fit inside preserved original `ota_0`
`[0x00020000,0x00410000)`. The semantic margin is `0x003B7840` bytes.
Untouched geometry includes bootloader, partition table, NVS, `otadata`,
`phy_init`, the reserved gap, remaining `ota_0` from `0x00059000`, `ota_1`,
assets, and the upper Flash tail. Geometry alone does not prove transport,
compatibility, or authority.

## 10. Recovery evidence and remaining rehash gate

Planning reverified four immutable 33,554,432-byte full images at SHA-256
`72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001`
and two immutable 4,128,768-byte complete original `ota_0` staging images at
SHA-256
`C8A2FE4AB0F9B7C823F1DCFDFC926682C9DBF1B079056461DC6F18A93A345D0E`.
No recovery asset was modified.

The immediate pre-device-gate rehash was performed again on 2026-09-20. All
four full images and both complete original `ota_0` staging copies matched the
paths, sizes, and SHA-256 values above. Any later missing path, size/hash
mismatch, or same-device uncertainty remains a stop condition. Level 1
complete original `ota_0` restoration and Level 2 complete 32 MiB restoration
remain distinct future `WRITE` operations under separate Changes and explicit
authorizations.

## 11. Manual recovery-entry evidence

Espressif's official ESP-VoCat v1.0 guide establishes the board-specific facts:

- dedicated `RST Button`: resets the main board;
- dedicated `BOOT Button`: hold it while powering on to enter download mode;
- Type-C (USB-C): power, programming download, and debugging; use a USB data
  cable for application development;
- controller: `ESP32-S3-WROOM-2-N32R16V`, 32 MB Flash, 16 MB PSRAM;
- `LCD_BLK`: GPIO44.

Source reviewed:

- https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp-vocat/user_guide_v1.0.html

This closes OpenSpec task 6.1 as `DOCUMENTED PROCEDURE`. It does not establish
`PHYSICALLY CONFIRMED ON THIS UNIT`; no physical BOOT attempt has yet been
inferred or performed by this evidence update. The guide does not require an
invented BOOT+RST combination. No eFuse, security, Flash-voltage, boot-mode,
or USB setting may be changed to work around a failed connection.

## 12. Non-executable future device-gate packet

The packet fields below are a checklist only; no copyable device command is
included.

- Bind the then-current branch, committed HEAD, clean status, candidate
  filename, 231,360-byte size, canonical SHA-256, source digest, and image
  descriptor.
- Bind ESP32-S3, PCB V1.0, the freshly identified USB Serial/JTAG endpoint, and
  sanitized minimum identity evidence. Historical COM7 is invalid as a
  selection rule.
- Bind semantic, 1,024-byte transport/padding, and 4,096-byte erase geometry;
  require no stub substitution, compression drift, alternate port, implicit
  monitor, automatic reset beyond the reviewed mechanism, or retry.
- Rehash all Level 1/Level 2 recovery assets immediately beforehand and bind
  their exact same-device scope.
- Classify identity collection as future `READ-ONLY`, candidate installation
  as future `WRITE`, startup as future `REBOOT`, and observation as future
  `READ-ONLY`. Each requires its own applicable review and explicit user
  authorization.
- Permit at most one write attempt only after the board-specific manual
  recovery-entry gap is closed and an exact non-executable command review has
  been converted into a separately presented command for fresh authorization.
- Stop on any branch/HEAD, hash, size, range, padding, recovery, identity,
  endpoint, chip, PCB, security, or tool-mode mismatch.

This packet is `NOT_AUTHORIZED`, contains no executable command, and does not
consume any prior authorization.

## 13. Separate bounded observation packet

After a separately reviewed successful write, and only after separate
authorization, one bounded startup/display observation would require:

- exact ordered single markers through fixed-duty READY;
- no reset loop, panic, watchdog, allocation/security failure, handle loss, or
  repeated initialization;
- no early/full-brightness flash;
- a visibly low, stable backlight with no flicker, pulsing, odor, heat, noise,
  or abnormal symptom;
- the fixed black/RGB/white-border/`ESP-VoCat LCD TEST` pattern with plausible
  colors, orientation, and stable refresh;
- privacy-safe evidence with no retained private device identifier;
- immediate stop without retry, duty increase, second reset/open, automatic
  rollback, or restore on any mismatch or inconclusive result.

Observation cannot confirm measured current, peak-current reduction, long-term
life, full brightness range, touch, other peripherals, recovery, or another
power cycle.

## 14. Generated files and Git boundary

The two build directories, generated sdkconfig files, `.pytest_cache`, Python
bytecode, and native harness build outputs are ignored generated state and must
not be committed. The commit scope is limited to source, host tests/fakes,
OpenSpec artifacts, this new manifest/review, and the generalized
reproducibility verifier. No predecessor evidence, recovery asset, unrelated
file, or generated firmware binary is modified or included.

## 15. Host gate disposition

Host implementation, tests, static checks, clean builds, byte identity, image
inspection, dependency/table/pattern audits, range calculation, manifest
verification, immediate recovery-asset rehash, and authoritative PCB V1.0
manual recovery-entry documentation all pass. The status is:

`READY FOR DEVICE-GATE REVIEW`

This status is deliberately narrower than device-ready or physically verified.
Fresh device identity/endpoint review has since completed, and the later
voltage reconciliation below clears its original blocker. Exact write-packet
review and fresh write authorization remain open. The documented
BOOT-during-power-on procedure is not yet physically confirmed on this unit.
Hardware status remains `UNVERIFIED`.

## 16. Later pre-write identity-gate result (2026-09-20)

The later authorized identity gate passed repository, artifact, recovery,
official-procedure, and unique-endpoint checks. One no-stub esptool
`flash_id` interrogation confirmed the expected ESP32-S3/32 MB/16 MB
PSRAM/USB Serial-JTAG identity surface but reported VDD_SPI eFuse voltage
`3.3V`, conflicting with the documented N32R16V 1.8 V configuration.

The gate stopped immediately. No exact future write command was generated and
no write, erase, readback, monitor, rollback, or restore occurred. See
`tests/hardware/pcb-v1-pre-write-physical-identity-gate-2026-09-20.md`.

### 16.1 Later voltage-resolution audit

The original fail-closed identity-gate result remains historical evidence. A
later host-only audit reconciled it with the repository's existing minimum
eFuse summary: `VDD_SPI_FORCE=1`, `VDD_SPI_XPD=1`, and `VDD_SPI_TIEH=0` select
a forced 1.8 V LDO. The 3.3 V line was caused by installed esptool.py
`v4.12.dev3` testing a broad combined-bit mask before its narrower branches.

Live RDID `C2 80 39` also maps exactly to Macronix MX25UM25645G, a 256-Mbit /
32-MiB Octal SPI NOR device specified for 1.65-2.0 V. No new device access was
required. The voltage blocker is resolved, while physical display/backlight
status remains `UNVERIFIED` and no Flash write is authorized. See
`tests/hardware/pcb-v1-vdd-spi-contradiction-resolution-2026-09-20.md`.
