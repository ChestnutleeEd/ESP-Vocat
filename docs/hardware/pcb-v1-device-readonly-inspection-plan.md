# ESP-VoCat PCB V1.0 Device Read-Only Inspection Plan

- Plan date: 2026-07-25
- Status: **NOT AUTHORIZED**
- Device access performed while preparing this plan: none
- Port placeholder: `<REVIEWED_PORT>`

## 1. Purpose

Define the smallest future device-read-only inspection needed to resolve the PCB V1.0 Flash/PSRAM configuration gate. This document is a review artifact only. It does not authorize opening a port, resetting a device, entering download mode, running a query, or collecting live output.

## 2. Why Offline Evidence Is Insufficient

The recovery images establish file bytes, image structures, and original header declarations. They do not establish current physical Flash capacity, electrical bus mode, voltage domain, live eFuse state, current port, or PSRAM detection. Compiled strings show available code paths and messages, not active Kconfig values or PCB wiring.

The original images declare DIO / 80 MHz / 16 MB, while prior repository records describe 32 MiB Octal Flash at 1.8 V and 16 MiB PSRAM. The tracked repository does not contain the raw device-read transcript needed to reconcile these evidence types.

## 3. Exact Evidence Needed

- exact ESP32-S3 identity for the reviewed PCB V1.0 unit;
- Flash manufacturer/device identification and detected capacity;
- evidence that resolves, or explicitly fails to resolve, physical Flash bus mode;
- evidence that resolves the Flash voltage domain without changing it;
- PSRAM detected capacity and mode, if obtainable through a reviewed read-only path;
- only the eFuse summary fields needed to review voltage domain, Secure Boot, Flash Encryption, download mode, JTAG, and USB Serial/JTAG state;
- exact current port supplied and reviewed as `<REVIEWED_PORT>`, never assumed from historical `COM7`;
- whether a future query requires a reset or download-mode transition.

## 4. Operation Classification

All proposed live data queries are classified **READ-ONLY** because their intended operation reads identification or summary data without writing Flash or eFuses.

Some tools may toggle control lines and reset the chip while establishing a connection. An explicit reset or download-mode transition is a separate **REBOOT** operation and is not authorized by approving a `READ-ONLY` query. If the reviewed tool cannot avoid or precisely describe that side effect, the packet must identify it and obtain separate authorization before execution.

Every category in this plan remains **NOT AUTHORIZED**.

## 5. Device Risks

- the reviewed port may refer to another serial device;
- a connection tool may reset the ESP32-S3 or alter its current runtime state without writing nonvolatile memory;
- entering download mode may interrupt the original firmware;
- an unexpected chip, PCB revision, security state, or voltage-domain result may make further work unsafe;
- repeated connection attempts may obscure the first failure or produce unnecessary resets;
- a query may expose more information than required.

No automatic retry is permitted.

## 6. Privacy Risks

Tool output may include MAC addresses, chip-unique identifiers, serial numbers, eFuse identifiers, or other unnecessary device-unique data.

- Do not request unique identifiers unless they are strictly necessary.
- Do not copy MAC addresses or unnecessary identifiers into tracked documents.
- If a tool prints them by default, redact them before evidence is added to Git.
- Do not read NVS, PHY contents, OTA metadata contents, assets, credentials, tokens, certificates, or network settings.
- Do not publish or share unredacted output.

## 7. Preconditions

Before any inspection:

1. D: and E: recovery copies still exist at their exact reviewed paths.
2. All four recovery files still have size `33554432` and SHA-256 `72421C6AF04F181D25400D75B34F7B0844CD55924F1C02B1B22A7410A5184001`.
3. The operator identifies the exact physical ESP-VoCat PCB V1.0 unit.
4. The user supplies and reviews the current port as `<REVIEWED_PORT>`; historical `COM7` is not substituted.
5. Tool path, tool version, query category, expected output, possible reset behavior, privacy handling, and stop conditions are reviewed.
6. The command packet contains no write, erase, restore, eFuse write, security change, or `read_flash`.
7. The user explicitly authorizes the exact inspection packet.

Any failed precondition keeps the plan `NOT AUTHORIZED`.

## 8. Proposed Command Categories

The following are category descriptions, not complete commands and not copyable execution instructions.

| Status | Classification | Tool/action category | Port field | Purpose |
|---|---|---|---|---|
| `NOT AUTHORIZED` | `READ-ONLY` | esptool-family chip identification query | `<REVIEWED_PORT>` | Confirm the expected ESP32-S3 family |
| `NOT AUTHORIZED` | `READ-ONLY` | esptool-family Flash ID/capacity query | `<REVIEWED_PORT>` | Record manufacturer/device identification and detected capacity |
| `NOT AUTHORIZED` | `READ-ONLY` | espefuse-family summary query, read-only mode only | `<REVIEWED_PORT>` | Review only required voltage-domain and security/download fields |
| `NOT AUTHORIZED` | `READ-ONLY` | reviewed current-port confirmation | `<REVIEWED_PORT>` | Confirm that the user-supplied port is the exact reviewed target |
| `NOT AUTHORIZED` | `REBOOT` | reviewed reset/download-mode transition, only if separately required | `<REVIEWED_PORT>` | Establish a known connection state without writing |

No executable command line is provided. Tool arguments, connection method, and any control-line behavior require a later packet review.

Reading an eFuse summary is fundamentally different from writing or burning eFuses: a summary query reads already-programmed state, while an eFuse write is irreversible. Even the read-only summary still requires explicit user authorization in this project because it accesses the physical device and may expose unique information. eFuse writes remain prohibited.

## 9. Expected Outputs

Expected sanitized evidence fields are:

- tool name and version;
- operation classification;
- reviewed port represented in the report without unrelated identifiers;
- chip family result;
- Flash manufacturer/device result and detected capacity;
- only required eFuse state fields;
- PSRAM capacity/mode result if the reviewed category can provide it safely;
- whether a reset occurred;
- exit code and concise success/failure state.

An absence of output, ambiguous field, generic capability string, or unsupported query is recorded as unresolved, not converted into an inferred value.

## 10. Stop Conditions

Stop immediately if:

- the port is not exactly the reviewed `<REVIEWED_PORT>`;
- the chip is not ESP32-S3 or the physical unit is not confirmed PCB V1.0;
- the tool proposes a write, erase, restore, `read_flash`, eFuse burn, security change, or voltage change;
- the tool requires an unreviewed reset/download transition;
- a unique identifier cannot be excluded or redacted from the planned evidence;
- Flash capacity, mode, voltage, PSRAM, or security evidence conflicts with repository records;
- the device disconnects, repeatedly resets, returns malformed output, or cannot be queried once;
- the operator would need to guess an option or retry automatically.

After a stop condition, preserve only sanitized necessary evidence and perform no additional device action.

## 11. Evidence Recording

The later record must include:

- the exact authorized categories and their classifications;
- tool paths and versions;
- the reviewed placeholder-to-port substitution approved by the user;
- timestamps and exit results;
- sanitized required output fields;
- redaction statement;
- any reset side effect;
- conflicts, unresolved facts, and the gate decision.

Do not commit raw transcripts containing MAC addresses or other unnecessary identifiers. Do not commit binaries, dumps, NVS data, partition slices, or private device data.

## 12. Explicit User Authorization Requirement

Before execution, the user must explicitly approve:

- the exact ESP-VoCat PCB V1.0 unit;
- the exact current port replacing `<REVIEWED_PORT>`;
- each exact query category;
- tool path and version;
- expected data;
- any possible reset/download-mode side effect;
- privacy/redaction handling;
- stop conditions;
- the exclusion of all writes, erases, restores, and eFuse changes.

Statements such as “继续”, “下一步”, “可以”, “proceed”, or “do the next step” are vague and do not authorize device access.

## 13. Operations Explicitly Excluded

- `read_flash`;
- any Flash write;
- any Flash erase, including `erase_flash`;
- restore or rollback execution;
- any eFuse write, burn, key, purpose, voltage, security, JTAG, USB Serial/JTAG, or download-mode change;
- Secure Boot or Flash Encryption activation or modification;
- firmware build, `idf.py`, monitor, or combined Flash/monitor operation;
- NVS, PHY, OTA metadata, or assets content reads;
- GPIO, display, touch, audio, microphone, motor, SD, battery, or power-control activity;
- port enumeration beyond the exact later-authorized method;
- automatic retry.

## 14. Post-Inspection Decision Rules

1. Treat device output only as evidence for the exact fields observed.
2. Do not infer PCB wiring, Flash voltage, or PSRAM mode from generic strings.
3. Reconcile physical results with both original image declarations and ESP-IDF v5.5.4 configuration semantics.
4. If every mandatory Flash configuration input becomes traceable and nonconflicting, update the board decision and request a separate review before firmware implementation.
5. If Flash mode, voltage, capacity, frequency, PSRAM mode, or their ESP-IDF mapping remains ambiguous, keep the Firmware Implementation Gate **CLOSED**.
6. Device-read-only authorization does not authorize firmware implementation, build, Flash, monitor, or rollback.
7. First Flash remains **NO-GO** until all later artifact, recovery, identity, port, and exact user-authorization gates are independently satisfied.
