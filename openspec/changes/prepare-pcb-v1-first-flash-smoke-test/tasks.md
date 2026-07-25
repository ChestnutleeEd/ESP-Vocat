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

- [ ] 3.1 **[READ-ONLY]** Locate traceable device-test evidence for physical Flash capacity/interface/voltage and PSRAM mode/size, then reconcile it with the original bootloader/app declarations of DIO, 80 MHz, and 16 MB; stop if reconciliation is incomplete.
- [x] 3.2 **[WRITE]** Record a PCB V1.0 configuration allowlist containing only reviewed ESP32-S3, serial, Flash, and optional PSRAM inputs, with every peripheral, GPIO, network, storage-mutation, OTA, and security feature defaulted off.
- [x] 3.3 **[READ-ONLY]** Make and record the PSRAM gate decision; keep PSRAM disabled unless exact mode, size, clock, voltage domain, and ESP-IDF v5.5.4 options are all supported.
- [ ] 3.4 **[READ-ONLY]** Decide whether one app-only write can be compatible with the preserved original bootloader and partition layout; stop rather than proposing a bootloader, partition-table, OTA-layout, or unused-tail change.

## 4. Firmware implementation

- [ ] 4.1 **[WRITE]** Restate the exact serial-only implementation scope, expected changed files, allowlisted inputs, assumptions, unresolved hardware facts, and acceptance criteria before editing.
- [ ] 4.2 **[WRITE]** Modify only the minimal repository firmware inputs needed for an ESP32-S3 startup application and its reviewed board configuration; do not add board drivers, product modules, third-party dependencies, or adjacent refactors.
- [ ] 4.3 **[WRITE]** Implement fixed smoke-test identity, compile-time version, PCB target, disabled-feature summary, and ready-marker logs followed by a bounded non-busy idle state.
- [ ] 4.4 **[WRITE]** If the PSRAM gate is satisfied, implement it as optional and nonessential to serial startup; otherwise include no PSRAM initialization or compatibility claim.

## 5. Host-only configure/build

- [ ] 5.1 **[READ-ONLY]** Activate and verify the existing ESP-IDF v5.5.4 environment process-locally, confirm output paths are Git-ignored, and review configure/build commands for absence of ports, device access, Flash, monitor, erase, restore, and eFuse operations.
- [ ] 5.2 **[WRITE]** Run the reviewed `esp32s3` configure operation as a standalone host-only command, write only ignored generated outputs, and stop before build on any error or configuration-allowlist deviation.
- [ ] 5.3 **[WRITE]** Run the reviewed host-only build as a separate command, preserve its real output, and make no hardware or Flash-authorization claim from compilation.

## 6. Static safety review

- [ ] 6.1 **[READ-ONLY]** Review all tracked firmware/configuration changes and confirm there is no GPIO number/API, PCB V1.2 value, generic-board dependency, driver initialization, NVS mutation, network, OTA, reboot loop, or product behavior.
- [ ] 6.2 **[READ-ONLY]** Inspect generated configuration and linked-component metadata against the allowlist, including explicit absence of display, touch, audio, microphone, motor, SD, battery, power-control, Wi-Fi, Bluetooth, OTA, Secure Boot, Flash Encryption, and eFuse mutation paths.
- [ ] 6.3 **[READ-ONLY]** Confirm that logs cannot emit MAC addresses, unique chip identifiers, credentials, tokens, NVS values, recovery contents, or other private data.

## 7. Artifact-layout review

- [ ] 7.1 **[READ-ONLY]** Inspect the custom app image offline for target, image version, entry point, segments, checksum/hash, effective image size, and header declarations; record declarations without treating them as physical facts.
- [ ] 7.2 **[READ-ONLY]** Calculate and record the exact candidate image SHA-256, offset, end address, byte range, partition fit, and compatibility with the preserved bootloader and partition table; stop on any ambiguity or overlap.
- [ ] 7.3 **[READ-ONLY]** Confirm that no bootloader, partition table, NVS, OTA metadata, PHY, assets, unused-tail, erase, or unrelated image is included in the candidate first write.
- [ ] 7.4 **[WRITE]** Prepare a non-executed operation packet and placeholder command structure containing the exact reviewed inputs while marking it unauthorized; do not connect to a device or perform Flash.

## 8. Pre-flash human review

- [ ] 8.1 **[READ-ONLY]** Present the exact physical device/PCB identity, user-supplied current port, custom image path/hash, offset/range, partition interpretation, recovery paths/hashes, risks, observation window, stop conditions, and rollback strategy for review.
- [ ] 8.2 **[READ-ONLY]** Reconfirm that historical COM7 is not assumed and that any change to device, port, image, hash, offset, range, recovery state, or command invalidates the packet.
- [ ] 8.3 **[READ-ONLY]** Record a `GO` or `NO-GO` human-review result; retain `NO-GO` while any evidence, redundancy, compatibility, or recovery-rehearsal item is incomplete.

## 9. Explicit user authorization

- [ ] 9.1 **[READ-ONLY]** Obtain an explicit user statement authorizing the one exact First Flash packet, including device, port, image, hash, offset, range, recovery status, and acknowledged risks; reject vague continuation language.
- [ ] 9.2 **[READ-ONLY]** Record the authorization verbatim with its exact scope and confirm it authorizes neither erase, eFuse, security changes, bootloader/table changes, unrelated images, automatic retry, nor rollback.

## 10. Device connection

- [ ] 10.1 **[DEVICE READ]** After exact authorization, connect only to the authorized port as a separate operation and stop without writing if the port cannot be opened exactly as reviewed.
- [ ] 10.2 **[DEVICE READ]** Confirm the authorized ESP32-S3 / ESP-VoCat PCB V1.0 identity using the smallest reviewed read-only handshake or existing serial identity evidence; collect no unrelated unique identifiers.
- [ ] 10.3 **[DEVICE READ]** Compare the observed identity and connection state with the authorization packet; any difference invalidates authorization and returns the workflow to pre-Flash review.

## 11. First Flash

- [ ] 11.1 **[DEVICE WRITE]** Execute only the exact authorized single-image, single-range Flash operation as a standalone command with no monitor, erase, restore, eFuse, bootloader, partition-table, or retry behavior.
- [ ] 11.2 **[DEVICE WRITE]** Preserve the complete actual result, then stop further writes whether the command succeeds or fails; success means only that the exact write command reported success.

## 12. Serial observation

- [ ] 12.1 **[DEVICE READ]** Open serial observation as a separate operation at the exact reviewed settings and capture only the bounded startup window without issuing device-control commands.
- [ ] 12.2 **[DEVICE READ]** Check for the expected smoke-test identity, version, target, disabled-feature summary, optional PSRAM result, and ready marker, while recording silence, unexpected output, resets, panics, watchdogs, or memory-allocation failures.
- [ ] 12.3 **[DEVICE READ]** Continue for the full authorized observation duration and then close the serial connection; do not extend into display, touch, audio, motor, network, or other peripheral tests.

## 13. Pass/fail decision

- [ ] 13.1 **[READ-ONLY]** Apply the serial-only acceptance criteria to the actual Flash and observation records and classify the result as pass, fail, or inconclusive without inferring untested hardware behavior.
- [ ] 13.2 **[READ-ONLY]** On any defined stop condition, freeze further device work, preserve evidence, and decide whether rollback should be proposed; do not retry or widen the write automatically.

## 14. Rollback if required

- [ ] 14.1 **[READ-ONLY]** Prepare a separate exact same-device rollback packet containing port, full-image path/size/hash, complete range, privacy impact, risks, and observation plan; do not treat First Flash authorization as rollback authorization.
- [ ] 14.2 **[DEVICE WRITE]** Only after a new explicit user authorization, restore the exact reviewed 32 MiB image as one separate operation without a preceding erase, automatic retry, or unrelated command.
- [ ] 14.3 **[DEVICE READ]** Observe and record original-device boot and required original-function recovery separately; stop the project hardware sequence if recovery cannot be verified.

## 15. Final evidence recording

- [ ] 15.1 **[WRITE]** Update the hardware-evidence audit, recovery/readiness report, and a task-specific test record with actual commands, outputs, hashes, observations, privacy handling, failures, and remaining uncertainty.
- [ ] 15.2 **[READ-ONLY]** Distinguish implemented, compiled, host-tested, device-written, serial-observed, restored, verified, failed, and not tested states; explicitly retain all untested peripherals as `UNVERIFIED`.
- [ ] 15.3 **[READ-ONLY]** Run OpenSpec strict validation, Git diff checks, generated-output ignore checks, and a scope audit confirming no recovery binary, dump, slice, NVS data, sdkconfig output, device identifier, or unrelated file is staged.
- [ ] 15.4 **[WRITE]** Submit only the reviewed source, configuration, documentation, and test-record changes after user-requested Git authorization, without archiving this Change until all applicable tasks and recovery evidence are complete.
