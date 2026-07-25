## 1. Preflight and safety checks

- [x] 1.1 **[READ-ONLY]** Confirm the repository root, branch `feat/establish-esp-idf-hello-world-build-baseline`, expected starting HEAD, worktree state, OpenSpec 1.6.0 health, and this Change's Apply instructions; stop on any mismatch.
- [x] 1.2 **[READ-ONLY]** Resolve the existing ESP-IDF v5.5.4 installation and PowerShell activation entry without activating tools, installing dependencies, upgrading dependencies, or changing persistent environment variables.
- [x] 1.3 **[READ-ONLY]** Review `.gitignore` with Git ignore checks for `firmware/build/`, `firmware/sdkconfig`, `firmware/sdkconfig.old`, and representative binary/map outputs; stop before file generation if any planned output is not ignored.

## 2. Minimal repository project

- [x] 2.1 **[WRITE]** Create only the minimal top-level `firmware/CMakeLists.txt` required for a standard ESP-IDF project and do not add board, bootloader, partition, OTA, Flash, PSRAM, or peripheral configuration.
- [x] 2.2 **[WRITE]** Create only `firmware/main/CMakeLists.txt`, registering one `main.c` source without external dependencies, board components, or additional include trees.
- [x] 2.3 **[WRITE]** Create `firmware/main/main.c` with an `app_main` that emits one ordinary Hello World build-baseline string and performs no hardware introspection, delay loop, restart, GPIO, peripheral, serial, Flash, eFuse, network, storage-device, or product operation.
- [x] 2.4 **[READ-ONLY]** Statically review the three project files and confirm they contain no PCB revision definitions, GPIO numbers, peripheral models, voltage values, partition offsets, board-specific `sdkconfig.defaults`, device commands, or dependencies on `STRONGLY SUPPORTED` or `UNVERIFIED` hardware facts.

## 3. Existing toolchain activation and verification

- [x] 3.1 **[READ-ONLY — process-local environment only]** Activate the existing ESP-IDF v5.5.4 environment in the current PowerShell session without modifying system/user PATH, installing tools, repairing tools, or writing repository files; stop if activation fails.
- [x] 3.2 **[READ-ONLY]** Record the resolved paths and versions for `idf.py`, Python, CMake, and Ninja; stop before configure if ESP-IDF is not exactly v5.5.4 or any tool resolves to an unrelated or incompatible environment.
- [x] 3.3 **[READ-ONLY]** Review the exact planned configure and build commands and confirm they contain no serial port, `monitor`, `flash`, `erase`, `restore`, `esptool`, eFuse, device, motor, audio, microphone, network, battery, or power-control operation.

## 4. Host-only configure, build, and inspection

- [ ] 4.1 **[WRITE — ignored generated files only]** From `firmware/`, run `idf.py set-target esp32s3` or the reviewed equivalent configure operation as a standalone command, allowing output only under previously confirmed ignored paths; preserve the real error and stop before build if configure fails.
- [ ] 4.2 **[READ-ONLY]** Inspect generated configuration and build metadata to confirm the selected compile target is `esp32s3`; treat generated `sdkconfig` as unverified build configuration and stop if the target is inconsistent.
- [ ] 4.3 **[WRITE — ignored generated files only]** Run a standalone host-only `idf.py build` from `firmware/` with no device attached or required and with no Flash or monitor target; preserve the real output and stop subsequent build claims if it fails.
- [ ] 4.4 **[READ-ONLY]** Inspect only host-side build metadata, artifact names, and artifact sizes needed to audit compilation; do not inspect a device, infer board settings, authorize a binary for Flash, or claim runtime/hardware correctness.

## 5. Result recording and final review

- [ ] 5.1 **[READ-ONLY]** Re-run Git ignore checks and Git status after the build; confirm generated configuration, binary, ELF, map, and build-system outputs are ignored and stop without staging, committing, deleting, or relocating files if an unexpected generated path appears.
- [ ] 5.2 **[WRITE]** Create `tests/build/esp-idf-hello-world-build-baseline.md` with the actual date and Git context, activation entry, resolved tool paths and versions, exact configure/build commands, exit results, target evidence, inspected artifact metadata, ignore checks, failures if any, and the truthful completion state.
- [ ] 5.3 **[READ-ONLY]** Perform the final scope and safety audit and report Git status, explicitly stating that no device was enumerated or connected, no `esptool` or Flash/eFuse/erase/restore operation ran, generated binaries have no Flash authorization, compilation is not hardware verification, and PCB V1.0 smoke testing remains a separate OpenSpec Change.

No task in this Change authorizes a Git commit, push, merge, archive, device connection, Flash operation, or hardware test.
