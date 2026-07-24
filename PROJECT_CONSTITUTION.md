# ESP-VoCat Custom Project Constitution
- Status: Active
- Version: 1.0
- Effective date: 2026-07-11
- Target hardware: ESP-VoCat PCB V1.0
- Applies to: human contributors, Codex, scripts, automation and development tools
This document defines the highest-level safety, engineering and scope constraints for the project.
When another document, prompt or implementation conflicts with this constitution, this constitution takes precedence.
---
## 1. Hardware identity
1. The only confirmed development target is ESP-VoCat PCB V1.0.
2. The main MCU is ESP32-S3 with 32 MB Flash and 16 MB PSRAM.
3. Hardware behavior, GPIO assignment and peripheral configuration must not be inferred from generic ESP32-S3 development boards.
4. PCB V1.2, reference development boards and unrelated examples must not be treated as electrically equivalent to PCB V1.0.
5. Any unverified pin, voltage, bus or protocol assumption must be explicitly marked as unverified before implementation.
---
## 2. Irreversible operations
The following operations are prohibited unless the user explicitly authorizes the exact operation after reviewing its risks:
- Writing or changing eFuses
- Enabling Secure Boot
- Enabling Flash Encryption
- Disabling JTAG, USB Serial/JTAG or download mode
- Changing VDD_SPI or Flash voltage
- Running `erase_flash`
- Erasing the complete device
- Modifying security keys or key purposes
- Performing any irreversible hardware configuration
No implementation task may silently include any of these operations.
---
## 3. Flash safety
1. Read-only operations are the default.
2. Every device command must be classified before execution as one of:
   - read-only
   - reboot
   - write
   - erase
   - irreversible
3. Build, flash and monitor are separate actions and must not be combined without explicit approval.
4. Codex must not independently run a Flash write, erase or recovery command.
5. Before every Flash write, the following must be confirmed:
   - correct serial port
   - correct target chip
   - correct PCB version
   - intended binary files
   - intended offsets
   - intended partition table
   - recovery image availability
   - recovery image SHA-256
6. The first custom Flash operation must use the approved PCB V1.0 smoke-test firmware.
7. Formal cat firmware must not be flashed before the original Xiaozhi firmware has been successfully restored at least once.
---
## 4. Backup protection
The original firmware backups are immutable recovery assets.
Primary backup:
`E:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi`
Cross-disk backup:
`D:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi`
Confirmed full-image SHA-256:
`72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001`
Rules:
1. Source code, build products and generated files must never be written into either backup directory.
2. Backup files must not be renamed, modified, patched or used as build output.
3. Restore tests must use copies or explicitly referenced immutable images.
4. Any backup hash mismatch is a stop condition.
---
## 5. Source and hardware evidence
1. GPIO numbers must not be guessed.
2. Peripheral models must not be replaced with similar components without evidence.
3. PCB V1.0 and PCB V1.2 definitions must not be mixed.
4. Example code must be checked for:
   - target board
   - GPIO configuration
   - voltage requirements
   - bus configuration
   - dependency versions
5. Source-derived information must be classified as:
   - confirmed by the current device
   - strongly supported by reference source
   - unverified and requiring a test
6. Unknown hardware behavior must be isolated in a small test before integration.
---
## 6. Development sequence
The required initial sequence is:
1. Compile the ESP-IDF Hello World example without flashing.
2. Build a dedicated ESP-VoCat PCB V1.0 smoke-test firmware.
3. Review the generated binaries and Flash layout.
4. Flash only the smoke-test firmware.
5. Verify serial output.
6. Verify display output.
7. Verify screen touch.
8. Verify top capacitive touch.
9. Restore the complete original Xiaozhi image.
10. Verify original-device recovery.
11. Begin formal custom firmware development.
No step may be skipped because a later feature appears easy.
---
## 7. Scope control
1. Each implementation task must have a defined scope and acceptance criteria.
2. Codex must not add unrelated features while implementing a requested task.
3. Large changes must be divided into independently testable increments.
4. Hardware drivers, product behavior and visual assets should be developed as separable modules.
5. Refactoring must not be mixed with new hardware functionality unless necessary.
6. A task is not complete merely because it compiles.
---
## 8. Firmware architecture principles
The firmware must preserve clear boundaries between:
- board support and hardware drivers
- display rendering
- cat animation
- touch input
- top capacitive touch
- audio input and output
- voice commands
- reminders and scheduling
- USB communication
- computer connection state
- base motor control
- settings and persistence
A fault in a nonessential module must not prevent the core cat experience from starting whenever technically possible.
---
## 9. Performance principles
1. Smooth cat animation is a primary product requirement.
2. Long-running work must not block display, touch, audio or reminder processing.
3. Main interactive animations target 30 FPS.
4. Idle animation targets 15-20 FPS.
5. Slow sleeping animation may use 10-15 FPS.
6. Performance changes must be measured rather than assumed.
7. Memory allocation failures, task starvation and watchdog resets are release blockers.
8. Large uncompressed full-screen frame sets must not be adopted without memory and storage analysis.
---
## 10. Offline-first behavior
The following core functions must continue without the computer companion application:
- cat idle animations
- screen interaction
- top petting
- local cat sounds
- basic fixed voice commands where supported locally
- cached schedules and reminders
- reminder triggering
- settings required for normal operation
- safe predefined base actions
Weather and Codex usage may use cached values, but must display their last update time and must not be presented as current when stale.
---
## 11. Computer companion boundary
The computer companion application may handle:
- weather retrieval
- Codex usage retrieval or aggregation
- complex reminder parsing
- dynamic speech generation
- schedule editing
- complete history storage
- device configuration and synchronization
The ESP32 must not store:
- ChatGPT browser cookies
- reusable account session credentials
- passwords
- private authentication tokens that are not strictly required on the device
Secrets must not be committed into the repository.
---
## 12. USB communication
1. The companion application must not permanently assume the device is COM7.
2. Device discovery must use a handshake or reliable device identity.
3. The communication protocol must be versioned.
4. Malformed messages must not crash or reset the device.
5. Unknown commands must be rejected safely.
6. Data updates must include enough metadata to determine freshness.
7. Protocol compatibility changes must be documented.
---
## 13. Motor and physical movement safety
1. The rotating base must not move until its UART protocol and board wiring have been verified.
2. Initial display and touch tests must not enable the motor.
3. Motor commands require angle, speed or duration limits.
4. Repeated direction changes require cooldown or debouncing.
5. Low-confidence sound direction must not cause physical rotation.
6. A stop command must remain available during movement.
7. Automatic turning must be user-configurable.
---
## 14. Audio and microphone safety
1. Microphone operation must support:
   - wake-word mode
   - touch-to-listen mode
   - disabled mode
2. The current microphone state must be visible to the user.
3. Microphone-disabled mode must stop unnecessary audio capture and recognition work.
4. Audio processing must not prevent reminders, touch handling or display refresh.
5. Initial smoke testing must not enable the microphone, speaker or source localization.
---
## 15. Documentation requirements
Each meaningful implementation must update the relevant project documentation.
At minimum, maintain:
- product specification
- hardware profile
- architecture
- communication protocol
- decisions
- acceptance criteria
- test results
- known risks
A design decision that exists only in chat is not considered fully recorded.
---
## 16. Codex operating rules
Codex must:
1. Read this constitution before changing the project.
2. Read the relevant specification before implementation.
3. State assumptions and unresolved hardware uncertainties.
4. Limit changes to the current task.
5. Preserve existing validated behavior.
6. Explain any command that can alter the device.
7. Stop before any Flash write and request explicit execution approval.
8. Never treat successful compilation as proof of hardware correctness.
9. Never fabricate test results.
10. Report incomplete work and unresolved risks honestly.
Codex must not:
- change hardware security settings
- execute irreversible commands
- guess GPIO assignments
- modify recovery backups
- silently widen scope
- conceal failed tests
- claim that untested hardware behavior works
---
## 17. Completion criteria
A feature is complete only when:
1. its specification is clear;
2. its implementation is limited to scope;
3. it builds successfully;
4. relevant automated or host-side tests pass;
5. required hardware behavior is tested when safe;
6. failure and offline behavior are verified;
7. documentation is updated;
8. no constitutional rule is violated.
---
## 18. Amendment rule
Changes to this constitution require:
1. an explicit reason;
2. identification of affected safety or architecture rules;
3. user approval;
4. a version update;
5. a recorded decision entry.
Convenience alone is not sufficient justification for weakening a safety rule.
