# PCB V1.0 esptool Single-Attempt Execution Mechanism

> **HOST-ONLY AUDIT AND TEST EVIDENCE**
>
> **NO DEVICE ACCESS**
>
> **NO FLASH AUTHORIZATION**

- Review date: 2026-07-26
- OpenSpec Change: `prepare-pcb-v1-first-flash-smoke-test`
- Selected mechanism: **B — process-local fail-closed override**
- Flash Authorization Readiness:
  **READY FOR EXPLICIT HUMAN FLASH AUTHORIZATION**
- Compatibility: **B — PLAUSIBLE BUT NOT PROVEN**
- Task 3.4: **NOT COMPLETED**
- First Flash: **NO-GO**
- Device access authorization: **NONE**
- Flash authorization: **NONE**

## 1. Executive Summary

Stock esptool v4.12.dev3 retries a failed unencrypted write operation up to
two whole-operation attempts and retries a failed Flash data block up to three
attempts. The outer count has no CLI, environment, configuration-file, or
`write_flash()` argument that can set it to one. The block count has an
official configuration-file setting, but no direct CLI or function argument.

The reviewed solution is a repository-owned, process-local harness:
`tools/first_flash/esptool_single_attempt.py`. It imports only the exact audited
installation, validates its version, interpreter, canonical package path,
console executable, source-tree digest, relevant source-file hashes,
signatures, constants, and retry-loop markers, and then applies reversible
in-memory overrides. It does not modify site-packages or the esptool executable.

The enforced values are:

- whole write operation: 1;
- data block: 1;
- connection: 1;
- initial port open: 1;
- sync transmission per connection: 1;
- reset-layer reopen/reset sequence: 1.

Sixteen pure-host tests passed. A synthetic outer failure called
`flash_begin()` exactly once; a synthetic block failure called
`check_command()` exactly once; a synthetic sync failure called `sync()`
exactly once. Every test replaced real serial opening and serial-port
enumeration with immediate-failure sentinels, and no real serial call occurred.

This resolves the host-side automatic-retry blocker. It does not authorize a
port, device, write, monitor, rollback, or recovery operation.

## 2. Scope

This review covers:

- the installed esptool v4.12.dev3 CLI and Python call path;
- write-operation, block, port-open, connection, sync, and reset retry loops;
- normal protocol chunking, acknowledgement, and verification behavior;
- a fail-closed host harness;
- pure-host mock/fake tests;
- offline staging manifest, artifact, hash, and geometry validation;
- installed-package integrity before and after runtime overrides.

It does not cover device behavior, physical Flash behavior, boot compatibility,
serial observation, rollback execution, or any peripheral.

## 3. Safety Boundary

All work in this review was computer-only. No command:

- enumerated a COM port;
- opened a serial port;
- connected to, reset, or queried a device;
- ran a device-side esptool operation;
- read, wrote, erased, monitored, restored, or flashed a device;
- ran `idf.py`, configure, or build;
- changed firmware, staging binaries, rollback binaries, recovery backups,
  site-packages, pyserial, ESP-IDF, Python, or the esptool executable.

The only esptool execution modes used were offline audit, self-test, manifest
validation, and artifact validation. The unit tests used fake loaders and
temporary files.

## 4. OpenSpec No-Retry Requirement

The controlling original text is:

- Task 11.1: execute only the exact authorized single-image, single-range
  operation with no monitor, erase, restore, eFuse, bootloader,
  partition-table, **or retry behavior**.
- Task 11.2: preserve the complete result and stop further writes whether the
  command succeeds or fails.
- Task 13.2: on a stop condition, freeze further device work and **do not retry
  or widen the write automatically**.
- Delta-spec `Failure stop condition`: on a defined stop condition, further
  device writes cease and rollback is evaluated; the workflow **MUST NOT retry
  or widen the write automatically**.
- Design `Failure Containment`: **Do not retry Flash automatically**.

The required meaning is not changed:

- a second whole-operation transmission after a serial exception is prohibited;
- a second transmission of the same failed Flash data block is prohibited;
- another invocation after failure requires a new human review and authority;
- sequential transmission of distinct blocks in one image is not retry;
- reading the ROM replies belonging to one command is not retry;
- one post-write ROM MD5 calculation is not retry.

The OpenSpec failure semantics directly control Flash retry. The already
reviewed operation-readiness policy additionally fixes initial port open and
connection attempts to one. This harness also reduces the hard-coded repeated
sync and reset-reopen paths to one so no hidden host reconnect behavior remains.
That additional control is recorded as execution-policy hardening, not as a
claim that normal acknowledgement consumption is prohibited by OpenSpec.

## 5. esptool Version and Source

| Item | Audited value |
|---|---|
| Python invocation | `C:\Espressif\tools\python\v5.5.4\venv\Scripts\python.exe` |
| Canonical Python path | `E:\Espressif\tools\python\v5.5.4\venv\Scripts\python.exe` |
| Python version | 3.13.9 |
| Console executable | `C:\Espressif\tools\python\v5.5.4\venv\Scripts\esptool.exe` |
| Canonical package root | `E:\Espressif\tools\python\v5.5.4\venv\Lib\site-packages\esptool` |
| Distribution metadata | `E:\Espressif\tools\python\v5.5.4\venv\Lib\site-packages\esptool-4.12.dev3.dist-info` |
| Version attribute | `4.12.dev3` |
| Distribution version | `4.12.dev3` |
| Console entry point | `esptool.py = esptool.__init__:_main` |
| Loaded config file | none |

The requested version spelling `v4.12.dev3` corresponds to the package value
`4.12.dev3` and the CLI banner spelling `v4.12.dev3`.

The device-operation call path is:

```text
esptool.exe
  -> esptool.__init__:_main()
  -> esptool.__init__.main(argv)
  -> exact chip-class construction (opens the exact supplied port)
  -> ESPLoader.connect()
  -> ESPLoader._connect_attempt()
  -> reset strategy + ESPLoader.sync()
  -> connection banner / Flash preparation
  -> esptool.cmds.write_flash()
  -> ESPLoader.flash_begin()
  -> sequential ESPLoader.flash_block()
  -> ESPLoader.flash_md5sum()
  -> after=no_reset
```

The harness uses an exact `esp32s3` chip argument and exact authorized port.
It never uses the CLI's auto-port or auto-chip paths.

## 6. Retry Taxonomy

| Retry category | Source file and line | Function/class | Control | Audited default attempts | Publicly configurable | Runtime configurable | Required value | Can be forced to one | Residual risk |
|---|---|---|---|---:|---|---|---:|---|---|
| Whole write operation | `esptool/cmds.py:629` | `write_flash()` | `esp.WRITE_FLASH_ATTEMPTS` | 2 | no | class/instance override only | 1 | yes | unknown source is rejected |
| Uncompressed block | `esptool/loader.py:1022-1041` | `ESPLoader.flash_block()` | module global `WRITE_BLOCK_ATTEMPTS` | 3 | config file only | module override | 1 | yes | no second failed-block send |
| Compressed block | `esptool/loader.py:1314-1332` | `ESPLoader.flash_defl_block()` | same module global | 3 | config file only | module override | 1 | yes | unreachable because compression is prohibited |
| Connection | `esptool/loader.py:755-811` | `ESPLoader.connect()` | `attempts` parameter | 7 | CLI, environment, Python API, config | wrapper and explicit argument | 1 | yes | failure propagates |
| Initial port open loop | `esptool/__init__.py:795-813`, `:1196-1240` | `main()`, `connect_loop()` | `open_port_attempts` | 1 | environment/config | environment and constants | 1 | yes | exact port only |
| Sync transmission | `esptool/loader.py:648-699` | `_connect_attempt()` | hard-coded `range(5)` | 5 | no | method override only | 1 | yes | one sync may still fail |
| Reset reopen | `esptool/reset.py:36-61` | `ResetStrategy.__call__()` | hard-coded `range(3)` | 3 | no | method override only | 1 | yes | first OS/reset error propagates |
| Outer reconnect after serial loss | `esptool/cmds.py:683-703` | `write_flash()` exception path | outer count plus `DEFAULT_CONNECT_ATTEMPTS` | reachable after first of 2 outer attempts | no complete control | process override | unreachable | yes | outer attempt 1 re-raises before reconnect |
| Mismatched response consumption | `esptool/loader.py:452-516` | `ESPLoader.command()` | `range(100)` reads | up to 100 reads | no | not overridden | unchanged | not a resend | reads stale/mismatched replies only |
| Extra sync replies | `esptool/loader.py:591-601` | `ESPLoader.sync()` | 7 `command(op=None)` calls | 7 reply reads | no | not overridden | unchanged | not a resend | consumes replies generated by one sync request |

## 7. Outer Operation Retry

`E:\Espressif\tools\python\v5.5.4\venv\Lib\site-packages\esptool\cmds.py`,
`write_flash()`, begins at line 358. Lines 627-629 save the whole image and
iterate:

```text
for attempt in range(1, esp.WRITE_FLASH_ATTEMPTS + 1)
```

`ESPLoader.WRITE_FLASH_ATTEMPTS` is defined as `2` in
`esptool/loader.py:305`. A `SerialException` on the first attempt closes the
port, tries to reconnect, restores `original_image`, and repeats Flash begin
and data transmission. This is a genuine prohibited retry because failed
operation bytes may have reached or erased Flash before automatic repetition.

No parser option or environment variable controls this class constant.
`cmds.write_flash(esp, args)` has no attempts parameter. The harness verifies
the exact source marker and sets `ESPLoader.WRITE_FLASH_ATTEMPTS = 1` before
device logic. A first `SerialException` therefore satisfies the
`attempt == WRITE_FLASH_ATTEMPTS` branch and is re-raised without reconnect.

## 8. Block-Level Retry

`esptool/loader.py:100` loads:

```text
WRITE_BLOCK_ATTEMPTS = cfg.getint("write_block_attempts", 3)
```

`ESPLoader.flash_block()` at lines 1022-1041 sends the same block within:

```text
range(WRITE_BLOCK_ATTEMPTS - 1, -1, -1)
```

On `FatalError`, the same block is sent again while attempts remain. This is a
genuine prohibited retry. The same global controls the compressed path at
lines 1314-1332, although compression is excluded by the operation packet.

`write_block_attempts` is an official esptool configuration-file field, and
`ESPTOOL_CFGFILE` can select a configuration file. There is no direct
`write_flash` CLI option, environment variable, or `flash_block()` parameter.
The harness rejects any config file or `ESPTOOL_CFGFILE`, then sets the audited
module global to `1` in-process.

## 9. Connect and Sync Behavior

Connection and sync occur before Flash transmission:

- `--connect-attempts` is public; the default is 7 and
  `ESPTOOL_CONNECT_ATTEMPTS` is supported.
- `ESPLoader.connect(attempts=...)` is a public Python parameter.
- initial open attempts default to 1 and can be changed through
  `ESPTOOL_OPEN_PORT_ATTEMPTS` or config.
- one `_connect_attempt()` performs one reset but calls `sync()` up to five
  times after a failed synchronization.
- `ResetStrategy.__call__()` can reopen and repeat the reset sequence up to
  three times on an `OSError`.

The OpenSpec stop condition is specifically about automatic Flash retry and
widening. Connection/sync therefore are not reclassified as block or
whole-image retries. Independently, the reviewed operation policy already
requires exactly one port-open and one connection attempt. The harness
enforces all four connection-layer counts as one to remove hidden automatic
reconnection:

- explicit `--connect-attempts 1`;
- environment and module open-port values fixed to 1;
- a wrapper rejects any `connect(attempts != 1)`;
- `_connect_attempt()` is replaced with the audited behavior containing one
  sync transmission;
- `ResetStrategy.__call__()` performs one open/reset sequence and propagates
  the first error.

Future device mode uses explicit `before=usb_reset` for the reviewed integrated
USB Serial/JTAG transport. This selects the USB reset strategy directly and
avoids `_get_pid()` serial-port enumeration. Both the top-level port-list
helper and pyserial `comports()` entry used by esptool are replaced with
fail-closed denial functions.

## 10. Protocol Operations That Are Not Retries

The following remain normal parts of one authorized operation:

- `flash_begin()` once, including the ROM's up-front sector erase;
- ordered transmission of each distinct 1 KiB no-stub data block;
- one request acknowledgement and status check for each command;
- `command()` reading past stale or wrong-operation replies without resending
  the request;
- `sync()` consuming seven additional replies produced by its one sync request;
- one Flash-ID/size query path already disclosed by the review package;
- one post-write ROM MD5 calculation and comparison;
- one final `after=no_reset` state transition.

Sequence-numbered blocks are different portions of the image, not repeated
attempts of one failed block.

## 11. Public Configuration Options

| Control | CLI | Environment | Config file | Public Python argument | Conclusion |
|---|---|---|---|---|---|
| outer write attempts | none | none | none | none | runtime override required |
| block attempts | none | only indirect `ESPTOOL_CFGFILE` selection | `write_block_attempts` | none | official config exists, but not sufficient for all retries |
| connection attempts | `--connect-attempts` | `ESPTOOL_CONNECT_ATTEMPTS` | `connect_attempts` | `connect(attempts=...)` | official control available |
| initial open attempts | none | `ESPTOOL_OPEN_PORT_ATTEMPTS` | `open_port_attempts` | internal helper parameter | official runtime control available |
| sync transmissions per connection | none | none | none | none | runtime method override required |
| reset reopen attempts | none | none | none | none | runtime method override required |

No public interface can set every relevant count to one, so solution A is not
available.

## 12. Selected Mechanism

**Solution B: process-local fail-closed override.**

The harness is:

`tools/first_flash/esptool_single_attempt.py`

It uses the installed package without editing it, applies only audited
in-memory attribute/method overrides, invokes `esptool.main(argv=...)` in the
same process, restores every original attribute and environment value in a
`finally` block, and rehashes the installation afterward.

Solution C is unnecessary because all host-side retry paths in the reviewed
no-stub, uncompressed operation can be forced to one and tested.

## 13. Runtime Override Design

`single_attempt_overrides()`:

1. verifies the untouched audited defaults;
2. saves every original value and method;
3. sets outer, block, connect, and open counts to 1;
4. wraps `connect()` to reject any value other than 1;
5. replaces `_connect_attempt()` with the same audited reset/boot-log/error
   behavior but one `sync()` call;
6. replaces reset `__call__()` with one open/reset sequence;
7. denies top-level and loader-level port enumeration;
8. verifies all effective values equal 1;
9. yields to the caller;
10. restores all values, methods, enumerators, and environment variables.

The future operation arguments are built only after independent authorization
validation. They hard-select:

- chip `esp32s3`;
- the exact authorized port;
- 115200 initial baud;
- explicit USB reset;
- no post-write reset;
- ROM/no-stub;
- one connection attempt;
- no compression;
- Flash mode/frequency/size `keep`;
- no progress output;
- one exact candidate offset and artifact.

The argument list is never printed or saved.

## 14. Version and Structure Guards

Device logic is refused unless all of the following match:

- Python 3.13.9 and the canonical audited interpreter path;
- esptool distribution and module version `4.12.dev3`;
- exact canonical package and dist-info paths;
- 28 Python source files and 30 tree entries;
- source-tree SHA-256
  `7BA6E754C34E202DEB9EF5C6E24376CDA93F24BD4DDB3B70E392BBA6CD5C7610`;
- exact console executable SHA-256;
- exact hashes of `__init__.py`, `cmds.py`, `config.py`, `loader.py`,
  `reset.py`, and `targets/esp32s3.py`;
- exact function signatures;
- exact outer/block/connect/open default constants;
- exact loop-source markers for outer, block, sync, and reset retry points;
- absence of an esptool config file and `ESPTOOL_CFGFILE`.

An unknown version or changed patch point is rejected; there is no
best-effort compatibility mode.

## 15. Fail-Closed Behavior

Before any future serial open, the harness rejects:

- wrong Python, esptool version, executable, package path, metadata path,
  source count, source digest, source-file hash, signature, default constant,
  or loop marker;
- any esptool config file or config override;
- missing or repository-resident authorization material;
- wrong authorization schema/status/Change/device/PCB/chip/transport;
- absent, wildcard, malformed, or substituted port;
- artifact path, size, hash, offset, length, end, or erase-envelope mismatch;
- staging manifest or rollback mismatch;
- stub, compression, encryption, erase-all, header rewrite, retry, reset, or
  observation-policy mismatch;
- missing human statement or risk acknowledgement;
- a failed sensitive-output-filter self-test;
- any attempt to enumerate serial ports.

A first runtime failure propagates and stops the process. It never causes a
second whole operation or block transmission.

## 16. Authorization Lock

No authorization file was created in this review. Without a future external
JSON file having the exact schema
`esp-vocat-first-flash-authorization-v1` and status
`EXPLICIT HUMAN FLASH AUTHORIZATION`, `--device` refuses before importing
device logic.

Authorization files are forbidden inside the repository. They must include
the exact device, exact port, exact artifact/path/hash/geometry, recovery
evidence, operation parameters, separate observation handoff, verbatim human
statement, risk acknowledgement, no automatic retry, and no rollback
authority.

Offline modes remain available without authorization:

- `--audit`;
- `--self-test`;
- `--validate-manifest`;
- `--validate-artifact`.

## 17. Artifact and Range Guards

Only the candidate at the exact D: or E: staging path is accepted:

- filename: `pcb_v1_first_flash_smoke_test.bin`;
- size: 160832 / `0x00027440`;
- SHA-256:
  `1B72A60DE9C9BB42DE401A58D7772B0525AFBC4DC3D8C85BB550D2D758F99DDC`;
- offset: `0x00020000`;
- end-exclusive: `0x00047440`;
- last byte: `0x0004743F`;
- erase envelope: `0x00020000-0x00047FFF`;
- erase end-exclusive: `0x00048000`;
- erase length: `0x00028000`.

The same staging manifest must validate the complete raw `ota_0` rollback
artifact and immutable full-image hash. Geometry fields are independently
checked for arithmetic consistency and containment.

## 18. Sensitive-Output Filtering

All future esptool stdout and stderr are captured in memory before display.
The filter removes complete lines identifying base MAC, MAC address, unique
chip ID, key digest, credential, password, secret, or access/refresh token and
also replaces colon- or hyphen-formatted MAC values anywhere else.

The filter self-test runs before serial logic. Runtime output is checked again
after filtering; raw output is never written to a log. The harness never
prints its constructed device argument list.

This protects output presentation. It does not claim that the normal esptool
connection path stops reading the ancillary eFuse/OTP/MAC fields already
disclosed in the readiness package.

## 19. Test Design

Test file:

`tests/host/test_esptool_single_attempt.py`

Coverage includes:

1. exact version accepted;
2. other version rejected;
3. exact source structure accepted;
4. missing outer patch point rejected;
5. config override rejected;
6. installed package unchanged;
7. outer failure exactly once;
8. block failure exactly once;
9. sync failure exactly once;
10. audit exactly-once assertion;
11. missing authorization rejected;
12. wrong artifact hash rejected;
13. every geometry-field mismatch rejected;
14. current staging manifest validated offline;
15. sensitive connection output filtered;
16. offline CLI mode cannot enter device logic.

Every test patches `serial.serial_for_url`, `serial.Serial`, and
`serial.tools.list_ports.comports` to raise immediately and asserts that each
was called zero times.

## 20. Test Results

Interpreter:

`C:\Espressif\tools\python\v5.5.4\venv\Scripts\python.exe`, Python 3.13.9.

Final syntax check:

```text
compile(read_text(...), path, "exec") for both Python files
result: 2 files passed
```

Final unit-test command:

```text
python.exe -B tests\host\test_esptool_single_attempt.py -v
```

Final result:

- 16 tests run;
- 16 passed;
- 0 failed;
- 0 skipped;
- 0 warnings;
- outer synthetic failure: 1 `flash_begin()` call;
- block synthetic failure: 1 `check_command()` call;
- sync synthetic failure: 1 `sync()` call;
- real serial open: 0;
- serial-port enumeration: 0.

The fake outer-path test prints esptool's planned erase-range text before its
synthetic first failure. It contains no port and no executable device command;
the fake performs no serial or Flash operation.

Two earlier test-run corrections are retained for honesty:

- `python -B -m unittest -v tests\host\...` failed before collection because
  the Windows file path was interpreted as a module name; zero tests ran.
- an early 15-test run had 14 passes and one correctly blocked fake-loader
  constructor that attempted the default serial path. The fake constructor was
  replaced with an in-memory constructor; no real serial port opened.

Final offline tool modes all exited 0:

```text
python.exe -B tools\first_flash\esptool_single_attempt.py --audit
python.exe -B tools\first_flash\esptool_single_attempt.py --self-test
python.exe -B tools\first_flash\esptool_single_attempt.py --validate-manifest <D staging manifest>
python.exe -B tools\first_flash\esptool_single_attempt.py --validate-artifact <D staged candidate>
```

No `--device`, port, device operation, or Flash operation was passed.

## 21. Installed-Package Integrity

Before and after audit, overrides, self-test, and unit tests:

- source file count: 28;
- tree entry count: 30;
- source-tree SHA-256:
  `7BA6E754C34E202DEB9EF5C6E24376CDA93F24BD4DDB3B70E392BBA6CD5C7610`;
- executable SHA-256:
  `A1F90587C195E8C824FE30D80B2D6062CE244CD346118502F08AFDC4496F8AF4`.

Relevant file hashes:

| File | SHA-256 |
|---|---|
| `esptool/__init__.py` | `805D998B4DE6A2FDE6CF9F922F21B245F67BDC929C364067F4533696810C60B7` |
| `esptool/cmds.py` | `D9786DDA365985A97504153B5A45BEBF55BDF5B8272BF16891CA682B47E2C3AF` |
| `esptool/config.py` | `BFEACFAA457A10866D816385622427987A45B480D255CE1F10A41ABA64F7AB07` |
| `esptool/loader.py` | `6C5F0C4A9D2047ADB1C9164AEE66208018789A0F6531922204F6B280168C80E8` |
| `esptool/reset.py` | `410334DF7CB09CAFC01EFA8D32ACB1903BBC5BA6F03DF3A0B39823C433B0EDEF` |
| `esptool/targets/esp32s3.py` | `2629FDDE0F4EA7D0D56011E83506CF49A161AEC219F1901EF38B119F3C2C421F` |

Installed-package modified: **NO**.

## 22. Remaining Uncontrolled Behavior

The harness cannot and does not claim to disable:

- operating-system USB/serial driver buffering or transport-level packet
  recovery below pyserial;
- USB link-layer retry/error recovery;
- ESP32-S3 ROM implementation details inside one accepted command;
- physical Flash-controller internal program/verify behavior;
- the ROM sending multiple sync responses to one sync request;
- acknowledgement/status response consumption;
- the one post-write ROM MD5 calculation;
- ancillary connection reads and volatile effects already listed in the
  readiness package.

The installed pyserial source contains no application-level serial-open,
read, or write retry loop relevant to this operation. Lower transport behavior
is outside esptool's Python controls and is not equated with an automatically
repeated authorized Flash operation.

The stock XMC startup and Flash-reset preparation paths can perform multiple
distinct commands and may suppress some preparation errors. They are not
retransmission of a failed image/block and were already part of the reviewed
ancillary connection boundary. This review makes no new device-behavior claim
about them.

## 23. Flash Authorization Readiness Decision

Flash Authorization Readiness is:

**READY FOR EXPLICIT HUMAN FLASH AUTHORIZATION**

The host-side decision conditions are satisfied:

1. every automatic retry in the reviewed operation is forced to one;
2. outer failure is tested at one call;
3. block failure is tested at one call;
4. version and source mismatch fail closed;
5. the installed package remains unchanged;
6. missing authorization refuses before serial;
7. artifact/hash/range mismatch refuses before serial;
8. sensitive output is filtered;
9. all final pure-host tests pass;
10. no host execution-parameter blocker remains.

This state means the host mechanism is ready to receive a future independently
reviewed packet. It does not mean that such a packet exists or is authorized.

The following remain unchanged:

- Compatibility: **B — PLAUSIBLE BUT NOT PROVEN**
- Task 3.4: **NOT COMPLETED**
- First Flash: **NO-GO**
- Device access authorization: **NONE**
- Flash authorization: **NONE**

## 24. Claims Explicitly Not Made

- No device was accessed.
- No serial port was enumerated or opened.
- No Flash, erase, monitor, restore, or rollback ran.
- The candidate has not booted on PCB V1.0.
- The preserved bootloader is not proven compatible.
- Physical Flash, PSRAM, GPIO, peripheral, or recovery behavior is not verified.
- Task 3.4 is not complete.
- First Flash is not `GO`.
- No future authorization file exists.
- No port is selected or authorized.
- No lower-level ROM/USB behavior is claimed to be disabled.

## 25. Source References

OpenSpec and project:

- `PROJECT_CONSTITUTION.md`
- `openspec/changes/prepare-pcb-v1-first-flash-smoke-test/tasks.md`
- `openspec/changes/prepare-pcb-v1-first-flash-smoke-test/design.md`
- `openspec/changes/prepare-pcb-v1-first-flash-smoke-test/specs/pcb-v1-first-flash-smoke-test/spec.md`
- `docs/hardware/pcb-v1-first-flash-operation-readiness.md`
- `docs/hardware/pcb-v1-first-flash-review-package.md`
- `tools/first_flash/esptool_single_attempt.py`
- `tests/host/test_esptool_single_attempt.py`

Installed esptool v4.12.dev3:

- `esptool/__init__.py:90`, `main()`: CLI operation dispatch and exact-port
  handling.
- `esptool/__init__.py:195`: public `--connect-attempts`.
- `esptool/__init__.py:328`: `write_flash` subparser; no outer/block-attempt
  option.
- `esptool/__init__.py:795-813`: initial open-attempt environment handling.
- `esptool/__init__.py:1196`, `connect_loop()`: port-open retry helper.
- `esptool/__init__.py:1243`, `get_default_connected_device()`: exact versus
  auto-port connection path.
- `esptool/__init__.py:1380`, `_main()`: console entry.
- `esptool/cmds.py:358`, `write_flash()`: write command.
- `esptool/cmds.py:627-703`: whole-operation save/retry/reconnect path.
- `esptool/cmds.py:745`: one post-write ROM MD5 request.
- `esptool/config.py:19-22`: supported attempt config fields.
- `esptool/config.py:63-94`, `load_config_file()`: config and
  `ESPTOOL_CFGFILE`.
- `esptool/loader.py:98-102`: connect, block, and open defaults.
- `esptool/loader.py:305`: outer write-attempt class constant.
- `esptool/loader.py:452-516`, `command()`: response-read loop without resend.
- `esptool/loader.py:591-601`, `sync()`: one sync request and extra reply
  consumption.
- `esptool/loader.py:648-699`, `_connect_attempt()`: hard-coded five sync
  transmissions.
- `esptool/loader.py:755-811`, `connect()`: public attempts parameter and
  reset-strategy loop.
- `esptool/loader.py:984-1020`, `flash_begin()`: one begin/erase command per
  operation attempt.
- `esptool/loader.py:1022-1041`, `flash_block()`: block retry.
- `esptool/loader.py:1314-1332`, `flash_defl_block()`: compressed block retry.
- `esptool/loader.py:1349`, `flash_md5sum()`: ROM MD5.
- `esptool/reset.py:36-61`, `ResetStrategy.__call__()`: three reset/reopen
  attempts.
- `esptool/targets/esp32s3.py:411-423`: ESP32-S3 stub block size and USB
  handling; no-stub retains the base 1 KiB block size.
