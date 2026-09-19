## ADDED Requirements

### Requirement: Reviewed predecessor baseline
The Change SHALL use commit `a0b7c726d90448af0f811b7f6c996897e2a16c8a` as its predecessor baseline and SHALL preserve the predecessor artifact as hard-disabled and ineligible for visual validation.

#### Scenario: Baseline is reviewed
- **WHEN** successor source or evidence is prepared
- **THEN** the baseline commit and predecessor hash `4E66B7EDCD38B5225B00E1DB790CA7DD9C842E765961E8929228105F9C10A05D` SHALL be recorded

#### Scenario: Predecessor is relabeled
- **WHEN** the hard-disabled artifact is described as a visual-validation candidate
- **THEN** acceptance SHALL fail

### Requirement: Hardware evidence levels remain explicit
The GPIO44 circuit and polarity SHALL remain `STRONGLY SUPPORTED — REQUIRES FUTURE DEVICE VALIDATION`, while illumination, PWM waveform, display health, color, orientation, refresh, and reset-time transient SHALL remain `UNVERIFIED` until actually observed.

#### Scenario: Source evidence is cited
- **WHEN** the official schematic or fixed V1.0 software is used
- **THEN** confirmed document/source content SHALL be distinguished from physical-device confirmation

#### Scenario: Host result overclaims hardware
- **WHEN** a build or host test labels physical backlight/display behavior confirmed
- **THEN** evidence validation SHALL fail

### Requirement: GPIO44 topology and polarity contract
The visual candidate SHALL treat GPIO44 as an active-high gate for an AO3400A N-channel MOSFET low-side LEDK switch with external gate pull-down, and SHALL NOT treat GPIO44 as a direct LED-current output or active-low control.

#### Scenario: Topology is configured
- **WHEN** the backlight implementation and manifest are reviewed
- **THEN** GPIO44, active-high polarity, no inversion, MOSFET gate role, and low-is-off semantics SHALL be explicit

#### Scenario: Polarity or role changes
- **WHEN** active-low, output inversion, direct LED drive, another GPIO, or another control IC is configured
- **THEN** host acceptance SHALL fail

### Requirement: Exact initial PWM contract
The only approved non-zero first-test configuration SHALL be ESP-IDF LEDC low-speed mode, timer 0, channel 0, APB clock, 2,000 Hz, 10-bit resolution, h-point 0, raw duty 10 of 1023, approximately 0.98%, on GPIO44 with active-high output.

#### Scenario: Exact configuration is present
- **WHEN** source, fake-call traces, generated configuration, symbols, and the artifact manifest are inspected
- **THEN** every PWM field SHALL equal the approved contract

#### Scenario: PWM parameter differs
- **WHEN** another mode, timer, channel, clock, frequency, resolution, h-point, polarity, GPIO, or non-zero duty is detected
- **THEN** implementation and artifact acceptance SHALL fail

### Requirement: Fixed test-only brightness
The candidate SHALL contain no 100% default, duty escalation, second non-zero duty, fade, ramp, dynamic setter, brightness service, NVS restore, companion control, or user adjustment.

#### Scenario: Fixed duty is audited
- **WHEN** the complete backlight source and linked symbols are scanned
- **THEN** zero and raw 10 SHALL be the only duty values used by the backlight path and no dynamic input SHALL reach LEDC

#### Scenario: Product brightness behavior appears
- **WHEN** another duty, percentage, fade API, NVS value, setting, wrapper, or runtime control can affect GPIO44
- **THEN** host validation SHALL fail

### Requirement: GPIO44 is forced off before display work
The firmware SHALL preload GPIO44 low, configure it as digital output with pull-down, reassert low, and verify low before any QSPI, panel, reset, initialization, display-on, draw, or LEDC call.

#### Scenario: Startup ordering is valid
- **WHEN** the successful host call trace is inspected
- **THEN** all preload/configure/reassert/read-low actions SHALL precede the first display or LEDC action

#### Scenario: Early high or peripheral call occurs
- **WHEN** a GPIO44 high/non-zero action or QSPI/panel/LEDC call occurs before the low invariant is established
- **THEN** host validation SHALL fail

### Requirement: Pattern completes before PWM preparation
The firmware SHALL complete panel initialization, display-on, and every bounded test-pattern transfer before configuring the LEDC timer or channel.

#### Scenario: Successful state order
- **WHEN** host tests run the success path
- **THEN** `TEST_PATTERN_DRAW` SHALL finish before `BACKLIGHT_PWM_PREPARE`

#### Scenario: LEDC is prepared early
- **WHEN** any LEDC call occurs before all pattern callbacks complete
- **THEN** host validation SHALL fail and no non-zero duty SHALL be permitted

### Requirement: Zero-duty LEDC handoff
`BACKLIGHT_PWM_PREPARE` SHALL configure the approved timer and then configure channel 0 on GPIO44 with initial duty zero and h-point zero, and SHALL verify effective duty zero before continuing.

#### Scenario: Zero-duty preparation succeeds
- **WHEN** timer and channel calls return success and the reported duty is zero
- **THEN** the state machine SHALL proceed to `BACKLIGHT_LOW_ENABLE`

#### Scenario: Preparation or zero verification fails
- **WHEN** timer configuration, channel configuration, or zero-duty verification fails
- **THEN** the machine SHALL enter `FAIL_SAFE` without any non-zero update

### Requirement: Single low-duty enable
`BACKLIGHT_LOW_ENABLE` SHALL perform exactly one successful non-zero update to raw duty 10, verify the reported duty equals 10, and emit the exact approved configuration marker before READY.

#### Scenario: Low-duty enable succeeds
- **WHEN** the one approved update and verification succeed
- **THEN** READY SHALL be reached with `visual=UNVERIFIED` and `backlight=LOW_FIXED_TEST_ONLY`

#### Scenario: Enable or verification fails
- **WHEN** the update fails or reported duty differs from 10
- **THEN** the machine SHALL enter `FAIL_SAFE`, request duty zero, and SHALL NOT emit READY

### Requirement: Deterministic successor state machine
The state graph SHALL contain exactly `BOOT_MARKER`, `BACKLIGHT_FORCED_OFF`, `QSPI_BUS_INIT`, `PANEL_IO_CREATE`, `PANEL_RESET`, `PANEL_INIT`, `DISPLAY_ON`, `TEST_PATTERN_DRAW`, `BACKLIGHT_PWM_PREPARE`, `BACKLIGHT_LOW_ENABLE`, `READY`, and `FAIL_SAFE` with no alternate path.

#### Scenario: State graph matches
- **WHEN** host state-machine tests enumerate the success and failure transitions
- **THEN** each success state SHALL appear once in order and every error SHALL transition directly to `FAIL_SAFE`

#### Scenario: State is skipped, repeated, or bypassed
- **WHEN** an alternate enable, retry, fallback, repeated reset, or direct READY transition is reachable
- **THEN** host validation SHALL fail

### Requirement: Fail-safe backlight shutdown
After LEDC channel ownership is possible, every failure path SHALL best-effort request duty zero, stop LEDC with idle level zero, restore GPIO44 as digital output low with pull-down, verify low where possible, and then continue existing reverse-order display cleanup.

#### Scenario: Failure occurs after LEDC preparation
- **WHEN** any injectable or invariant failure occurs at or after `BACKLIGHT_PWM_PREPARE`
- **THEN** the shutdown actions SHALL occur in the specified order, no later enable/READY action SHALL occur, and the original failure SHALL remain controlling

#### Scenario: One cleanup action fails
- **WHEN** a duty-zero, stop, GPIO restore, or later cleanup call reports an error
- **THEN** remaining cleanup SHALL still be attempted, no second failure marker SHALL appear, and no retry SHALL occur

### Requirement: No automatic retry or reset
The candidate SHALL NOT retry an LEDC, GPIO, display, allocation, transfer, cleanup, connection, write, or observation operation and SHALL NOT automatically reset the MCU or panel after a failure.

#### Scenario: Runtime operation fails
- **WHEN** any checked call, invariant, or timeout fails
- **THEN** its attempt count SHALL remain one and the firmware SHALL settle in a stable non-busy fail-safe state

#### Scenario: Retry or reset fallback exists
- **WHEN** source or host traces show another attempt, fallback parameter, MCU restart, boot loop, or repeated panel reset
- **THEN** acceptance SHALL fail

### Requirement: Exact successor markers
The firmware SHALL emit deterministic markers for PWM preparation, exact configuration, low enable, and READY without including private or device-unique data.

#### Scenario: Success markers are emitted
- **WHEN** the host success path runs
- **THEN** markers SHALL state GPIO44, active-high, 2,000 Hz, 10 bits, initial duty zero, raw duty 10/1023, approximately 0.98%, `visual=UNVERIFIED`, and `backlight=LOW_FIXED_TEST_ONLY` once and in order

#### Scenario: Marker is wrong or unsafe
- **WHEN** a marker omits the fixed contract, changes `UNVERIFIED`, repeats, appears out of order, or contains sensitive data
- **THEN** host validation SHALL fail

### Requirement: Complete host fault injection
Host fakes and the native harness SHALL instrument every real GPIO/LEDC/display call boundary introduced or retained by the successor, with an explicit successor call-point inventory and complete matrix coverage for every injectable failure.

#### Scenario: Fault matrix is complete
- **WHEN** the native harness runs
- **THEN** each injectable point SHALL fire exactly once in its case, preserve ordered fail-safe behavior, prohibit READY, and leave the modeled backlight off

#### Scenario: Call point is untested
- **WHEN** an actionable call has no matrix case or a configured injection never fires
- **THEN** host acceptance SHALL fail

### Requirement: Static backlight regression guard
Static tests SHALL reject any unauthorized polarity, duty, frequency, resolution, timer/channel, clock, fade, NVS, dynamic-control, UART0 ownership, direct GPIO44-high, wrapper/alias, or unrelated-peripheral path.

#### Scenario: Static contract matches
- **WHEN** source, generated config, component tree, map, symbols, strings, and tests are audited
- **THEN** only the exact approved LEDC surface and existing PCB V1.0 display allowlist SHALL remain

#### Scenario: Unsafe regression appears
- **WHEN** any unexplained forbidden match or indirect path is found
- **THEN** build/artifact acceptance SHALL be blocked

### Requirement: Existing display contract is preserved
The successor SHALL preserve ST77916 v1.0.1, the locked 184-command vendor table and normalized hash, QSPI on SPI2_HOST at no more than 40 MHz, the exact PCB V1.0 GPIO allowlist, RGB565 geometry, one internal-DMA strip, and all unrelated-peripheral prohibitions.

#### Scenario: Regression suite passes
- **WHEN** predecessor display tests and successor additions run together
- **THEN** all unchanged geometry, table, buffer, transfer, cleanup, denylist, and dependency requirements SHALL pass

#### Scenario: Adjacent display behavior changes
- **WHEN** the pattern, panel table, QSPI mapping, dependency, buffer model, or unrelated feature changes without a separately justified specification
- **THEN** scope validation SHALL fail

### Requirement: Reproducible visual candidate
The successor App SHALL be configured and built independently in two new ignored build directories from the complete recorded source with ESP-IDF v5.5.4 and reproducible-build configuration.

#### Scenario: Clean builds agree
- **WHEN** both clean builds finish
- **THEN** their App BIN lengths and SHA-256 values SHALL be identical and BIN/ELF/MAP/bootloader/partition/sdkconfig identities SHALL be recorded

#### Scenario: Build evidence is stale or differs
- **WHEN** a prior build directory, copied output, stale ignored BIN, or differing clean output is used
- **THEN** artifact and device-gate acceptance SHALL fail

### Requirement: Distinct visual-candidate manifest
The successor SHALL create a new artifact manifest/review that identifies the image as a visual-validation candidate and records its exact GPIO44/PWM/fail-safe behavior without modifying the predecessor evidence.

#### Scenario: Manifest is complete
- **WHEN** artifact review finishes
- **THEN** it SHALL contain the new canonical hash, source digest, toolchain, exact PWM contract, `visual=UNVERIFIED`, dependency/table evidence, and explicit distinction from the predecessor hash

#### Scenario: Evidence is overwritten or ambiguous
- **WHEN** the predecessor manifest is relabeled, the new hash is absent, or GPIO44 behavior is not exact
- **THEN** successor handoff SHALL be blocked

### Requirement: Independent range model
Before any write review, candidate semantic range, ROM transport blocks and padding, and sector erase envelope SHALL be independently calculated from the new artifact and checked against the preserved original `ota_0` partition.

#### Scenario: Geometry is complete
- **WHEN** the artifact packet is reviewed
- **THEN** exact start, end-exclusive, length, transport block count, final padding bytes/value, erase sectors, containment, and preserved regions SHALL be explicit

#### Scenario: Range is inferred or mismatched
- **WHEN** transport/erase geometry is omitted, inferred only from file length, exceeds containment, or differs from the proposed operation
- **THEN** the future write gate SHALL remain closed

### Requirement: Recovery assets are prerequisites
Immediately before any future device-write review, all four immutable 32 MiB images and both Level 1 complete original `ota_0` staging copies SHALL exist and match their reviewed sizes and SHA-256 values without modification.

#### Scenario: Recovery evidence matches
- **WHEN** the future pre-write host check runs
- **THEN** full images SHALL be 33,554,432 bytes with SHA-256 `72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001`, and Level 1 images SHALL be 4,128,768 bytes with SHA-256 `C8A2FE4AB0F9B7C823F1DCFDFC926682C9DBF1B079056461DC6F18A93A345D0E`

#### Scenario: Recovery evidence is missing or changed
- **WHEN** a path, size, hash, same-device scope, or Level 1/Level 2 distinction fails
- **THEN** all future device work SHALL stop

### Requirement: Existing same-device backup remains controlling
The Change SHALL use the immutable pre-project same-device image as the controlling backup and SHALL NOT require or perform a new live firmware readback merely to preserve the currently installed reproducible project candidate.

#### Scenario: Backup scope is reviewed
- **WHEN** the device gate is prepared
- **THEN** the original full image, Level 1 original `ota_0`, committed source, and artifact hashes SHALL be identified as the available recovery evidence

#### Scenario: Live backup is proposed casually
- **WHEN** a Flash readback is added without a separate need, classification, range review, and authorization
- **THEN** scope validation SHALL reject it

### Requirement: Manual download-mode recovery procedure gate
An authoritative PCB V1.0 manual BOOT/RESET procedure for ROM download-mode entry SHALL be documented and reviewed before any future write authorization if automatic USB Serial/JTAG connection could fail.

#### Scenario: Recovery entry procedure is known
- **WHEN** the future device packet is reviewed
- **THEN** it SHALL state the authoritative button sequence, expected enumeration, stop conditions, and how success is judged without changing eFuses or security settings

#### Scenario: Procedure remains unknown
- **WHEN** no authoritative manual sequence is available
- **THEN** host implementation SHALL remain eligible for completion but the physical write gate SHALL remain blocked

### Requirement: Fresh device identity and endpoint gate
Any future device operation SHALL freshly confirm ESP32-S3, PCB V1.0, USB Serial/JTAG, current endpoint, and required download-mode/security availability without assuming historical COM7 or retaining private identifiers.

#### Scenario: Identity and endpoint match
- **WHEN** a separately authorized minimum inspection completes
- **THEN** its sanitized result SHALL match the exact operation packet before a write can be authorized

#### Scenario: Identity, port, or mode differs
- **WHEN** any required field is absent, mismatched, stale, or inferred from history
- **THEN** the operation SHALL stop before a write

### Requirement: Fresh exact write authorization
No Flash write SHALL occur until the user explicitly authorizes one exact candidate, hash, port, chip, PCB revision, offset, semantic range, transport range/padding, erase envelope, partition containment, recovery evidence, tool/mode, and one-attempt behavior.

#### Scenario: Exact authorization matches
- **WHEN** every reviewed field equals the proposed operation immediately before execution
- **THEN** no more than one `WRITE` SHALL be performed under that fresh authorization

#### Scenario: Authorization is absent, stale, reused, or mismatched
- **WHEN** a prior authorization or a packet with any changed field is presented
- **THEN** no device command SHALL be executed

### Requirement: Device operations remain separated
Configure, build, artifact inspection, device identity, Flash write, startup reset, serial observation, and recovery SHALL remain separate operations with separate results and authorization where required.

#### Scenario: Future operation sequence is followed
- **WHEN** device work is eventually reviewed
- **THEN** the write SHALL stop on its own result before a separately authorized `REBOOT` and `READ-ONLY` observation begin

#### Scenario: Combined or automatic sequence is proposed
- **WHEN** flash-monitor, automatic retry, automatic observation, automatic rollback, or another combined action appears
- **THEN** operation review SHALL fail

### Requirement: One-attempt write boundary
Any future visual-candidate write SHALL use exactly one outer invocation and one allowed attempt at each enforced connection, sync, open, block, and operation layer, with no automatic retry or unreviewed range.

#### Scenario: One attempt completes
- **WHEN** the exact authorized write returns
- **THEN** execution SHALL stop and preserve the actual sanitized result and transmitted geometry

#### Scenario: Retry or geometry deviation occurs
- **WHEN** an automatic/manual retry is attempted or actual semantic/transport/erase scope differs
- **THEN** the operation SHALL stop as inconclusive and no startup or rollback SHALL follow without new review and authorization

### Requirement: Bounded visual acceptance
A separately authorized observation SHALL evaluate boot stability, exact markers, absence of early full-brightness flash, intended low backlight level, fixed pattern/text, plausible colors/orientation, and stable refresh for a bounded window.

#### Scenario: All bounded checks pass
- **WHEN** the exact candidate boots once and every defined visual/log criterion is observed without a stop condition
- **THEN** only those observed facts SHALL be eligible for promotion to confirmed for that bounded run

#### Scenario: Output is blank or ambiguous
- **WHEN** READY appears but illumination or pattern evidence is absent or inconclusive
- **THEN** the result SHALL be inconclusive and SHALL NOT authorize a higher duty or retry

### Requirement: Visual stop conditions
The observation SHALL stop without retry on unexpected brightness, early flash, flicker/pulsing, wrong/repeating markers, reset loop, panic, watchdog, allocation failure, USB-handle loss, odor, heat, abnormal sound, or any identity/command/range mismatch.

#### Scenario: Stop condition occurs
- **WHEN** any listed condition is observed
- **THEN** no further device action SHALL occur; manual power removal SHALL be permitted only for urgent physical danger, and any rollback SHALL require the independent Recovery gate

#### Scenario: Stop condition is ignored
- **WHEN** observation, retry, duty increase, write, or automatic rollback continues after a stop condition
- **THEN** the hardware-validation process SHALL fail

### Requirement: Recovery remains independent
Level 1 complete original `ota_0` restore, Level 2 same-device 32 MiB recovery, post-restore observation, and original-function verification SHALL remain independently reviewed and authorized under DEC-023 and DEC-024.

#### Scenario: Display validation succeeds
- **WHEN** the bounded visual observation passes
- **THEN** the mandatory restoration rehearsal and Recovery Change SHALL remain required before formal custom-cat firmware work

#### Scenario: Display validation fails
- **WHEN** rollback appears necessary
- **THEN** this Change SHALL stop and SHALL NOT automatically execute either recovery level

### Requirement: Historical results remain unchanged
The Change SHALL preserve First Flash as `STOPPED / INCONCLUSIVE — RANGE-SCOPE DEVIATION`, startup observation as `PASS`, and prior minimal runtime as `PASS FOR THIS MINIMAL SMOKE TEST`.

#### Scenario: Historical evidence is referenced
- **WHEN** successor planning, implementation, or observation records discuss prior device work
- **THEN** all exact bounded statuses SHALL remain consistent

#### Scenario: New evidence rewrites history
- **WHEN** a later display result is used to call the historical First Flash exact-range successful
- **THEN** documentation validation SHALL fail

### Requirement: Planning and host implementation grant no device authority
Creating, validating, applying, building, or committing this Change SHALL NOT authorize serial access, reset, esptool, Flash, readback, erase, eFuse, rollback, or restore operations.

#### Scenario: Host work completes
- **WHEN** all source, tests, clean builds, and artifact evidence pass
- **THEN** the result SHALL be only `READY FOR DEVICE-GATE REVIEW` and hardware SHALL remain `UNVERIFIED`

#### Scenario: Device command is inferred from Change readiness
- **WHEN** OpenSpec or host acceptance is treated as permission to operate hardware
- **THEN** the action SHALL be rejected

### Requirement: Prohibited hardware and feature scope
The candidate SHALL NOT modify eFuses, Flash voltage, Secure Boot, Flash Encryption, JTAG, USB Serial/JTAG, download-mode availability, partition layout, bootloader, NVS, OTA metadata, PHY, assets, GPIO9 power control, touch, LVGL, PSRAM, audio, microphone, motor, SD, Wi-Fi, Bluetooth, network, filesystem, or companion behavior.

#### Scenario: Scope remains minimal
- **WHEN** source, configuration, dependencies, map, symbols, and artifact ranges are reviewed
- **THEN** only the existing display path plus exact LEDC support SHALL be added

#### Scenario: Prohibited change appears
- **WHEN** any listed hardware/security/feature change or executable erase/irreversible operation is introduced
- **THEN** scope validation SHALL fail
