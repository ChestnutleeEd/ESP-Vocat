## Context

The completed host-only baseline proves only that three repository-owned source files compile for the ESP-IDF `esp32s3` target. It provides no PCB V1.0 board configuration and grants no Flash authorization.

This planning round independently verified two 32 MiB files in the `D:` recovery directory with the expected SHA-256. Offline analysis found one valid original partition table at `0x00008000`, a valid ESP32-S3 bootloader image, and a valid `ota_0` ESP32-S3 application image. Both image headers declare `DIO`, `80 MHz`, and `16 MB`; repository hardware records describe the physical device as 32 MiB Octal Flash at 1.8 V with 16 MiB PSRAM. These statements concern different evidence domains and cannot be collapsed into one board configuration. The `E:` primary recovery directory was not present during this round.

During Apply preflight on 2026-07-25, the two D: files were reverified, exact copies were created at the recorded E: path, and all four files matched the expected size and SHA-256. This establishes D:/E: cross-volume redundancy without proving separate physical disks. A fresh partition/image analysis reproduced the planning results, while a limited string search and tracked-repository search still found no traceable raw device-read output capable of resolving the Flash/PSRAM configuration conflict.

A later, separately authorized minimum inspection completed `chip_id` and `flash_id` on the user-reviewed `COM7` with esptool.py `v4.12.dev3`, `--no-stub`, exit code 0, MAC redaction, and a final RTS hard reset. It confirmed ESP32-S3 QFN56 revision v0.2, a 32 MB JEDEC capacity code, the ESP32-S3 Octal Flash-type eFuse selector, and embedded 16 MB PSRAM package fields (`AP_1v8`). Installed-source audit also found that this version's inherited voltage helper prints 3.3 V for any nonzero combination of three masked eFuse voltage bits, so its 3.3 V line does not expose a unique bit pattern or resolve the prior 1.8 V record. No device was accessed again during that audit.

Stakeholders are the device owner reviewing risk and authorizing any exact device operation, and the later Apply agent implementing only the approved host-side portion before a separate Flash checkpoint.

### Assumptions

- The two hash-matching files in the immutable `D:` directory are same-device recovery reads, as recorded by repository documents.
- The original partition table describes the backup image accurately; it is not automatically a safe custom-firmware layout.
- The first smoke test can meet its purpose without PSRAM or any external peripheral.
- A stable device identity and exact current port will be supplied and reviewed before each future authorization. `COM7` is confirmed only for the completed read-only query event and is not a permanent port assignment.

### Unresolved hardware facts

- The exact ESP-IDF Flash-mode/header-size/frequency and PSRAM mode/clock/options configuration that is safe for this PCB V1.0 unit.
- Physical Flash voltage: current tool output says 3.3 V, prior repository evidence says 1.8 V, and the installed helper does not uniquely reveal its input bit pattern.
- Individual VDDSPI eFuse bits, Secure Boot, Flash Encryption, download-mode, JTAG, and running-firmware state. USB Serial/JTAG was observed only in the completed query context.
- Exact electrical wiring and safe initialization for all display, touch, audio, motor, SD, battery, power-control, and GPIO signals.
- Whether BMI270 is physically fitted and whether original-firmware component strings represent populated hardware.

### Expected later affected files

- The later Apply may change only the three tracked files currently under `firmware/` and add the smallest reviewed board-configuration input needed by this capability.
- It must add a task-specific host build/test record under `tests/`.
- It must update the two hardware/readiness reports when new evidence is obtained.
- It must not modify the archived host-baseline Change, the synchronized `esp-idf-host-build-baseline` main spec, recovery backups, or unrelated product modules.

### Dependency choices

- Reuse ESP-IDF v5.5.4 and its installed host tools.
- Add no third-party firmware dependency, driver, board package, or toolchain upgrade.
- Use OpenSpec 1.6.0 for the planning and validation workflow.

## Goals / Non-Goals

**Goals:**

- Define an evidence-gated PCB V1.0 board-configuration decision.
- Produce a later minimal ESP32-S3 startup image with fixed serial logs and compile-time version information.
- Keep PSRAM optional and disabled until its exact configuration is accepted.
- Preserve original bootloader, partition table, NVS, OTA metadata, PHY data, and assets unless an exact later review proves a narrower safe action is impossible.
- Separate configure, build, inspect, connect, Flash, serial observation, and rollback.
- Require a recoverable, exact, user-authorized first Flash and a deterministic stop/rollback path.

**Non-Goals:**

- Display, touch, audio, microphone, motor, SD, battery, power-control, network, Bluetooth, OTA, NVS, companion-app, or cat-product behavior.
- PCB V1.2 support or generic ESP32-S3 board support.
- Bootloader modification, partition redesign, full-device erase, eFuse work, Secure Boot, or Flash Encryption.
- Hardware-completeness, electrical-safety, performance, or recovery-success claims before controlled device tests.

## Evidence Model

Evidence is typed by what it proves:

| Evidence class | Proves | Does not prove |
|---|---|---|
| Prior recorded device evidence | The repository recorded an observation on this unit | That the observation was revalidated in this round |
| Immutable-file evidence | File size, hash, bytes, and parseable image structure | Current device state or physical electrical configuration |
| Image-header evidence | What bootloader/app images declare | Physical Flash capacity, voltage, bus wiring, or eFuse state |
| Original-firmware strings | Components/configuration compiled into the original firmware | PCB population, wiring, or safe custom initialization |
| Matching reference source | A plausible PCB V1.0 implementation candidate | Electrical confirmation on this exact unit |
| Controlled future device evidence | The exact observation made by an approved device-read/test step | Any behavior outside that test |

No evidence may be promoted across rows without a recorded verification.

## Board-Configuration Decision Process

The later Apply SHALL build a configuration allowlist before changing firmware:

1. Start with only target `esp32s3`, fixed serial logging, no peripheral drivers, no GPIO references, no NVS, and no network.
2. Map every proposed Flash/PSRAM option to an evidence source and state exactly what that source proves.
3. Reject generic defaults, PCB V1.2 values, and values derived only from component strings.
4. Reconcile the original image-header declarations (`DIO`, `80 MHz`, `16 MB`) with prior physical-device records (32 MiB, Octal, 1.8 V) without treating either as interchangeable.
5. Disable PSRAM initialization if its mode, size, clock, voltage, and pin ownership are not all accepted.
6. Stop before configure if any required option is outside the allowlist.

The allowlist decision must be recorded; it must not be hidden in generated `sdkconfig`.

## Device-Read-Only Configuration Gate

Offline evidence plus the completed `chip_id`/`flash_id` query is still insufficient to select the mandatory PCB V1.0 Flash configuration. Firmware implementation, tracked board configuration, configure, and build therefore remain blocked before the existing implementation steps.

The first separately authorized inspection is complete and recorded in `docs/hardware/pcb-v1-device-readonly-inspection-result.md`. It consumed only the approved chip/Flash identification scope and grants no continuing device authority. The remaining possible evidence-gathering action is a separately reviewed, sanitized, minimum read-only eFuse summary. It remains `NOT AUTHORIZED` and must:

- use an exact newly user-reviewed `<REVIEWED_PORT>` rather than reusing the completed event's `COM7`;
- identify each read-only query category, tool/version, expected output, reset risk, privacy handling, and stop condition;
- provide no complete executable command before review;
- exclude `read_flash`, writes, erases, restores, eFuse writes, security changes, monitor, firmware build, and peripheral behavior;
- redact MAC addresses and unnecessary unique identifiers;
- require an explicit authorization statement for the exact packet, with vague continuation language rejected.

The completed inspection confirms current-device Flash capacity and the Octal eFuse selector but not frequency, physical voltage, or the required ESP-IDF v5.5.4 mapping. The firmware gate therefore remains closed. PSRAM capacity is confirmed at the package/eFuse level, but PSRAM remains disabled until its independent mode/clock/voltage/options gate is satisfied.

## Recovery-First Strategy

- Recovery images remain immutable and outside Git.
- Both full-image files must be rehashed immediately before the pre-Flash review.
- The exact D: and E: recovery paths now contain verified matching files. Treat this as cross-volume redundancy only; do not claim separate physical disks without evidence.
- A host-only recovery rehearsal must verify file availability, hash, intended same-device scope, tool availability, and a reviewed placeholder procedure without writing a device.
- Full recovery is reserved for the same exact device because the image includes NVS, PHY data, and potentially unique/private state.
- Any rollback command is prepared separately and remains unauthorized until its own exact review.

## Minimal Firmware Architecture

The initial implementation has one minimal startup path:

```text
ESP32-S3 entry
    |
    +-- compile-time identity/version constants
    |
    +-- fixed serial startup log
    |
    +-- optional PSRAM gate (default disabled)
    |
    `-- bounded idle loop with no storage, network, GPIO, or peripheral work
```

No board-support, display, touch, audio, motor, power, storage-service, companion, or product-behavior module is introduced in this first revision. Future peripheral probes remain independent Changes.

## Boot Flow

1. ROM and the reviewed existing-or-generated boot chain start the application.
2. The application emits a fixed smoke-test identifier, build version, PCB target label, and explicit “serial-only / peripherals disabled” state.
3. If and only if the PSRAM gate was satisfied at build review, the application may report the initialization result without using PSRAM as a startup dependency.
4. The application performs no NVS initialization or write, network setup, OTA check, external-bus access, or GPIO configuration.
5. It remains alive in a bounded non-busy idle state so serial observation can detect resets or watchdog failure.

## Logging Strategy

- Logs are deterministic and short: smoke-test identifier, semantic/build version, target label, feature-disable summary, optional PSRAM result, and a ready marker.
- Logs must contain no MAC address, chip-unique ID, credentials, tokens, NVS data, or recovery-file contents.
- Serial observation is a separate `DEVICE READ` task after Flash and does not use a combined Flash/monitor command.
- Acceptance requires the expected ready marker and an observation window with no reset loop, panic, watchdog, or memory-allocation failure.

## Flash and PSRAM Configuration Gates

Flash configuration is mandatory for a safe build and is not resolved by the backup alone. The image headers' `DIO / 80 MHz / 16 MB` declaration is recorded as original-image metadata. Current device evidence now confirms a 32 MB JEDEC capacity code and an Octal eFuse Flash-type selector; these describe physical identification/eFuse fields rather than the image header layer. The installed-tool 3.3 V text conflicts with the prior 1.8 V record and is limited by ambiguous source logic, so voltage and exact ESP-IDF mapping remain unresolved.

PSRAM is disabled by default. The current query confirms embedded 16 MB package fields and the `AP_1v8` PSRAM vendor/variant label, but not active bus mode or clock. It can be enabled only when exact PCB V1.0 mode, size, clock, voltage domain, and ESP-IDF options are reviewed together. Failure to initialize optional PSRAM must not block serial startup.

No design task changes VDD_SPI, Flash voltage, eFuses, security state, or memory-device wiring.

## Peripheral Isolation Strategy

- The source must contain no GPIO number and no driver/component initialization.
- Display, CST816-family touch, top capacitive touch, ES7210, ES8311, BMI270, motor UART, SD, battery, power-control, Wi-Fi, and Bluetooth are compile-time absent.
- Original-firmware strings are discovery evidence only.
- A future peripheral Change must identify one peripheral, its exact PCB V1.0 evidence, safe GPIO/bus allowlist, failure isolation, and independent acceptance test.

## Partition Strategy

The original backup has a uniquely validated table at `0x00008000` with `nvs`, `otadata`, `phy_init`, `ota_0`, `ota_1`, and `assets`. This Change does not alter that table.

The preferred first custom write is one reviewed application image into one exact reviewed application range while preserving bootloader, partition table, NVS, OTA metadata, PHY data, and assets. This preference is conditional: compatibility between the custom app, selected slot, original bootloader, and partition metadata must be proven during artifact review. If it cannot be proven, the workflow stops; it does not expand the write range.

No unused tail region is repurposed.

## First Flash Strategy

The exact Flash operation is prepared only after host build and artifact inspection. The authorization packet must include:

- exact physical device and PCB V1.0 identity;
- exact currently reviewed port, never historical `COM7` by assumption;
- exact image path and SHA-256;
- exact offset and byte range;
- exact partition-layout interpretation;
- recovery-image paths, sizes, and SHA-256;
- confirmation that the command does not erase, modify eFuses, or include unrelated images;
- risks, observation window, stop conditions, and rollback plan.

OpenSpec Apply may create firmware and prepare a non-executed command. It must stop before Flash until the user explicitly authorizes that exact packet. Generic continuation language is invalid.

## Rollback Strategy

- On an authorization mismatch, connection mismatch, Flash failure, unexpected boot output, reset loop, panic, watchdog, or silence, stop all further writes.
- Preserve the actual output and determine whether another write would increase risk.
- If rollback is required and separately authorized, restore the reviewed full 32 MiB same-device image with no preceding erase.
- Observe original-device boot and functionality in separate tasks.
- If full recovery cannot be verified, stop the hardware sequence; do not continue to peripheral testing or formal firmware.

## Risk Analysis

- **Image-header/configuration layering:** The original images declare 16 MB DIO while the current device query reports 32 MB and the Octal eFuse selector. Treat these as different layers requiring exact compatibility mapping, not a value-selection contest.
- **Voltage conflict:** Current installed-tool output says 3.3 V, prior repository evidence says 1.8 V, and the installed helper hides the exact bit pattern. Do not choose or alter a voltage until authoritative evidence is reconciled.
- **Recovery-location semantics:** D: and E: now contain verified copies, but only cross-volume separation is established; physical-disk independence is not claimed.
- **Private state in recovery image:** Restrict full-image use to the same device and never commit or print NVS contents.
- **App-only compatibility risk:** Require bootloader/partition/app compatibility review; stop rather than widen the write.
- **Port identity drift:** Never reuse historical `COM7`; mismatch invalidates authorization.
- **False confidence from serial success:** Serial-only pass proves only the bounded startup behavior.
- **Optional PSRAM failure:** Keep it disabled or nonessential so startup remains observable.

## Failure Containment

- Default-deny every external peripheral and storage mutation.
- Keep the application behavior bounded and deterministic.
- Use no reboot loop as a recovery mechanism.
- Do not retry Flash automatically.
- Treat every changed image, path, hash, offset, range, port, or device identity as invalidating authorization.
- Preserve logs and stop at the first failure condition.

## Explicit User Authorization Checkpoint

The authorization checkpoint is a hard boundary after all host-only work and human review. It is satisfied only by an explicit statement that identifies the exact device, exact port, exact image and hash, exact offset, exact range, recovery status, and acknowledged risks. No task after the boundary that writes the device may start without it.

## Testing Plan

1. Separately review and explicitly authorize the minimum device-read-only inspection needed to resolve the configuration gate.
2. Reconcile sanitized device evidence with image declarations and exact ESP-IDF v5.5.4 options; keep the firmware gate closed on ambiguity.
3. Host-only static review of source and configuration allowlist.
4. Separate ESP-IDF v5.5.4 configure and build commands.
5. Host-only inspection of target, image metadata, image hash, size, write range, partition compatibility, and Git state.
6. Host-only recovery rehearsal and human review.
7. Exact First Flash authorization checkpoint.
8. Separate device connection/identity check; any mismatch invalidates authorization.
9. One exact Flash write.
10. Separate serial observation for ready marker, reset/panic/watchdog absence, and bounded stability window.
11. Pass/fail decision and, if required, separately authorized full-image rollback.
12. Evidence recording that distinguishes compiled, host-tested, device-tested, restored, and unverified claims.

This smoke test does not exercise animation, frame time, display memory, audio tasks, or product workloads. It must nevertheless report heap/PSRAM initialization failures and watchdog/reset behavior visible in startup logs; performance validation remains future work.

## Open Questions

- Which authoritative electrical evidence resolves the installed-tool 3.3 V output versus the prior 1.8 V record?
- Which exact ESP-IDF v5.5.4 options reconcile current 32 MB/Octal device evidence with the original images' DIO/16 MB headers?
- Should PSRAM remain entirely disabled for the first serial-only revision?
- Can an app-only write be proven compatible with the preserved original bootloader and OTA layout?
- What exact observation duration will be accepted at pre-Flash review?

The E: location question is resolved at the volume-path level. Whether D: and E: are on different physical disks was not investigated and is not claimed.

## Alternatives Considered

- **Reuse the host-baseline binaries:** Rejected; they have no board configuration or Flash authorization.
- **Copy generic ESP32-S3 defaults:** Rejected; they are not PCB V1.0 evidence.
- **Adopt original image-header values directly:** Rejected; header declarations are not physical-device configuration and conflict with prior records.
- **Enable all repository-recorded peripherals in one smoke test:** Rejected; component/source evidence does not prove wiring and prevents failure isolation.
- **Include display/touch immediately:** Deferred to later independent Changes after serial startup and exact peripheral evidence.
- **Flash bootloader, partition table, and app together:** Rejected as the default because it unnecessarily widens the first write.
- **Use the erased upper 16 MiB:** Rejected; the region is not part of the reviewed original partition plan.
- **Enable PSRAM unconditionally:** Rejected; it is unnecessary for serial acceptance and its exact configuration gate is unresolved.
