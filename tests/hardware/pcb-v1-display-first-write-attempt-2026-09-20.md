# PCB V1.0 Display Validation — Single Authorized First Write

Date: 2026-09-20

Change: `validate-pcb-v1-display-backlight-on-hardware`

> **RESULT: WRITE PASS**
>
> **DEVICE STATE: STAYING IN ROM BOOTLOADER / NO RESET**
>
> **HARDWARE/DISPLAY STATUS: UNVERIFIED**

## 1. Scope and classification

Classification: `WRITE`.

Exactly one explicitly authorized App-only Flash invocation was performed for
the reviewed PCB V1.0 display-validation candidate. The command did not start
the application, open a monitor, observe the display, read Flash, restore an
image, or perform any eFuse/security/voltage operation.

## 2. Fresh repository gate

Immediately before the device identity gate and again immediately before the
write:

- branch: `feat/validate-pcb-v1-display-backlight-on-hardware`;
- HEAD: `4b54ff1da07bf745cef6d707216dd70b19e6fbd7`;
- upstream: `origin/feat/validate-pcb-v1-display-backlight-on-hardware`;
- ahead/behind: `0/0`;
- tracked worktree: clean.

The reviewed guard/tool files were the exact files at the required clean HEAD.
The exact argument vector was accepted by the committed guard.

## 3. Fresh candidate and recovery gate

Candidate:

`D:\ESP-VoCat-Project\custom-vocat\firmware\build-backlight-repro-a-20260920-r1\pcb_v1_minimal_display_smoke_test.bin`

- bytes: `231360`;
- SHA-256:
  `FEE9C3D1AA77CD0DF1B2C2D55A86B7BEAA2871E5E8443B9D458B10324FF8F362`.

The immediate offline guard audit rehashed all six recovery assets:

| Asset group | Copies | Bytes each | SHA-256 | Result |
|---|---:|---:|---|---|
| same-device full image | 4 | 33554432 | `72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001` | PASS |
| complete original `ota_0` | 2 | 4128768 | `C8A2FE4AB0F9B7C823F1DCFDFC926682C9DBF1B079056461DC6F18A93A345D0E` | PASS |

No recovery asset was modified.

## 4. Fresh device identity

The exact `COM7` Windows metadata record was present and identified
`VID_303A/PID_1001`, MI_00, status OK. One reviewed read-only ROM `flash_id`
interrogation then returned exit code 0 and reported:

- ESP32-S3 QFN56 revision v0.2;
- USB Serial/JTAG;
- embedded 16 MB PSRAM (`AP_1v8`);
- 40 MHz crystal;
- Flash manufacturer `C2`, device `8039`;
- detected Flash size 32 MB;
- octal / 8-data-line Flash type;
- staying in bootloader.

The installed esptool v4.12.dev3 again printed its known erroneous `3.3V`
summary. This was not a new contradiction: the already-reviewed individual
fields remain `VDD_SPI_FORCE=1`, `VDD_SPI_XPD=1`, and `VDD_SPI_TIEH=0`, which
select a forced enabled 1.8 V LDO. No eFuse was written or newly queried.
Secure Boot and Flash Encryption remained disabled in the controlling evidence,
and the live ROM operation produced no new security rejection or contradiction.

The interrogation authorization allowed its one transient ROM-loader reset.
No alternate port was scanned or probed.

## 5. Authorization and final immediate gate

Authorization path:

`D:\ESP-VoCat_First_Write_Authorizations\pcb-v1-display-validation-authorization.json`

- schema: `esp-vocat-display-first-write-authorization-v1`;
- status at execution: `EXPLICIT HUMAN FLASH AUTHORIZATION`;
- bound HEAD: `4b54ff1da07bf745cef6d707216dd70b19e6fbd7`;
- pre-execution authorization-file SHA-256:
  `D6101571C0AEE119E4AA1140DBA82EDF4F3FCC33F3324DFD66F4DFA17ACB7DBB`;
- exact candidate, `COM7`, ESP32-S3, PCB V1.0, offset, semantic/transport/erase
  geometry, and all six one-attempt controls were accepted by the guard;
- automatic retry, rollback, startup, observation, `erase_flash`, monitor,
  visual validation, and eFuse operations were explicitly not authorized.

The final immediate gate rehashed the candidate, rechecked Git and `COM7`,
validated the authorization and all local evidence through the guard, and
found zero conflicting Python/esptool/COM7 processes.

## 6. Physical mutation result

The exact reviewed command executed once under esptool.py v4.12.dev3.

| Scope | Actual result |
|---|---|
| sector erase envelope | `[0x00020000,0x00059000)` / reported `0x00020000` through `0x00058FFF` |
| semantic payload | 231360 bytes / `[0x00020000,0x000587C0)` |
| ROM transport | 231424 bytes / 226 blocks / `[0x00020000,0x00058800)` |
| final padding | 64 bytes of reviewed `0xFF` transport padding |
| write offset | `0x00020000` |
| verification | `Hash of data verified.` |
| guarded write invocations | 1 |
| additional write attempts | 0 |

The tool reported:

- `Took 1.03s to erase flash block`;
- `Wrote 231424 bytes at 0x00020000 in 2.7 seconds (679.0 kbit/s)`;
- `Hash of data verified.`;
- `Leaving...`;
- `Staying in bootloader.`;
- process exit code `0`.

The guard had freshly validated every process-local attempt control as one:
outer operation, block, connect, port open, sync per connect, and reset/reopen.
Execution evidence records one mutation invocation. The implementation does
not maintain a separate persistent cross-process attempt ledger. Immediately
after the successful invocation, the external authorization status was changed
to `CONSUMED HUMAN FLASH AUTHORIZATION` without deleting the evidence. Its
post-consumption SHA-256 is
`6EC57F4DE49E28E023B6497A5FA6E7D66541A9F0E223276DF825304FBB1258DE`.
A host-only call to the guard's authorization validator then rejected it on
the consumed status before any serial/device path. It cannot be reused by the
reviewed guard.

## 7. Final device and prohibition state

The last observed device state is ROM bootloader under `--after no_reset`.
No command followed the successful write.

| Prohibited/follow-up operation | Count |
|---|---:|
| additional write attempt | 0 |
| independent erase / `erase_flash` | 0 |
| restore / rollback | 0 |
| application startup/reset | 0 |
| serial monitor | 0 |
| display/GPIO44 visual validation | 0 |
| Flash readback | 0 |
| eFuse/security/voltage write | 0 |

The sector erase performed internally by `write_flash` was exactly the reviewed
erase envelope and is not a separate erase command.

## 8. OpenSpec disposition

Tasks 7.4 and 7.5 are complete. Startup/result-review task 8.1 and all reboot,
serial/display observation, recovery, and closeout tasks remain open and require
their own applicable review and authorization. Progress after this evidence is
`56/67` complete, `11` open.

Physical display illumination, GPIO44 PWM behavior, pixels, color, orientation,
refresh, reset-time transient, and all unobserved hardware facts remain
`UNVERIFIED`.

`PASS — SINGLE AUTHORIZED FIRST WRITE COMPLETED; DEVICE NOT STARTED; STARTUP REQUIRES SEPARATE AUTHORIZATION`
