## ADDED Requirements

### Requirement: PCB V1.0-only implementation
The implementation SHALL target ESP-VoCat PCB V1.0 only and SHALL expose no runtime PCB revision selection.

#### Scenario: V1.0 target is explicit
- **WHEN** the future source and generated configuration are reviewed
- **THEN** every board identity SHALL state PCB V1.0 and the host audit SHALL pass

#### Scenario: Another revision appears
- **WHEN** source, configuration, or linked symbols support PCB V1.2, generic boards, or revision auto-detection
- **THEN** host validation SHALL fail before build acceptance

### Requirement: Evidence level preservation
The Change SHALL label the display hardware baseline `STRONGLY SUPPORTED — REQUIRES FUTURE DEVICE VALIDATION` and SHALL keep physical outcomes `UNVERIFIED`.

#### Scenario: Host-source fact is recorded
- **WHEN** a fixed source proves a software value
- **THEN** the record SHALL distinguish confirmed source content from strongly supported hardware mapping

#### Scenario: Hardware certainty is overstated
- **WHEN** any document or acceptance result calls an untested display mapping or physical outcome `CONFIRMED`
- **THEN** documentation validation SHALL fail

### Requirement: ST77916 controller baseline
The implementation SHALL configure only an ST77916 panel controller and SHALL not silently substitute another controller.

#### Scenario: Controller configuration is audited
- **WHEN** source and component metadata are inspected
- **THEN** ST77916 SHALL be the sole configured LCD controller

#### Scenario: Another controller is present
- **WHEN** a different LCD controller initializer or linked panel driver is detected
- **THEN** host validation SHALL fail

### Requirement: 360 by 360 display baseline
The implementation SHALL use a 360 × 360 active area, zero X/Y offsets, and half-open draw coordinates within `[0,360)`.

#### Scenario: Pattern bounds are valid
- **WHEN** host tests enumerate all strip windows
- **THEN** exactly 360 rows and 360 columns SHALL be covered without overlap or overrun

#### Scenario: Geometry differs
- **WHEN** width, height, offsets, or a draw endpoint exceeds the baseline
- **THEN** host validation SHALL fail

### Requirement: QSPI SPI2_HOST mapping
The implementation SHALL use QSPI on `SPI2_HOST`, a clock ceiling of 40 MHz, 32 command bits, 8 parameter bits, quad data mode, and QSPI command-phase encoding.

#### Scenario: QSPI configuration matches
- **WHEN** bus and panel-IO constants are audited
- **THEN** every transport field SHALL equal the specified baseline

#### Scenario: Transport changes
- **WHEN** another SPI host, non-QSPI transport, or clock above 40 MHz is detected
- **THEN** host validation SHALL fail

### Requirement: Strict GPIO allowlist
The only display GPIOs SHALL be `{18,14,46,13,11,12,3,44}` for CLK, CS, D0, D1, D2, D3, reset, and backlight respectively; DC SHALL be `GPIO_NUM_NC`.

#### Scenario: All GPIOs trace to the allowlist
- **WHEN** literals, macros, API calls, generated configuration, and linked symbols are scanned
- **THEN** every display GPIO occurrence SHALL map to its one permitted role

#### Scenario: Unlisted GPIO appears
- **WHEN** any unexplained display GPIO outside the exact set is found
- **THEN** host validation SHALL fail

### Requirement: PCB V1.2 denylist
The implementation SHALL exclude PCB V1.2 reset, clock, power, BSP, and auto-detection definitions.

#### Scenario: Denylist is absent
- **WHEN** the full source/configuration/symbol audit runs
- **THEN** no unexplained V1.2 definition SHALL be present

#### Scenario: V1.2 configuration is detected
- **WHEN** reset47, 80 MHz, V1.2 power behavior, auto-detection, or a V1.2 BSP name is found
- **THEN** host validation SHALL fail

### Requirement: GPIO9 LCD_EN prohibition
GPIO9 SHALL NOT be used as LCD power, LCD_EN, or any display-related control because controlling project evidence associates it with main power control.

#### Scenario: GPIO9 is absent from display code
- **WHEN** source and generated artifacts are scanned for `GPIO_NUM_9`, GPIO literal 9 in display APIs, and LCD_EN mappings
- **THEN** no unexplained match SHALL exist

#### Scenario: GPIO9 display use appears
- **WHEN** GPIO9 is assigned to LCD power or LCD_EN
- **THEN** implementation acceptance SHALL be blocked

### Requirement: GPIO47 reset prohibition
GPIO47 SHALL NOT be used as LCD reset or any display signal.

#### Scenario: V1.0 reset is used
- **WHEN** reset configuration is inspected
- **THEN** reset SHALL be GPIO3 active low and GPIO47 SHALL be absent

#### Scenario: GPIO47 appears
- **WHEN** `GPIO_NUM_47` or equivalent reset47 configuration is found
- **THEN** host validation SHALL fail

### Requirement: GPIO45 DC prohibition
GPIO45 SHALL NOT be used as DC; QSPI DC SHALL remain `GPIO_NUM_NC`.

#### Scenario: QSPI command phase supplies DC semantics
- **WHEN** panel IO is configured
- **THEN** DC SHALL be `GPIO_NUM_NC` and command-phase encoding SHALL be enabled

#### Scenario: GPIO45 DC appears
- **WHEN** `GPIO_NUM_45` or equivalent DC45 configuration is found
- **THEN** host validation SHALL fail

### Requirement: 80 MHz LCD clock prohibition
The LCD QSPI clock SHALL NOT exceed 40 MHz, and 80 MHz LCD configuration SHALL be prohibited.

#### Scenario: Clock is at or below ceiling
- **WHEN** source and generated configuration are audited
- **THEN** the LCD clock SHALL be a fixed value no greater than 40 MHz

#### Scenario: 80 MHz is configured
- **WHEN** `80000000`, `80 MHz`, or an equivalent LCD clock is found in implementation scope
- **THEN** host validation SHALL fail

### Requirement: Fixed dependency version
The future manifest SHALL declare `espressif/esp_lcd_st77916: "==1.0.1"` and SHALL NOT use a caret, wildcard, floating branch, or latest version.

#### Scenario: Exact component resolves
- **WHEN** dependency metadata and the component lock are inspected
- **THEN** the resolved component SHALL be exactly v1.0.1 and its integrity evidence SHALL be recorded

#### Scenario: Dependency floats or differs
- **WHEN** the declaration can resolve to another version or the lock reports another version
- **THEN** build acceptance SHALL fail

### Requirement: Locked initialization-sequence source
The implementation SHALL preserve all 365 entries of `vendor_specific_init_yysj` from commit `49ac8a6da399f27a9546d4f73640b7f86c24bac6`, file `main/boards/esp-vocat/esp_vocat.cc`.

#### Scenario: Table identity matches
- **WHEN** source and implementation initializer bodies are whitespace-normalized and hashed
- **THEN** both SHALL have SHA-256 `e8a1f2ea307b51be59d3daa201bb444b5f5ddc2d8da2931fda6e91579c4522be`, 365 entries, and the final 0x21/0x11/120 ms sequence

#### Scenario: Table is defaulted or altered
- **WHEN** the component default is used, an entry differs, or the source cannot be reproduced
- **THEN** implementation acceptance SHALL be blocked

### Requirement: No PSRAM
The minimal display test SHALL neither require nor allocate PSRAM.

#### Scenario: Internal-only implementation
- **WHEN** source, sdkconfig, map, and symbols are audited
- **THEN** the display path SHALL contain no PSRAM dependency or allocation

#### Scenario: PSRAM is required
- **WHEN** a display buffer or startup path depends on external RAM
- **THEN** host validation SHALL fail

### Requirement: No LVGL
The minimal display test SHALL not include, configure, initialize, or link LVGL.

#### Scenario: Direct pattern path is used
- **WHEN** the component tree and symbols are reviewed
- **THEN** rendering SHALL use only the minimal pattern generator and panel API

#### Scenario: LVGL appears
- **WHEN** LVGL source, configuration, symbols, or initialization is detected
- **THEN** host validation SHALL fail

### Requirement: No touch
The Change SHALL not initialize, read, configure, or link screen touch or top capacitive touch.

#### Scenario: Touch remains absent
- **WHEN** source and linked-component scans run
- **THEN** no touch driver or touch GPIO/API SHALL appear

#### Scenario: Touch path appears
- **WHEN** any screen-touch or top-touch initializer is detected
- **THEN** host validation SHALL fail

### Requirement: No unrelated peripherals
The display smoke test SHALL exclude audio, microphone, speaker, amplifier, motor, SD, battery/power control, network, Bluetooth, Wi-Fi, NVS/OTA, filesystem, and board-level peripheral initialization.

#### Scenario: Display-only component tree
- **WHEN** linked components, symbols, source calls, and sdkconfig are audited
- **THEN** only the minimal display dependencies SHALL be present

#### Scenario: Unrelated peripheral appears
- **WHEN** an excluded peripheral initializer or service is found
- **THEN** host validation SHALL fail

### Requirement: Minimal strip-buffer rendering
The renderer SHALL use one reusable strip sized for at most 360 × 80 RGB565 pixels and SHALL submit half-open row windows covering all 360 rows once.

#### Scenario: Five strips render the pattern
- **WHEN** host tests execute the pattern generator
- **THEN** four 80-row strips and one 40-row strip SHALL produce the specified black/RGB/white/text output

#### Scenario: Strip bounds are invalid
- **WHEN** a strip overruns 57,600 bytes, overlaps rows, omits rows, or uses stale bytes
- **THEN** host tests SHALL fail

### Requirement: DMA-capable internal memory
The strip SHALL be allocated from memory satisfying `MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL`, shall be at least four-byte aligned, and SHALL be null-checked.

#### Scenario: Allocation is valid
- **WHEN** the future implementation and map/size evidence are reviewed
- **THEN** one 57,600-byte internal DMA allocation and sufficient margin SHALL be demonstrated

#### Scenario: Allocation fails or is ineligible
- **WHEN** allocation returns null, lacks required capabilities, is misaligned, or memory margin is not demonstrated
- **THEN** the state machine SHALL enter FAIL_SAFE and host acceptance SHALL fail as applicable

### Requirement: No full framebuffer
The implementation SHALL not allocate a 360 × 360 framebuffer or a second strip buffer.

#### Scenario: One strip is present
- **WHEN** allocation sites, map data, and symbols are audited
- **THEN** exactly one display pixel buffer of at most 57,600 bytes SHALL exist

#### Scenario: Larger or duplicate buffer appears
- **WHEN** a full framebuffer, double buffer, or equivalent large pixel array is detected without an approved design revision
- **THEN** host validation SHALL fail

### Requirement: Backlight fail-closed
Before any QSPI or panel initialization call, the implementation SHALL configure GPIO44 as a digital output low and SHALL retain or best-effort restore that low state on every normal, terminal, cleanup, and error path.

The current implementation SHALL NOT configure LEDC or another PWM engine; contain a non-zero duty, brightness percentage, PWM frequency/resolution, fade, NVS brightness restore, dynamic brightness API, GPIO44-high operation, hidden wrapper/alias, or compile-time/runtime backlight-enable path.

The controlling decision SHALL be `RESOLVED FOR HOST IMPLEMENTATION — GPIO44 SHALL remain hard-disabled; non-zero backlight output and visual device execution remain NOT AUTHORIZED pending a separately reviewed successor Change.`

#### Scenario: Hard-disabled initialization order
- **WHEN** the successful host-designed state path is simulated
- **THEN** GPIO44 SHALL be configured output-low before the first QSPI/panel call and SHALL remain low through READY

#### Scenario: Any state fails
- **WHEN** host fault injection fails any actionable state
- **THEN** GPIO44 SHALL remain or be best-effort restored low and no high/non-zero backlight path SHALL execute

#### Scenario: Prohibited backlight mechanism appears
- **WHEN** LEDC/PWM, brightness/fade/NVS behavior, GPIO44 high, a non-zero constant, an enable flag, or an indirect wrapper/alias path is found
- **THEN** host validation SHALL fail

### Requirement: No NVS brightness restore
The display test SHALL not initialize NVS to read or restore brightness and SHALL not contain a brightness service, fade, percentage, PWM frequency/resolution, raw duty, or non-zero backlight constant.

#### Scenario: Hard-disabled source path
- **WHEN** backlight source is inspected
- **THEN** the only permitted GPIO44 output SHALL be low and no LEDC/PWM or brightness path SHALL exist

#### Scenario: Stored brightness path appears
- **WHEN** NVS, persisted brightness, or dynamic brightness code can affect GPIO44
- **THEN** host validation SHALL fail

### Requirement: Deterministic initialization state machine
The state machine SHALL contain exactly `BOOT_MARKER`, `BACKLIGHT_FORCED_OFF`, `QSPI_BUS_INIT`, `PANEL_IO_CREATE`, `PANEL_RESET`, `PANEL_INIT`, `DISPLAY_ON`, `TEST_PATTERN_DRAW`, `BACKLIGHT_POLICY_GATE`, `READY`, and `FAIL_SAFE` in the defined transition graph. The current reachable graph SHALL NOT contain `BACKLIGHT_LOW_ENABLE`.

#### Scenario: Happy path is ordered
- **WHEN** host state-machine tests simulate success
- **THEN** each non-failure state SHALL be entered once in order, `TEST_PATTERN_DRAW` SHALL transition to `BACKLIGHT_POLICY_GATE`, the gate SHALL transition to READY, and GPIO44 SHALL remain low

#### Scenario: Transition is missing or bypassed
- **WHEN** a state is skipped, repeated, reordered, a fallback state is introduced, or `BACKLIGHT_LOW_ENABLE` is reachable
- **THEN** host tests SHALL fail

### Requirement: Serial stage markers
Each entered state SHALL emit one deterministic `LCD_SM_ENTER <STATE>` marker. `BACKLIGHT_POLICY_GATE` SHALL emit `DISPLAY_BACKLIGHT_POLICY: DISABLED_NOT_AUTHORIZED`, and READY SHALL emit `LCD_SM_READY visual=UNVERIFIED backlight=DISABLED_NOT_AUTHORIZED`.

#### Scenario: Success markers are ordered
- **WHEN** host tests capture a successful simulated run
- **THEN** markers SHALL occur once in state order, SHALL include the exact policy and READY boundaries, and SHALL contain no private/device identifier

#### Scenario: Marker is missing or duplicated
- **WHEN** an entry/policy/READY marker is absent, repeated, out of order, weakens `UNVERIFIED`, or contains sensitive data
- **THEN** host validation SHALL fail

### Requirement: Single failure marker
The first error SHALL emit exactly one `LCD_SM_FAIL state=<STATE> err=<CODE>` marker, which SHALL remain the originating diagnosis.

#### Scenario: One state fails
- **WHEN** host fault injection fails any actionable state
- **THEN** exactly one failure marker SHALL name that state and no READY marker SHALL follow

#### Scenario: Cleanup also fails
- **WHEN** best-effort cleanup reports another error
- **THEN** no second failure marker SHALL be emitted and the original cause SHALL remain controlling

### Requirement: No automatic retry
The display state machine SHALL perform no automatic retry of allocation, bus, panel, reset, initialization, draw, callback wait, or backlight operations.

#### Scenario: Failure occurs
- **WHEN** an API call, invariant, or timeout fails
- **THEN** its call count SHALL remain one and the next state SHALL be FAIL_SAFE

#### Scenario: Retry logic appears
- **WHEN** host tests or static review detect a repeated attempt
- **THEN** acceptance SHALL fail

### Requirement: No automatic reset
The firmware SHALL not automatically reset the MCU or repeat the panel-reset sequence after a failure.

#### Scenario: Failure stabilizes
- **WHEN** any state fails
- **THEN** the main task SHALL remain alive without MCU reset or boot loop

#### Scenario: Reset fallback appears
- **WHEN** restart, reboot, watchdog-trigger, or repeated panel reset is used as recovery
- **THEN** host validation SHALL fail

### Requirement: Stable FAIL_SAFE state
FAIL_SAFE SHALL force GPIO44 off, stop later display work, release non-backlight resources in reverse order, retain GPIO44 low, and enter a stable non-busy terminal loop.

#### Scenario: Fault injection reaches fail-safe
- **WHEN** each actionable state is failed in host tests
- **THEN** no later state or unrelated peripheral call SHALL occur and the terminal result SHALL be stable

#### Scenario: Reinvocation follows a terminal result
- **WHEN** the run-once API is called after READY or FAIL_SAFE
- **THEN** it SHALL return the stored result without new GPIO, reset, bus, transfer, or backlight actions

### Requirement: Host-only configure and build acceptance
Configure and build SHALL be separate host-only actions using ESP-IDF v5.5.4 and SHALL create no device access or hardware-success claim. READY SHALL mean only that the host-designed initialization sequence and software transfer calls reached their logical terminal result without a detected software error.

#### Scenario: Host build succeeds
- **WHEN** exact dependencies resolve and configure, compile, and link complete with exit code zero
- **THEN** acceptance SHALL record only host configuration/build/artifact success

#### Scenario: Logical READY is recorded
- **WHEN** a successful state-machine simulation reaches READY
- **THEN** READY SHALL include `visual=UNVERIFIED` and `backlight=DISABLED_NOT_AUTHORIZED` and SHALL NOT mean screen illumination, backlight operation, visual success, or device verification

#### Scenario: Build output is overclaimed
- **WHEN** a host result is described as proof of display illumination, electrical correctness, or screen health
- **THEN** evidence review SHALL fail

### Requirement: Dependency audit
The future host review SHALL record the exact resolved component version, lock entry, registry/source revision, archive/tree integrity, and relevant source hashes.

#### Scenario: Dependency evidence is complete
- **WHEN** the component tree and resolved files are inspected
- **THEN** all evidence SHALL identify v1.0.1 and match the reviewed API lineage

#### Scenario: Integrity is absent or mismatched
- **WHEN** a hash, lock identity, or exact version cannot be reproduced
- **THEN** build/artifact acceptance SHALL be blocked

### Requirement: GPIO audit
The future host review SHALL scan source, generated configuration, component metadata, map/symbol data, and GPIO API use against the exact allowlist and denylist.

#### Scenario: Audit passes
- **WHEN** every hit is classified
- **THEN** allowed firmware hits SHALL trace to one permitted role and explained documentation-only hits SHALL be separately scoped

#### Scenario: Hit is unexplained
- **WHEN** an unclassified GPIO or denylist value remains
- **THEN** host validation SHALL fail

### Requirement: Symbol and configuration audit
The future host review SHALL prove absence of V1.2 BSP, PSRAM, LVGL, touch, audio, network, motor, SD, NVS brightness, LCD power, and unrelated board-peripheral symbols/configuration. It SHALL also reject every `ledc_*` call; brightness/fade/NVS-display service; GPIO44-high operation; non-zero duty constant; wrapper/alias bypass; `BACKLIGHT_LOW_ENABLE`; and unapproved compile-time/runtime enable gate.

#### Scenario: Minimal link surface is shown
- **WHEN** sdkconfig, component tree, map, and symbols are reviewed
- **THEN** only required ESP-IDF and ST77916 display paths SHALL remain, the GPIO44-low call order SHALL precede every QSPI/panel call, and the hard-disabled policy markers SHALL be present

#### Scenario: Prohibited feature is linked
- **WHEN** any prohibited symbol, configuration, direct call, wrapper, alias, non-zero constant, or enable flag is present
- **THEN** host validation SHALL fail

### Requirement: Binary and artifact audit
The future artifact review SHALL record binary SHA-256/size/header/segments, Flash mode/frequency/size declarations, partition containment, semantic range, transport plan, erase envelope, ROM padding, dependency evidence, table hash, and safety scans as separate evidence. Its manifest SHALL record `backlight=hard-disabled`, `visual=UNVERIFIED`, and `not-for-visual-validation`, plus the absence of LEDC and non-zero duty.

#### Scenario: Artifact packet is complete
- **WHEN** a candidate is prepared for successor review
- **THEN** every field SHALL be independently calculated and internally consistent

#### Scenario: Range is inferred from file length
- **WHEN** transport or erase scope is missing, ambiguous, or inferred only from candidate length
- **THEN** successor handoff SHALL be blocked

### Requirement: Device access prohibition
This Change SHALL NOT enumerate/open a serial port, identify/reset/monitor a device, or execute any device-management/read operation.

#### Scenario: Host-only work proceeds
- **WHEN** documentation, source inspection, host tests, build, or artifact analysis run
- **THEN** no serial/device API or command SHALL be invoked

#### Scenario: Device access is proposed
- **WHEN** a task or command would enumerate, open, reset, monitor, identify, or read a device
- **THEN** it SHALL be rejected as outside this Change

### Requirement: No Flash in this Change
This Change SHALL NOT execute or provide an executable Flash, readback, erase, eFuse, rollback, or restore command.

#### Scenario: Artifact review completes
- **WHEN** the future candidate packet is produced
- **THEN** it SHALL stop at non-executable host evidence

#### Scenario: Write-capable instruction appears
- **WHEN** an executable device write/erase/restore command or authorization package appears
- **THEN** scope validation SHALL fail

### Requirement: Separate successor write Change
Any device execution SHALL belong to a separately created and reviewed successor, proposed as `execute-pcb-v1-minimal-display-smoke-test`.

#### Scenario: Host handoff is ready
- **WHEN** all implementation/build/artifact tasks are complete
- **THEN** this Change MAY prepare a non-executable successor evidence checklist, but the hard-disabled artifact SHALL remain ineligible for visual validation

#### Scenario: Non-zero visual successor is proposed
- **WHEN** a future successor seeks illumination or visual observation
- **THEN** it SHALL approve exact GPIO, polarity, frequency, resolution, raw duty, and percentage; rebuild and re-review the artifact; obtain fresh exact device authorization; and SHALL NOT reuse any prior First Flash/startup/observation authorization

#### Scenario: Device work is placed here
- **WHEN** identity, port, Flash, reset, observation, rollback, or restore work is added to this Change
- **THEN** scope validation SHALL fail

### Requirement: Screen health remains UNVERIFIED
The physical display, connection, controller response, and backlight health SHALL remain `UNVERIFIED` throughout this Change.

#### Scenario: Black screen is described
- **WHEN** current behavior is recorded
- **THEN** it SHALL be described as expected from firmware that does not initialize the display and as proving neither health nor failure

#### Scenario: Health conclusion appears
- **WHEN** host evidence is used to claim the screen is healthy or damaged
- **THEN** evidence review SHALL fail

### Requirement: Host build does not prove display success
A successful host build SHALL NOT be accepted as proof of display initialization, illumination, color, orientation, refresh, DMA runtime, or long-term stability.

#### Scenario: Build passes
- **WHEN** configure/compile/link succeed
- **THEN** the record SHALL explicitly list all physical outcomes as untested

#### Scenario: Physical PASS is inferred
- **WHEN** a host-only result is labeled as a physical display PASS
- **THEN** acceptance SHALL fail

### Requirement: Historical First Flash result preservation
The record SHALL preserve First Flash as `STOPPED / INCONCLUSIVE — RANGE-SCOPE DEVIATION`, startup observation as `PASS`, and runtime validation as `PASS FOR THIS MINIMAL SMOKE TEST`.

#### Scenario: History is referenced
- **WHEN** proposal, design, tasks, or handoff discusses prior device work
- **THEN** all three exact bounded statuses SHALL remain consistent

#### Scenario: First Flash is rewritten
- **WHEN** later startup or display evidence is used to call the historical write successful
- **THEN** documentation validation SHALL fail

### Requirement: Recovery requirements remain unchanged
DEC-023, the restoration rehearsal, Level 1 complete original `ota_0` restore, Level 2 32 MiB full recovery, and independent Recovery verification SHALL remain in force.

#### Scenario: Display work progresses
- **WHEN** host build or a future display observation succeeds
- **THEN** neither recovery level nor the restoration milestone SHALL be waived

#### Scenario: Recovery is weakened
- **WHEN** this Change merges the two levels, modifies backups, performs restore, or treats a lit screen as recovery evidence
- **THEN** safety review SHALL fail

### Requirement: Old authorization cannot be reused
All prior device authorizations SHALL remain consumed/closed or `NONE` and SHALL NOT authorize any future display operation.

#### Scenario: Successor is later reviewed
- **WHEN** a successor proposes device work
- **THEN** it SHALL freshly bind candidate hash/ranges, identity, current port, recovery scope, operation, and one exact authorization

#### Scenario: Prior authorization is cited
- **WHEN** an old First Flash, startup, observation, or recovery authorization is offered as authority
- **THEN** the operation SHALL be rejected

### Requirement: No PCB V1.2 BSP package reuse
The implementation SHALL extract only audited V1.0 facts and the locked table; it SHALL NOT reuse a complete V1.2 or board-level BSP package.

#### Scenario: Minimal component architecture is used
- **WHEN** dependencies and source imports are inspected
- **THEN** no ESP-VoCat BSP package or whole board setup function SHALL be linked

#### Scenario: Whole BSP is imported
- **WHEN** a V1.2/V1.0 board package brings auto-detection, power, touch, audio, LVGL, NVS brightness, or unrelated initialization
- **THEN** host validation SHALL fail
