## ADDED Requirements

### Requirement: Verified evidence prerequisite
The workflow SHALL map every board configuration, memory option, image, offset, and acceptance claim to reviewed PCB V1.0 evidence classified as `CONFIRMED`, `STRONGLY SUPPORTED`, or `UNVERIFIED`. It MUST NOT use `STRONGLY SUPPORTED` or `UNVERIFIED` evidence for a potentially unsafe hardware action.

#### Scenario: Required configuration lacks verified evidence
- **WHEN** a proposed configuration value is supported only by generic defaults, original-firmware strings, matching source, or an unresolved inference
- **THEN** the workflow stops before configure or device execution, records the missing evidence, and does not substitute a guessed value

### Requirement: Exact PCB revision isolation
The smoke-test workflow SHALL target only ESP-VoCat PCB V1.0 and MUST NOT import PCB V1.2 or generic ESP32-S3 board definitions.

#### Scenario: Another board revision appears
- **WHEN** source, configuration, review material, or a device identity indicates PCB V1.2 or an unspecified board
- **THEN** the workflow rejects that input and stops before build or device access

### Requirement: Recovery assets prerequisite
The workflow SHALL require immutable, same-device, full-Flash recovery assets to be available outside the repository with the expected redundancy before authorizing a first custom Flash.

#### Scenario: A required recovery location is missing
- **WHEN** either required recovery location is unavailable or the same-device scope cannot be established
- **THEN** the first-Flash gate remains `NO-GO` and no device write is authorized

### Requirement: Full-backup hash verification
The workflow SHALL verify each 32 MiB recovery image immediately before the pre-Flash review and SHALL require size `33554432` bytes and SHA-256 `72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001`.

#### Scenario: Recovery file differs
- **WHEN** a recovery file is missing, has another size, cannot be read, or has another SHA-256
- **THEN** the workflow stops before Flash and reports the actual mismatch without modifying the file

### Requirement: Partition-layout verification
The workflow SHALL identify the original partition table by validated magic, entry structure, checksum or digest, bounds, non-overlap, and image consistency rather than assuming a standard offset.

#### Scenario: Layout is ambiguous
- **WHEN** no unique valid partition table is found or an app/write range conflicts with a partition boundary
- **THEN** the workflow keeps the layout `UNVERIFIED` and stops before preparing a device write

### Requirement: Board configuration allowlist
The implementation SHALL use the reviewed allowlist in `docs/hardware/pcb-v1-esp-idf-configuration-map.md`, containing only the exact ESP32-S3, serial, Flash, and disabled-PSRAM settings supported for the initial PCB V1.0 candidate; all other board features SHALL default to disabled.

#### Scenario: A generated configuration includes an unlisted option
- **WHEN** static or generated-configuration review finds a board, memory, peripheral, storage, network, security, or GPIO option outside the allowlist
- **THEN** the build artifact is rejected and no Flash command is prepared

### Requirement: Device-read-only configuration evidence gate
When offline evidence cannot resolve a mandatory PCB V1.0 Flash or PSRAM configuration input, the workflow SHALL stop before firmware implementation, tracked board configuration, configure, or build and SHALL require a separately reviewed device-read-only inspection packet. The packet MUST remain `NOT AUTHORIZED` until the user explicitly approves the exact device, current reviewed port, query categories, tool/version, reset risk, privacy handling, and stop conditions. Vague continuation language MUST NOT authorize device access.

#### Scenario: Offline evidence remains ambiguous
- **WHEN** original image declarations, prior device records, and compiled strings do not establish one traceable physical Flash/PSRAM configuration
- **THEN** the Firmware Implementation Gate remains closed, PSRAM remains disabled, and the workflow prepares only a non-executed read-only inspection plan

#### Scenario: Read-only inspection is later reviewed
- **WHEN** a future packet proposes chip identification, Flash identification/capacity, or read-only eFuse summary evidence
- **THEN** it uses `<REVIEWED_PORT>` until an exact current port is explicitly authorized, excludes `read_flash` and every write/erase/restore/eFuse-write operation, and requires redaction of MAC addresses and unnecessary unique identifiers

#### Scenario: Evidence and source mapping support a host-only candidate
- **WHEN** device evidence and local ESP-IDF source identify exact target, Flash bus/sample mode, frequency, conservative header size, console, disabled PSRAM path, and a no-VDDSPI-mutation boundary without guessing
- **THEN** the Firmware Implementation Gate may become `OPEN FOR HOST-ONLY IMPLEMENTATION` while device access remains unauthorized and First Flash remains `NO-GO`

### Requirement: Host-only implementation gate is not Flash authorization
An open Firmware Implementation Gate SHALL authorize only a separate firmware edit, candidate `sdkconfig.defaults`, host-only configure/build, static review, and artifact inspection. Partition/write offset or range selection, device connection, Flash, monitor, and hardware-validation claims SHALL remain independently gated.

#### Scenario: Host-only implementation gate opens
- **WHEN** the reviewed ESP-IDF map opens the Firmware Implementation Gate
- **THEN** no device command is prepared or executed, no Flash authorization is inferred, and the generated artifact still requires preserved-bootloader/partition/OTA compatibility review

### Requirement: No unverified GPIO use
The initial smoke-test source and configuration MUST NOT name, configure, read, write, reserve experimentally, or toggle any GPIO.

#### Scenario: GPIO use is detected
- **WHEN** static review finds any GPIO number, GPIO API, peripheral pin assignment, LED blink, or raw pin operation
- **THEN** the implementation fails review and does not proceed to configure or build

### Requirement: Minimal startup behavior
The smoke-test application SHALL only start on the reviewed ESP32-S3 configuration, emit fixed startup and compile-time version information, optionally report gated PSRAM initialization, and enter a bounded idle state.

#### Scenario: Minimal startup succeeds
- **WHEN** the exact authorized image boots on the exact reviewed PCB V1.0 device
- **THEN** the serial output contains the smoke-test identifier, build version, target label, disabled-feature summary, and ready marker without starting product behavior

### Requirement: Serial-log-only initial acceptance
The first smoke-test revision SHALL use serial output as its only hardware acceptance channel and SHALL NOT treat display, touch, audio, network, storage, or movement behavior as acceptance evidence.

#### Scenario: Serial ready marker is observed
- **WHEN** the expected serial ready marker is received during the reviewed observation window
- **THEN** the result records only serial startup acceptance and makes no peripheral or hardware-completeness claim

### Requirement: Optional PSRAM gate
PSRAM initialization SHALL remain disabled unless exact PCB V1.0 mode, size, clock, voltage domain, and ESP-IDF configuration are all supported by reviewed evidence. If enabled, PSRAM MUST remain nonessential to serial startup.

#### Scenario: PSRAM evidence is incomplete
- **WHEN** any required PSRAM fact is missing, conflicting, or inferred only from strings or generic defaults
- **THEN** PSRAM initialization remains disabled and the serial-only smoke test continues without it

### Requirement: No peripheral initialization by default
The initial smoke test MUST NOT initialize the display, screen touch, top capacitive touch, motor, microphone, speaker, amplifier, audio codecs, IMU, SD, battery device, power-control signal, Wi-Fi, or Bluetooth.

#### Scenario: A peripheral dependency appears
- **WHEN** source, configuration, linked components, or startup logs show initialization of an excluded peripheral
- **THEN** the artifact fails static safety review and is not eligible for Flash

### Requirement: No NVS mutation
The smoke-test application MUST NOT initialize NVS for product use, erase NVS, write settings, update OTA selection, or mutate any data partition.

#### Scenario: Storage mutation path is found
- **WHEN** static review or linked-component inspection finds an NVS, OTA-data, filesystem, PHY-data, or other persistent write path
- **THEN** the artifact is rejected before device execution

### Requirement: No network or OTA
The smoke-test application MUST NOT start Wi-Fi, Bluetooth, TCP/IP, cloud communication, provisioning, OTA checking, OTA download, or OTA activation.

#### Scenario: Network or OTA component is enabled
- **WHEN** source, configuration, link metadata, or logs indicate a network or OTA path
- **THEN** the artifact fails review and no Flash is authorized

### Requirement: No eFuse mutation
The Change MUST NOT include or execute any eFuse write, burn, key-purpose change, download-mode restriction, JTAG restriction, USB Serial/JTAG restriction, or Flash-voltage operation.

#### Scenario: An eFuse mutation is proposed
- **WHEN** a task, command, configuration, or dependency could mutate eFuses or an irreversible hardware-security setting
- **THEN** the operation is rejected as outside this Change and is not converted into an implementation task

### Requirement: No Secure Boot or Flash Encryption
The smoke-test build and device workflow MUST NOT enable, migrate, provision, or modify Secure Boot or Flash Encryption.

#### Scenario: Security enablement is detected
- **WHEN** configuration or an operation would enable Secure Boot, Flash Encryption, signing-key provisioning, or encryption-key handling
- **THEN** the workflow stops and rejects the artifact or operation

### Requirement: Explicit Flash authorization gate
OpenSpec Apply MAY create firmware, run host-only configure/build, inspect artifacts, and prepare a non-executed command, but MUST NOT execute Flash until the user explicitly authorizes one exact operation including device, port, image, image hash, offset, write range, recovery status, and risks. “Continue”, “execute the next step”, or equivalent vague language MUST NOT count as authorization.

#### Scenario: Authorization is absent or vague
- **WHEN** host-only preparation is complete but the user has not explicitly approved every required field for the exact operation
- **THEN** the workflow stops before opening the device for a write and reports that Flash remains unauthorized

### Requirement: Exact reviewed port and offset requirement
Every future device write SHALL use the exact port and offset contained in the authorization packet; historical `COM7`, auto-selected ports, inferred offsets, wildcards, and substituted images MUST NOT be used.

#### Scenario: Runtime target differs from authorization
- **WHEN** the connected port, device identity, image, hash, offset, or byte range differs from the reviewed packet
- **THEN** the authorization becomes invalid and the workflow stops before writing

### Requirement: Pre-flash recovery rehearsal
Before Flash authorization, the workflow SHALL complete a host-only recovery rehearsal that verifies recovery-file availability and hashes, same-device scope, tool availability, procedure separation, and rollback decision points without executing a device write.

#### Scenario: Rehearsal is incomplete
- **WHEN** any recovery input, hash, procedure step, operator judgment point, or same-device constraint is unresolved
- **THEN** the pre-Flash review remains incomplete and the first-Flash gate is `NO-GO`

### Requirement: Post-flash observation window
After the exact Flash write, the workflow SHALL perform serial observation as a separate operation for a duration fixed in the authorization packet and SHALL capture startup, ready, reset, panic, watchdog, and memory-initialization results.

#### Scenario: Stable serial-only startup
- **WHEN** the expected logs and ready marker appear and no reset, panic, watchdog, or memory-allocation failure occurs for the full observation window
- **THEN** the result is recorded as a serial-only smoke-test pass

### Requirement: Failure stop condition
The workflow SHALL stop immediately on identity mismatch, hash mismatch, partition ambiguity, Flash failure, unexpected output, silence past the reviewed timeout, reset loop, panic, watchdog, or memory-allocation failure and MUST NOT retry or widen the write automatically.

#### Scenario: A stop condition occurs
- **WHEN** any defined stop condition is observed
- **THEN** further device writes cease, the actual output is preserved, and rollback is evaluated without claiming success

### Requirement: Rollback requirement
If the pass/fail decision requires rollback, the workflow SHALL first use only
the reviewed complete raw original `ota_0` partition image under a separate
exact authorization. It SHALL preserve the bootloader, partition table, NVS,
`otadata`, `phy_init`, `ota_1`, assets, and upper unused region. A complete
same-device full recovery image MAY be considered only as a higher-risk Level 2
escalation after Level 1 cannot restore original boot behavior, and it SHALL
require another independent review and exact authorization. The workflow SHALL
verify original-device recovery before further hardware work.

#### Scenario: Rollback is required
- **WHEN** the smoke test fails or the user directs recovery after reviewing the failure
- **THEN** no Level 1 restore runs until the exact complete raw `ota_0` image,
  hash, port, `0x00020000` start, `0x003F0000` length, same-device scope,
  preserved regions, and risks are explicitly authorized

#### Scenario: App-only rollback cannot restore original boot
- **WHEN** the separately authorized Level 1 complete-`ota_0` restore cannot
  restore original boot behavior
- **THEN** the workflow stops and treats a 32 MiB same-device restore as
  `NOT AUTHORIZED`, `HIGHER RISK`, and `SEPARATE REVIEW REQUIRED`

### Requirement: No hardware-completeness claim
Successful compilation or serial startup MUST NOT be described as verification of Flash electrical mode, Flash voltage, PSRAM correctness, partition safety beyond the reviewed write, GPIO wiring, display, touch, audio, microphone, motor, network, product behavior, or full recovery.

#### Scenario: Serial smoke test passes
- **WHEN** the serial-only acceptance criteria are met
- **THEN** the report lists the exact behavior verified and keeps every untested hardware area `UNVERIFIED`

### Requirement: Separate future peripheral changes
Display, screen-touch, top-touch, audio, microphone, motor, SD, battery, power-control, network, and other peripheral enablement SHALL be specified and implemented in separate future Changes after the serial-only result and required recovery rehearsal are recorded.

#### Scenario: Peripheral work is requested during this Change
- **WHEN** an implementation task would add an excluded peripheral or adjacent product feature
- **THEN** it is deferred to a separate Change and the current scope remains serial-only
