## 1. Evidence and planning baseline

- [x] 1.1 Verify branch, clean baseline commit `a0b7c726d90448af0f811b7f6c996897e2a16c8a`, synchronized upstream, completed predecessor status, canonical hard-disabled hash, and physical status `UNVERIFIED`.
- [x] 1.2 Review repository schematic/module/source/ESP-IDF evidence and record GPIO44 as a strongly supported active-high AO3400A gate with 10 kΩ pull-down, not a direct LED-current output or confirmed physical fact.
- [x] 1.3 Compare the 25 kHz and 2 kHz V1.0 software precedents and document the controlled engineering selection of low-speed LEDC, APB clock, 2 kHz, 10 bits, timer/channel 0, raw duty 10/1023 (approximately 0.98%), h-point 0, no inversion, and no fade.
- [x] 1.4 Rehash all four immutable 32 MiB full images and both Level 1 complete original `ota_0` staging copies read-only; record the matching sizes/hashes without modifying recovery assets.
- [x] 1.5 Record that GPIO44 is default U0RXD but not identified as an ESP32-S3 strapping pin in reviewed local ESP-IDF evidence, retain USB Serial/JTAG console ownership, and keep the reset-time transient `UNVERIFIED`.
- [x] 1.6 Record the missing authoritative manual BOOT/RESET download-mode procedure as a future device-gate blocker, not a blocker to host implementation.
- [x] 1.7 Complete OpenSpec strict validation, full-repository validation, formatting/diff/scope review, and this planning task's final no-device-operation record.

## 2. Host-side firmware implementation

- [x] 2.1 Restate the exact implementation scope, affected files, assumptions, unresolved physical facts, acceptance criteria, and prohibition on device access before editing firmware.
- [x] 2.2 Add only the required ESP-IDF LEDC component dependency while preserving ESP-IDF v5.5.4, ST77916 `==1.0.1`, reproducible-build configuration, partition configuration, and all security prohibitions.
- [x] 2.3 Extend the public display state enum to the exact 12-state successor graph with `BACKLIGHT_PWM_PREPARE` and `BACKLIGHT_LOW_ENABLE`, removing the predecessor policy gate only from the new candidate path.
- [x] 2.4 Preserve the existing GPIO44 preload-low, output/pull-down configuration, reassert-low, and read-low sequence before every QSPI, panel, and LEDC call.
- [x] 2.5 Implement fixed LEDC timer 0 in low-speed mode using APB clock, 2,000 Hz, 10-bit resolution, with checked single-attempt configuration.
- [x] 2.6 Implement fixed LEDC channel 0 on GPIO44 with active-high/no inversion, initial duty 0, h-point 0, and checked zero-duty verification after all pattern transfers complete.
- [x] 2.7 Implement exactly one checked non-zero update to raw duty 10 and one enabled-duty verification; provide no other non-zero constant, setter, fade, ramp, retry, NVS path, or runtime input.
- [x] 2.8 Implement fail-safe shutdown ordering: duty-zero update where applicable, `ledc_stop` with idle level 0, GPIO latch preload low, GPIO output/pull-down restore, reassert/read low, then existing reverse display cleanup.
- [x] 2.9 Preserve first-error priority, one failure marker, cleanup continuation, no reset/retry/fallback, run-once idempotence, and stable non-busy READY/FAIL_SAFE behavior.
- [x] 2.10 Replace hard-disabled terminal markers with the exact successor configuration, enable, READY, and `visual=UNVERIFIED` markers without adding private/device identifiers.
- [x] 2.11 Confirm no unintended change to the pattern generator, vendor table, geometry, QSPI mapping, buffer model, main product behavior, partition layout, bootloader, or unrelated peripheral code.

## 3. Host tests, fakes, and static safety

- [x] 3.1 Add the smallest fake LEDC types and APIs needed to compile the real implementation on the host; do not invent behavior outside ESP-IDF v5.5.4 signatures.
- [x] 3.2 Rebaseline the native low-level call-point inventory and classify every successor point as injectable or success/order-only with no uncovered real call boundary.
- [x] 3.3 Add success-path assertions for exact timer/channel/clock/frequency/resolution/GPIO/polarity/h-point, initial duty zero, one raw-10 update, reported-duty checks, and exact marker order/cardinality.
- [x] 3.4 Add matrix cases for every failable timer, channel, duty verification, enable update, cleanup-zero, stop, and GPIO-restoration boundary, including failures before and after non-zero enable.
- [x] 3.5 Prove every failure prohibits READY/later states, preserves the originating diagnosis, attempts the complete shutdown/cleanup suffix, and leaves the modeled backlight off without retry.
- [x] 3.6 Prove no LEDC call occurs before `TEST_PATTERN_DRAW` completes and no non-zero call occurs before successful zero-duty preparation.
- [x] 3.7 Add static guards rejecting every duty except 0/10, another frequency/resolution/timer/channel/clock, output inversion, high-speed mode, fade, dynamic brightness, NVS, wrapper/alias, UART0 ownership, direct GPIO44 high, or unrelated GPIO/peripheral path.
- [x] 3.8 Rerun and preserve every predecessor pattern/table/QSPI/DMA/callback/timeout/cleanup/denylist test alongside successor tests.
- [x] 3.9 Run the complete native harness, unittest, and pytest collections under the exact approved interpreter and record actual counts/results without hiding environment failures.

## 4. Clean configure, build, and generated-state audit

- [x] 4.1 Review source and tests before configure; stop if the exact PWM contract, GPIO44-low ordering, cleanup behavior, or unrelated-feature boundary is unresolved.
- [x] 4.2 Configure Clean Build A in a new ignored directory with a new generated sdkconfig and ESP-IDF v5.5.4; perform no device action.
- [x] 4.3 Build Clean Build A separately and preserve actual compile/link/partition-size output without claiming hardware success.
- [x] 4.4 Configure Clean Build B in another new ignored directory and generated sdkconfig with no reuse of Build A cache or outputs.
- [x] 4.5 Build Clean Build B with ccache disabled and preserve actual results.
- [x] 4.6 Verify both resolved configurations retain reproducible build, USB Serial/JTAG, security-off constraints, partition offset, and no PSRAM/LVGL/touch/audio/network/motor/SD/NVS brightness/UART0 ownership.
- [x] 4.7 Audit component tree, map, symbols, strings, and size output for exactly the required display plus LEDC surface and sufficient internal-memory/stack margin.
- [x] 4.8 Verify the two App BINs are byte-identical and record BIN/ELF/MAP/bootloader/partition/sdkconfig sizes and SHA-256 values plus source/toolchain identity.

## 5. Visual-candidate artifact gate

- [x] 5.1 Create a new successor artifact manifest/review rather than editing or relabeling the predecessor evidence.
- [x] 5.2 Record the new canonical artifact path/name, size, SHA-256, app descriptor, entry point, header, segments, checksum, validation hash, source digest, toolchain, and clean-build identities.
- [x] 5.3 Record `visual-validation-candidate`, GPIO44 active high, AO3400A gate role, 2 kHz, 10 bits, raw 10/1023, 0.98%, no inversion/fade/dynamic path, and fail-safe GPIO low.
- [x] 5.4 Prove the new hash differs from the predecessor hard-disabled hash and that the predecessor remains `not-for-visual-validation`.
- [x] 5.5 Independently calculate candidate semantic range, ROM no-stub transport blocks/final `0xFF` padding, and sector erase envelope; verify preserved `ota_0` containment and list every untouched region.
- [x] 5.6 Reverify exact dependency, lock/source hashes, 184-command table/365-token audit, GPIO allowlist/denylist, pattern/buffer contract, and unrelated-feature absence.
- [x] 5.7 Rehash all recovery assets again immediately before future device-gate review and stop on any path/size/hash/same-device discrepancy.
- [x] 5.8 Complete a final host diff/status/generated-file audit and record `READY FOR DEVICE-GATE REVIEW` only if every host/artifact task passes; do not claim physical validation.

## 6. Manual recovery-entry and operation planning gates

- [x] 6.1 Locate authoritative PCB V1.0 documentation for the exact manual BOOT/RESET sequence into ROM download mode, including button order/timing, expected USB enumeration, success judgment, and stop conditions.
- [x] 6.2 Cross-check the manual procedure against GPIO0 BOOT evidence, USB Serial/JTAG, enabled download mode, and prohibited eFuse/security/voltage changes; keep the physical write gate closed if any step remains uncertain.
- [x] 6.3 Prepare a non-executable device-gate packet binding current branch/HEAD, exact visual artifact/hash/geometry, expected chip/PCB/interface, recovery hashes, same-device scope, tool/mode, and one-attempt behavior.
- [x] 6.4 Review the single-attempt mechanism against the new candidate's exact transport and erase geometry, including all ROM padding; prohibit automatic retry, stub substitution, compression drift, alternate-port selection, and implicit reset/monitor.
- [x] 6.5 Prepare a non-executable exact write-command review and authorization checklist; do not include or execute a copyable device command before the fresh gate is reviewed.
- [x] 6.6 Define a separate bounded startup/display observation packet with exact expected markers, duration, visual checklist, privacy handling, and stop conditions; do not combine it with the write.

## 7. Future device identity and Flash gate — NOT AUTHORIZED

- [x] 7.1 `READ-ONLY` — After separate explicit authorization, confirm the exact current endpoint and minimum ESP32-S3 / PCB V1.0 / USB Serial-JTAG identity needed for the packet; retain no private identifier and do not assume COM7.
- [x] 7.2 `READ-ONLY` — Compare the sanitized fresh identity/endpoint result with the reviewed packet and stop on any mismatch; perform no Flash, readback, reset beyond reviewed connection effects, or extra query.
- [x] 7.3 `WRITE` — Present the exact candidate, hash, port, chip, PCB revision, offset, semantic/transport/erase ranges, padding, partition layout, recovery evidence, risks, stop rules, and command for fresh user authorization; an unchecked task or Change readiness grants no authority. Completed as a reviewed, non-executed packet in `docs/hardware/pcb-v1-display-first-write-review-packet.md`; device execution remains `NOT AUTHORIZED`.
- [x] 7.4 `WRITE` — Only after the exact authorization in 7.3, execute at most one exact App-only attempt with all enforced attempt counts equal to one, preserve sanitized actual output/geometry, and stop immediately on the result.
  Completed 2026-09-20 as exactly one guarded invocation on `COM7`: the
  reviewed erase envelope `[0x00020000,0x00059000)` was used, 231424 transport
  bytes were written at `0x00020000`, ROM plaintext MD5 verification passed,
  and `--after no_reset` left the device staying in the ROM bootloader. See
  `tests/hardware/pcb-v1-display-first-write-attempt-2026-09-20.md`.
- [x] 7.5 Record that no `ERASE` or `IRREVERSIBLE` operation, independent erase, eFuse/security/voltage change, automatic retry, monitor, readback, rollback, or restore occurred.
  The write's bounded sector erase was the only erase effect. Additional write
  attempts, independent erase/`erase_flash`, eFuse/security/voltage changes,
  application startup, serial monitor, visual observation, readback, rollback,
  and restore were all zero. The Flash authorization is consumed and cannot
  authorize any startup or observation task.

## 8. Future bounded startup and visual observation — SEPARATE AUTHORIZATION

- [x] 8.1 Review the completed write result and actual geometry independently; do not proceed if it is failed, inconclusive, range-deviating, or mismatched.
  The committed 2026-09-20 write evidence was rechecked at exact synchronized
  HEAD `491d750f48e1540e765abbe88fe332c5edb91dd6`: one write passed with the
  reviewed semantic/transport/erase geometry and ROM hash verification; the
  candidate hash remained unchanged and the write authorization remained
  consumed and guard-rejected.
- [x] 8.2 `REBOOT` — Obtain separate explicit authorization for exactly one startup reset on the freshly reviewed endpoint; do not reuse the write authorization.
  A new user authorization explicitly allowed exactly one controlled startup
  reset of the already-written display candidate on `COM7` and prohibited a
  second reset, Flash mutation, recovery, or reuse of the write authorization.
- [x] 8.3 `READ-ONLY` — Obtain separate explicit authorization for one bounded serial/display observation using the same reviewed handle, with zero serial writes, no enumeration/reopen/retry, and no automatic follow-up.
  The same new authorization allowed one bounded serial observation after the
  startup. The reviewed Win32 backend opened exact `COM7` once for read access,
  issued zero serial writes, used no enumeration/reopen/retry, and closed on the
  15-second READY deadline failure.
- [ ] 8.4 Execute the separately authorized reset and observation only once; verify ordered single markers, no reset loop/panic/watchdog/allocation/security failure, and stable handle for the full bound.
  Disposition: **EXECUTED ONCE / STARTUP EVIDENCE FAILED** — one reset and one
  observation session occurred, but no serial line or READY marker was captured
  before the 15-second deadline. The required marker/stability verification was
  not achieved, so this task remains unchecked. No retry is permitted.
- [ ] 8.5 Observe and record whether there is an early/full-brightness flash, whether the backlight stays at a visibly low stable level, and whether flicker, pulsing, odor, heat, noise, or another abnormal symptom occurs.
- [ ] 8.6 Observe and record the fixed black/RGB/white-border/`ESP-VoCat LCD TEST` pattern, plausible colors/orientation, and refresh stability without claiming measured electrical values.
- [x] 8.7 Stop without retry, duty increase, second reset, second open, or automatic rollback on any defined stop condition or inconclusive visual result.
  Observation stopped at the first READY-deadline failure. Second reset/open,
  duty change, Flash action, rollback, restore, and automatic corrective action
  were all zero.
- [x] 8.8 Preserve actual evidence and classify only directly observed bounded facts as confirmed; retain every unobserved hardware fact as `UNVERIFIED`.
  The sanitized host log and exact failure are recorded in
  `tests/hardware/pcb-v1-display-startup-observation-2026-09-20.md`. Only the
  one-open/one-reset/zero-write observation behavior and absence of captured
  serial lines are established; application startup, display/LEDC execution,
  reset reason, and all physical visual behavior remain `UNVERIFIED` pending
  the user's current-state observation.

## 9. Recovery boundary and closeout

- [ ] 9.1 If display validation passes, record that DEC-023 still requires the independent restoration rehearsal and original-function verification before formal custom-cat firmware work.
- [ ] 9.2 If rollback appears necessary, stop this Change with no automatic recovery and hand off to a separately reviewed Recovery verification Change.
- [ ] 9.3 Preserve Level 1 complete original `ota_0` and Level 2 same-device 32 MiB recovery as distinct `WRITE` operations requiring their own exact review and authorization.
- [x] 9.4 Update relevant hardware/test/decision records with actual results, affected files, evidence levels, remaining risks, and authorization closure without rewriting the historical First Flash result.
- [x] 9.5 Run final OpenSpec, documentation, Git scope, privacy, generated-file, and no-device-overclaim audits before any completion/archive review.
