## 1. Preconditions and evidence

- [x] 1.1 **[READ-ONLY]** Re-read the constitution, hardware profile, product specification, decision/architecture records, host-build main spec and test record, and every artifact in this Change; record any contradiction and stop the affected work.
- [x] 1.2 **[READ-ONLY]** Confirm the exact PCB V1.0 scope, current Git branch and clean worktree, unchanged archived host-baseline Change, unchanged synchronized host-build main spec, and unchanged three-file firmware baseline before Apply work.
- [x] 1.3 **[READ-ONLY]** Build an Apply-time evidence ledger that distinguishes prior device records, immutable-file facts, image-header declarations, original-firmware strings, reference-source evidence, current-device state, and electrical facts.
- [x] 1.4 **[READ-ONLY]** Resolve or explicitly retain every First Flash blocker from the two readiness reports; do not begin firmware configuration while a required Flash or PSRAM input remains unsupported.

## 2. Backup and recovery verification

- [x] 2.1 **[READ-ONLY]** Locate the immutable `E:` and `D:` recovery directories by their exact reviewed paths without broad disk scanning, and restore reviewed cross-disk redundancy before First Flash can become `GO`.
- [x] 2.2 **[READ-ONLY]** Verify that each full recovery file is exactly 33554432 bytes and has SHA-256 `72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001`; stop on any missing file or mismatch.
- [x] 2.3 **[READ-ONLY]** Repeat the host-only partition-table, bootloader, and app-layout verification against one hash-matching recovery file and confirm that no private NVS content is decoded, logged, or copied into the repository.
- [x] 2.4 **[READ-ONLY]** Complete and record a host-only recovery rehearsal covering same-device scope, exact recovery image/hash/range review, tool availability, operation separation, stop conditions, and operator judgment points without connecting to or writing a device.

## 3. Board-configuration decision

- [x] 3.1 **[READ-ONLY]** Locate traceable device-test evidence for physical Flash capacity/interface/voltage and PSRAM mode/size, then reconcile it with the original bootloader/app declarations of DIO, 80 MHz, and 16 MB; stop if reconciliation is incomplete.
- [x] 3.2 **[WRITE]** Record a PCB V1.0 configuration allowlist containing only reviewed ESP32-S3, serial, Flash, and optional PSRAM inputs, with every peripheral, GPIO, network, storage-mutation, OTA, and security feature defaulted off.
- [x] 3.3 **[READ-ONLY]** Make and record the PSRAM gate decision; keep PSRAM disabled unless exact mode, size, clock, voltage domain, and ESP-IDF v5.5.4 options are all supported.
- [ ] 3.4 **[READ-ONLY]** Decide whether one app-only write can be compatible with the preserved original bootloader and partition layout; stop rather than proposing a bootloader, partition-table, OTA-layout, or unused-tail change.

  The 2026-07-26 host-only compatibility assessment parsed the historical backup `otadata`: entry 0 is sequence 1 / `VALID` / CRC-valid and historically selects `ota_0`; entry 1 is erased. The exact candidate range `0x00020000-0x0004743F` fits and has no partition overlap, and the app has no factory/OTA/NVS/assets dependency. Compatibility remains classification B, **PLAUSIBLE BUT NOT PROVEN**, because the snapshot is not current state, vendor `v5.5.3-dirty` changes are unavailable, current security/anti-rollback state is unknown, and the exact preserved bootloader has not loaded the v5.5.4 DOUT candidate. No bootloader, table, OTA-layout, or tail change is proposed. Task 3.4 therefore remains incomplete; First Flash remains `NO-GO`.

  A separately authorized 2026-07-26 current snapshot later found the exact bootloader, table, `otadata`, `ota_0` header window, and erased `ota_1` header window byte-identical to the historical backup. Current sequence 1 remains `VALID`/CRC-valid and selects `ota_0`. This closes the listed live-layout uncertainty at snapshot time, but not the vendor-dirty or actual candidate-boot risk. The esptool connection path also performed implicit eFuse/OTP/MAC banner reads outside the literal boundary; values were redacted and not retained, but the deviation prevents promotion to classification A. Task 3.4 remains incomplete.

  The exact blocker remains that vendor `v5.5.3-dirty` Bootloader modifications
  are unknown and the preserved Bootloader has not actually started the
  candidate DOUT app. The current read-only snapshot is not runtime
  compatibility proof. First Flash still requires an independent human review
  and explicit operation-specific authorization.

## 4. Firmware implementation

- [x] 4.1 **[WRITE]** Restate the exact serial-only implementation scope, expected changed files, allowlisted inputs, assumptions, unresolved hardware facts, and acceptance criteria before editing.
- [x] 4.2 **[WRITE]** Modify only the minimal repository firmware inputs needed for an ESP32-S3 startup application and its reviewed board configuration; do not add board drivers, product modules, third-party dependencies, or adjacent refactors.
- [x] 4.3 **[WRITE]** Implement fixed smoke-test identity, compile-time version, PCB target, disabled-feature summary, and ready-marker logs followed by a bounded non-busy idle state.
- [x] 4.4 **[WRITE]** If the PSRAM gate is satisfied, implement it as optional and nonessential to serial startup; otherwise include no PSRAM initialization or compatibility claim.

## 5. Host-only configure/build

- [x] 5.1 **[READ-ONLY]** Activate and verify the existing ESP-IDF v5.5.4 environment process-locally, confirm output paths are Git-ignored, and review configure/build commands for absence of ports, device access, Flash, monitor, erase, restore, and eFuse operations.
- [x] 5.2 **[WRITE]** Run the reviewed `esp32s3` configure operation as a standalone host-only command, write only ignored generated outputs, and stop before build on any error or configuration-allowlist deviation.
- [x] 5.3 **[WRITE]** Run the reviewed host-only build as a separate command, preserve its real output, and make no hardware or Flash-authorization claim from compilation.

## 6. Static safety review

- [x] 6.1 **[READ-ONLY]** Review all tracked firmware/configuration changes and confirm there is no GPIO number/API, PCB V1.2 value, generic-board dependency, driver initialization, NVS mutation, network, OTA, reboot loop, or product behavior.
- [x] 6.2 **[READ-ONLY]** Inspect generated configuration and linked-component metadata against the allowlist, including explicit absence of display, touch, audio, microphone, motor, SD, battery, power-control, Wi-Fi, Bluetooth, OTA, Secure Boot, Flash Encryption, and eFuse mutation paths.
- [x] 6.3 **[READ-ONLY]** Confirm that logs cannot emit MAC addresses, unique chip identifiers, credentials, tokens, NVS values, recovery contents, or other private data.

## 7. Artifact-layout review

- [x] 7.1 **[READ-ONLY]** Inspect the custom app image offline for target, image version, entry point, segments, checksum/hash, effective image size, and header declarations; record declarations without treating them as physical facts.
- [x] 7.2 **[READ-ONLY]** Calculate and record the exact candidate image SHA-256, offset, end address, byte range, partition fit, and compatibility with the preserved bootloader and partition table; stop on any ambiguity or overlap.
- [x] 7.3 **[READ-ONLY]** Confirm that no bootloader, partition table, NVS, OTA metadata, PHY, assets, unused-tail, erase, or unrelated image is included in the candidate first write.
- [x] 7.4 **[WRITE]** Prepare a non-executed operation packet and placeholder command structure containing the exact reviewed inputs while marking it unauthorized; do not connect to a device or perform Flash.

  Evidence for 7.2-7.4 is in `docs/hardware/pcb-v1-app-only-compatibility-assessment.md` and the non-executable, unauthorized field manifest in `docs/hardware/pcb-v1-first-flash-review-package.md`. The package contains no Flash command and grants no write authority.

## 8. Pre-flash human review

- [x] 8.1 **[READ-ONLY]** Present the exact physical device/PCB identity, user-supplied current port, custom image path/hash, offset/range, partition interpretation, recovery paths/hashes, risks, observation window, stop conditions, and rollback strategy for review.
- [x] 8.2 **[READ-ONLY]** Reconfirm that historical COM7 is not assumed and that any change to device, port, image, hash, offset, range, recovery state, or command invalidates the packet.
- [x] 8.3 **[READ-ONLY]** Record a `GO` or `NO-GO` human-review result; retain `NO-GO` while any evidence, redundancy, compatibility, or recovery-rehearsal item is incomplete.

  The complete host-only packet is recorded in
  `docs/hardware/pcb-v1-first-flash-operation-readiness.md` and
  `docs/hardware/pcb-v1-first-flash-review-package.md`. It includes `COM7` as
  `FUTURE RECONFIRMATION REQUIRED`, the staged candidate and rollback hashes,
  image and erase geometry, partition containment, ancillary esptool reads,
  fixed observation/stop criteria, and two-level recovery. The recorded result
  is `NO-GO`. At the initial operation-readiness review, Flash Authorization
  Readiness was `NOT READY FOR FLASH AUTHORIZATION` because stock esptool
  v4.12.dev3 retained automatic retry behavior that conflicted with the
  no-retry boundary.

  The 2026-07-26 host-only single-attempt review resolved that execution
  blocker with the exact-version, exact-source process-local harness in
  `tools/first_flash/esptool_single_attempt.py`. Sixteen pure-host tests in
  `tests/host/test_esptool_single_attempt.py` passed; synthetic outer, block,
  and sync failures each produced exactly one call, no real serial open or port
  enumeration occurred, and the installed esptool remained unchanged. The
  controlling audit is
  `docs/hardware/pcb-v1-esptool-single-attempt-execution-mechanism.md`.
  Flash Authorization Readiness is now
  `READY FOR EXPLICIT HUMAN FLASH AUTHORIZATION`. This does not change the
  recorded `NO-GO` result or create device/Flash authorization.

## 9. Explicit user authorization

- [x] 9.1 **[READ-ONLY]** Obtain an explicit user statement authorizing the one exact First Flash packet, including device, port, image, hash, offset, range, recovery status, and acknowledged risks; reject vague continuation language.
- [x] 9.2 **[READ-ONLY]** Record the authorization verbatim with its exact scope and confirm it authorizes neither erase, eFuse, security changes, bootloader/table changes, unrelated images, automatic retry, nor rollback.

  On 2026-07-26 the user supplied an explicit, operation-specific statement
  covering exact ESP-VoCat PCB V1.0, `COM7`, candidate path/size/hash,
  `0x00020000` start, `0x00047440` candidate end-exclusive, `0x00048000`
  erase end-exclusive, all preserved regions, ancillary reads, volatile
  effects, one-attempt controls, MD5, observation, stop conditions,
  compatibility class B, and recovery separation. The verbatim final
  authorizing clause and exact structured scope are recorded in
  `tests/hardware/pcb-v1-first-flash-attempt-2026-07-26.md`. The one-time
  external executor authorization JSON was deleted after use; device and
  Flash authorization are consumed and closed, `NONE`, and not reusable.

## 10. Device connection

- [ ] 10.1 **[DEVICE READ]** After exact authorization, connect only to the authorized port as a separate operation and stop without writing if the port cannot be opened exactly as reviewed.
- [ ] 10.2 **[DEVICE READ]** Confirm the authorized ESP32-S3 / ESP-VoCat PCB V1.0 identity using the smallest reviewed read-only handshake or existing serial identity evidence; collect no unrelated unique identifiers.
- [ ] 10.3 **[DEVICE READ]** Compare the observed identity and connection state with the authorization packet; any difference invalidates authorization and returns the workflow to pre-Flash review.

  The current boot-chain snapshot authorization was not a First Flash packet, so Tasks 10.1-10.3 remain unchecked. Its five explicit Flash ranges succeeded on user-supplied `COM7`, but esptool automatically read eFuse/OTP-derived description and base-MAC registers for its connection banner. No MAC value was emitted into the repository, no persistent write occurred, and device work stopped after the final hard reset. See `docs/hardware/pcb-v1-current-boot-chain-readonly-snapshot.md`.

## 11. First Flash

- [ ] 11.1 **[DEVICE WRITE]** Execute only the exact authorized single-image, single-range Flash operation as a standalone command with no monitor, erase, restore, eFuse, bootloader, partition-table, or retry behavior.
- [x] 11.2 **[DEVICE WRITE]** Preserve the complete actual result, then stop further writes whether the command succeeds or fails; success means only that the exact write command reported success.

  One harness invocation connected to exact `COM7`, confirmed ESP32-S3 QFN56
  revision v0.2 over USB Serial/JTAG, erased the reviewed sector envelope,
  reported write success and candidate-range MD5 success, and stayed in the
  ROM loader. No automatic retry occurred. Task 11.1 remains incomplete
  because esptool reported 161792 transmitted bytes rather than the
  authorized 160832 bytes: its 1024-byte ROM block path padded the final
  64-byte candidate block with 960 `0xFF` bytes and sent them over
  `0x00047440-0x000477FF`. This was inside the erase envelope but outside the
  candidate byte range. The exact result and source analysis are preserved in
  `tests/hardware/pcb-v1-first-flash-attempt-2026-07-26.md`. All further device
  writes stopped.

## 12. Serial observation

- [ ] 12.1 **[DEVICE READ]** Open serial observation as a separate operation at the exact reviewed settings and capture only the bounded startup window without issuing device-control commands.
- [ ] 12.2 **[DEVICE READ]** Check for the expected smoke-test identity, version, target, disabled-feature summary, optional PSRAM result, and ready marker, while recording silence, unexpected output, resets, panics, watchdogs, or memory-allocation failures.
- [ ] 12.3 **[DEVICE READ]** Continue for the full authorized observation duration and then close the serial connection; do not extend into display, touch, audio, motor, network, or other peripheral tests.

  The separately authorized startup reset and 60-second observation were not
  performed. The 960-byte final-block padding triggered the user's
  possible-unauthorized-range stop condition immediately after the write
  result. The last known state is ROM loader.

## 13. Pass/fail decision

- [x] 13.1 **[READ-ONLY]** Apply the serial-only acceptance criteria to the actual Flash and observation records and classify the result as pass, fail, or inconclusive without inferring untested hardware behavior.
- [x] 13.2 **[READ-ONLY]** On any defined stop condition, freeze further device work, preserve evidence, and decide whether rollback should be proposed; do not retry or widen the write automatically.

  Result: **STOPPED / INCONCLUSIVE**. No serial acceptance criterion was
  evaluated because observation did not start. Device work is frozen. No
  automatic rollback is proposed from this result; any Level 1 proposal
  requires a new independent review and explicit authorization. Level 2
  remains higher-risk and separately gated.

## 14. Rollback if required

- [x] 14.1 **[READ-ONLY]** Prepare a separate exact same-device rollback packet containing the Level 1 complete raw original `ota_0` image path/size/hash/range, future-reconfirmed port, preserved regions, risks, and observation plan; keep Level 2 full-image recovery higher-risk and separately reviewed, and do not treat First Flash authorization as rollback authorization.
- [ ] 14.2 **[DEVICE WRITE]** Only after a new explicit user authorization, restore the exact reviewed Level 1 complete raw original `ota_0` image as one separate operation without a preceding erase, automatic retry, or unrelated command; do not escalate to the 32 MiB Level 2 image without another independent review and authorization.
- [ ] 14.3 **[DEVICE READ]** Observe and record original-device boot and required original-function recovery separately; stop the project hardware sequence if recovery cannot be verified.

  Host-only Level 1 evidence is staged at the isolated D:/E: package roots.
  The complete `ota_0` artifact is 4128768 bytes with SHA-256
  `C8A2FE4AB0F9B7C823F1DCFDFC926682C9DBF1B079056461DC6F18A93A345D0E`;
  its proposed range is `0x00020000-0x0040FFFF`. Level 2 remains
  `NOT AUTHORIZED / HIGHER RISK / SEPARATE REVIEW REQUIRED`. No rollback or
  observation was executed.

## 15. Final evidence recording

- [x] 15.1 **[WRITE]** Update the hardware-evidence audit, recovery/readiness report, and a task-specific test record with actual commands, outputs, hashes, observations, privacy handling, failures, and remaining uncertainty.
- [x] 15.2 **[READ-ONLY]** Distinguish implemented, compiled, host-tested, device-written, serial-observed, restored, verified, failed, and not tested states; explicitly retain all untested peripherals as `UNVERIFIED`.
- [x] 15.3 **[READ-ONLY]** Run OpenSpec strict validation, Git diff checks, generated-output ignore checks, and a scope audit confirming no recovery binary, dump, slice, NVS data, sdkconfig output, device identifier, or unrelated file is staged.
- [x] 15.4 **[WRITE]** Submit only the reviewed source, configuration, documentation, and test-record changes after user-requested Git authorization, without archiving this Change until all applicable tasks and recovery evidence are complete.

  The final 2026-07-26 pure-host review is recorded in
  `docs/hardware/pcb-v1-first-flash-human-authorization-review.md`. It
  reverified the unchanged candidate and rollback copies, full-backup
  provenance, manifests, geometry, single-attempt controls, 16 host tests,
  installed esptool integrity, ancillary reads, volatile effects,
  observation/stop criteria, and recovery boundaries. Its decision is
  `READY TO REQUEST EXPLICIT USER FLASH AUTHORIZATION`; the record remains
  `UNSIGNED / NOT AUTHORIZED / DO NOT EXECUTE`. This is additional evidence
  for already completed host-review tasks and completes no new Task, Task 3.4,
  explicit authorization, or device task.

## Post-attempt host-only assessment note (2026-07-27)

The later assessment in
`docs/hardware/pcb-v1-post-flash-padding-and-next-action-assessment.md`
reverified the historical affected bytes and established that both the
960-byte padding-corresponding range and 2048-byte erase-only tail contained
valid non-`0xFF` historical Xiaozhi App bytes. The preceding sector erase
already intended to remove those bytes. esptool sent the final 960 bytes as
`0xFF`; this requests no additional logical main-array `1`-to-`0` data-bit
transition, while physical readback, ECC/internal state, wear, disturb, and
exact ROM handling remain unverified.

The candidate image boundary ends exactly before the padding. The standard
non-Secure-Boot simple-hash path ignores the tail; a Secure Boot v2
enforcement path can hash the `0xFF` sector alignment and then expect a
signature block, with preserved vendor configuration still unverified. Host
component/symbol review found no intended OTA/NVS/partition/core-dump write
path. Normal startup includes volatile MXIC Flash configuration-register
initialization plus a conditional second-stage BP-unlock path, and retains
residual vendor-dirty bootloader uncertainty. Prior normal boots strongly
support that the non-volatile BP state is already clear, but it was not
reread. The next review recommendation is **STARTUP-ONLY AUTHORIZATION
REVIEW**, not immediate rollback. This note creates no authorization and completes no
checkbox. Task 3.4, Tasks 10.1-10.3, 11.1, 12.1-12.3, 14.2, and 14.3 remain
unchecked. Progress remains 38/48.
