# esp-idf-host-build-baseline Specification

## Purpose
TBD - created by archiving change establish-esp-idf-hello-world-build-baseline. Update Purpose after archive.
## Requirements
### Requirement: Existing ESP-IDF v5.5.4 toolchain
The build baseline SHALL use the existing ESP-IDF v5.5.4 installation. The workflow MUST NOT install, upgrade, downgrade, repair, or reinitialize ESP-IDF or any toolchain dependency.

#### Scenario: Correct existing installation is available
- **WHEN** the Apply workflow resolves the existing ESP-IDF installation
- **THEN** it confirms the reported ESP-IDF version is exactly v5.5.4 before configure begins
- **THEN** it records the resolved installation and activation entry without changing system PATH

#### Scenario: Required installation is missing or reports another version
- **WHEN** ESP-IDF v5.5.4 cannot be resolved or the active `idf.py` reports another version
- **THEN** the workflow stops before configure
- **THEN** it reports the actual result without installing or upgrading anything

### Requirement: Explicit toolchain activation
The workflow SHALL activate the existing ESP-IDF v5.5.4 environment only for the current shell session and SHALL verify the actual `idf.py`, Python, CMake, and Ninja commands selected after activation. It MUST NOT silently use unrelated tools already present on PATH.

#### Scenario: Activation selects the expected tools
- **WHEN** the existing ESP-IDF activation entry is applied to the current shell session
- **THEN** the workflow records the resolved paths and versions of `idf.py`, Python, CMake, and Ninja
- **THEN** all required tools resolve from the intended ESP-IDF v5.5.4 environment

#### Scenario: A tool resolves outside the intended environment
- **WHEN** `idf.py`, Python, CMake, or Ninja is missing, has an incompatible version, or resolves to an unrelated environment
- **THEN** the workflow stops before configure
- **THEN** it records the mismatch and does not continue with a fallback toolchain

### Requirement: Repository-owned minimal project
The repository SHALL contain a minimal ESP-IDF project under `firmware/` with only the required top-level CMake definition, one `main` component CMake definition, and one Hello World source file. The project SHALL remain independent of examples stored inside the installed ESP-IDF directory.

#### Scenario: Minimal project structure is reviewed
- **WHEN** the project skeleton is created during Apply
- **THEN** the tracked implementation consists only of `firmware/CMakeLists.txt`, `firmware/main/CMakeLists.txt`, and one minimal source file under `firmware/main/`
- **THEN** the project can be configured from the repository without copying build inputs from the installed example directory

### Requirement: Minimal app_main behavior
The Hello World `app_main` SHALL contain only ordinary build-baseline text output and MUST NOT query, configure, initialize, or control hardware or product features.

#### Scenario: Source passes static scope review
- **WHEN** the source and CMake files are reviewed before configure
- **THEN** they contain no GPIO, peripheral, serial-port, Flash, eFuse, network, storage-device, board-support, product-feature, restart, or hardware-introspection calls
- **THEN** the output text makes no claim about physical hardware validation

### Requirement: ESP32-S3 compile target
The configure workflow SHALL select only the ESP-IDF `esp32s3` target. Target selection means only that the project is compiled for the ESP32-S3 architecture and MUST NOT be interpreted as PCB V1.0 validation.

#### Scenario: Configure selects esp32s3
- **WHEN** the configure step completes successfully
- **THEN** the generated configuration and build metadata identify `esp32s3` as the target
- **THEN** no PCB V1.0, PCB V1.2, GPIO, peripheral, Flash-interface, or PSRAM claim is inferred from that target selection

#### Scenario: Another target is selected
- **WHEN** generated configuration or metadata identifies a target other than `esp32s3`
- **THEN** the workflow stops before build
- **THEN** the mismatch is recorded as a failed configure result

### Requirement: No board-level configuration
The baseline MUST NOT add a board-specific `sdkconfig.defaults`, custom bootloader configuration, custom partition table, OTA layout, 32 MiB Flash setting, Octal/OPI Flash setting, Flash-voltage setting, PSRAM setting, GPIO assignment, peripheral model, voltage assumption, partition offset, or driver configuration.

#### Scenario: Configuration inputs are inspected
- **WHEN** tracked project inputs are reviewed
- **THEN** no board-specific defaults or custom storage-layout inputs are present
- **THEN** any generated `sdkconfig` is treated only as an ignored configure output and not as a verified PCB configuration

### Requirement: Hardware evidence isolation
Existing repository facts marked `CONFIRMED` SHALL be treated only as prior documentation and SHALL NOT be revalidated by this change. `STRONGLY SUPPORTED` facts MUST NOT be implementation dependencies, and `UNVERIFIED` facts MUST NOT enter code, configuration, or acceptance criteria. PCB V1.0 and PCB V1.2 definitions MUST NOT be mixed.

#### Scenario: Planning and implementation are reviewed for evidence use
- **WHEN** the baseline artifacts and project inputs are reviewed
- **THEN** they contain no GPIO, peripheral, voltage, partition-offset, or driver value derived from `STRONGLY SUPPORTED` or `UNVERIFIED` evidence
- **THEN** they make no new `CONFIRMED` hardware claim

### Requirement: Device-free configure and build
Configure and build SHALL be host-only operations and MUST complete without enumerating, opening, monitoring, reading, writing, erasing, restoring, or otherwise accessing any serial port or physical device. The workflow MUST NOT manually, directly, or independently execute a device-side `esptool` command, use `esptool` to enumerate or open a serial port, or perform any device-side operation. An `idf.py build` invocation MAY internally call `esptool.py` only to generate host-side firmware image files, provided that it specifies no serial port, accesses no device, performs no Flash, read, erase, restore, or eFuse operation, and grants no Flash authorization to generated outputs.

#### Scenario: Configure and build run with no device
- **WHEN** the later Apply executes configure and build
- **THEN** no device connection is required
- **THEN** no serial port is enumerated or opened, and no manual, direct, or independent device-side `esptool` command is executed
- **THEN** no monitor, Flash, read, erase, restore, eFuse, motor, audio, microphone, network, battery, or power-control operation is executed

#### Scenario: ESP-IDF build internally generates firmware images
- **WHEN** `idf.py build` internally invokes `esptool.py` only to generate host-side image files
- **THEN** no serial port is specified or opened
- **THEN** no physical device is enumerated or accessed
- **THEN** no Flash, read, erase, restore, or eFuse operation occurs
- **THEN** the internal invocation is recorded as host-side image generation and is not treated as device authorization

#### Scenario: A proposed command includes device access
- **WHEN** a command would manually or directly invoke a device-side `esptool`, enumerate or open a serial port, monitor firmware, or perform a Flash, read, erase, restore, eFuse, or other device operation
- **THEN** the workflow stops before executing that command
- **THEN** the command is rejected as outside this change

### Requirement: Configure and build separation
Configure and build SHALL be separate, independently recorded steps. Neither step may be combined with Flash, monitor, erase, restore, or another device operation.

#### Scenario: Configure succeeds
- **WHEN** the environment and static-scope checks pass
- **THEN** the workflow runs only the `esp32s3` target-selection or equivalent configure operation
- **THEN** it evaluates and records configure success before starting build

#### Scenario: Configure fails
- **WHEN** configure returns a nonzero result or produces inconsistent target metadata
- **THEN** the workflow stops before build
- **THEN** it preserves and records the actual error

#### Scenario: Build runs after successful configure
- **WHEN** configure has been independently confirmed successful
- **THEN** the workflow runs a host-only build as a separate command
- **THEN** no Flash or monitor target is included

### Requirement: Ignored generated files
All configure and build outputs SHALL be written only to `firmware/build/`, `firmware/sdkconfig`, or another generated path that has first been explicitly reviewed and confirmed ignored by Git. Generated firmware binaries and configuration outputs MUST NOT be staged or committed.

#### Scenario: Ignore coverage is confirmed before configure
- **WHEN** the workflow prepares to generate files
- **THEN** it verifies with Git ignore rules that every planned generated path is ignored
- **THEN** configure does not begin if any planned generated path lacks confirmed ignore coverage

#### Scenario: Post-build Git review finds generated files
- **WHEN** the build and result recording are complete
- **THEN** Git status contains no untracked or tracked generated build product
- **THEN** any unexpected generated path causes the workflow to stop and report it without staging or committing

### Requirement: Auditable build result
The workflow SHALL create a tracked build record containing the actual activation method, resolved tool paths and versions, configure command, build command, exit results, selected target evidence, inspected artifact metadata, Git-ignore check, and any failure output. The record MUST distinguish planned, executed, successful, failed, and not tested work.

#### Scenario: Build succeeds
- **WHEN** the host-only build exits successfully
- **THEN** the record identifies the commands and toolchain actually used
- **THEN** it records only that the repository-owned project compiled for the `esp32s3` target

#### Scenario: Build fails
- **WHEN** configure, build, or inspection fails
- **THEN** the record contains the real failure result and the step where work stopped
- **THEN** it does not claim compilation success or substitute an inferred result

### Requirement: No Flash authorization or hardware-verification claim
Generated binaries SHALL have no Flash authorization. A successful build MUST NOT be described as validation of PCB V1.0, 32 MiB Flash, Octal/OPI mode, Flash voltage, PSRAM, GPIOs, peripherals, electrical behavior, runtime behavior, recovery, or device safety.

#### Scenario: Successful build is reported
- **WHEN** all host-side build acceptance checks pass
- **THEN** the final result explicitly states that Apply performed no device access and no Flash operation
- **THEN** the result explicitly states that no physical hardware behavior was verified

### Requirement: Independent PCB V1.0 smoke-test change
PCB V1.0 smoke-test specification, board support, display, screen touch, top capacitive touch, artifact-layout review for a first Flash, device connection, Flash authorization, device execution, and recovery rehearsal SHALL be handled by one or more later OpenSpec changes and MUST NOT be added to this baseline.

#### Scenario: Baseline reaches completion
- **WHEN** the repository-owned Hello World build and record satisfy this specification
- **THEN** no smoke-test implementation or device task is started
- **THEN** any PCB V1.0 smoke-test work remains pending a separate reviewed OpenSpec change

