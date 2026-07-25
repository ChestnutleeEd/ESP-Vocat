## Context

The repository has a documented ESP-VoCat PCB V1.0 product and safety baseline but no compilable ESP-IDF project. The constitution and DEC-023 require the first development step to compile ESP-IDF Hello World without Flashing, before a dedicated PCB V1.0 smoke-test firmware is specified or implemented.

This design creates the smallest repository-owned project that can verify the existing ESP-IDF v5.5.4 host toolchain and the `esp32s3` compile target. It deliberately does not establish a board configuration. Existing hardware facts remain evidence recorded by repository documents; this change does not access the device or revalidate them.

Stakeholders are the project owner and future contributors who need an auditable, repeatable first build without crossing the hardware-safety boundary.

### Assumptions

- The existing ESP-IDF v5.5.4 installation remains available and has a usable PowerShell activation entry.
- Apply will start from the named feature branch with a reviewable worktree.
- The build can complete without a connected ESP-VoCat or any serial device.
- `firmware/build/` and `firmware/sdkconfig` remain covered by `.gitignore`.

### Unresolved hardware facts

Display, touch, audio, GPIO, Flash-interface, PSRAM, storage-layout, motor, SD, battery, and power-control details remain outside this change regardless of evidence level. `CONFIRMED` means only that current repository documentation records prior physical verification; `STRONGLY SUPPORTED` information is not an implementation dependency; `UNVERIFIED` information cannot enter code, configuration, or acceptance criteria.

### Expected affected files

During a later Apply, tracked implementation and result files are limited to:

```text
firmware/
├── CMakeLists.txt
└── main/
    ├── CMakeLists.txt
    └── main.c

tests/
└── build/
    └── esp-idf-hello-world-build-baseline.md
```

Generated, ignored files are limited to:

```text
firmware/build/
firmware/sdkconfig
firmware/sdkconfig.old   # only if ESP-IDF generates it
```

No existing constitution, hardware profile, product specification, architecture document, agent instruction, companion application, or asset is changed by this design.

## Goals / Non-Goals

**Goals:**

- Establish a repository-owned minimal ESP-IDF project.
- Use exactly the existing ESP-IDF v5.5.4 environment.
- Configure only the ESP-IDF `esp32s3` compile target.
- Keep configure, build, and inspection as separate host-only steps.
- Make tool selection, commands, outputs, and conclusions auditable.
- Ensure generated files remain ignored and generated binaries receive no Flash authorization.
- Leave a clean boundary for a later PCB V1.0 smoke-test change.

**Non-Goals:**

- Device discovery, serial access, monitor, `esptool`, Flash, erase, restore, eFuse, or recovery execution.
- Board-specific `sdkconfig.defaults`, custom bootloader, partition table, OTA, Flash, OPI, PSRAM, GPIO, voltage, or peripheral configuration.
- LED blink, display, touch, audio, network, UART base, motor, SD, battery, or power control.
- Windows companion, cat animation, UI, product services, or formal firmware architecture.
- Physical-device validation or any claim that compilation validates hardware.
- Installation, upgrade, downgrade, repair, or reinitialization of tools.

## Decisions

### Decision 1: Keep the minimal project at `firmware/`

The first project uses the repository's intended firmware root rather than a temporary directory or the installed ESP-IDF examples tree. This makes the build inputs version-controlled and provides a stable root for later changes.

**Alternative considered:** Compile `examples/get-started/hello_world` directly from the ESP-IDF installation. This can test one installation but does not establish an auditable repository baseline and couples validation to files outside the repository.

### Decision 2: Use only three minimal tracked project files

The project contains the top-level `firmware/CMakeLists.txt`, `firmware/main/CMakeLists.txt`, and `firmware/main/main.c`. No components, board-support package, custom partitions, dependencies, or configuration defaults are introduced.

The top-level CMake file uses the standard ESP-IDF project include and declares the baseline project. The component CMake file registers only `main.c` with no private include tree or external component dependency.

**Alternative considered:** Copy the complete official Hello World example. That would import extra example behavior and metadata not needed for this baseline and would make the scope harder to audit.

### Decision 3: Keep `app_main` hardware-neutral

`app_main` emits one ordinary string identifying the ESP-IDF Hello World host-build baseline and then returns. It does not call chip-information APIs, inspect Flash, configure GPIO, initialize a peripheral, connect to a network, restart, delay in a loop, or express a PCB claim.

**Alternative considered:** Reuse the official example's chip, Flash, core-count, and restart reporting. Those behaviors imply runtime hardware inspection and are unnecessary for a compile-only baseline.

### Decision 4: Do not create `sdkconfig.defaults`

Only `idf.py set-target esp32s3` or an equivalent ESP-IDF configure operation selects the compile architecture. A board-level defaults file would prematurely choose settings for Flash size, Octal/OPI mode, voltage, PSRAM, console, bootloader, partition layout, or other hardware behavior.

Generated `firmware/sdkconfig` is an ignored configure output, not a reviewed PCB configuration and not hardware evidence.

**Alternative considered:** Configure the documented 32 MiB Flash, OPI Flash, and PSRAM now. Even without Flashing, that would blur the boundary between toolchain validation and PCB configuration and would make later reviewers more likely to treat the image as board-ready.

### Decision 5: Activate the existing environment only in the current shell

Apply resolves the existing v5.5.4 activation entry, applies it to the current PowerShell session, then confirms the resolved `idf.py`, Python, CMake, and Ninja paths and versions. It does not persist PATH changes or fall back to the pre-existing Anaconda Python or unrelated host tools.

If activation fails, reports a different ESP-IDF version, or selects an unexpected tool, Apply stops before configure. It does not install or repair dependencies.

**Alternative considered:** Invoke tools through whichever commands are already on PATH. Prior exploration showed that the unactivated shell may not expose `idf.py`, CMake, or Ninja and may expose an unrelated Python, so silent PATH fallback is not reproducible.

### Decision 6: Separate configure, build, and inspection

The sequence is:

```text
environment validation
→ static scope review
→ configure/select esp32s3
→ verify configure result
→ build
→ inspect host-side metadata
→ record actual result
→ verify Git state
```

Configure and build use distinct commands and results. No command includes `flash`, `monitor`, `erase`, `restore`, a port, or `esptool`.

**Alternative considered:** A combined configure/build or `flash monitor` command. Combined device targets violate the project sequence and make it harder to stop on configuration errors.

### Decision 7: Constrain generated paths before generation

Before configure, Apply uses Git's ignore evaluation to verify all expected generated paths. The normal build directory is `firmware/build/`; generated `firmware/sdkconfig` and `firmware/sdkconfig.old` are also ignored. Any additional path must be identified and confirmed ignored before it is used.

After build, Git status is reviewed. Unexpected generated or tracked build outputs are a stop condition; they are not staged, committed, moved, or deleted automatically.

**Alternative considered:** Run the build and update `.gitignore` afterward if needed. That could expose generated firmware or configuration as untracked data and is inconsistent with the required inspect-before-write sequence.

### Decision 8: Store a tracked, truthful build record

Apply writes `tests/build/esp-idf-hello-world-build-baseline.md` after executing the relevant steps. The record contains:

- date, branch, and commit context;
- activation entry used;
- resolved `idf.py`, Python, CMake, and Ninja paths and versions;
- exact configure and build commands;
- exit codes and concise real output or error references;
- evidence that the configured target is `esp32s3`;
- inspected artifact names and sizes as host-side metadata only;
- Git-ignore and final worktree observations;
- explicit declarations that no device was accessed, no Flash operation occurred, generated binaries are unauthorized for Flash, and no hardware was verified.

A failed build is still recorded honestly with its stop point. No success is inferred.

**Alternative considered:** Report the result only in chat. Project rules require durable records for meaningful engineering results.

### Decision 9: Keep the smoke test in a separate Change

This baseline ends after host-side build inspection and recording. PCB V1.0 board support, display, screen touch, top touch, binary-offset and partition review for a first write, connection, Flash approval, monitoring, restoration, and recovery verification require a dedicated later OpenSpec change.

This separation prevents `STRONGLY SUPPORTED` or `UNVERIFIED` board data from becoming an accidental dependency of the toolchain baseline.

## Risks / Trade-offs

- **[Risk] The minimal project can produce Flashable-looking binaries even though it is not board-ready.** → Treat every binary as host-build output only, keep it ignored, and state that it has no Flash authorization.
- **[Risk] `set-target` generates default configuration that could be mistaken for PCB V1.0 settings.** → Do not track `sdkconfig`; explicitly document that it is generated and unverified.
- **[Risk] Session activation may select a different tool than expected.** → Record resolved paths and versions and stop before configure on any mismatch.
- **[Risk] ESP-IDF may create an unexpected generated path.** → Verify ignore coverage before generation and stop on unexpected post-build Git status; do not automatically delete files.
- **[Risk] A successful compile may create false confidence about hardware.** → Use narrowly worded acceptance criteria and repeat the no-hardware-verification statement in the build record.
- **[Trade-off] Omitting board defaults means the output is not intended for the real PCB.** → This is intentional; safe PCB configuration belongs to the later smoke-test change.
- **[Trade-off] A repository-owned example duplicates a few lines from the conceptual ESP-IDF Hello World structure.** → The small duplication provides stable, reviewable inputs and avoids dependence on an external example tree.

## Migration Plan

1. Apply creates the three minimal project files and the build record only.
2. If configure or build fails, retain the tracked project inputs and record the actual failure; do not install tools, access hardware, or fabricate completion.
3. If the minimal baseline must be rolled back before adoption, revert only the files introduced by this change through a separately authorized Git operation. Generated files remain ignored and are not automatically deleted by this plan.
4. A later PCB V1.0 smoke-test change may evolve the firmware project only after it defines its own evidence, configuration, safety, artifact-review, and recovery requirements. It must not reinterpret this baseline build as device validation.

## Open Questions

None required before Apply. Apply must use the existing v5.5.4 installation and stop for user direction if its activation entry or resolved tools do not match the recorded environment.
