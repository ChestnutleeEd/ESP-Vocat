# PCB V1.0 Display Startup Failure — Offline Diagnostic Audit

Date: 2026-09-20

Change: `validate-pcb-v1-display-backlight-on-hardware`

> **RESULT: PRIMARY CAUSE IDENTIFIED — LIVE CONFIRMATION REQUIRED**
>
> **PRIMARY CAUSE: THE ONE-STARTUP OBSERVER DID NOT IMPLEMENT THE WINDOWS
> USB SERIAL/JTAG RTS PROPAGATION SEQUENCE USED BY ESPTOOL AND IDF MONITOR**
>
> **NO DEVICE OPERATION WAS PERFORMED**
>
> **HARDWARE/DISPLAY STATUS: UNVERIFIED**

## 1. Scope and safety boundary

This audit used repository files, the immutable same-device 32 MiB recovery
image, saved partition slices, build outputs, the installed ESP-IDF v5.5.4 and
esptool.py v4.12.dev3 source, and existing sanitized evidence only.

It did not open or enumerate `COM7`, reset the ESP32-S3, run a device-facing
esptool command, read live Flash, write or erase Flash, change `otadata`, touch
an eFuse, manipulate GPIO44, run a monitor, or request a button press, cable
change, or power cycle. Recovery assets were read and hashed only and were not
modified.

## 2. Repository baseline

The audit began from:

- branch: `feat/validate-pcb-v1-display-backlight-on-hardware`;
- HEAD: `2c961967ba7e98d9e75fde023fcb2faf5908e5fa`;
- upstream: `origin/feat/validate-pcb-v1-display-backlight-on-hardware`;
- ahead/behind: `0/0`;
- tracked and complete worktree: clean.

The First Write evidence remains `WRITE PASS`; its last observed device state
is `STAYING IN ROM BOOTLOADER / NO RESET`. The one later observation remains
`STARTUP FAIL — READY ABSENT AT 15-SECOND DEADLINE`, with zero captured device
serial lines and no retry.

## 3. Immutable-image integrity

All four full-image copies were rehashed read-only during this audit. Each is
33,554,432 bytes and has SHA-256:

`72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001`

The primary source was:

`E:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi\esp-vocat_full_flash_32MB_2026-07-10.bin`

Its partition-table, `otadata`, bootloader, `ota_0`, and `ota_1` byte ranges
were byte-identical to the independently saved D: slices used for tool
inspection.

## 4. Actual saved partition layout

ESP-IDF v5.5.4 `gen_esp32part.py` decoded the actual 4 KiB table at `0x8000`:

| Region | Type / subtype | Offset | Size | End-exclusive |
|---|---|---:|---:|---:|
| bootloader reserved window | ESP32-S3 image, outside table | `0x00000000` | to `0x00008000` | `0x00008000` |
| partition table | ESP-IDF table | `0x00008000` | `0x00001000` | `0x00009000` |
| `nvs` | data / nvs | `0x00009000` | `0x00004000` | `0x0000D000` |
| `otadata` | data / ota | `0x0000D000` | `0x00002000` | `0x0000F000` |
| `phy_init` | data / phy | `0x0000F000` | `0x00001000` | `0x00010000` |
| reserved gap | no table entry | `0x00010000` | `0x00010000` | `0x00020000` |
| `ota_0` | app / ota_0 | `0x00020000` | `0x003F0000` | `0x00410000` |
| `ota_1` | app / ota_1 | `0x00410000` | `0x003F0000` | `0x00800000` |
| `assets` | data / spiffs | `0x00800000` | `0x00800000` | `0x01000000` |
| upper erased Flash | no table entry | `0x01000000` | `0x01000000` | `0x02000000` |

There is no factory or test App partition. `ota_0` does begin at `0x20000`.

The candidate is 231,360 bytes (`0x387C0`), occupying
`[0x00020000,0x000587C0)`. The actual ROM transport occupied
`[0x00020000,0x00058800)` and the sector erase envelope was
`[0x00020000,0x00059000)`. All three are fully contained in `ota_0`; the
semantic image has `0x003B7840` bytes of partition margin.

## 5. Saved OTA selection state

The two 32-byte `esp_ota_select_entry_t` records were decoded from the start
of each 4 KiB sector. CRC was independently checked as
`esp_rom_crc32_le(UINT32_MAX, &ota_seq, 4)`.

| Sector | Sequence | State | Stored CRC | CRC valid | Mapped slot |
|---|---:|---|---:|---|---|
| 0 | `1` | `ESP_OTA_IMG_VALID` | `0x4743989A` | yes | `ota_0` |
| 1 | `0xFFFFFFFF` | erased / undefined | `0xFFFFFFFF` | no valid entry | none |

With two OTA slots, the bootloader mapping is
`(ota_seq - 1) % app_count`; sequence 1 maps to `ota_0`.

**Before the First Write, the saved OTA state selected `ota_0`.** The First
Write did not touch `otadata`; it changed only `[0x00020000,0x00059000)`.
The candidate source has no OTA/partition-write call, rollback is disabled,
and its application code has no path that mutates OTA metadata. Therefore the
best-supported post-write inference is that `ota_0` remained selected.

This is an inference from the immutable pre-write state plus the recorded
write geometry and source audit. It is not a live read of current `otadata`.

## 6. Original App slots

`ota_0` contains a valid ESP32-S3 App image:

- project: `xiaozhi`;
- version: `2.2.6`;
- ESP-IDF: `v5.5.3-dirty`;
- effective parsed image length: 2,615,808 bytes (`0x27EA00`);
- header: DIO / 80 MHz / 16 MB;
- chip ID: ESP32-S3;
- revision range: v0.0 through v0.99;
- secure version: 0;
- checksum and appended validation hash: valid.

The complete saved `ota_0` partition is 4,128,768 bytes and has SHA-256
`C8A2FE4AB0F9B7C823F1DCFDFC926682C9DBF1B079056461DC6F18A93A345D0E`.

`ota_1` is 4,128,768 bytes of `0xFF` and contains no valid App image. There is
therefore no second original Xiaozhi OTA image to select or fall back to.

Had the saved pre-write state been reset normally before replacement,
`ota_0` would have booted Xiaozhi. After the candidate replaced the beginning
of `ota_0`, normal selection should instead load the candidate. Selecting
`ota_1` would find an erased slot, not another original UI.

## 7. Candidate console route

The exact resolved candidate configuration contains:

- `CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG=y`;
- `CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG_ENABLED=y`;
- `CONFIG_ESP_CONSOLE_SECONDARY_NONE=y`;
- `CONFIG_ESP_CONSOLE_UART_NUM=-1`;
- UART default/custom console not selected;
- USB CDC console not selected;
- `CONFIG_USJ_ENABLE_USB_SERIAL_JTAG=y`;
- monitor baud 115200;
- bootloader log level INFO;
- application default and maximum log level INFO.

ESP-IDF routes the primary standard-I/O file descriptor to
`/dev/usbserjtag` for this selection. `CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG`
is not selected because USB Serial/JTAG is already the primary console.

**If the candidate reaches `app_main`, its `puts`/`printf`, ESP-IDF logs, and
READY output should appear on the same USB Serial/JTAG CDC endpoint represented
by the reviewed `COM7`.** The answer is `YES`, not UART0 or both.

## 8. READY marker route and timing

The candidate uses direct `puts`/`printf`, not removable `ESP_LOGx` calls, for
its fixed evidence. The first seven candidate lines are emitted in `app_main`
before `pcb_v1_display_run_once()` and therefore before any display, QSPI,
panel, pattern, or LEDC action.

The success path then emits one state-entry line for each state. Duty 10 can
be reached only after:

1. panel reset and initialization;
2. display-on;
3. all test-pattern transfers;
4. LEDC timer/channel preparation at verified duty zero;
5. one successful duty-10 update and duty readback.

Reaching duty 10 necessarily emits
`DISPLAY_BACKLIGHT_ENABLED ... raw_duty=10 ...` and then the READY state and
terminal result unless output transport itself fails at the same time.
Any checked display/LEDC failure emits a FAIL marker after the seven pre-display
lines and drives the backlight fail-safe path.

Consequently, zero captured serial lines is meaningful evidence: it is not the
normal signature of a panel or backlight failure after `app_main`. It points
earlier, to no effective reset/boot, pre-`app_main` failure, or failure of the
USB observation path.

## 9. One-startup reset audit

The performed observer sequence was:

1. open the exact COM handle once;
2. `CLR_DTR` once, intended to release GPIO0 for normal boot;
3. `SET_RTS` once;
4. wait 200 ms;
5. `CLR_RTS` once;
6. wait 200 ms;
7. read on the same handle.

The installed esptool and IDF Monitor implementations contain an additional
Windows `usbser.sys` requirement: every `setRTS()` is immediately followed by
`setDTR(current_dtr)`. Their source explains that this dummy DTR operation is
what causes the combined USB CDC control-line state, including the new RTS
value, to be propagated on Windows.

An offline fake-port replay produced:

```text
observer: CLR_DTR, SET_RTS, CLR_RTS
esptool HardReset: setRTS(true), setDTR(current),
                   setRTS(false), setDTR(current)
```

The observer used `EscapeCommFunction` directly and omitted both dummy DTR
refreshes. It is therefore **not equivalent** to esptool's reviewed normal-run
hard-reset path or IDF Monitor's hard reset.

The same observer happened to succeed on this unit on 2026-07-27. That proves
the shorter sequence can work under at least one prior control-line state; it
does not prove deterministic RTS propagation for the failed run. The differing
initial Windows/USB line state can explain why the same code later failed.

Answers to the reset questions:

1. The sequence was logically intended to reset normally, but was not
   sufficient to prove that a reset control request reached the native USB
   Serial/JTAG peripheral on this Windows run.
2. Yes. Because the First Write explicitly ended in ROM bootloader, a missed
   reset would simply leave the chip there. A stale/incorrect GPIO0 control
   state during a delivered reset could also select download mode.
3. Yes. Opening the CDC endpoint and changing DTR/RTS affect the combined USB
   control-line state. The observer did not normalize the lines using the
   installed Espressif implementation before relying on them.
4. No. The performed sequence omitted the `usbser.sys` DTR refresh after each
   RTS transition and did not perform the ESP32-S3 esptool pre-reset handling.
5. No. There is no concrete evidence that the CPU exited ROM bootloader after
   the First Write: no ROM banner, bootloader line, App line, READY marker, or
   visible display effect was captured.

## 10. Offline boot-compatibility audit

No offline rejection condition was found:

| Check | Result |
|---|---|
| image target | PASS — ESP32-S3 chip ID 9 |
| device revision | PASS — candidate v0.0-v0.99 contains recorded v0.2 |
| eFuse block revision | PASS — candidate v0.0-v1.99 |
| image format | PASS — image v1, six structurally valid segments |
| checksum / appended hash | PASS — both valid |
| secure version | PASS — candidate 0, original App 0 |
| partition offset/alignment | PASS — `0x20000`, 64 KiB aligned |
| size / partition fit | PASS — `0x387C0` inside `0x3F0000` |
| Flash frequency | PASS — 80 MHz, matching original headers |
| Flash-size header | PASS — 16 MB, matching original headers and covering the complete partitioned 16 MB region on a 32 MB chip |
| Flash mode | PASS / explained — candidate DOUT versus original DIO; ESP-IDF has no OPI header code, and the same preserved bootloader already started the earlier v5.5.4 DOUT minimal candidate |
| Secure Boot | no rejection indicated — recorded disabled; plaintext images parse normally |
| Flash Encryption | no rejection indicated — recorded disabled |
| rollback / anti-rollback | no mismatch indicated — App rollback disabled, state VALID, secure version 0 |
| ESP-IDF patch level | no incompatibility found — preserved bootloader v5.5.3-dirty, candidate v5.5.4; earlier DOUT v5.5.4 candidate booted with this boot chain |
| PSRAM | PASS for startup assumption — intentionally disabled |

The unknown vendor `-dirty` bootloader delta and absence of candidate readback
remain residual uncertainties, but neither supplies a concrete offline reason
for rejection. ROM plaintext hash verification passed for the recorded write.

## 11. Black-screen interpretation

The black screen does not prove GPIO44 failure.

- If the normal reset never occurred, the chip remained in ROM and GPIO44's
  external pull-down should keep the backlight off.
- If the candidate started but failed before the duty-10 state, the firmware's
  designed result is also backlight off.
- If the candidate reached duty 10, `10 / 1023 = 0.9775%` average duty may be
  visually indistinguishable from off in normal room lighting. That possibility
  explains the visual report but not the missing serial output.
- There is no alternate original OTA slot whose historical UI should have
  appeared: `ota_1` is erased, and the selected `ota_0` App header was replaced
  by the candidate.

Therefore the black screen is consistent with several paths and cannot isolate
the panel, GPIO44, MOSFET, PWM, or candidate execution state.

## 12. Root-cause ranking

| Cause | Classification | Evidence |
|---|---|---|
| A. `otadata` did not select written `ota_0` | **WEAKENED** | Saved entry 0 is seq 1 / VALID / CRC-valid and maps to `ota_0`; write did not touch `otadata`; no candidate OTA mutation path. Current live bytes were not read in this task. |
| B. Startup action did not leave ROM / did not generate intended reset | **SUPPORTED — PRIMARY** | Prior state was explicitly ROM; all output and display effects were absent; observer omitted the Windows RTS-propagation sequence present in esptool and IDF Monitor. Historical one-time success prevents calling it live-proven. |
| C. Candidate booted but COM7 is not its console | **RULED OUT by build/source evidence** | USB Serial/JTAG is the primary stdout/log console; UART number is -1 and no secondary route is selected. |
| D. Candidate selected but preserved bootloader rejected it | **WEAKENED** | All offline compatibility checks pass, ROM hash verification passed, and the same boot chain already started an earlier v5.5.4 DOUT candidate. No current boot log exists, so it is not absolutely excluded. |
| E. Candidate executed and failed before backlight enable | **WEAKENED** | A display/LEDC failure should still leave seven pre-display lines plus a FAIL marker. Only a pre-`app_main` or simultaneous USB-output failure fits zero lines. |
| F. Candidate executed normally but 0.98% looked black | **PLAUSIBLE FOR BLACK SCREEN ONLY** | Sub-1% duty can look unlit, but normal execution necessarily predicts the fixed serial sequence, which was not captured. |

The best evidence-based result is:

`PRIMARY CAUSE IDENTIFIED — LIVE CONFIRMATION REQUIRED`

The primary cause is the non-equivalent, potentially non-propagating Windows
USB Serial/JTAG reset primitive. The exact physical state after the failed run
cannot be proven offline.

## 13. Exactly one next physical diagnostic gate

The next gate, if separately reviewed and explicitly authorized later, should
be exactly one corrected normal-start reset on the freshly confirmed native
USB Serial/JTAG endpoint. It must not use the current observer's raw three-call
sequence.

Classification: `REBOOT`.

Required reset semantics, expressed non-executably:

1. open the exact endpoint once and retain that handle;
2. hold DTR deasserted so GPIO0 requests normal Flash boot;
3. assert RTS, then immediately reapply the unchanged deasserted DTR state so
   Windows sends the combined USB control-line request;
4. hold reset for the reviewed bounded interval;
5. deassert RTS, then immediately reapply unchanged deasserted DTR again;
6. make no further control-line change, reopen, retry, enumeration, Flash
   operation, or second reset;
7. use the same handle for bounded passive observation of ROM/bootloader/App
   output and the existing visual checklist.

Success is any concrete normal-boot evidence leading to the expected ordered
candidate sequence. Stop on silence, download-mode text, handle loss, fatal
text, unexpected brightness, or missing READY; do not retry.

This audit does not authorize or execute that gate.

## 14. OpenSpec and hardware status

Tasks 8.4-8.6 and 9.1-9.3 remain incomplete. The failed startup observation is
not converted to PASS, and the display/backlight is not promoted.

Hardware status remains:

`UNVERIFIED`

`PARTIAL — PRIMARY STARTUP CAUSE IDENTIFIED; ONE READ-ONLY PHYSICAL CONFIRMATION REQUIRED`
