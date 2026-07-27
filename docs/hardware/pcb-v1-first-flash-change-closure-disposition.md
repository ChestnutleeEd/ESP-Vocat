# PCB V1.0 First-Flash Change Closure Disposition

- Document status: Current closure disposition
- Disposition date: 2026-07-27
- OpenSpec Change: `prepare-pcb-v1-first-flash-smoke-test`
- Change primary status:
  **DEVICE OPERATIONS CLOSED — DOCUMENTATION CLOSURE PENDING**
- Evidence cutoff: repository HEAD
  `79c9e69a98cf3972f348d1090e3ea958965e42f7`

## 1. Document purpose and authority

This document is the latest closure disposition for the current Change. It
summarizes and explains the existing planning, readiness, execution, and
observation records without rewriting, deleting, or weakening their historical
evidence.

Historical records remain authoritative for what was known and decided at
their respective times. Some earlier records state that Task 3.4 was not
complete because the preserved Bootloader had not yet started the candidate.
For the current Task 3.4 state, the later startup observation and the current
`tasks.md` are controlling: the preserved boot chain loaded the candidate and
the bounded observation supplied the missing runtime evidence.

That supersession applies only to state evolution. It does not supersede or
reclassify the First Flash attempt. The First Flash write result remains
**STOPPED / INCONCLUSIVE — RANGE-SCOPE DEVIATION** and must not be described
as a pass, a successful exact-range write, a completed First Flash, or a fully
verified write.

This document contains no device command, authorization template, archive
authorization, or authority for future device access.

## 2. Executive disposition

| Dimension | Current state |
|---|---|
| Change implementation status | **PARTIALLY COMPLETED — TERMINATED WITH EVIDENCE PRESERVED** |
| First Flash write result | **STOPPED / INCONCLUSIVE — RANGE-SCOPE DEVIATION** |
| Startup observation | **PASS** |
| Runtime validation | **PASS FOR THIS MINIMAL SMOKE TEST** |
| Rollback condition | **CONDITION NOT TRIGGERED / NOT AUTHORIZED / NOT PERFORMED** |
| Device-operation status | **CLOSED — NO CURRENT AUTHORIZATION** |
| Archive eligibility | **NOT ARCHIVE-READY — DOCUMENTATION CLOSURE PENDING** |

The startup and runtime results are **CONFIRMED** for the bounded minimal
smoke test. They prove that the preserved boot chain selected and loaded the
candidate, the expected serial lines and ready marker appeared, and no listed
failure occurred during the 60-second observation. They do not convert the
range-deviating write into a successful First Flash.

## 3. Confirmed accomplishments

The following accomplishments are supported by the repository evidence:

- **CONFIRMED:** PCB V1.0 configuration, hardware evidence, evidence levels,
  and safety boundaries were reviewed and recorded.
- **CONFIRMED:** same-device original recovery assets were prepared and
  hash-checked at the recorded readiness and operation checkpoints.
- **CONFIRMED:** the minimal serial-only smoke-test firmware was implemented,
  configured, built host-only, statically reviewed, and inspected as an
  artifact.
- **CONFIRMED:** the candidate excludes project GPIO use, PSRAM
  initialization, peripheral initialization, product storage, network, OTA,
  and deliberate persistent-data mutation.
- **CONFIRMED:** a fail-closed single-attempt execution tool and pure-host
  tests were prepared for the reviewed attempt boundary.
- **CONFIRMED:** one First Flash attempt occurred and its actual output,
  transmitted geometry, authorization deviation, privacy handling, and stop
  response were preserved.
- **CONFIRMED:** a later independently authorized startup-only observation
  opened the reviewed endpoint once, reset once, sent no serial data, and used
  the same handle for the bounded observation.
- **CONFIRMED:** the eight expected candidate lines appeared once and in
  order, the ready marker appeared once within the deadline, and the minimal
  runtime remained free of the listed reset, panic, watchdog, allocation,
  Bootloader-fatal, and security-rejection symptoms for 60 seconds.
- **CONFIRMED:** Task 3.4 and Tasks 12.1–12.3 have current completion
  evidence.
- **CONFIRMED:** the current OpenSpec Task state is `42/48`, with six Tasks
  still unchecked.

These accomplishments are not evidence for any untested peripheral or for
complete PCB, product, recovery, or long-term behavior.

## 4. Explicitly incomplete or unverified scope

The following remain incomplete, unverified, or not performed:

- **APPLICABLE-INCOMPLETE:** an exact-range-compliant First Flash write;
- **HISTORICAL-PROCESS-GAP:** the independently completed and recorded
  pre-write connection, identity, and authorization-comparison steps in Tasks
  10.1–10.3;
- **UNVERIFIED:** PSRAM initialization and runtime behavior;
- **UNVERIFIED / NOT TESTED:** display initialization, display output, screen
  touch, top capacitive touch, audio, microphone, motor, Wi-Fi, Bluetooth,
  other GPIOs, and other peripheral hardware;
- **UNVERIFIED:** stability beyond the bounded 60-second observation;
- **UNVERIFIED / NOT TESTED:** a complete power-off and power-on cycle;
- **NOT AUTHORIZED / NOT PERFORMED:** rollback or restore execution;
- **UNVERIFIED / NOT TESTED:** original-device startup and original-function
  recovery after a restore;
- **NOT PERFORMED:** a Level 2 32 MiB full-recovery rehearsal.

The currently observed black screen cannot be classified by this document as
either a display failure or normal display operation. The minimal smoke-test
firmware intentionally did not initialize the display, so display health
remains **UNVERIFIED**.

## 5. First Flash range-scope deviation

The controlling ranges are:

| Meaning | Range or value |
|---|---|
| candidate semantic range | `0x00020000–0x0004743F` |
| candidate end-exclusive | `0x00047440` |
| candidate size | `160832 bytes` / `0x00027440` |
| sector erase envelope | `0x00020000–0x00047FFF` |
| esptool actual transmitted range | `0x00020000–0x000477FF` |
| esptool actual transmitted endpoint | `0x000477FF` |
| transport size | `161792 bytes` |
| final ROM-block padding | `960 bytes of 0xFF` at `0x00047440–0x000477FF` |
| erased but not transmitted tail | `0x00047800–0x00047FFF` |

Esptool reported that the candidate-range hash was verified. Installed-source
analysis confirms that the ROM no-stub path transmitted a complete final
1024-byte block consisting of 64 candidate bytes and 960 generated `0xFF`
bytes. The candidate header, segments, checksum, and appended hash end before
that padding and were not changed by it.

The padding remained inside the reviewed sector erase envelope. Host analysis
and NOR logical data semantics **STRONGLY SUPPORT** that sending `0xFF` over
the just-erased padding range requested no additional main-array `1`-to-`0`
data transition beyond the preceding erase. Actual tail readback, internal
ECC or metadata, program stress, wear, disturb, and every other physical side
effect remain **UNVERIFIED**.

The exact authorization distinguished candidate-covered bytes from the later
erased-only range. Because the actual transmitted range extended 960 bytes
beyond the authorized candidate semantic range, exact-range acceptance was
not satisfied. Task 11.1 therefore remains unchecked and incomplete.

## 6. Remaining Task dispositions

The six Tasks below remain unchecked. These terminal dispositions explain
their current state; they do not mark the Tasks complete.

| Task | Terminal disposition | Reason | Successor ownership |
|---|---|---|---|
| 10.1 | `HISTORICAL-PROCESS-GAP` | The authorized endpoint was opened within the WRITE invocation rather than completed and recorded as the required separate connection operation. Repeating it now cannot retroactively repair that process gap. | Future First Flash retry Change with a newly reviewed device, endpoint, and authorization |
| 10.2 | `HISTORICAL-PROCESS-GAP` | Identity was observed through the write invocation's esptool connection banner rather than through the separately scoped minimum read-only identity step. | Future First Flash retry Change with new identity evidence and authorization |
| 10.3 | `HISTORICAL-PROCESS-GAP` | The connection and identity match were evaluated inside the WRITE invocation rather than closed as an independent pre-write gate. | Future First Flash retry Change with a new independent pre-write comparison |
| 11.1 | `APPLICABLE-INCOMPLETE` | One attempt occurred, but the ROM no-stub transport extended 960 `0xFF` bytes beyond the authorized candidate semantic range, so exact-range acceptance was not met. | Future First Flash retry Change with a newly reviewed transport plan and new exact authorization |
| 14.2 | `CONDITION-NOT-TRIGGERED` | The later startup/runtime observation passed for this minimal smoke test, so the failure-driven immediate Level 1 rollback condition was not triggered. No rollback was authorized or performed. | Future Recovery verification Change if and when its independent recovery gate is reviewed |
| 14.3 | `CONDITION-NOT-TRIGGERED` | No Level 1 rollback occurred, so the post-rollback original-device observation condition did not arise. | Future Recovery verification Change with its own startup and original-function evidence |

Tasks 10.1–10.3 cannot be replayed in this Change to rewrite the historical
First Flash process. Any future retry requires new Tasks, a fresh device and
endpoint review, and a new exact authorization. A condition not being
triggered does not mean that Task 14.2 or 14.3 was executed.

The current progress remains **42/48** and must not be represented as all
Tasks complete.

## 7. Rollback and recovery disposition

### 7.1 Failure-driven rollback

The independent startup observation and runtime validation are
**CONFIRMED PASS** for this minimal smoke test. No current evidence required
an immediate failure-driven Level 1 rollback. Rollback was not authorized and
was not performed.

Performing a restore solely to make unchecked Tasks appear complete would add
an unnecessary device write, would require new authority, and would violate
the distinction between a triggered recovery action and an unexecuted Task.

### 7.2 Mandatory future recovery milestone

The recovery milestone before formal custom-cat firmware remains in force
under `PROJECT_CONSTITUTION.md`, `docs/PRODUCT_SPEC_DRAFT.md`, and DEC-023 in
`docs/DECISION_LOG.md`. The fact that failure-driven Tasks 14.2 and 14.3 were
not triggered does not waive that future milestone.

A Level 1 complete original `ota_0` restore and a Level 2 32 MiB full recovery
are different operations with different ranges and risks. They must not be
described as interchangeable. Whether and when either operation is proposed,
and what evidence is required to satisfy the complete original-recovery
milestone, must be decided in an independent Recovery verification Change.
That future Change must preserve the complete-recovery requirement rather
than weakening it.

## 8. Device-operation closure

No further device operation is permitted in the current Change.

| Authorization | Current state |
|---|---|
| Device access authorization | **NONE** |
| Startup/observation authorization | **NONE** |
| Flash authorization | **NONE** |
| Readback authorization | **NONE** |
| Rollback authorization | **NONE** |
| Restore authorization | **NONE** |

Every prior one-time authorization was consumed and closed. The current
Change permits no reconnect, device identification, reset, observation,
Flash write, readback, erase, rollback, restore, or recovery action.

Any future device action requires a new independent Change, a fresh review,
and a new exact authorization. Historical port assignments, identity
observations, hashes, staging packages, review decisions, and authorization
material must not be assumed current or reused automatically.

## 9. Successor Change boundaries

This section defines scope only. It does not create a successor Change and
contains no device command.

### 9.1 First Flash retry Change

A future First Flash retry Change should include:

- new device connection and PCB V1.0 / ESP32-S3 identity verification;
- a new exact authorization packet tied to current evidence;
- separate review of the candidate semantic range, actual transport range,
  and sector erase envelope;
- a host-only pre-review of the complete ROM no-stub block plan;
- explicit authorization of any transport padding, or a separately proven
  execution path that does not exceed the authorized range;
- one newly authorized write attempt with no reuse of old authority;
- a separate startup/runtime observation and bounded acceptance record;
- an explicit rule that the successor result must not rewrite the historical
  First Flash attempt as a pass.

### 9.2 Recovery verification Change

A future Recovery verification Change should include:

- a separately reviewed Level 1 complete original `ota_0` restore;
- separate original-firmware startup observation after restore;
- a predefined and evidence-based original-function verification checklist;
- an independent escalation gate for any Level 2 32 MiB restore;
- explicit completion criteria for the required recovery milestone before
  formal custom-cat firmware work.

Neither successor may reuse the current Change's consumed authorizations.

## 10. Archive eligibility gates

Current archive eligibility is:

**NOT ARCHIVE-READY — DOCUMENTATION CLOSURE PENDING**

This state is **CONFIRMED** by the current incomplete documentation closure
and the six unchecked Tasks. Before a future archive review, at least the
following host-only documentation conditions must be satisfied:

- this closure disposition has been reviewed;
- all six unchecked Tasks have explicit terminal annotations in `tasks.md`
  without changing their checkboxes;
- `proposal.md` and `design.md` record the actual outcome;
- Task 15.4's meaning is clarified without treating it as archive approval;
- `docs/DECISION_LOG.md` records device-operation termination and successor
  separation;
- document cross-checks pass;
- OpenSpec strict validation passes;
- the Git change scope is exact and reviewed;
- no document describes the First Flash write as a pass or exact-range
  success;
- the mandatory future recovery milestone remains explicit.

This document does not authorize archive and does not prove that archive has
been approved. Whether a stopped or partially completed Change may be
archived after all documentation work is complete remains **UNVERIFIED** and
requires an independent future review.

Archive review does not require retrying First Flash inside this Change and
does not require an immediate device restore solely to close this Change.
Those statements do not waive the separate recovery milestone before formal
custom-cat firmware work.

## 11. Evidence index

| Repository evidence | Role |
|---|---|
| [`PROJECT_CONSTITUTION.md`](../../PROJECT_CONSTITUTION.md) | Highest-level Flash, recovery, evidence, and development-sequence constraints |
| [`docs/HARDWARE_PROFILE.md`](../HARDWARE_PROFILE.md) | Current hardware evidence and synchronized First Flash/startup/runtime state |
| [`docs/PRODUCT_SPEC_DRAFT.md`](../PRODUCT_SPEC_DRAFT.md) | Product-stage recovery requirement before formal custom firmware |
| [`docs/DECISION_LOG.md`](../DECISION_LOG.md) | DEC-023 First Flash validation and recovery sequence |
| [`proposal.md`](../../openspec/changes/prepare-pcb-v1-first-flash-smoke-test/proposal.md) | Original Change purpose, scope, and non-goals |
| [`design.md`](../../openspec/changes/prepare-pcb-v1-first-flash-smoke-test/design.md) | Evidence model, safety design, First Flash, rollback, and observation rationale |
| [`spec.md`](../../openspec/changes/prepare-pcb-v1-first-flash-smoke-test/specs/pcb-v1-first-flash-smoke-test/spec.md) | Normative gates, exact authorization, stop, observation, rollback, and successor requirements |
| [`tasks.md`](../../openspec/changes/prepare-pcb-v1-first-flash-smoke-test/tasks.md) | Current `42/48` Task state and synchronized final evidence |
| [`pcb-v1-first-flash-smoke-test-host-build.md`](../../tests/build/pcb-v1-first-flash-smoke-test-host-build.md) | Host-only implementation, build, static review, and artifact evidence |
| [`pcb-v1-first-flash-review-package.md`](pcb-v1-first-flash-review-package.md) | Historical pre-Flash candidate, range, recovery, and authorization review package |
| [`pcb-v1-first-flash-operation-readiness.md`](pcb-v1-first-flash-operation-readiness.md) | Historical operation mechanism, affected-range, and rollback readiness analysis |
| [`pcb-v1-recovery-and-first-flash-readiness.md`](pcb-v1-recovery-and-first-flash-readiness.md) | Recovery assets, hashes, original layout, and recovery rehearsal readiness |
| [`pcb-v1-first-flash-attempt-2026-07-26.md`](../../tests/hardware/pcb-v1-first-flash-attempt-2026-07-26.md) | Controlling sanitized First Flash attempt and range-scope deviation record |
| [`pcb-v1-post-flash-padding-and-next-action-assessment.md`](pcb-v1-post-flash-padding-and-next-action-assessment.md) | ROM no-stub padding, logical/physical-effect limits, and post-attempt decision analysis |
| [`pcb-v1-startup-observation-2026-07-27.md`](../../tests/hardware/pcb-v1-startup-observation-2026-07-27.md) | Controlling startup observation and bounded minimal-runtime PASS evidence |

The evidence files retain their own review-time meanings. This index is a
navigation and responsibility map, not a second execution record.
