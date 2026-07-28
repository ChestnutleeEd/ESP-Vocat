## 1. Evidence freeze — READ-ONLY

- [x] 1.1 Freeze the official PCB V1.0 guide, schematic, and display-module specification references, including version/date limitations and source conflicts.
- [x] 1.2 Freeze Xiaozhi commit `49ac8a6da399f27a9546d4f73640b7f86c24bac6`, its V1.0 display configuration, component manifest, and vendor-table symbol.
- [x] 1.3 Freeze the current repository hardware profile, firmware evidence audit, product specification, architecture, decision, and relevant test records at baseline HEAD.
- [x] 1.4 Record each software/source fact separately from the `STRONGLY SUPPORTED — REQUIRES FUTURE DEVICE VALIDATION` hardware mapping and `UNVERIFIED` physical outcomes.
- [x] 1.5 Record the touch-evidence conflict (CST816S in reference/profile evidence but absent from the limited original-app token scan) while keeping all touch out of scope.
- [x] 1.6 Record the GPIO45 schematic/DC conflict, GPIO9 power-role conflict, and strict V1.0/V1.2 boundary without silently resolving either from a whole BSP.

## 2. Dependency decision — READ-ONLY

- [x] 2.1 Inventory all seven published `espressif/esp_lcd_st77916` versions and compare the reviewed 1.x and 2.x constraints, API shape, QSPI path, vendor configuration, and source organization.
- [x] 2.2 Confirm from component metadata that v1.0.1 supports ESP-IDF v5.5.4 and that the historical `^1.0.1` range resolves to v1.0.1 within the reviewed registry set.
- [x] 2.3 Lock the future manifest to `espressif/esp_lcd_st77916: "==1.0.1"` and record v2.0.2 plus its `draw_bitmap` error-propagation fix as the rejected major-version alternative.
- [x] 2.4 Define future component integrity evidence: exact version, lock entry, registry source revision, source archive/tree SHA-256, and relevant file hashes.
- [x] 2.5 Compare historical, component-default, current official V1.0, current official V1.2, and module-spec initialization evidence.
- [x] 2.6 Lock the complete 365-entry historical V1.0 table by commit/file/symbol/count/normalized SHA-256 and prohibit the generic component default or silent edits.
- [x] 2.7 Record `RESOLVED FOR HOST IMPLEMENTATION — GPIO44 SHALL remain hard-disabled; non-zero backlight output and visual device execution remain NOT AUTHORIZED pending a separately reviewed successor Change.`, retain the missing-evidence record, and do not invent a percentage, raw duty, frequency, or resolution.

## 3. Safety design — READ-ONLY

- [x] 3.1 Define the exact GPIO allowlist `{18,14,46,13,11,12,3,44}`, `GPIO_NUM_NC` DC, QSPI/SPI2 mapping, 40 MHz ceiling, RGB565/RGB, inversion, orientation, and offsets.
- [x] 3.2 Define the V1.2/unrelated-feature denylist and host scans for GPIO9/47/45, 80 MHz, whole BSP, LCD power, revision detection, touch, LVGL, audio, NVS brightness, and board-peripheral initialization.
- [x] 3.3 Define all eleven initialization states including `BACKLIGHT_POLICY_GATE`, exact policy/READY markers, success/failure transitions, run-once idempotence, reverse cleanup, and stable terminal behavior.
- [x] 3.4 Define a single failure marker, no retry, no automatic reset, no boot loop, no fallback configuration, and no later-state execution after failure.
- [x] 3.5 Define GPIO44 output-low before all QSPI/panel calls and on every normal/error path; prohibit LEDC, PWM parameters, non-zero duty, GPIO44 high, brightness/fade/NVS behavior, and hidden compile-time/runtime enable paths.
- [x] 3.6 Define the one-strip 57,600-byte internal DMA allocation, max transfer size, queue depth 1, transfer-completion wait, half-open windows, and no unsafe buffer reuse.
- [x] 3.7 Define the fixed black/RGB/white-border/`ESP-VoCat LCD TEST` pattern and tiny compile-time glyph subset without LVGL, filesystem, image, or network.
- [x] 3.8 Define host-only acceptance limits, successor separation, First Flash status preservation, and unchanged DEC-023/Level 1/Level 2 recovery boundaries.

## 4. OpenSpec documentation review — READ-ONLY

- [x] 4.1 Cross-review `proposal.md` against design, specification, and tasks for scope, non-goals, success criteria, evidence levels, blockers, and operation boundaries.
- [x] 4.2 Cross-review `design.md` for all 26 required sections, source traceability, decisions, state-machine completeness, memory/DMA analysis, and recovery/successor boundaries.
- [x] 4.3 Verify the capability specification has exactly 42 unique Requirements, valid Scenarios for normal/prohibited/auditable outcomes, and no physical overclaim.
- [x] 4.4 Verify task IDs, checkbox truthfulness, eight required phases, operation classifications, and absence of executable device commands.
- [x] 4.5 Complete the first consistency review, correct every issue found, and rerun applicable structure checks.
- [x] 4.6 Complete the second adversarial review for evidence promotion, V1.2/GPIO contamination, device authority, recovery weakening, floating sources/dependencies, and premature checkboxes; correct every issue found.
- [x] 4.7 Run strict validation for this Change and the full repository plus UTF-8/BOM/newline/whitespace/link/heading/duplicate/scope checks, then record actual results.

## 5. Firmware implementation — REPOSITORY WRITE

- [ ] 5.1 Reconfirm before firmware work that host implementation uses GPIO44 hard-disabled, no LEDC or non-zero path is permitted, READY remains non-visual, and visual device execution remains blocked pending a separately reviewed successor.
- [ ] 5.2 Add exact dependency `espressif/esp_lcd_st77916: "==1.0.1"` without adding a BSP or floating version, then preserve the resolved lock/integrity evidence.
- [ ] 5.3 Add `pcb_v1_display.h/.c` with GPIO44 configured output-low before QSPI/panel work, the exact allowlist, QSPI bus/panel setup, locked vendor table, eleven-state run-once machine with `BACKLIGHT_POLICY_GATE`, markers, timeout, cleanup, and hard-disabled fail-safe behavior.
- [ ] 5.4 Add `pcb_v1_display_test_pattern.h/.c` with the one-strip RGB565 generator, white border, black/RGB regions, and minimal glyphs for `ESP-VoCat LCD TEST`.
- [ ] 5.5 Integrate the state machine minimally in `main.c` so it invokes once, emits the exact hard-disabled policy marker plus READY `visual=UNVERIFIED backlight=DISABLED_NOT_AUTHORIZED` or one FAIL summary, and remains in a stable non-busy loop.
- [ ] 5.6 Add host tests for pattern pixels/byte order/bounds; GPIO44-low-before-QSPI order; every-state fault injection with GPIO44 remaining low; `TEST_PATTERN_DRAW` -> `BACKLIGHT_POLICY_GATE` -> `READY`; exact policy/READY markers; no reachable `BACKLIGHT_LOW_ENABLE`; marker cardinality; idempotence; cleanup; timeout; and buffer-reuse rules.
- [ ] 5.7 Confirm source includes no `ledc_*`, GPIO44-high/non-zero-duty path, brightness/fade/NVS-display service, wrapper/alias bypass, compile-time/runtime backlight-enable gate, unrelated peripheral, product behavior, LVGL, PSRAM, full framebuffer, filesystem, image asset, or executable device command.
- [ ] 5.8 Update the Change and task-specific documentation with the exact files changed, assumptions, unresolved physical facts, and actual host-test results.

## 6. Host build and static validation — READ-ONLY

- [ ] 6.1 Review the implementation and exact dependency before any configure action; stop if the GPIO44 hard-disabled/no-LEDC/no-nonzero invariant or any source-integrity check is unresolved.
- [ ] 6.2 Run host configure as a separate action with the existing ESP-IDF v5.5.4 and preserve its actual output without device access.
- [ ] 6.3 Run host build as a separate action and preserve compile/link results without claiming display success.
- [ ] 6.4 Inspect the resolved component tree and lock evidence for exact v1.0.1 source identity and absence of board BSP/LVGL/touch/audio dependencies.
- [ ] 6.5 Audit all display GPIO literals, macros, APIs, configuration, and symbols against the exact allowlist, and prove GPIO44 output-low configuration precedes every QSPI/panel call on all paths.
- [ ] 6.6 Audit GPIO9/47/45, 80 MHz, V1.2 BSP names, LCD power features, auto-detection, touch/audio/LVGL, and unexplained denylist matches.
- [ ] 6.7 Audit source, call graph, sdkconfig, map, symbols, and component metadata for absence of `ledc_*`, brightness/fade/NVS display settings, GPIO44 high, non-zero duty, wrapper/alias bypasses, enable flags, PSRAM, full framebuffer, double buffer, filesystem, network, motor, SD, and unrelated peripherals.
- [ ] 6.8 Audit the vendor table for fixed source, 365 entries, normalized SHA-256, final 0x21/0x11/120 ms sequence, and no silent modification.
- [ ] 6.9 Audit map/size output for one 57,600-byte internal DMA strip, task/driver overhead, stack margin, and no memory-allocation release blocker.
- [ ] 6.10 Run host unit/fault-injection tests and verify all states, transfers, callback waits, timeouts, GPIO44-low invariants, `BACKLIGHT_POLICY_GATE`, exact policy/READY markers, absence of `BACKLIGHT_LOW_ENABLE`, fail-safe paths, and stable terminal behavior.
- [ ] 6.11 Record that configure/build/static PASS proves only host acceptance; READY means logical completion with `visual=UNVERIFIED` and `backlight=DISABLED_NOT_AUTHORIZED`, not physical display success.

## 7. Artifact review — READ-ONLY

- [ ] 7.1 Record the app binary path, SHA-256, file length, effective image size, target, version, entry point, checksum/hash status, and segment layout.
- [ ] 7.2 Inspect and record header-declared Flash mode, frequency, and size without promoting them to physical-device facts.
- [ ] 7.3 Verify exact partition containment and all preserved regions against the reviewed partition layout.
- [ ] 7.4 Calculate the candidate semantic start/end-exclusive range independently of file length.
- [ ] 7.5 Calculate the future transport start/end range and per-block plan independently of the semantic range.
- [ ] 7.6 Calculate the erase start/end envelope independently of both semantic and transport ranges.
- [ ] 7.7 Analyze ROM no-stub final-block padding and require every transported padding byte/range to be explicitly represented.
- [ ] 7.8 Attach exact dependency/lock/source integrity, initialization-table hash/count, GPIO allowlist, denylist absence, GPIO44 hard-disabled call-order evidence, and no-LEDC/no-nonzero/PSRAM/LVGL/peripheral evidence.
- [ ] 7.9 Reverify immutable recovery asset paths and hashes without modification and keep Level 1/Level 2 scopes distinct.
- [ ] 7.10 Produce a non-executable artifact-review record and manifest containing `backlight=hard-disabled`, `visual=UNVERIFIED`, `not-for-visual-validation`, no LEDC, and no non-zero duty; include no device command and grant no authorization.

## 8. Successor handoff — READ-ONLY

- [ ] 8.1 Reconfirm all implementation, host-build, static, and artifact tasks are complete and that the hard-disabled artifact is not eligible for visual validation before recommending any successor proposal.
- [ ] 8.2 Define the future `execute-pcb-v1-minimal-display-smoke-test` scope without creating it; require separate approval of exact GPIO, polarity, frequency, resolution, raw duty, and percentage before any visual execution proposal.
- [ ] 8.3 Require fresh branch/HEAD, candidate hash, semantic/transport/erase ranges, ROM block plan, and immutable recovery evidence.
- [ ] 8.4 Require fresh exact device identity and current port review; never assume historical COM7 or another prior identity result.
- [ ] 8.5 Require a new exact, single-attempt authorization and keep write and startup/display observation as separate operations.
- [ ] 8.6 Require the successor to rebuild and re-audit the non-zero artifact, then define a bounded screen checklist for illumination, pattern regions/text, color plausibility, orientation, refresh stability, stop conditions, and retained `UNVERIFIED` facts.
- [ ] 8.7 Preserve the historical First Flash result and prohibit reuse of any consumed First Flash/startup/observation/recovery authorization.
- [ ] 8.8 Preserve DEC-023, separate Level 1 original `ota_0` and Level 2 32 MiB recovery, rollback decision, restoration rehearsal, and an independent Recovery verification Change.
