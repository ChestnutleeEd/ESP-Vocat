# AGENTS.md
## 1. Project identity
This repository develops custom software for:
- Device: ESP-VoCat
- PCB revision: V1.0
- MCU: ESP32-S3
- Flash: 32 MiB, octal, 1.8 V
- PSRAM: 16 MiB
- Firmware framework: ESP-IDF v5.5.4 development environment
- Product direction: embodied desktop cat assistant
The repository includes:
- ESP32 firmware
- Windows companion application
- cat animation and audio assets
- host-side tools
- specifications
- hardware tests
The current project stage is specification and preparation.
Do not assume that formal firmware implementation or device flashing has been authorized.
---
## 2. Mandatory reading order
Before planning or changing files, read these documents in order:
1. `PROJECT_CONSTITUTION.md`
2. `docs/HARDWARE_PROFILE.md`
3. `docs/PRODUCT_SPEC_DRAFT.md`
4. the specification under `specs/` for the current task
5. relevant decision records under `docs/decisions/`
6. relevant test records under `tests/`
If any required document is missing or contradictory, report the conflict before implementation.
The project constitution has the highest priority.
---
## 3. Hardware evidence levels
Hardware information is classified as:
- `CONFIRMED`
- `STRONGLY SUPPORTED`
- `UNVERIFIED`
Rules:
1. Do not treat reference source code as physical-device confirmation.
2. Do not promote evidence levels without a recorded test.
3. Do not guess GPIO assignments.
4. Do not mix PCB V1.0 and PCB V1.2 definitions.
5. Do not copy generic ESP32-S3 board configuration into this project.
6. Isolate unresolved hardware behavior in the smallest safe test.
---
## 4. Absolute prohibitions
Never perform, recommend as routine, or silently include:
- eFuse writes
- Secure Boot activation
- Flash Encryption activation
- VDD_SPI or Flash-voltage changes
- JTAG disable
- USB Serial/JTAG disable
- download-mode disable
- security-key changes
- `erase_flash`
- full-device erase
- irreversible security configuration
- arbitrary GPIO output experiments
- unverified motor commands
- modification of recovery backup files
Do not execute a Flash write unless the user explicitly authorizes that exact write after reviewing:
- target port
- target chip
- PCB revision
- binary files
- offsets
- partition layout
- recovery image
- recovery-image SHA-256
---
## 5. Recovery assets
Recovery backups are immutable.
Primary backup directory:
`E:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi`
Cross-disk backup directory:
`D:\ESP-VoCat_Backup\2026-07-10_original_xiaozhi`
Expected full-image SHA-256:
`72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001`
Never:
- use a backup directory as build output;
- rename or patch recovery files;
- place source code inside a backup directory;
- commit private NVS dumps;
- expose credentials or tokens from NVS.
A missing backup or hash mismatch is a stop condition.
---
## 6. Command classification
Before presenting or executing a device-related command, label it as exactly one of:
- `READ-ONLY`
- `REBOOT`
- `WRITE`
- `ERASE`
- `IRREVERSIBLE`
Explain:
1. what the command does;
2. why it is necessary;
3. what data or hardware it may affect;
4. how success will be judged;
5. what result requires stopping.
Computer-only file creation, documentation and compilation should also be described accurately, but do not need to be presented as hardware writes.
---
## 7. Work granularity
Work in small, independently testable increments.
For every implementation task:
1. restate the exact scope;
2. identify files expected to change;
3. list assumptions;
4. identify unresolved hardware facts;
5. define acceptance criteria;
6. implement only the requested scope;
7. build or test where appropriate;
8. report actual results;
9. update documentation.
Do not add adjacent features merely because they seem useful.
Do not combine unrelated refactoring with hardware enablement.
Do not rewrite working modules without a task-specific reason.
---
## 8. Required development sequence
The initial sequence is fixed:
1. compile ESP-IDF Hello World without flashing;
2. create a PCB V1.0-specific smoke-test specification;
3. implement the smoke-test firmware;
4. review build artifacts and Flash layout;
5. obtain explicit approval for the first Flash write;
6. verify serial output;
7. verify display output;
8. verify screen touch;
9. verify top capacitive touch;
10. restore the complete original Xiaozhi image;
11. verify original-device recovery;
12. begin formal custom-cat firmware work.
Do not skip the restoration rehearsal.
---
## 9. First smoke-test boundary
The first smoke-test firmware may include only:
- serial boot output;
- display initialization;
- solid-color screen test;
- simple test text;
- screen-touch coordinate reporting;
- top capacitive-touch reporting.
It must not enable:
- Wi-Fi
- Bluetooth
- microphone
- speaker
- amplifier
- source localization
- motorized base
- SD interface
- battery-control writes
- main-power-control writes
- arbitrary LEDs or GPIOs
A generic blink example must not select an arbitrary GPIO.
---
## 10. Build and Flash separation
Treat these as separate actions:
- configure
- build
- inspect artifacts
- connect device
- flash
- monitor
- restore
Do not use combined commands such as `flash monitor` during early development unless explicitly approved.
A successful build proves only that the code compiled.
It does not prove:
- pin correctness;
- electrical safety;
- display correctness;
- touch correctness;
- audio correctness;
- motor safety;
- runtime stability.
---
## 11. Firmware architecture boundaries
Keep clear module boundaries for:
- board support package
- display driver
- rendering
- cat animation state machine
- screen touch
- top capacitive touch
- audio input
- audio output
- wake word
- fixed voice commands
- reminders and schedules
- local persistence
- USB protocol
- companion connection
- motorized base
- settings
- diagnostics
Do not place product behavior directly inside low-level drivers.
Do not place hardware-register operations inside UI or animation code.
A nonessential module failure should not prevent the local cat experience from starting whenever technically possible.
---
## 12. Performance requirements
Smooth animation is a primary requirement.
Targets:
- major interaction and transition animation: 30 FPS
- normal idle animation: 15-20 FPS
- slow sleep animation: 10-15 FPS
- boot to interactive home: within 5 seconds
Do not block rendering, touch, audio or reminders with:
- network work;
- long storage operations;
- speech processing;
- USB parsing;
- asset decoding;
- motor communication.
Measure rather than assume:
- frame rate;
- frame time;
- heap;
- PSRAM usage;
- Flash usage;
- task stack margin;
- watchdog behavior;
- input latency.
Memory-allocation failures, watchdog resets and task starvation are release blockers.
---
## 13. Offline-first requirements
The following must remain functional without the companion application:
- cat idle behavior;
- screen interaction;
- top petting;
- local cat sounds;
- supported local fixed commands;
- cached schedules;
- reminder triggering;
- local settings;
- safe predefined base actions after verification.
Weather and Codex usage may be cached.
Cached external data must show:
- last update time;
- connection status;
- stale state when applicable.
Never present cached data as live data.
---
## 14. Companion application boundary
The Windows companion application may handle:
- weather retrieval;
- Codex-usage retrieval or aggregation;
- complex reminder parsing;
- dynamic speech generation;
- complete reminder history;
- configuration;
- synchronization.
The ESP32 must not store:
- ChatGPT browser cookies;
- reusable account sessions;
- passwords;
- unnecessary private tokens.
The companion application must not permanently assume `COM7`.
Device discovery must use a handshake or stable device identity.
---
## 15. USB protocol rules
The future USB protocol must be:
- versioned;
- length-bounded;
- validated;
- tolerant of malformed input;
- explicit about data freshness;
- backward-compatible where practical.
Unknown or malformed commands must be rejected safely.
A malformed host message must not:
- crash the device;
- reset the device;
- write outside a buffer;
- alter unrelated settings;
- activate physical movement.
---
## 16. Motor safety
Do not activate the base until both wiring and protocol are verified.
Motor implementation must include:
- bounded angle or duration;
- bounded speed where supported;
- stop command;
- cooldown;
- repeated-command suppression;
- direction-confidence threshold;
- user-configurable automatic turning.
Low-confidence sound direction must not produce physical movement.
Display animation may simulate looking toward a sound without moving the base.
---
## 17. Audio and privacy
Microphone modes:
1. wake-word mode;
2. touch-to-listen mode;
3. disabled mode.
Disabled mode must stop unnecessary capture and recognition processing.
The UI must show microphone state.
Cat interaction audio and information narration must remain conceptually separate.
Do not allow audio processing to starve:
- touch;
- display refresh;
- reminders;
- USB communication.
---
## 18. Documentation rule
A decision existing only in a chat is not fully recorded.
Update relevant documentation when changing:
- product behavior;
- hardware evidence;
- architecture;
- protocol;
- partition layout;
- test procedure;
- safety boundary;
- acceptance criteria.
Do not silently replace uncertainty with certainty.
Record:
- what changed;
- why it changed;
- supporting evidence;
- affected files;
- remaining risks.
---
## 19. Test honesty
Never fabricate:
- compilation success;
- hardware output;
- screen behavior;
- touch behavior;
- measured performance;
- restored functionality;
- passing tests.
Clearly distinguish:
- implemented;
- compiled;
- host-tested;
- device-tested;
- verified;
- not tested.
When a command fails, preserve and analyze the real error output.
---
## 20. Interaction with the user
The user is new to hardware development.
For hardware operations:
- give one atomic step at a time;
- explain the purpose first;
- provide one complete copyable command block;
- classify risk;
- explain the minimum necessary concept;
- state the most important judgment point;
- wait for the actual output before advancing.
Do not overload the user with several device operations at once.
Do not ask the user to run a dangerous command merely to gather information that can be obtained read-only.
---
## 21. Current project status
Currently established documents:
- `PROJECT_CONSTITUTION.md`
- `docs/HARDWARE_PROFILE.md`
- `docs/PRODUCT_SPEC_DRAFT.md`
- `AGENTS.md`
Current stage:
- product design documentation;
- hardware evidence consolidation;
- development-process definition.
Not yet authorized:
- formal firmware implementation;
- Flash write;
- Flash erase;
- motor testing;
- audio testing;
- partition redesign.
---
## 22. Definition of done
A task is complete only when:
- scope is satisfied;
- constitutional rules are respected;
- assumptions are documented;
- relevant builds or tests were actually run;
- results are reported honestly;
- acceptance criteria are met;
- documentation is updated;
- unresolved risks are stated.
Compilation alone is never the complete definition of done for hardware behavior.
