## Context

The current PCB V1.0 firmware is the previously reviewed minimal serial smoke test. Its First Flash attempt remains `STOPPED / INCONCLUSIVE — RANGE-SCOPE DEVIATION`; a later independently authorized startup observation is `PASS`, and runtime validation is `PASS FOR THIS MINIMAL SMOKE TEST`. That firmware never initializes the display, so its black screen is expected and says nothing about physical display health. Screen health, color, orientation, backlight behavior, refresh stability, and DMA behavior all remain `UNVERIFIED`.

This Change plans the smallest next increment: a PCB V1.0-only ST77916 QSPI initialization and deterministic test pattern, implemented and validated on the host before a separate successor Change can request any device operation. It changes no firmware in this planning round.

Stakeholders are the device owner, who must approve any future exact non-zero backlight configuration and any later exact device operation; the host-side implementer; and reviewers responsible for evidence, GPIO, dependency, binary, and recovery boundaries.

### Assumptions

- The project hardware profile, official PCB V1.0 documents, historical V1.0 source, and fixed official source snapshots describe the intended V1.0 display path closely enough to form a `STRONGLY SUPPORTED` design baseline.
- A single 360 × 80 RGB565 strip is sufficient to create the fixed test pattern without a framebuffer.
- The future implementation can remain independent of PSRAM, LVGL, filesystem, assets, touch, and every unrelated board peripheral.
- ESP-IDF v5.5.4 at commit `735507283d5b2f9fb363a1901172dbd9e847945d` remains the required local framework.

### Unresolved hardware facts

- Whether this physical screen, flex connection, backlight circuit, and ST77916 controller are healthy.
- Whether the proposed RGB order, orientation, inversion, offsets, 40 MHz ceiling, and vendor sequence produce the intended physical output on this unit.
- The safe non-zero GPIO44 PWM frequency, resolution, raw duty, and percentage for any future illuminated test.
- The MCU GPIO44 transient before firmware configures it as output low; the schematic pull-down supports an off bias, but the actual power-on/reset transient remains `UNVERIFIED`.
- Runtime internal-memory margin, DMA stability, and actual display-transfer completion on the device.
- Any touch, audio, SD, motor, power-control, or other board-peripheral behavior.

### Expected later affected files

Only a future implementation Apply may propose changes to:

- `firmware/main/idf_component.yml`
- `firmware/main/pcb_v1_display.h`
- `firmware/main/pcb_v1_display.c`
- `firmware/main/pcb_v1_display_test_pattern.h`
- `firmware/main/pcb_v1_display_test_pattern.c`
- the existing minimal `firmware/main/main.c` and the smallest required component/CMake declaration
- a task-specific host-build test record under `tests/build/`

The exact affected set must be restated before implementation. No suggested firmware file exists as a result of this design round.

## Goals

- Freeze an auditable PCB V1.0 display evidence baseline without promoting hardware facts to `CONFIRMED`.
- Lock one exact ST77916 component version and one exact vendor initialization-table source.
- Enforce the exact GPIO allowlist and actively reject V1.2 and unrelated-peripheral configuration.
- Define deterministic startup, marker, timeout, cleanup, and fail-safe behavior.
- Configure GPIO44 as output low before any QSPI or panel operation and keep it low on every reachable normal and failure path.
- Render a fixed, diagnostic RGB565 pattern from one DMA-capable internal-memory strip.
- Define future host build, static analysis, dependency verification, and artifact review.
- Preserve a hard boundary between host-prepared evidence and separately authorized device work.

## Non-goals

- Device/port discovery, identity read, reset, monitor, Flash, readback, erase, eFuse, rollback, restore, or authorization.
- Proof that the screen lights, is healthy, has correct color/orientation, or refreshes stably.
- PCB V1.2 support, revision auto-detection, complete BSP reuse, or generic-board support.
- Touch, LVGL, PSRAM, audio, microphone, speaker, motor, SD, battery, power control, Wi-Fi, Bluetooth, NVS, OTA, image assets, animation, filesystem, network, companion application, or product UI.
- Dynamic brightness or a user brightness service.
- LEDC, PWM backlight control, non-zero duty/percentage, PWM frequency/resolution, fade, NVS brightness restore, or any hidden compile-time/runtime backlight-enable path in the current implementation.
- Bootloader, partition layout, Flash voltage, security, or immutable recovery-asset changes.

## Current evidence status

Software-source facts read from fixed commits, registry versions, and the local ESP-IDF tag are `CONFIRMED` only within the host-source domain. They do not confirm PCB wiring or physical behavior. Every display hardware mapping in this design is uniformly:

`STRONGLY SUPPORTED — REQUIRES FUTURE DEVICE VALIDATION`

The screen's present physical health and all future visual outcomes are:

`UNVERIFIED`

The current black screen is expected because the minimal smoke-test firmware does not initialize the display. It proves neither a healthy screen nor a failed screen. A host build can prove configuration, dependency resolution, compilation, linking, static conformance, and artifact generation only.

Touch evidence has a retained conflict: the hardware profile/reference evidence names CST816S and classifies it as `STRONGLY SUPPORTED`, while the limited original-app token scan did not find `CST816S`/`cst816`. Token absence proves neither controller absence nor wiring, and token presence would not prove physical population. Touch remains entirely outside this display Change.

## Evidence/source matrix

| Source | Version/commit | Path/reference | Supported fact | Evidence level | Conflict | Intended use |
|---|---|---|---|---|---|---|
| Official ESP-VoCat V1.0 user guide | Git commit `2e9e9dcd066db0d34bbea93a5f7a4c5385ab1e1d` | [user_guide_v1.0.rst](https://github.com/espressif/esp-dev-kits/blob/2e9e9dcd066db0d34bbea93a5f7a4c5385ab1e1d/docs/en/esp-vocat/user_guide_v1.0.rst) | Separates V1.0/V1.2; identifies 1.85-inch 360 × 360 ST77916 QSPI display and GPIO44 backlight | `CONFIRMED` source content; `STRONGLY SUPPORTED` hardware | Does not prove this unit or full signal mapping | Hardware-design corroboration |
| Official ESP-VoCat V1.0 schematic | PCB V1.0, published 2025-05-23, updated 2025-07-09 | [ESP-VoCat_SCH_V1_0.pdf](https://dl.espressif.com/AE/esp-dev-kits/ESP-VoCat_SCH_V1_0.pdf) | CLK18, CS14, D0/46, D1/13, D2/11, D3/12, reset3, backlight44; `LCD_BLK` drives AO3400A N-MOS Q2 gate, R5 10 kΩ pulls the gate to GND, and Q2 switches LEDK through R4 20Ω; labels GPIO9 `POWER_CTRL` | `CONFIRMED` document content; `STRONGLY SUPPORTED` hardware | Also labels GPIO45 `LCD_DC`; QSPI software sources use no DC; actual transient/population is not device-verified | Mapping and off-bias corroboration; conflict retained, not copied |
| Official display-module specification | `UE018HV-RB39-A002A V1.0` | [module specification](https://dl.espressif.com/AE/esp-dev-kits/UE018HV-RB39-A002A%20%20V1.0%20SPEC.pdf) | Physical module/resolution; LEDA/LEDK interface; three parallel LEDs at 20 mA each, typical 3 V/60 mA/0.18 W backlight values | `CONFIRMED` document content; `UNVERIFIED` on this unit | Gives no PWM input range, safe percentage, recommended frequency, or audited vendor register table | Module identification; demonstrates non-zero-parameter evidence gap |
| Historical Xiaozhi V1.0 board source | Commit `49ac8a6da399f27a9546d4f73640b7f86c24bac6`; Git blob `4e14c4a9f6a8cee1d9b724d97d88a6804fc0ef77` | [esp_vocat.cc](https://github.com/78/xiaozhi-esp32/blob/49ac8a6da399f27a9546d4f73640b7f86c24bac6/main/boards/esp-vocat/esp_vocat.cc) | Exact 365-entry `vendor_specific_init_yysj`, QSPI mode, RGB/16 bpp, reset selection, display setup | `CONFIRMED` source content; `STRONGLY SUPPORTED` hardware | Whole file also initializes prohibited peripherals and supports V1.2 | Initialization-table owner only; no whole-file reuse |
| Historical Xiaozhi V1.0 configuration | Commit `49ac8a6da399f27a9546d4f73640b7f86c24bac6`; Git blob `a4ae32fab3d91abb89c2a9af32b8f2e66c3031a1` | [config.h](https://github.com/78/xiaozhi-esp32/blob/49ac8a6da399f27a9546d4f73640b7f86c24bac6/main/boards/esp-vocat/config.h) | 360 × 360, `SPI2_HOST`, GPIO map, 40 MHz, reset3, backlight44, RGB565/RGB, zero offsets, no mirror/swap | `CONFIRMED` source content; `STRONGLY SUPPORTED` hardware | Also contains V1.2 reset47 branch | Exact V1.0 design extraction only |
| Historical Xiaozhi backlight implementation | Commit `49ac8a6da399f27a9546d4f73640b7f86c24bac6` | [backlight.cc](https://github.com/78/xiaozhi-esp32/blob/49ac8a6da399f27a9546d4f73640b7f86c24bac6/main/boards/common/backlight.cc), [backlight.h](https://github.com/78/xiaozhi-esp32/blob/49ac8a6da399f27a9546d4f73640b7f86c24bac6/main/boards/common/backlight.h) | Historical product path uses active-high LEDC at 25 kHz/10-bit, initializes duty zero, then restores/fades a product brightness value | `CONFIRMED` source content only | Product defaults/restored 75% or fallback 10% are not first-test safety approvals | Frequency conflict and prohibited-product-behavior evidence only |
| Historical commit metadata | Commit `49ac8a6da399f27a9546d4f73640b7f86c24bac6`, tree `17829eca4428cae0bab2a3e0e2b05af6b1df9762`, 2026-04-17 | [fixed commit](https://github.com/78/xiaozhi-esp32/commit/49ac8a6da399f27a9546d4f73640b7f86c24bac6) | Fixes the source identity used by this design | `CONFIRMED` host-source identity | Upstream project is not device evidence | Source lock |
| Historical component manifest | Same commit; Git blob `9fbbb352da4a6a8f9b2b5ce7828a15021f8b1773` | [main/idf_component.yml](https://github.com/78/xiaozhi-esp32/blob/49ac8a6da399f27a9546d4f73640b7f86c24bac6/main/idf_component.yml) | Declares `espressif/esp_lcd_st77916: ^1.0.1` and ESP-IDF `>=5.5.2` | `CONFIRMED` source content | Caret range is not acceptable for the future implementation | Dependency provenance |
| Current repository hardware profile | Current baseline commit `6f48ded10c78b5f2a0bd05ffb8ed2ca488b90bc2` | [`docs/HARDWARE_PROFILE.md`](../../../docs/HARDWARE_PROFILE.md) | PCB V1.0 evidence classes; display mapping; GPIO9 associated with main power control | Repository-controlled evidence | Some public BSP labels conflict with GPIO9 role | Controlling project hardware evidence |
| Current firmware evidence audit | Same repository baseline | [`docs/hardware/pcb-v1-firmware-evidence-audit.md`](../../../docs/hardware/pcb-v1-firmware-evidence-audit.md) | Separates reference-firmware facts from physical confirmation | Repository-controlled evidence | Explicitly prevents source-to-hardware promotion | Evidence classification |
| Architecture and product constraints | Same repository baseline | [`docs/SYSTEM_ARCHITECTURE_DRAFT.md`](../../../docs/SYSTEM_ARCHITECTURE_DRAFT.md), [`docs/PRODUCT_SPEC_DRAFT.md`](../../../docs/PRODUCT_SPEC_DRAFT.md) | Driver/product boundaries, smooth rendering priority, offline-first behavior | Repository design authority | Does not provide device verification | Architecture boundary |
| Local ESP-IDF | Tag `v5.5.4`, commit `735507283d5b2f9fb363a1901172dbd9e847945d` | `components/esp_lcd/include/esp_lcd_panel_ops.h`, `components/esp_lcd/spi/esp_lcd_panel_io_spi.c`, `components/heap/include/esp_heap_caps.h`, `components/esp_driver_spi/include/driver/spi_common.h` | Half-open draw coordinates; queued color transfer; completion callback; DMA/internal caps; explicit max-transfer semantics | `CONFIRMED` local source facts | Cannot prove runtime DMA/display behavior | API and memory design |
| Official ST77916 component v1.0.1 | Registry v1.0.1; registry source revision `6a112f4ddfeaf30ec360567ea9260a39e195c385`; driver blob `c286636deb6734469000b64235e3bda25683f350` | [registry v1.0.1](https://components.espressif.com/components/espressif/esp_lcd_st77916/versions/1.0.1/readme?language=en) | QSPI macro uses DC -1, 40 MHz example, 32 command bits, 8 parameter bits, quad mode; supports ESP-IDF `>5.0.4,!=5.1.1` | `CONFIRMED` component metadata/source | v1.0.1 does not propagate `tx_color` errors from `draw_bitmap` | Exact future dependency, with timeout mitigation |
| Official ST77916 component v2.0.2 | Registry v2.0.2; registry source revision `91aeb7fb41e8a3e76aeb21371f9f83711c74cf3f`; SPI blob `d52eae7ef4f0c7a1f940f6c46cc56bd94d67c263` | [registry v2.0.2](https://components.espressif.com/components/espressif/esp_lcd_st77916/versions/2.0.2/readme?language=en) | Supports ESP-IDF >=5.4; preserves QSPI core and fixes `draw_bitmap` error propagation; 2.x adds MIPI structure | `CONFIRMED` component metadata/source | Larger API/source-lineage deviation from historical firmware | Rejected alternative and risk record |
| Official ESP-Brookesia board source | Commit `5361118b9953800e4fe710e57e5314fccec9cbcf`; V1.0 setup Git blob `ff5f2fa177cbf7dafbae3a810b8566355f830371` | [V1.0 board snapshot](https://github.com/espressif/esp-brookesia/tree/5361118b9953800e4fe710e57e5314fccec9cbcf/hal/brookesia_hal_boards/boards/espressif/esp_vocat_board_v1_0) | Independently reproduces the 365-entry vendor table; V1.0 backlight precedent is 2 kHz, 10-bit, initial 0% | `CONFIRMED` source content; `STRONGLY SUPPORTED` hardware | Board YAML includes GPIO45 DC, GPIO9 LCD/SD power, wildcard dependencies, and unrelated peripherals | Table corroboration and off-state precedent only |

Public sources are technical research inputs, not current-device validation. Fixed commits/versions are used wherever available; mutable pages are not the sole implementation basis.

### Backlight circuit evidence and decision boundary

The PCB V1.0 schematic shows `LCD_BLK` driving the gate of AO3400A N-channel MOSFET Q2. R5 10 kΩ pulls that gate to GND. Q2 is a low-side switch from the module's LEDK path, with R4 20Ω in series between LEDK and the Q2 drain. These connections are `CONFIRMED` schematic content and `STRONGLY SUPPORTED` for the current board, not physical-device confirmation. The polarity is therefore strongly supported as active high: GPIO44 high turns Q2 on and pulls LEDK toward GND; GPIO44 low turns Q2 off. R5 strongly supports an off bias while GPIO44 is undriven, but the actual MCU pin and gate transient before firmware initialization remain `UNVERIFIED`.

The module exposes LEDA/LEDK rather than a logic-level PWM input. Its specification gives typical current/voltage/power values but no accepted PWM range, recommended frequency, minimum duty, maximum duty, brightness linearity, or safe first-test percentage. Historical Xiaozhi V1.0 source uses 25 kHz/10-bit product PWM, while the fixed ESP-Brookesia V1.0 snapshot uses 2 kHz/10-bit with initial 0%. Those source values are `CONFIRMED` only as historical software facts; their disagreement leaves a safe current PWM frequency/resolution `UNVERIFIED`.

Historical product brightness defaults, including 75% and the saved-value fallback of 10%, are not approved values for this smoke test. A low PWM duty would shorten average on-time but does not prove a lower LED peak current while Q2 is conducting. No non-zero percentage, raw duty, frequency, or resolution is approved.

The controlling decision is:

`RESOLVED FOR HOST IMPLEMENTATION — GPIO44 SHALL remain hard-disabled; non-zero backlight output and visual device execution remain NOT AUTHORIZED pending a separately reviewed successor Change.`

This resolves only the full host-implementation blocker. It retains a backlight-code blocker for every non-zero output, a device-execution blocker for this Change and its intended visual successor until separately reviewed, and an observation blocker because a hard-disabled artifact cannot establish any visual result.

## PCB V1.0 hardware mapping

Every row below has the same classification: `STRONGLY SUPPORTED — REQUIRES FUTURE DEVICE VALIDATION`.

| Item | Design baseline |
|---|---|
| LCD controller | ST77916 |
| Resolution/display size | 360 × 360 |
| Bus | QSPI on `SPI2_HOST` |
| Initial clock ceiling | 40 MHz |
| QSPI CLK | GPIO18 |
| QSPI CS | GPIO14 |
| QSPI D0 | GPIO46 |
| QSPI D1 | GPIO13 |
| QSPI D2 | GPIO11 |
| QSPI D3 | GPIO12 |
| Reset | GPIO3, active low |
| Backlight gate | GPIO44, active high electrically; current implementation output-low only |
| DC | `GPIO_NUM_NC` |
| Command transport | QSPI command-phase encoding; 32 command bits, 8 parameter bits, quad mode |
| Pixel format | RGB565 / 16 bpp |
| Color order | RGB |
| Inversion | command `0x21` |
| Mirror / swap XY | disabled / disabled |
| X/Y offset | 0 / 0 |

Future physical testing may reveal color, orientation, backlight, clock, or timing changes. Any such change requires new evidence and a documentation update; it must not retroactively rewrite this baseline as confirmed.

## GPIO allowlist

The complete display GPIO allowlist is exactly:

`{18, 14, 46, 13, 11, 12, 3, 44}`

| GPIO | Sole permitted display role |
|---|---|
| 18 | QSPI CLK |
| 14 | QSPI CS |
| 46 | QSPI D0 |
| 13 | QSPI D1 |
| 11 | QSPI D2 |
| 12 | QSPI D3 |
| 3 | LCD reset, active low |
| 44 | Backlight MOSFET gate, current implementation output-low only |

DC is not a GPIO allocation and must remain `GPIO_NUM_NC`. Any display-related GPIO literal or GPIO API use outside the set fails future host validation. Constants, macros, generated configuration, component configuration, linked symbols, and source calls are all in audit scope.

## PCB V1.2 denylist

The future implementation must reject:

- GPIO47 as LCD reset;
- GPIO9 as LCD power or LCD_EN;
- GPIO45 as DC;
- `80000000` or 80 MHz as LCD pixel clock;
- the complete V1.2 BSP, revision auto-detection, or any V1.2 power feature;
- unchecked `LCD_EN`, touch, LVGL, audio, NVS brightness restore, or board-level peripheral initialization;
- whole-package reuse of either V1.0 or V1.2 BSP.

GPIO9 is associated with main power control in controlling project evidence and is prohibited as LCD_EN. The schematic's GPIO45 `LCD_DC` label is retained as a source conflict; QSPI implementations use command-phase encoding with `GPIO_NUM_NC`, so GPIO45 is deliberately excluded.

Future static scans must search at minimum for `GPIO_NUM_9`, `GPIO_NUM_47`, `GPIO_NUM_45`, `80000000`, `80 MHz`, known V1.2 BSP names, LCD power features, and touch/audio/LVGL initialization. An unexplained match blocks acceptance. Evidence-table quotations and explicit denylist tests are explained matches; firmware/configuration hits are not.

## Dependency version analysis

The official registry currently exposes seven versions: 0.0.1, 0.0.2, 1.0.0, 1.0.1, 2.0.0, 2.0.1, and 2.0.2.

| Candidate | ESP-IDF declaration | QSPI/API characteristics | Compatibility/behavior assessment |
|---|---|---|---|
| `1.0.1` | `>5.0.4,!=5.1.1` | Historical single SPI/QSPI driver; DC -1, 32-bit command/8-bit parameter, quad mode; vendor override supported | Directly matches historical `^1.0.1`; compatible with v5.5.4; smallest V1.0 lineage deviation |
| `2.0.0` / `2.0.1` | >=5.4 family | Adds MIPI support and reorganizes vendor structures/source; SPI/QSPI core retained | Compatible in principle but increases unneeded surface |
| `2.0.2` | >=5.4 | Same expanded 2.x surface; fixes failure propagation from `tx_color` in `draw_bitmap` | Better direct error propagation, but not the historical dependency line |

At the registry state reviewed in this Change, caret `^1.0.1` means `>=1.0.1,<2.0.0`, and 1.0.1 is the only published matching 1.x version. A future publication could change that resolution, so the caret expression is not an acceptable lock.

The v1.0.1 `draw_bitmap` implementation does not return a `tx_color` queueing error. The design mitigates this known software limitation with a transfer-completion callback and a bounded wait for every strip; absence of completion enters `FAIL_SAFE`. This mitigation does not prove pixels reached the panel.

## Dependency decision or blocker

Decision: the future manifest must use the exact dependency:

`espressif/esp_lcd_st77916: "==1.0.1"`

Rationale:

- It is compatible with ESP-IDF v5.5.4 by the component's own constraint.
- It is the actual resolution of the historical V1.0 `^1.0.1` line against the reviewed registry set.
- Its QSPI command encoding and vendor configuration are the source lineage used by the historical V1.0 implementation.
- It minimizes behavior deviation while the completion-wait design addresses the known unpropagated queue-error risk.

Rejected alternative: v2.0.2 provides a useful error-propagation fix but adds a new major-version/MIPI source organization unnecessary for this minimal test. Moving to it requires a deliberate OpenSpec revision and API/static audit, not a floating upgrade.

Future integrity verification must record the resolved component name/version, registry source revision, component-manager lock entry, source tree/archive SHA-256, and relevant file hashes. The v1.0.1 driver blob observed in registry metadata is `c286636deb6734469000b64235e3bda25683f350`; the future build must derive and record its own resolved integrity evidence rather than merely copying this value.

This decision is resolved. No dependency is installed or resolved in this planning round.

## Initialization-sequence analysis

Four sequence candidates were compared:

1. Historical Xiaozhi V1.0: an explicit 365-entry `vendor_specific_init_yysj` at fixed commit `49ac8a6...`.
2. Component v1.0.1 default: a generic component-owned sequence, available when no vendor override is provided.
3. Current official ESP-Brookesia V1.0: a 365-entry `vendor_specific_init_default` at fixed commit `5361118...`.
4. Current official ESP-Brookesia V1.2: textually the same 365-entry table at that commit.

After extracting only the initializer body and removing all whitespace, all three board-specific tables have SHA-256:

`e8a1f2ea307b51be59d3daa201bb444b5f5ddc2d8da2931fda6e91579c4522be`

The historical raw initializer-body SHA-256 is `a12f3e6e91aea314a308997a69930405e50930ab21aa69202a84792d89579667`; the current Brookesia raw body differs only in formatting and hashes to `8c20f6d27aaf5212215b24579dfd67b42b4ab614a090043dbb717611e8012d03`.

The shared table ends with inversion command `0x21`, Sleep Out `0x11`, and a final `0x00` entry carrying 120 ms delay. It does not contain `0x36`, `0x3A`, or display-on `0x29`; RGB order and RGB565 therefore remain explicit panel-config fields, while display-on remains a separate state. The module specification does not supply an audited replacement register table or a safe PWM duty.

Table equality across current V1.0 and V1.2 corroborates the panel-module sequence but does not make an entire V1.2 BSP safe or prove that all revisions are electrically identical. The component default is rejected because it is generic and would create a larger behavior deviation from the historical V1.0 board source.

## Initialization ownership decision or blocker

Decision: the future implementation must explicitly preserve the historical V1.0 365-entry table from:

- commit: `49ac8a6da399f27a9546d4f73640b7f86c24bac6`;
- file: `main/boards/esp-vocat/esp_vocat.cc`;
- symbol: `vendor_specific_init_yysj`;
- range: the complete array initializer, all 365 entries, through the final 120 ms entry.

The implementation must pass the preserved table through the v1.0.1 vendor configuration and must not rely on the component default. It must preserve command order, parameter bytes, per-entry delays, `0x21`, `0x11`, and the final 120 ms delay without silent cleanup, reformatting-based omission, or V1.2 BSP import.

The host audit must:

1. re-fetch or otherwise inspect the fixed source without using a floating branch as sole authority;
2. extract the complete source initializer;
3. remove all whitespace and verify the expected normalized SHA-256 above;
4. apply the same normalization to the implementation initializer and require the same hash;
5. count 365 command entries;
6. separately verify the final `0x21`, `0x11`, and 120 ms sequence;
7. verify RGB565, RGB order, no mirror/swap, and zero offsets in panel configuration.

This ownership decision is resolved. The table is source evidence, not physical validation.

## Component architecture

The future minimal architecture is:

```text
main.c
  -> pcb_v1_display_run_once()
       -> state machine / GPIO allowlist / QSPI / panel / backlight
       -> pcb_v1_display_test_pattern_fill_strip()
       -> terminal READY or FAIL_SAFE result
  -> one final marker
  -> stable non-busy loop
```

Proposed responsibilities:

- `main.c`: emit boot identity, invoke the display state machine once, emit final READY or FAIL marker, then remain stable.
- `pcb_v1_display.h/.c`: own the exact GPIO constants, QSPI bus/panel setup, state transitions, transfer-completion synchronization, backlight, cleanup, and fail-safe.
- `pcb_v1_display_test_pattern.h/.c`: fill one supplied RGB565 strip with deterministic black/RGB regions, white border, and minimal bitmap text.

No product behavior belongs in the driver. No hardware-register operation belongs in the pattern generator. The implementation must not import whole BSP setup functions.

## Initialization state machine

The machine is single-shot. Each successful state emits exactly one entry marker before its action. Any failure emits exactly one failure marker in the form `LCD_SM_FAIL state=<STATE> err=<CODE>`, immediately transitions to `FAIL_SAFE`, and emits no later state marker. `READY` emits `LCD_SM_READY visual=UNVERIFIED backlight=DISABLED_NOT_AUTHORIZED`. Retry is never allowed.

| State | Entry marker | Action | Success condition | Failure marker | Failure behavior | Backlight state | Retry | Next state |
|---|---|---|---|---|---|---|---|---|
| `BOOT_MARKER` | `LCD_SM_ENTER BOOT_MARKER` | Establish run-once context and log PCB V1.0/test identity | Context is initialized and invocation is first or returns prior terminal result without new actions | `LCD_SM_FAIL state=BOOT_MARKER err=<CODE>` | Enter fail-safe without peripheral initialization | Off/not driven by this state | No | `BACKLIGHT_FORCED_OFF` |
| `BACKLIGHT_FORCED_OFF` | `LCD_SM_ENTER BACKLIGHT_FORCED_OFF` | Before any QSPI/panel call, configure GPIO44 as a digital output low without configuring LEDC | GPIO configuration succeeds and a checked software/read-back invariant reports low | `LCD_SM_FAIL state=BACKLIGHT_FORCED_OFF err=<CODE>` | Best-effort retain/drive GPIO44 low, enter fail-safe | Forced low | No | `QSPI_BUS_INIT` |
| `QSPI_BUS_INIT` | `LCD_SM_ENTER QSPI_BUS_INIT` | Initialize `SPI2_HOST` QSPI bus with GPIO18/46/13/11/12, DMA, and `max_transfer_sz=57600` | ESP-IDF bus call succeeds and runtime configuration equals allowlist | `LCD_SM_FAIL state=QSPI_BUS_INIT err=<CODE>` | Release partial bus resources, keep GPIO44 low | Forced off | No | `PANEL_IO_CREATE` |
| `PANEL_IO_CREATE` | `LCD_SM_ENTER PANEL_IO_CREATE` | Create QSPI panel IO with CS14, DC `GPIO_NUM_NC`, <=40 MHz, 32 command bits, 8 parameter bits, quad mode, queue depth 1, completion callback | Valid non-null IO handle and callback synchronization object | `LCD_SM_FAIL state=PANEL_IO_CREATE err=<CODE>` | Reverse-order cleanup, keep GPIO44 low | Forced off | No | `PANEL_RESET` |
| `PANEL_RESET` | `LCD_SM_ENTER PANEL_RESET` | Create panel with GPIO3 active-low and issue exactly one component reset sequence | Panel creation and reset calls return success | `LCD_SM_FAIL state=PANEL_RESET err=<CODE>` | No second reset; reverse cleanup; GPIO44 low | Forced off | No | `PANEL_INIT` |
| `PANEL_INIT` | `LCD_SM_ENTER PANEL_INIT` | Initialize with exact vendor table, RGB565/RGB, no mirror/swap, zero offsets | Init returns success and the table's final 120 ms delay completes | `LCD_SM_FAIL state=PANEL_INIT err=<CODE>` | Stop sequence; cleanup; GPIO44 low | Forced off | No | `DISPLAY_ON` |
| `DISPLAY_ON` | `LCD_SM_ENTER DISPLAY_ON` | Issue one display-on request | Display-on API returns success | `LCD_SM_FAIL state=DISPLAY_ON err=<CODE>` | Cleanup; GPIO44 low | Forced off | No | `TEST_PATTERN_DRAW` |
| `TEST_PATTERN_DRAW` | `LCD_SM_ENTER TEST_PATTERN_DRAW` | Allocate/fill one strip and draw five half-open windows covering rows 0..359; wait for each completion before buffer reuse | Every API call succeeds, every callback arrives within its bound, and all 360 rows are submitted exactly once | `LCD_SM_FAIL state=TEST_PATTERN_DRAW err=<CODE>` | Stop remaining strips; cleanup transfer resources; GPIO44 low | Forced low | No | `BACKLIGHT_POLICY_GATE` |
| `BACKLIGHT_POLICY_GATE` | `LCD_SM_ENTER BACKLIGHT_POLICY_GATE`; `DISPLAY_BACKLIGHT_POLICY: DISABLED_NOT_AUTHORIZED` | Configure no LEDC/PWM, verify GPIO44 remains low, and record that current policy prohibits all non-zero output | The low invariant holds and the exact policy marker is emitted once | `LCD_SM_FAIL state=BACKLIGHT_POLICY_GATE err=<CODE>` | Best-effort retain/drive GPIO44 low and enter fail-safe | Forced low | No | `READY` |
| `READY` | `LCD_SM_ENTER READY`; terminal `LCD_SM_READY visual=UNVERIFIED backlight=DISABLED_NOT_AUTHORIZED` | Retain display/IO resources, retain GPIO44 low, prevent reinvocation, and return logical terminal success to `main.c` | READY is recorded once and main enters a stable non-busy loop; no visual claim is made | Not applicable after terminal success | No later hardware actions | Forced low | No | Terminal |
| `FAIL_SAFE` | No additional entry marker after the single failure marker | Best-effort retain/drive GPIO44 low; release strip, semaphore, panel, IO, and bus in reverse order while retaining GPIO44 low; record terminal failure | Main remains alive and no further display/peripheral action occurs | The originating `LCD_SM_FAIL...` is the only error marker | Stable non-busy loop; no reset, retry, boot loop, or other peripheral init | Forced low | No | Terminal |

Idempotence means that once READY or FAIL_SAFE is recorded, a repeated function call returns the stored terminal result and performs no GPIO, bus, panel, reset, transfer, or backlight operation. Cleanup failures are recorded only as fields in the stored terminal context and do not emit a second failure marker or trigger a retry. On READY, resources remain owned and GPIO44 remains low; on FAIL_SAFE, GPIO44 remains explicitly held low even after other resources are released. A hard-disabled READY is a software terminal result, not a panel failure and not a visual success.

## Backlight safety strategy

GPIO44 is active high electrically in the strongly supported design baseline, but the current implementation uses only the digital output-low state. `BACKLIGHT_FORCED_OFF` must configure GPIO44 low before any QSPI/panel call. Every subsequent state, READY, cleanup path, allocation failure, callback timeout, invariant failure, and FAIL_SAFE must retain or best-effort restore that low state.

The current implementation must not configure LEDC or another PWM engine; contain any non-zero duty/raw value, brightness percentage, PWM frequency/resolution, fade, NVS brightness read/restore, brightness API, hidden wrapper/alias, or compile-time/runtime enable flag; or ever drive GPIO44 high. `BACKLIGHT_POLICY_GATE` verifies this hard-disabled policy and emits exactly:

`DISPLAY_BACKLIGHT_POLICY: DISABLED_NOT_AUTHORIZED`

The state may then enter logical READY with:

`LCD_SM_READY visual=UNVERIFIED backlight=DISABLED_NOT_AUTHORIZED`

READY means only that the host-designed initialization path and software transfer calls reached their terminal result without a detected software error. It does not mean that the screen is visible, the backlight works, colors or orientation are correct, GPIO mapping is correct on the unit, DMA is stable on the unit, or the screen is healthy.

Any future non-zero implementation belongs to a separately reviewed successor. That Change must explicitly approve GPIO, polarity, frequency, resolution, raw duty, percentage, initialization/failure order, rebuild the artifact, repeat static/artifact review, obtain fresh device authorization, and retain the prohibition on old authorization reuse.

## Rendering and strip-buffer design

The expected pattern is a black background with clearly separated red, green, and blue regions, a one-pixel white border, and fixed text `ESP-VoCat LCD TEST`. The pattern is generated algorithmically; it uses no image file, filesystem, font library, or network.

One 360 × 80 × 2-byte strip (57,600 bytes) is reused for row windows:

- `[0, 80)`
- `[80, 160)`
- `[160, 240)`
- `[240, 320)`
- `[320, 360)` using only 28,800 bytes

ESP-IDF v5.5.4 documents `x_end` and `y_end` as excluded, so calls use x `[0,360)` and the row windows above. The test-pattern function receives absolute y origin and row count, fills only the active bytes, and never reads beyond the last 40-row strip.

RGB565 values are defined symbolically (`0x0000`, `0xF800`, `0x07E0`, `0x001F`, `0xFFFF`) and converted into the byte order required by the reviewed panel API/component path. A host unit test must verify exact bytes for known pixels, boundaries, the four corners, region transitions, border, and glyph locations. Physical color correctness remains unverified.

The font is a compile-time bitmap containing only the unique glyphs needed by `ESP-VoCat LCD TEST` (or a documented minimal ASCII subset if smaller/equivalent). No full font library or asset loader is allowed.

This pattern defines expected output only; it does not claim that the screen displayed it.

## Memory and DMA analysis

- Allocation: one explicit heap allocation of 57,600 bytes using `MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL`; no large stack array and no PSRAM.
- Alignment: use the capability allocator's normal alignment, assert at least four-byte alignment, and fail before QSPI transfer if the pointer is null or misaligned.
- Transfer size: configure the SPI bus `max_transfer_sz` to 57,600 bytes. ESP-IDF defaults to 4,092 bytes with DMA when zero, so relying on the default is prohibited.
- Queue depth: one color transaction in flight. The panel IO queue depth is 1.
- Reuse rule: because v5.5.4 queues color transfers, the producer must wait on the registered color-transfer-done callback before modifying the strip.
- Timeout: each completion wait is bounded by a compile-time constant justified against the reviewed 40 MHz transfer size; timeout enters `FAIL_SAFE`. The exact value is a later software implementation choice subject to static review, not physical proof.
- Buffering: no double buffer unless this design is revised with measured need and a new memory budget.
- Lifetime: allocate before the first strip, free after all transfers or during fail-safe cleanup. Never free or overwrite while in flight.
- SRAM acceptance: the later map/size report and runtime allocation path must show the 57,600-byte internal DMA allocation plus task/driver overhead fits without memory-allocation failure. A source estimate alone is insufficient.
- Task stability: pattern generation and transfer waits yield/block normally; no busy loop. `main.c` enters a bounded non-busy terminal loop. No watchdog disable or priority inflation is allowed.

## Serial marker design

Markers are deterministic ASCII and contain no device identifier, MAC address, token, NVS data, or recovery content.

- Entry: `LCD_SM_ENTER <STATE>`
- Failure: exactly one `LCD_SM_FAIL state=<STATE> err=<CODE>`
- Backlight policy: `DISPLAY_BACKLIGHT_POLICY: DISABLED_NOT_AUTHORIZED`
- Terminal success: `LCD_SM_READY visual=UNVERIFIED backlight=DISABLED_NOT_AUTHORIZED`
- Main terminal failure summary: `LCD_TEST_RESULT FAIL` without duplicating the state-machine error marker
- Main terminal success summary: `LCD_TEST_RESULT READY visual=UNVERIFIED backlight=DISABLED_NOT_AUTHORIZED`

Each state entry appears at most once and in the state-machine order. After a failure, no later entry or READY marker may appear. Host source tests validate marker strings and transition order; future device observation belongs to the successor Change.

## Error handling and fail-safe

Every API return, allocation, callback wait, configuration invariant, table hash/count check, and strip range is checked. One failure records its originating state/error, forces GPIO44 off, prevents later display states, performs best-effort reverse cleanup, and settles into stable FAIL_SAFE.

There is:

- no automatic retry;
- no automatic reset;
- no boot loop;
- no repeated panel reset;
- no fallback to a different clock, GPIO, table, dependency, or PCB revision;
- no initialization of another peripheral;
- no backlight enable on an error path;
- no continuation after an unrecognized denylist/configuration hit.

A cleanup error cannot replace the original failure or trigger a second marker. The main task remains alive for later serial diagnosis but performs no device-control operation.

## Host-only build strategy

The later implementation phase must separate:

1. source/manifest review;
2. host configure;
3. host build;
4. generated configuration/component-tree inspection;
5. source/static audits;
6. binary/artifact review.

It must use the existing ESP-IDF v5.5.4 and exact component version only. Configure/build output must stay in ignored build locations. A dependency download or resolution is not performed in this planning round; any later component-manager action must be explicit, reviewed, and recorded as part of implementation.

Host acceptance can establish only that:

- configuration is accepted;
- exact dependencies resolve;
- compilation and linking succeed;
- source/generated configuration conforms to the allowlist/denylist;
- the binary artifact is generated and internally inspectable.

It cannot establish GPIO electrical correctness, controller identity, illumination, backlight operation, color, orientation, DMA runtime stability, physical screen health, long-term stability, or recovery.

## Static and GPIO audit

The later audit must cover tracked source, manifests, lock data, generated configuration, component tree, map file, symbols, strings, and build logs.

Pass conditions:

- every display GPIO occurrence traces to `{18,14,46,13,11,12,3,44}`;
- DC is exactly `GPIO_NUM_NC`;
- QSPI is `SPI2_HOST`, <=40 MHz, RGB565/RGB, inversion 0x21, no mirror/swap, zero offsets;
- dependency is exactly v1.0.1 and the vendor table passes count/hash/end-sequence checks;
- no GPIO9/47/45, 80 MHz, V1.2 BSP, whole board BSP, LCD_EN, revision detection, or power feature is linked or configured;
- no PSRAM, LVGL, touch, audio, network, motor, SD, NVS brightness, filesystem, image asset, or unrelated peripheral initializer is linked or called;
- one internal DMA strip, one-in-flight queue, completion synchronization, and fail-safe markers are present;
- GPIO44 is configured output-low before every QSPI/panel call and remains low in all normal/fault-injection paths;
- host fault injection covers every actionable state and proves the GPIO44-low invariant after each failure;
- there is no `ledc_*` call, PWM/brightness/fade/NVS-brightness service, GPIO44-high operation, non-zero duty constant, wrapper/alias bypass, `BACKLIGHT_LOW_ENABLE`, or compile-time/runtime non-zero enable gate;
- the state order is `TEST_PATTERN_DRAW` -> `BACKLIGHT_POLICY_GATE` -> `READY`, the exact policy marker is present, and READY contains `visual=UNVERIFIED` and `backlight=DISABLED_NOT_AUTHORIZED`;
- no executable device/Flash command is introduced.

Any unexplained match fails host validation. Documentation/spec text is excluded from firmware-symbol pass/fail only through an explicit path-scoped audit; it is never silently ignored.

## Future artifact audit

Before any successor Change is proposed as executable, the host evidence package must record:

- app binary path, SHA-256, exact file length, and effective image size;
- ESP32-S3 app header, entry point, checksum/hash status, and segment layout;
- header-declared Flash mode, frequency, and size without treating them as physical facts;
- exact partition containment and preserved regions;
- exact candidate semantic start/end-exclusive range;
- separately calculated transport start/end range and block plan;
- separately calculated erase start/end envelope;
- ROM no-stub final-block padding behavior and all bytes that may be transported;
- exact dependency version, lock evidence, source/archive integrity, and relevant component hashes;
- initialization-table entry count and normalized SHA-256;
- GPIO allowlist evidence and denylist absence;
- PSRAM, LVGL, and unrelated-peripheral absence;
- map/size evidence for internal memory, strip buffer, task stacks, and linked components;
- an artifact manifest recording `backlight=hard-disabled`, `visual=UNVERIFIED`, and `not-for-visual-validation`, plus evidence of no LEDC and no non-zero duty;
- immutable recovery-asset paths/hashes reverified without modification.

Candidate file length, semantic range, transport range, and erase envelope are separate values. The historical range-scope deviation demonstrates that one must never infer the transport range only from file length. Any ambiguity blocks the successor.

## Successor Change boundary

The proposed future device Change name is:

`execute-pcb-v1-minimal-display-smoke-test`

This round must not create it. It may be proposed only after this Change's firmware, build, static, artifact, and handoff tasks are complete. The current hard-disabled artifact is not eligible for visual validation. Before a successor is proposed for visual device execution, it must separately resolve and approve the exact non-zero backlight GPIO, polarity, frequency, resolution, raw duty, and percentage, then rebuild and re-audit the resulting artifact.

The successor must freshly review branch/HEAD and immutable candidate hash; semantic range; transport range; erase envelope; ROM block/padding plan; exact device identity and current port; same-device recovery scope; screen acceptance checklist; stop/rollback decision; one-attempt behavior; and a new exact authorization. Write and observation must be separate operations. Prior First Flash, startup, observation, or recovery authorization is consumed/closed and cannot be reused.

The successor may observe whether the screen illuminates, pattern/colors/orientation appear plausible, and refresh is stable within its exact checklist. It cannot rewrite the historical First Flash attempt as PASS.

DEC-023 remains controlling. Level 1 original `ota_0` restore and Level 2 32 MiB full recovery are distinct, separately reviewed operations. Recovery verification requires an independent Change. Neither a host build nor a lit display waives the restoration rehearsal before formal custom-cat firmware work.

## Risks and mitigations

- [Hardware mapping remains unverified] -> Keep every mapping `STRONGLY SUPPORTED`, use only the exact allowlist, and require a separate bounded device Change.
- [GPIO9/GPIO45 public-source conflicts] -> Record conflicts explicitly; deny both as display GPIOs and fail static validation on firmware hits.
- [V1.2 contamination] -> Deny reset47, 80 MHz, auto-detection, power features, and whole BSP imports.
- [v1.0.1 does not propagate `tx_color` error] -> Require one-in-flight completion callback plus bounded wait; reconsidering v2.0.2 requires a design revision.
- [Vendor table copied incompletely or altered] -> Lock commit/file/symbol/count and normalized hash; audit final 0x21/0x11/120 ms entries.
- [Backlight power-on transient remains unverified] -> Rely on the schematic R5 pull-down as strongly supported off bias, configure GPIO44 output-low before QSPI/panel work, prohibit all high/non-zero paths, and retain physical behavior as UNVERIFIED.
- [A hard-disabled artifact is mistaken for visual evidence] -> Emit explicit policy/READY markers, mark the artifact `not-for-visual-validation`, and require a rebuilt/re-audited successor after exact non-zero approval.
- [DMA buffer exhaustion] -> One 57,600-byte internal DMA allocation, no framebuffer/double buffer/PSRAM, map/size audit, and fail-safe on allocation failure.
- [Asynchronous buffer reuse corrupts pixels] -> Queue depth 1 and wait for completion before every refill/free.
- [Host build overclaim] -> State physical outcomes as UNVERIFIED and move all device validation to the successor.
- [Range-scope repetition] -> Independently audit semantic, transport, erase, and padding ranges before any future authorization.
- [Recovery weakened by display progress] -> Preserve DEC-023, both recovery levels, restoration rehearsal, and separate verification.

## Decision status and remaining questions

Resolved for current host implementation:

`RESOLVED FOR HOST IMPLEMENTATION — GPIO44 SHALL remain hard-disabled; non-zero backlight output and visual device execution remain NOT AUTHORIZED pending a separately reviewed successor Change.`

Missing evidence still blocks every non-zero backlight implementation and visual observation: no source or controlled engineering justification selects a safe GPIO44 frequency, resolution, raw duty, or percentage. Historical 25 kHz and 2 kHz software precedents conflict, and historical 75%/10% product brightness behavior is not safety approval. No parameter is guessed.

This is a combined backlight-code, device-execution, and observation blocker, not a full host-implementation blocker. Firmware implementation, host configure/build, static validation, and artifact review may proceed only under the hard-disabled policy. Current device operations remain prohibited.

Non-blocking until implementation review:

- Exact bounded per-strip completion timeout, to be justified from 57,600 bytes, <=40 MHz QSPI, driver overhead, and generous scheduling margin.
- Whether the tiny glyph set is encoded row-major or column-major; host pixel tests must make the choice auditable without changing the visible string.

## Evidence-level preservation

Host-confirmed facts are limited to source identity/content, version constraints, local API semantics, table comparisons/hashes, and document structure. Hardware mapping is never promoted above `STRONGLY SUPPORTED — REQUIRES FUTURE DEVICE VALIDATION`. Physical screen health and all visual/runtime outcomes remain `UNVERIFIED`.

The following statements remain unchanged:

- First Flash: `STOPPED / INCONCLUSIVE — RANGE-SCOPE DEVIATION`
- Startup observation: `PASS`
- Runtime validation: `PASS FOR THIS MINIMAL SMOKE TEST`
- Current device authorizations: `NONE`

No host result, public source, future implementation, or future lit screen may silently rewrite those evidence domains or weaken recovery requirements.
