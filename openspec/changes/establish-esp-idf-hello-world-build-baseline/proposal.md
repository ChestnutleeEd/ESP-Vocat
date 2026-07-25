## Why

The repository currently contains project documentation, OpenSpec configuration, and empty implementation directories, but no repository-owned ESP-IDF project that can be configured or compiled. A minimal, reproducible ESP-IDF v5.5.4 ESP32-S3 Hello World baseline is required to complete the constitutionally mandated first development step while keeping device access and hardware enablement completely out of scope.

Using only the example under an installed ESP-IDF directory would test one workstation copy but would not establish a reviewable, version-controlled project structure for this repository. A repository-owned minimal project makes the build inputs auditable and provides a controlled starting point for a later, independent PCB V1.0 smoke-test change.

## What Changes

### Scope

- Plan a minimal ESP-IDF project under `firmware/` containing only the required top-level CMake file, one `main` component CMake file, and one minimal Hello World source file.
- Require the existing ESP-IDF v5.5.4 installation and the `esp32s3` build target without installing or upgrading any dependency.
- Limit `app_main` to ordinary build-baseline output; it will not configure GPIOs, initialize peripherals, inspect the device, or encode product behavior.
- Define separate configure and build operations for a later Apply session. Both operations are host-only and must run without a connected device.
- Require generated files to remain in `firmware/build/`, `firmware/sdkconfig`, or another path that is explicitly reviewed and confirmed ignored before use.
- Require an auditable build record containing the actual commands, resolved tool versions and paths, outcome, and truthful interpretation of the result.
- Establish that compilation success means only that the repository-owned project compiled for the ESP32-S3 target. It does not verify PCB V1.0, Flash, PSRAM, GPIOs, peripherals, electrical behavior, runtime behavior, or recovery.
- Establish that generated binaries have no Flash authorization.
- Reserve PCB V1.0 smoke-test specification, implementation, artifact review, device access, and execution for a separate OpenSpec change.

### Hardware evidence boundary

- `CONFIRMED` means only that existing repository documents record a fact as verified on the current physical device. This change does not reconnect to or revalidate the device.
- `STRONGLY SUPPORTED` hardware information is not an implementation dependency of this change.
- `UNVERIFIED` hardware information must not appear in this change's code, configuration, or acceptance criteria.
- PCB V1.0 and PCB V1.2 definitions must not be mixed.
- GPIOs, peripheral models, voltages, partition offsets, driver settings, Flash interface settings, and PSRAM settings must not be guessed.

### Non-goals

- Serial-port enumeration, serial connection, serial monitoring, or any other device connection.
- Use of `esptool`.
- Flash reads, writes, erasure, restoration, or any authorization to Flash generated binaries.
- eFuse operations or irreversible security configuration.
- Bootloader, partition-table, OTA, or recovery-layout design changes.
- Board-level configuration or physical validation of 32 MiB Flash, Octal/OPI Flash, 1.8 V Flash, PSRAM, or any storage characteristic.
- Any GPIO configuration, probing, scanning, or LED blink behavior.
- Display or touch support, including ST77916, CST816S, or top capacitive touch.
- Audio support, including ES7210, ES8311, microphone, speaker, amplifier, or source localization.
- Wi-Fi or Bluetooth.
- Base UART, motor, physical movement, or direction estimation.
- SD card support, battery/fuel-device access, or power-control behavior.
- Windows companion application work.
- Cat animation, UI, reminders, voice behavior, assets, or formal product firmware.
- Any physical-device or hardware verification.
- Any claim that compilation proves PCB identity, Flash or PSRAM configuration, GPIO correctness, peripheral correctness, electrical safety, runtime stability, or recovery readiness.

## Capabilities

### New Capabilities

- `esp-idf-host-build-baseline`: Defines the reproducible, auditable, device-free ESP-IDF v5.5.4 ESP32-S3 configure-and-build baseline.

### Modified Capabilities

None.

## Impact

- **Firmware:** A later Apply may create only the minimal build-baseline project files under `firmware/`; no board support, drivers, product modules, or board-specific `sdkconfig.defaults` are included.
- **Companion application:** No impact.
- **Documentation:** This change's artifacts and later build-result record document the baseline; existing constitution, hardware profile, product specification, architecture, and agent rules are not changed by this proposal.
- **Assets:** No impact.
- **Tests and verification:** Host-side static review, configure, build, artifact metadata inspection, Git-ignore verification, and truthful result recording only.
- **Dependencies:** Reuse the existing ESP-IDF v5.5.4 installation. No dependency installation or upgrade is permitted.
- **Operations:** Build is included for a later Apply. Device connection, Flash write, erase, restore, eFuse, motor, audio, microphone, network, and power-control operations are excluded.
