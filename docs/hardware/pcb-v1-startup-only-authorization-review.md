# PCB V1.0 Startup-Only Authorization Review

> **FINAL HOST-ONLY STARTUP REVIEW**
>
> **UNSIGNED**
>
> **NOT AUTHORIZED**
>
> **DO NOT EXECUTE**

- Review date: 2026-07-27
- OpenSpec Change: `prepare-pcb-v1-first-flash-smoke-test`
- Scope: one future startup reset plus one bounded read-only observation
- Device access in this review: **NONE**
- Port enumeration/open in this review: **NONE**
- Reset/startup/observation in this review: **NONE**
- Flash/readback/rollback in this review: **NONE**
- Current startup/observation authorization: **NONE**
- Current Flash authorization: **NONE**
- Current rollback authorization: **NONE**

## 1. Executive Summary

This final host-only review concludes:

**READY TO REQUEST STARTUP-ONLY AUTHORIZATION**

This decision means only that a human may now review whether to grant one
future, exact startup-reset and read-only observation authorization. It is not
that authorization. It does not approve a current port, reset, startup,
serial open, observation, readback, Flash write, rollback, restore, or
recovery operation.

No immediate rollback is technically required before the bounded startup
opportunity. The candidate image remains a valid 160832-byte ESP32-S3 image;
its parser-derived end is `0x00047440`. The 960-byte transport padding and
2048-byte erase-only tail are not part of the candidate. No additional
logical NOR main-array `1`-to-`0` transition is expected from sending `0xFF`
over the just-erased padding range, although physical readback, ECC/internal
state, stress, wear, disturb, and exact ROM handling remain unverified.

Standard startup has no intended App-partition, NVS, `otadata`, PHY,
filesystem, or core-dump write path. The preserved second-stage Bootloader
does contain a standard conditional Flash-protection unlock path: if
non-volatile BP bits are unexpectedly set, it can clear them. Prior original
boots and the successful App-range erase/write strongly support that those
bits are already clear, but they were not reread. Normal Octal-Flash startup
also writes documented volatile ODS and STR-OPI configuration fields while
carrying unchanged protection-field values through a combined register
command. The exact physical effect of rewriting unchanged non-volatile fields
and the exact `v5.5.3-dirty` vendor modifications remain unverified residual
risks.

Stock ESP-IDF Monitor is rejected for this future observation because it
enumerates ports, can list alternate ports after failure, can repeatedly
reopen after USB loss, and consults port PID to choose a reset path. The
reviewed repository harness instead uses one exact COM name, one read-only
Win32 COM handle, one reset attempt, no enumeration, no retry, no reopen, no
serial data write, no raw-log file, bounded filtering, a 15-second ready
deadline, and a 60-second total observation window. Any USB handle loss stops
the operation rather than following a port.

Compatibility remains **B — PLAUSIBLE BUT NOT PROVEN** until actual runtime
evidence exists. First Flash remains **STOPPED / INCONCLUSIVE**. OpenSpec Task
3.4 remains **NOT COMPLETED** and progress remains 38/48.

## 2. Scope

Included:

- re-read committed First Flash, compatibility, current snapshot, build, and
  post-attempt records;
- rehash and offline inspection of the unchanged candidate, ELF, MAP,
  resolved `sdkconfig`, and component metadata;
- local ESP-IDF v5.5.4 source review;
- installed esptool v4.12.dev3 and ESP-IDF Monitor source review;
- startup-chain persistent-write audit;
- reset and USB re-enumeration audit;
- design and pure-host testing of a fail-closed startup observer;
- one unsigned, human-readable future-authorization template;
- success, stop, failure, Task 3.4, and rollback boundaries.

Excluded:

- COM enumeration or port discovery;
- opening `COM7` or any other serial endpoint;
- connection, reset, startup, monitor, or observation;
- `chip_id`, `flash_id`, eFuse query, `read_flash`, Flash write, erase,
  rollback, restore, or recovery;
- configure, build, `idf.py`, or firmware modification;
- candidate, rollback, staging, or recovery-artifact modification;
- machine-readable authorization material;
- Task 3.4 completion, runtime acceptance, Change archive, or peripheral test.

## 3. Current Device State

The following is the last recorded state, not a new observation:

| Item | State |
|---|---|
| exact device | historical same ESP-VoCat PCB V1.0 unit; operation-time reconfirmation required |
| historical port | `COM7`; not current identity and not assumed |
| last observed execution state | staying in ROM Bootloader |
| candidate-range write/MD5 | success reported for one invocation |
| candidate startup | unobserved |
| ready marker | unobserved |
| runtime stability | unobserved |
| First Flash attempt | `STOPPED / INCONCLUSIVE` |
| Compatibility | `B — PLAUSIBLE BUT NOT PROVEN` |
| Task 3.4 | `NOT COMPLETED` |
| device access authorization | `NONE` |
| startup/observation authorization | `NONE` |
| Flash authorization | `NONE` |
| rollback authorization | `NONE` |

The 2026-07-26 current snapshot confirmed sequence 1 / `VALID` / CRC-valid
`otadata` selecting `ota_0`. The later write did not overlap `otadata`, and
no startup followed, so unchanged OTA metadata is strongly supported but was
not reread after the attempt.

## 4. Prior Attempt Status

The one App-only invocation:

- erased `0x00020000-0x00047FFF`;
- transmitted 161792 bytes over `0x00020000-0x000477FF`;
- included the 160832-byte candidate over
  `0x00020000-0x0004743F`;
- included 960 final-block `0xFF` padding over
  `0x00047440-0x000477FF`;
- left `0x00047800-0x00047FFF` erased and untransmitted;
- reported candidate-range MD5 success;
- performed no automatic retry;
- stayed in the ROM loader.

The 960 transmitted bytes exceeded the exact candidate-byte authorization.
The stop condition therefore remains valid. The attempt must not be relabeled
PASS merely because esptool returned success.

## 5. Padding Assessment

| Range | Length | Classification |
|---|---:|---|
| candidate | `0x27440` / 160832 | legal candidate image |
| transmitted ROM blocks | `0x27800` / 161792 | candidate plus final-block padding |
| `0xFF` padding | `0x3C0` / 960 | transport data outside candidate |
| erased-only tail | `0x800` / 2048 | sector erase only |

**CONFIRMED**

- the valid candidate ends at `0x00047440`;
- the historical padding and erase-only ranges contained valid, non-`0xFF`
  Xiaozhi App bytes;
- installed esptool sent 960 `0xFF` padding bytes;
- candidate header, segment table, checksum, and appended SHA-256 all end
  before that padding;
- ROM MD5 covered exactly 160832 bytes and did not cover either tail range.

**STRONGLY SUPPORTED**

- after a successful sector erase, programming `0xFF` requests no additional
  logical main-array `1`-to-`0` user-data transition;
- the standard non-Secure-Boot simple-hash path ignores both tail ranges.

**UNVERIFIED**

- actual post-attempt tail bytes and internal ECC/metadata state;
- program stress, wear, disturb, retention, or neighboring-cell effects;
- whether ROM skipped any all-`0xFF` program chunk;
- exact vendor Secure Boot v2 enforcement configuration.

## 6. Startup-chain Persistent-write Audit

| Startup area | Classification | Finding |
|---|---|---|
| normal ROM loading | `STRONGLY SUPPORTED` | no intended main-array or partition write merely to load the second-stage Bootloader |
| exact ROM implementation absolute no-write claim | `UNVERIFIED` | binary ROM handler source is unavailable |
| standard second-stage OTA selection with sequence 1 / `VALID` | `STRONGLY SUPPORTED` | selection reads `otadata`; normal branch does not write it |
| standard conditional BP unlock path | `CONFIRMED` in v5.5.4 source | reads status and writes only if protected bits differ from unlocked state |
| BP clear occurrence on this startup | `UNVERIFIED`, expected absent | prior successful boots/write strongly support an already-clear state |
| candidate App main-array/data-partition write intent | `STRONGLY SUPPORTED` absent | final ELF and component closure expose no reachable mutation path for the candidate |
| candidate NVS/PHY/filesystem/core-dump write | `STRONGLY SUPPORTED` absent | components/symbols/configuration are absent |
| Octal Flash ODS/STR-OPI register initialization | `CONFIRMED` in source | register commands run during normal OPI startup |
| ODS/CR2 persistence | `STRONGLY SUPPORTED` volatile | official Flash evidence classifies these selected fields as volatile |
| unchanged BP/TB physical register-cycle effect | `UNVERIFIED` | combined register write carries unchanged non-volatile fields |
| exact vendor Bootloader behavior | `UNVERIFIED` | `v5.5.3-dirty` modifications are unavailable |

No deliberate App main-array, partition, NVS, OTA, PHY, filesystem, or
core-dump write is part of the proposed startup.

## 7. ROM Bootloader Behavior

**STRONGLY SUPPORTED**

On reset with the download strap released, ESP32-S3 ROM selects the normal
Flash boot path, reads the preserved second-stage Bootloader, and transfers
control to it. No reviewed ROM boot action intentionally programs the App
partition or OTA metadata.

**UNVERIFIED**

The full ROM implementation is not locally source-auditable. This review does
not claim that every internal ROM side effect is known. A future ROM error,
download-mode loop, image rejection, or silence is a stop condition.

## 8. Preserved Vendor Bootloader Risks

The preserved Bootloader identifies itself as ESP-IDF `v5.5.3-dirty`.

**CONFIRMED**

- its bytes matched the historical backup at the 2026-07-26 snapshot;
- it previously booted the original Xiaozhi App;
- the later App-only operation did not target its Flash range.

**STRONGLY SUPPORTED**

- v5.5.x standard logic can load a newer patch-level App image;
- DOUT is a normal image-header mode and the candidate geometry is valid;
- current sequence 1 / `VALID` metadata selects `ota_0`;
- already-clear protection bits would cause the standard unlock function to
  skip WRSR.

**UNVERIFIED**

- vendor patches and Kconfig;
- actual acceptance of the v5.5.4 candidate;
- exact anti-rollback/security handling;
- non-standard OTA, protection, or persistence behavior.

This residual risk cannot be eliminated by more host-only reasoning. It is
the principal reason Compatibility stays classification B.

## 9. otadata Behavior

The original and later current snapshot both showed:

- sector 0: sequence 1, `VALID`, CRC valid;
- sector 1: erased;
- selected slot: `ota_0`.

Standard v5.5.4 source:

- writes `PENDING_VERIFY` to `ABORTED` only when rollback is enabled;
- writes `NEW` to `PENDING_VERIFY` only when rollback is enabled;
- performs neither transition for a `VALID` entry in the normal branch;
- can enter an anti-rollback secure-version update path only under the
  corresponding build option.

Result:

- `CONFIRMED`: snapshot-time sequence 1 / `VALID` state;
- `STRONGLY SUPPORTED`: the App-only attempt left it unchanged and one normal
  startup does not write it;
- `UNVERIFIED`: exact vendor-dirty behavior and current post-startup state,
  because startup has not occurred.

Any `otadata` write indication is a stop condition.

## 10. Candidate App Persistent-write Audit

The unchanged candidate remains:

- 160832 bytes;
- SHA-256
  `1B72A60DE9C9BB42DE401A58D7772B0525AFBC4DC3D8C85BB550D2D758F99DDC`;
- ESP32-S3 image v1;
- DOUT / 80 MHz / 16 MB;
- six segments;
- checksum `0x98`, valid;
- appended hash
  `35F99732C55EBAEA00AFE9ECD372D0B0C681E871D175290A763A7455943C50F3`,
  valid;
- secure version 0.

`main.c.obj` has exactly three undefined references: `printf`, `puts`, and
`vTaskDelay`. The final ELF contains `spi_flash_init_chip_state()` and
`esp_opiflash_init()` for required Flash-interface initialization, but no
defined OTA mutation, partition/raw-Flash write/erase, NVS, PHY calibration,
filesystem, or core-dump write symbol.

Some mutation names appear in discarded archive sections in the MAP. They are
not final-ELF defined symbols and are not called by `app_main`. This
distinction prevents a false positive from a raw MAP text search.

Classification:

- source and final ELF absence: **CONFIRMED host artifact fact**;
- no intentional runtime persistent-data write: **STRONGLY SUPPORTED**;
- behavior of an unexpected fault or vendor Bootloader before `app_main`:
  separately bounded, not inferred from `app_main`.

## 11. Panic/Core-dump Behavior

Resolved configuration selects print-and-reboot on panic. The candidate does
not include `espcoredump`, no `CONFIG_ESP_COREDUMP_ENABLE` symbol is resolved,
and no core-dump writer is in the final ELF.

- panic persistent core-dump write: **STRONGLY SUPPORTED absent**;
- panic control effect: reboot;
- watchdog control effect: reset or panic depending on the triggered
  watchdog path;
- repeated boot after panic/watchdog: possible and a failure;
- each reboot can repeat Bootloader protection checks and volatile OPI
  initialization.

Any panic, watchdog, allocation failure, or reboot sequence stops the
observation. No automatic reset or rollback follows.

## 12. Flash-register Initialization

Standard v5.5.4 ESP32-S3 startup calls `spi_flash_init_chip_state()`. With
Octal Flash enabled, it calls `esp_opiflash_init()`. The MXIC path:

1. reads status and configuration registers;
2. issues WREN plus WRSR/WRCR to set output-driver strength while preserving
   the read status byte and unrelated configuration bits;
3. issues WREN plus WRCR2 address 0 to select STR OPI;
4. configures controller/cache state.

The reviewed manufacturer evidence classifies the selected ODS and STR-OPI
configuration fields as volatile. These commands do not address the NOR
main array.

The disclosure boundary is:

- volatile ODS/STR-OPI and controller state: expected and accepted only if
  the human explicitly authorizes it;
- non-volatile BP/TB logical values: intended to be preserved by the
  combined App-stage register command;
- physical effect of carrying unchanged BP/TB values through a register
  write: unverified;
- Bootloader BP unlock: separate conditional path that can clear BP bits if
  unexpectedly set.

The future human must explicitly state whether the conditional BP-clear risk
is accepted. If not accepted, startup must remain frozen.

## 13. Startup Reset Mechanism

The last reported state is ROM download mode. Normal App startup requires one
reset with the download strap released.

Reviewed mechanism:

- open only the exact human-confirmed COM endpoint;
- use the integrated USB Serial/JTAG control lines;
- deassert DTR so GPIO0 is released for normal boot;
- assert RTS once to hold reset for 0.2 seconds;
- deassert RTS once to release reset;
- allow 0.2 seconds for the USB peripheral/CPU to leave reset;
- never touch DTR/RTS again during observation.

This is classified `REBOOT`:

1. it restarts the MCU from ROM;
2. it is necessary to leave the held ROM-loader session and attempt normal
   Flash boot;
3. it affects volatile CPU, peripheral, USB, and Flash-register state and may
   invoke the disclosed Bootloader paths;
4. success is judged only by the ordered application lines and 60-second
   stability;
5. uncertainty, port loss, missing output, or any persistent-write sign
   requires stopping.

No copyable reset or observation command is included in this review.

## 14. USB Re-enumeration Risk

A reset may:

- keep the existing COM handle usable;
- temporarily remove the USB endpoint;
- return the same COM name;
- return under another COM name;
- fail to return.

The reviewed harness deliberately does not enumerate or reopen. It opens one
exact handle before reset and continues reading that same handle. Therefore:

- a handle that remains usable may pass;
- a disconnect/read failure stops immediately;
- appearance under another port is not followed;
- even return under the former name does not authorize reopening;
- a future human may need a new review if Windows cannot preserve the handle.

This is stricter than a monitor that waits and reconnects. It trades a higher
chance of an inconclusive result for a provable single-attempt boundary.

## 15. Observation Tool Requirements

Stock ESP-IDF Monitor is not approved because source audit found:

- port enumeration on initial failure;
- alternate-port listing;
- PID enumeration to select reset behavior;
- indefinite reconnect loop after USB disappearance;
- repeated `open_serial()` calls;
- DTR/RTS changes on open and reset;
- optional logging behavior not required here.

Reviewed tool:

- `tools/startup_observation/startup_observer.py`;
- pure-host tests:
  `tests/host/test_startup_observer.py`;
- exact COM-name validation;
- Win32 `GENERIC_READ` handle;
- one `CreateFileW` attempt;
- one DTR/RTS reset;
- zero `WriteFile` or other serial-data output;
- zero port enumeration;
- zero retry or reopen;
- same-handle read only after reset;
- bounded 1 MiB / 1000-line / 512-character-per-line limits;
- exact ordered-line and ready-marker state machine;
- immediate fail-closed behavior.

The source provides no raw-log filename option. Device mode was not invoked
in this review.

Pure-host test result:

- syntax compilation: passed;
- offline audit: device access 0, enumeration 0, opens 0, resets 0, serial
  data writes 0;
- new startup-observer fake tests: 11/11 passed;
- existing single-attempt harness tests: 16/16 passed;
- combined host suite: 27/27 passed.

Two earlier unittest launcher attempts failed before running tests: the first
used a package-style name although `tests/host` is not a Python package; the
second exposed a missing `sys.modules` registration in the test's dynamic
import. The import setup was corrected, then the complete discovery run
passed. Neither failed launcher attempt imported a device backend, enumerated
a port, opened a port, or reset a device.

## 16. Sensitive-output Filtering

Raw serial bytes exist only transiently in process memory for decoding.
Nothing writes them to disk.

Before retention or display, the observer:

- replaces MAC addresses with `[REDACTED_MAC]`;
- replaces lines naming base MAC, MAC address, unique/device/chip ID, device
  serial, key digest/hash, or `BLK_KEY` with
  `[REDACTED_SENSITIVE_LINE]`;
- retains only sanitized lines in the result;
- has no raw transcript or log-file option.

Fatal-condition matching occurs before redaction so a sensitive line cannot
hide a safety failure, but raw content is not retained.

## 17. Exact Future Parameters

These are review fields, not an authorization:

| Parameter | Exact future value |
|---|---|
| exact device | same physical ESP-VoCat unit covered by recovery evidence |
| PCB | V1.0 |
| MCU | ESP32-S3 |
| current port | `<EXACT CURRENT COM PORT — HUMAN RECONFIRMATION REQUIRED>` |
| historical COM7 | context only; not assumed |
| interface | integrated USB Serial/JTAG |
| serial settings | 115200, 8 data bits, no parity, 1 stop bit |
| port enumeration | 0 |
| alternate-port selection | forbidden |
| port-open attempts | 1 total |
| reset attempts | 1 |
| reset control | DTR released; one RTS assert/deassert hard reset |
| observation start | reset release |
| observation duration | 60 seconds |
| ready deadline | 15 seconds from reset release |
| serial data writes | 0 |
| serial input | none |
| retry/reopen | 0 |
| port following | forbidden |
| raw logging | forbidden |
| sensitive filtering | mandatory before display/retention |
| readback/Flash/rollback | none |

Reset and observation are logically separate phases but share the one handle:

1. Phase A performs the single authorized reset.
2. Phase B permanently disables further control-line operations and performs
   read-only observation.

No second open is permitted. If Phase A succeeds but Phase B cannot read the
same handle, the result is stopped/inconclusive.

## 18. Expected Output

The exact App lines, once and in order:

```text
ESP-VoCat PCB V1.0 Smoke Test Candidate
Compile-time ESP-IDF version: 5.5.4
PCB target: ESP-VoCat PCB V1.0
Host-built candidate
Device execution not yet authorized
PSRAM intentionally disabled
No peripheral initialization
Ready marker: PCB_V1_SMOKE_TEST_CANDIDATE_READY
```

The line `Device execution not yet authorized` is immutable candidate build
text describing its host-built provenance; it does not override or create a
future human authorization.

Expected frequency:

- each of the eight exact lines: once;
- ready marker: once;
- after `app_main`: one-second `vTaskDelay` loop;
- periodic application output: none.

Normal additional output may precede the eight lines:

- ROM reset cause, boot mode, load addresses, and entry;
- second-stage Bootloader version/chip revision/Flash/partition/image-load
  information;
- ESP-IDF App startup, heap, scheduler, main-task, and
  `Calling app_main()` information.

Failure output includes invalid image/header/segment, checksum/hash/security
rejection, anti-rollback rejection, no bootable App, waiting-for-download
loop, panic, Guru Meditation, watchdog, allocation failure, or repeated reset
banner/startup sequence.

## 19. Success Criteria

All criteria are mandatory:

1. exactly one startup reset;
2. exactly one open of the exact approved observation endpoint;
3. all eight exact App lines appear once and in order within 15 seconds;
4. ready marker appears exactly once;
5. the observation continues for the complete 60 seconds;
6. no repeated startup;
7. no panic or Guru Meditation;
8. no watchdog failure;
9. no allocation failure;
10. no ROM/Bootloader fatal error;
11. no continuous boot loop or download-mode loop;
12. the original single handle remains usable for the window, including any
    same-port recovery that does not require a reopen;
13. no evidence of an unauthorized persistent write;
14. no enumeration, alternate-port selection, retry, reopen, or serial input;
15. no raw sensitive log is retained.

Success proves only:

- the preserved Bootloader loaded this candidate;
- the candidate reached `app_main`;
- the fixed USB Serial/JTAG output appeared;
- no listed failure appeared in the bounded 60-second window.

It does not prove peripherals, long-term stability, recovery, or product
firmware readiness.

## 20. Stop Conditions

Stop before reset if:

- exact physical-device identity cannot be confirmed;
- PCB V1.0 or same-device recovery scope is uncertain;
- current exact port is not explicitly confirmed;
- historical COM7 is being assumed rather than reconfirmed;
- tool/source/test state differs from this review;
- an automatic enumeration, retry, alternate-port, logging, or input path is
  enabled;
- any required human-risk choice is absent.

Stop immediately after opening/reset if:

- the exact port cannot be opened once;
- reset completion is uncertain;
- USB/handle read fails or the port disappears;
- a changed port is observed or proposed;
- the tool tries to enumerate, retry, reopen, write serial data, or reset
  again;
- the complete ready marker is absent at 15 seconds;
- any expected line is missing, out of order, or repeated;
- panic, watchdog, continuous reboot, ROM/Bootloader error, download mode,
  invalid image, checksum/hash/security rejection, or allocation failure
  appears;
- USB is absent for 15 seconds; the harness normally stops earlier on handle
  failure;
- any undisclosed Flash, NVS, `otadata`, eFuse, protection, or persistent
  write evidence appears;
- bounded byte/line/length limits are exceeded.

On any stop condition:

- close the existing handle;
- do not reset again;
- do not enumerate or open another port;
- do not read Flash;
- do not retry;
- do not roll back automatically;
- preserve only the sanitized result;
- wait for a new human review and authorization.

## 21. Task 3.4 Acceptance Path

Original Task 3.4:

> **[READ-ONLY]** Decide whether one app-only write can be compatible with the
> preserved original bootloader and partition layout; stop rather than
> proposing a bootloader, partition-table, OTA-layout, or unused-tail change.

A later record may support completion only when it demonstrates:

1. the preserved original Bootloader selected `ota_0`;
2. that Bootloader loaded the exact reviewed candidate rather than rejecting
   it;
3. the ordered candidate identity/version/target/safety lines appeared;
4. the ready marker appeared once within 15 seconds;
5. the candidate remained stable for 60 seconds;
6. no compatibility, security, image, reset, panic, watchdog, allocation, or
   persistent-write error appeared;
7. the actual sanitized observation record is reviewed against this plan.

This host-only review supplies none of that runtime evidence. Therefore:

- Task 3.4 is not checked in this round;
- if startup fails, it remains unchecked;
- if observation is not executed, it remains unchecked;
- a tool-open/reset result alone cannot complete it;
- Flash-tool MD5 alone cannot complete it.

## 22. Failure Handling

Failure classification:

- pre-reset mismatch: `NOT EXECUTED / STOPPED`;
- reset uncertain or handle lost: `STOPPED / INCONCLUSIVE`;
- missing/late marker: `FAIL` or `INCONCLUSIVE` according to the exact
  captured evidence, never PASS;
- explicit Bootloader/candidate error, panic, watchdog, allocation failure,
  or reboot loop: `FAIL`;
- successful eight-line output but less than 60 seconds:
  `INCONCLUSIVE`.

Failure never implies authorization to:

- reset again;
- reopen a port;
- collect a readback;
- write or erase Flash;
- roll back;
- extend observation;
- enable a peripheral.

## 23. Rollback Boundary

No rollback is included.

Level 1 remains the separately reviewed complete raw original `ota_0` image:

- size `0x003F0000` / 4128768 bytes;
- range `0x00020000-0x0040FFFF`;
- expected SHA-256
  `C8A2FE4AB0F9B7C823F1DCFDFC926682C9DBF1B079056461DC6F18A93A345D0E`;
- separate target/port/artifact/hash/range/tool/retry review;
- separate exact human authorization;
- separate original-device startup observation.

Level 2 full-image recovery remains higher risk and independently gated.
Neither level is authorized by startup-only approval or failure.

## 24. Human Authorization Requirements

Before any future startup, the user must explicitly confirm all of:

1. exact physical device;
2. exact current port;
3. permission for one startup reset;
4. permission for normal ROM/Bootloader/App startup;
5. acceptance of USB disappearance/re-enumeration/inconclusive-result risk;
6. acceptance of the unaudited vendor-dirty Bootloader residual risk;
7. acceptance of volatile Flash ODS/STR-OPI/controller initialization;
8. an explicit yes/no choice on conditional non-volatile BP-bit clearing if
   protection is unexpectedly set;
9. permission for one read-only open of the exact endpoint;
10. integrated USB Serial/JTAG and 115200/8N1;
11. 60-second observation;
12. 15-second ready-marker deadline;
13. no enumeration;
14. no retry or reopen;
15. no serial input or data write;
16. no unfiltered/raw log retention;
17. no Flash readback;
18. no App/main-array/partition Flash write;
19. no automatic rollback;
20. immediate stop on failure;
21. Level 1 rollback remains separately reviewed and authorized.

Any missing field, ambiguous acceptance, or changed device/port/tool/parameter
invalidates the request.

## 25. Authorization Statement Template

> **UNSIGNED**
>
> **NOT AUTHORIZED**
>
> **DO NOT EXECUTE**

Human-readable template only:

```text
I identify the exact physical device as:
<EXACT ESP-VoCat PCB V1.0 UNIT / SAME-DEVICE RECOVERY SCOPE>

I confirm its exact current observation endpoint as:
<EXACT CURRENT COM PORT>

I authorize exactly one startup reset and one 60-second read-only observation
on that exact endpoint using integrated USB Serial/JTAG at 115200/8N1.

I authorize normal ROM Bootloader, preserved vendor Bootloader, and candidate
App startup behavior. I accept the risk of USB disappearance or
re-enumeration and understand that the observer will not enumerate, retry,
reopen, or follow another port.

I acknowledge the unaudited ESP-IDF v5.5.3-dirty vendor Bootloader residual
risk and the normal volatile ODS/STR-OPI/controller initialization.

Conditional BP-bit clearing choice:
<I ACCEPT / I DO NOT ACCEPT> the risk that the preserved Bootloader may clear
non-volatile Flash BP protection bits if they are unexpectedly set.

I authorize one port-open attempt, one reset attempt, no serial input or data
write, no raw-log retention, a 15-second ready-marker deadline, and immediate
stop on any defined failure.

I do not authorize port enumeration, an alternate port, retry, reopen,
read_flash, any App/partition/main-array Flash write, erase, eFuse/security
change, rollback, restore, recovery, or peripheral test.

I understand that Level 1 rollback remains a separate operation requiring a
new review and exact authorization.

Human name:
<UNSIGNED>

Date/time and timezone:
<UNSIGNED>
```

This template is not a token, signature, JSON file, executor input, command,
or authorization. It is deliberately unsigned.

## 26. Final Readiness Decision

### READY TO REQUEST STARTUP-ONLY AUTHORIZATION

Reasons:

- immediate rollback is not necessary before the bounded candidate startup
  opportunity;
- no startup path intentionally modifies main-array or partition data;
- `VALID` sequence-1 `otadata` is not expected to be rewritten;
- candidate NVS/PHY/filesystem/core-dump/OTA mutation paths are absent;
- volatile Flash-register initialization, conditional BP clearing, and
  vendor-dirty uncertainty are fully disclosed;
- the reviewed harness enforces one open and one reset with no enumeration,
  retry, reopen, serial data write, raw log, or port following;
- exact expected output, success criteria, and stop conditions are defined;
- pure-host tests cover success and fail-closed paths;
- no remaining host-side execution-policy blocker is known.

This decision remains conditional on final OpenSpec/Git validation. It does
not change:

- First Flash: **STOPPED / INCONCLUSIVE**;
- Compatibility: **B — PLAUSIBLE BUT NOT PROVEN**;
- Task 3.4: **NOT COMPLETED**;
- device access authorization: **NONE**;
- startup/observation authorization: **NONE**;
- Flash authorization: **NONE**;
- rollback authorization: **NONE**.

## 27. Claims Explicitly Not Made

This report does not claim:

- that COM7 currently exists or belongs to the device;
- that the device is still physically in the last recorded state;
- that the candidate has booted;
- that USB will remain connected;
- that the original Bootloader accepts the candidate;
- that no vendor-dirty behavior exists;
- that BP bits are currently clear;
- that combined Flash register writes have no physical non-volatile effect;
- that any current Flash byte was read back;
- that padding had no ECC, wear, stress, or disturb effect;
- that Task 3.4 or runtime acceptance is complete;
- that display, touch, PSRAM, audio, motor, network, storage, or product
  behavior works;
- that recovery has been performed or verified;
- that startup, observation, Flash, rollback, or device access is authorized.

## 28. Source References

Repository evidence:

- `PROJECT_CONSTITUTION.md`
- `docs/HARDWARE_PROFILE.md`
- `docs/PRODUCT_SPEC_DRAFT.md`
- `docs/DECISION_LOG.md`
- `docs/SYSTEM_ARCHITECTURE_DRAFT.md`
- `docs/hardware/pcb-v1-post-flash-padding-and-next-action-assessment.md`
- `tests/hardware/pcb-v1-first-flash-attempt-2026-07-26.md`
- `docs/hardware/pcb-v1-first-flash-operation-readiness.md`
- `docs/hardware/pcb-v1-first-flash-review-package.md`
- `docs/hardware/pcb-v1-app-only-compatibility-assessment.md`
- `docs/hardware/pcb-v1-current-boot-chain-readonly-snapshot.md`
- `tests/build/pcb-v1-first-flash-smoke-test-host-build.md`
- `firmware/main/main.c`
- `firmware/sdkconfig`
- `firmware/build/config/sdkconfig.h`
- `firmware/build/project_description.json`
- `firmware/build/pcb_v1_first_flash_smoke_test.elf`
- `firmware/build/pcb_v1_first_flash_smoke_test.map`
- `tools/startup_observation/startup_observer.py`
- `tests/host/test_startup_observer.py`
- OpenSpec proposal, design, delta spec, and tasks for this Change.

Local ESP-IDF v5.5.4:

- `components/bootloader_support/src/bootloader_utility.c`
- `components/bootloader_support/bootloader_flash/src/bootloader_flash_config_esp32s3.c`
- `components/bootloader_support/bootloader_flash/src/bootloader_flash.c`
- `components/efuse/src/esp_efuse_fields.c`
- `components/esp_system/port/cpu_start.c`
- `components/esp_system/startup.c`
- `components/esp_system/panic.c`
- `components/freertos/app_startup.c`
- `components/spi_flash/flash_ops.c`
- `components/spi_flash/esp32s3/spi_flash_oct_flash_init.c`

Installed host tools:

- esptool v4.12.dev3 `cmds.py`, `loader.py`, and `reset.py`;
- ESP-IDF Monitor `base/serial_reader.py`, `base/reset.py`, and logger/config
  paths;
- pySerial Windows backend `serialwin32.py`.

Manufacturer evidence is inherited from the committed post-attempt report's
review of Macronix MX25UM25645G revision 1.1. This round did not promote any
manufacturer-derived evidence to physical-device confirmation.

## 29. Limitations

- This review is entirely host-only.
- Current port and device state were not re-observed.
- The exact vendor Bootloader source and configuration are unavailable.
- ROM internals are not fully source-auditable.
- The candidate was inspected but not rebuilt.
- The production observer's physical behavior is not device-tested; only
  injected fake backends were used.
- A same-handle, no-reopen policy may make USB re-enumeration produce an
  inconclusive result even when the App boots.
- No raw serial output exists to validate the sensitive filter against this
  exact future startup.
- No binary, dump, slice, authorization file, or raw log is added to Git.
- Final readiness depends on the strict validation and Git-scope checks
  recorded after this document is complete.
