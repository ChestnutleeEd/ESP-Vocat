# ESP-VoCat Project Decision Log
- Document status: Active
- Version: 0.1
- Established: 2026-07-11
- Target hardware: ESP-VoCat PCB V1.0
This document records confirmed product, architecture and development-process decisions.
Rules:
1. Existing decisions must not be silently deleted.
2. A changed decision must retain the previous decision and add a new superseding entry.
3. Every entry should record:
   - decision;
   - reason;
   - alternatives considered;
   - consequences;
   - affected documents or modules.
4. Product requirements remain defined in `PRODUCT_SPEC_DRAFT.md`.
5. Safety constraints remain defined in `PROJECT_CONSTITUTION.md`.
---
## Document registry
### PROJECT_CONSTITUTION.md
Purpose:
- highest-level safety and engineering constraints;
- prohibited operations;
- development and Flash-safety rules.
SHA-256 when initially recorded:
`F41FA811FF8BD8DC69F7EA5F4EF767E74A07792B02F5630BB16251187C8271E3`
### docs/PRODUCT_SPEC_DRAFT.md
Purpose:
- current product behavior;
- interaction design;
- voice, reminder, animation and companion-app requirements.
SHA-256 when initially recorded:
`A3B374A53CEA839B0F53EC3AB1A1793EC81B152EF111E51E922C8F403755F354`
### docs/HARDWARE_PROFILE.md
Purpose:
- confirmed hardware facts;
- strongly supported reference information;
- unresolved hardware questions.
SHA-256 when initially recorded:
`BCAA48C392C7C97F0E425A6EE3B9026D42E9FC9E1D26A5A1AFD87E532D14699B`
### AGENTS.md
Purpose:
- operational rules for Codex and other development agents;
- command classification;
- scope, testing and documentation requirements.
SHA-256 when initially recorded:
`C3C75C5326E532D45C128082AE0F1D21E4AF2C671B748020C2A63C8E5EF74B64`
---
# Confirmed decisions
## DEC-001: Product positioning
Date:
2026-07-11
Status:
Accepted
Decision:
The product will be designed as an embodied desktop cat assistant.
Priority order:
1. electronic-pet experience;
2. desktop information display;
3. fixed-command voice assistant;
4. rotating-base embodiment;
5. open-ended AI conversation in a later stage.
Reason:
The primary value is the feeling of a living desktop cat, not a generic information screen with a cat theme.
Alternatives considered:
- information terminal first;
- voice assistant first;
- direct recreation of the existing Xiaozhi AI experience.
Consequences:
- animation, interaction latency and personality have higher priority than adding many information functions;
- open-ended AI dialogue is not a first-release requirement.
Affected areas:
- product specification;
- firmware state machine;
- animation system;
- UI design;
- release roadmap.
---
## DEC-002: First cat appearance
Date:
2026-07-11
Status:
Accepted
Decision:
The first cat will be a full-body orange tabby.
Visual style:
- between a realistic kitten and a chibi character;
- slightly enlarged head;
- Japanese cartoon influence;
- clean flat illustration;
- complete body, legs and tail.
Reason:
A full-body cat can express walking, sleeping, stretching, turning and leaving the screen, which are central to the product experience.
Alternatives considered:
- cat head only;
- upper body only;
- highly realistic rendering;
- strongly exaggerated chibi rendering.
Consequences:
- the animation system must support body movement and locomotion;
- assets must share a common future-compatible character contract;
- first release contains one cat, while the architecture reserves future cat selection.
Affected areas:
- cat assets;
- animation state machine;
- touch hit regions;
- transitions;
- companion settings.
---
## DEC-003: Personality architecture
Date:
2026-07-11
Status:
Accepted
Decision:
Personality will primarily modify behavior parameters rather than require completely independent animation sets.
Initial personality directions:
- gentle and affectionate;
- lively and mischievous;
- lazy and aloof.
Personality may affect:
- action probability;
- cooldown;
- interaction response;
- cat-sound choice;
- wording;
- active-behavior frequency.
Reason:
This provides meaningful variation while controlling the animation-asset workload.
Alternatives considered:
- one fixed personality;
- fully separate asset set for every personality.
Consequences:
- animation actions should expose configurable probability and timing parameters;
- personality data should be stored separately from core animations.
Affected areas:
- behavior engine;
- settings;
- audio selection;
- animation scheduling.
---
## DEC-004: Home-screen information
Date:
2026-07-11
Status:
Accepted
Decision:
The cat remains the visual focus. Lightweight information appears near the screen edge when the cat is relatively still.
Information may include:
- time;
- weather icon;
- battery status;
- computer connection;
- mute state;
- microphone state;
- reminder indicator.
Reason:
Information must be available without turning the home screen into a dashboard.
Alternatives considered:
- no information on the pet home screen;
- permanent full dashboard;
- separate information-only home page.
Consequences:
- large cat actions cause lightweight information to fade out;
- breathing, blinking and minor tail movement may keep information visible;
- background themes change between day, dusk and night.
Affected areas:
- renderer;
- home UI;
- animation events;
- time-theme controller.
---
## DEC-005: Screen interaction
Date:
2026-07-11
Status:
Accepted
Decision:
Screen interaction uses hit regions combined with randomized action pools.
Regions:
- head;
- body;
- tail;
- empty background.
Long press:
- approximately 1 second: cat shows uncertainty;
- approximately 1.5 seconds: transition toward the information menu;
- noticeable finger movement cancels menu activation.
Reason:
Region-aware reactions feel more alive, while random pools prevent repetitive feedback.
Alternatives considered:
- every tap produces the same reaction;
- gesture-heavy interaction;
- menu button always visible.
Consequences:
- touch coordinates must be mapped into character-space hit regions;
- interaction actions need cooldown and random selection;
- long press requires movement cancellation and duration thresholds.
Affected areas:
- CST816S input;
- interaction controller;
- animation state machine;
- UI transition logic.
---
## DEC-006: Top petting interaction
Date:
2026-07-11
Status:
Accepted
Decision:
The top capacitive-touch input will provide progressive petting behavior.
Behavior:
- short touch: look up, squint or short meow;
- 1-3 seconds: relax and respond;
- more than 3 seconds: close eyes, lie down or purr;
- repeated touches: temporary happiness or personality-specific response.
Reason:
Top petting is a distinctive physical interaction already associated with the device.
Alternatives considered:
- use top touch as a menu control;
- use it only as a binary button;
- remove top interaction.
Consequences:
- top touch must remain separate from screen-menu navigation;
- first version may use a temporary happiness state;
- complex feeding or economy systems are excluded.
Affected areas:
- capacitive-touch driver;
- behavior state;
- audio;
- animation.
---
## DEC-007: Information-menu design
Date:
2026-07-11
Status:
Accepted
Decision:
The information menu will use a radial layout.
Behavior:
- the full-body cat exits the home scene;
- a small cat head appears in the center;
- menu icons surround the cat head;
- the head and eyes look toward the selected function;
- the selected icon enlarges;
- inactive icons reduce emphasis.
Initial functions:
- weather;
- schedule;
- reminders;
- Codex usage;
- sound;
- settings.
Reason:
The radial menu fits the circular screen and retains character presence.
Alternatives considered:
- three large icons per page;
- one card per page;
- ordinary grid menu.
Consequences:
- circular touch targets need expanded invisible hit areas;
- the small cat head requires directional gaze states;
- function pages do not show the cat continuously.
Affected areas:
- menu renderer;
- touch navigation;
- transition animation;
- function-page framework.
---
## DEC-008: Navigation and timeout
Date:
2026-07-11
Status:
Accepted
Decision:
Navigation rules:
- return icon: return to radial menu;
- screen long press: return directly to cat home;
- voice command "回去": return one level;
- voice command "回到主页": return to cat home;
- 30 seconds without operation: automatically return to cat home.
Reason:
Multiple return paths improve usability while keeping the information interface temporary.
Alternatives considered:
- manual exit only;
- swipe-only return;
- 15-second timeout;
- different timeout for every function.
Consequences:
- all function pages require consistent timeout handling;
- voice navigation and touch navigation must use the same navigation controller.
Affected areas:
- navigation state machine;
- voice commands;
- function-page lifecycle.
---
## DEC-009: Computer-first external data architecture
Date:
2026-07-11
Status:
Accepted
Decision:
The Windows companion application will initially retrieve and prepare external data, then send simplified data to the ESP-VoCat through USB.
Computer responsibilities include:
- weather retrieval;
- Codex-usage retrieval or aggregation;
- schedule and reminder editing;
- dynamic speech generation;
- complete history;
- settings synchronization.
Device responsibilities include:
- cat behavior;
- rendering;
- touch;
- fixed local commands;
- local reminders;
- cached data;
- audio playback;
- safe base control after verification.
Reason:
The device will normally be powered while connected to a computer. Computer-side retrieval avoids unnecessary HTTPS, account-session and API complexity on ESP32.
Alternatives considered:
- device retrieves all data directly through Wi-Fi;
- device depends completely on the computer;
- cloud server between computer and device.
Consequences:
- USB protocol and companion application become first-class modules;
- core pet functions must remain available when the computer disconnects;
- account cookies and reusable sessions must not be stored on ESP32.
Affected areas:
- system architecture;
- USB protocol;
- companion application;
- offline behavior;
- security.
---
## DEC-010: Companion application first version
Date:
2026-07-11
Status:
Accepted
Decision:
The first companion application will be a lightweight Windows configuration window.
Behavior:
- may start automatically with Windows;
- auto-start can be disabled;
- may continue in the system tray;
- automatically discovers the device;
- must not permanently assume COM7.
Initial configuration includes:
- connection state;
- cat display name;
- personality;
- audio;
- brightness;
- time themes;
- weather city;
- schedules and reminders;
- data timestamps;
- automatic sound-direction turning;
- immediate synchronization.
Reason:
A lightweight configuration application is sufficient for the first version and reduces desktop-development scope.
Alternatives considered:
- full desktop pet application;
- command-line-only bridge;
- web interface.
Consequences:
- rich animation preview and asset management are postponed;
- the communication layer should remain reusable by a future full application.
Affected areas:
- companion application;
- protocol;
- device discovery;
- synchronization.
---
## DEC-011: Data synchronization and caching
Date:
2026-07-11
Status:
Accepted
Decision:
Initial proposed synchronization intervals:
- weather: every 30 minutes;
- Codex usage: every 10 minutes;
- time calibration: at connection and every hour;
- schedules, reminders and settings: immediately after changes.
When disconnected:
- preserve the last weather and Codex values;
- display their last update time;
- indicate stale or cached state;
- continue pet, touch and local reminder behavior.
Reason:
Cached information is more useful than hiding the function, provided that freshness is explicit.
Alternatives considered:
- hide external-data pages when disconnected;
- always treat cached data as current;
- require direct device Wi-Fi retrieval.
Consequences:
- every external-data payload requires timestamps;
- connection state and freshness are separate concepts.
Affected areas:
- USB messages;
- local storage;
- weather page;
- Codex page;
- companion scheduler.
---
## DEC-012: Voice-name strategy
Date:
2026-07-11
Status:
Accepted
Decision:
The cat display name and the technical wake word may be different.
- display name is user-configurable;
- first-release wake word is fixed or selected from supported models;
- arbitrary custom-name offline wake-word generation is not required initially.
Reason:
Reliable wake-word detection is more important than allowing arbitrary names in the first version.
Alternatives considered:
- display name must always be the wake word;
- fixed display name and wake word;
- computer-only voice activation.
Consequences:
- UI and narration use the display name;
- low-level speech detection uses a separately configured wake word.
Affected areas:
- settings;
- wake-word engine;
- companion application;
- UI text.
---
## DEC-013: Microphone modes
Date:
2026-07-11
Status:
Accepted
Decision:
Three microphone modes will be supported:
1. continuous wake-word detection;
2. touch-to-listen;
3. microphone disabled.
Reason:
This balances natural interaction with privacy and resource control.
Alternatives considered:
- always listening only;
- touch-to-listen only.
Consequences:
- microphone state must be visible;
- disabled mode must stop unnecessary audio-processing work;
- touch interaction must be able to enter listening state directly.
Affected areas:
- audio input;
- wake word;
- UI status;
- settings.
---
## DEC-014: Reminder parsing
Date:
2026-07-11
Status:
Accepted
Decision:
Reminder creation uses a hybrid parser.
- simple expressions are handled locally where reliable;
- complex expressions may be processed by the companion application;
- unsupported expressions must not be guessed.
Reason:
Basic offline usefulness is retained without requiring a large language or speech system on the ESP32.
Alternatives considered:
- all parsing on device;
- all parsing on computer;
- no voice-created reminders.
Consequences:
- parser capability must be explicit;
- offline failure responses should guide the user toward simpler phrasing.
Affected areas:
- voice commands;
- reminder parser;
- companion application;
- error feedback.
---
## DEC-015: Sound-direction turning
Date:
2026-07-11
Status:
Accepted
Decision:
The rotating base turns only after:
1. a valid wake word is detected;
2. a direction estimate is available;
3. confidence exceeds a threshold.
Additional rules:
- front-facing dead zone;
- movement cooldown;
- repeated-command suppression;
- user setting to disable automatic turning;
- low confidence produces only screen animation.
Reason:
Physical movement must not respond unpredictably to environmental noise.
Alternatives considered:
- turn toward every sound;
- turn immediately after every wake-word detection;
- no physical turning.
Consequences:
- source localization requires confidence scoring;
- screen gaze can provide a safe fallback;
- base testing remains blocked until UART and protocol verification.
Affected areas:
- microphone processing;
- base driver;
- animation;
- settings;
- safety tests.
---
## DEC-016: Audio identity
Date:
2026-07-11
Status:
Accepted
Decision:
- interaction retains cat-like sounds;
- information narration uses a clear, gentle Mandarin voice;
- first release uses one narration voice;
- personality affects wording, animation and cat-sound choice;
- wake response defaults to a short meow;
- human-voice wake response can be enabled;
- narration detail can be configured as short or detailed.
Reason:
Cat sounds preserve pet identity, while human speech improves information clarity.
Alternatives considered:
- all-human voice;
- cat sounds only;
- separate narration voice for every personality.
Consequences:
- cat sound assets remain local;
- dynamic narration may be generated by the companion application;
- audio categories require separate settings.
Affected areas:
- audio assets;
- playback;
- narration;
- settings.
---
## DEC-017: Offline reminder behavior
Date:
2026-07-11
Status:
Accepted
Decision:
When the computer is disconnected and a reminder is due:
1. play the fixed voice template "你有一个提醒";
2. show a reminder animation;
3. display the complete reminder text.
Snooze options:
- 5 minutes;
- 10 minutes;
- 30 minutes;
- custom.
Missed reminders after power loss:
- show silently after boot;
- do not automatically speak loudly.
Reason:
This guarantees reliable reminders without requiring arbitrary offline speech synthesis.
Alternatives considered:
- no offline reminder;
- cat sound only;
- low-quality on-device arbitrary speech synthesis.
Consequences:
- fixed narration assets must be stored locally;
- reminder text must be cached on device;
- missed reminders require boot-time reconciliation.
Affected areas:
- reminder engine;
- local storage;
- audio;
- UI.
---
## DEC-018: Schedule and reminder scope
Date:
2026-07-11
Status:
Accepted
Decision:
First-release recurrence rules:
- once;
- daily;
- weekdays;
- selected weekday or weekdays.
Definitions:
- schedule: an event occupying a time range;
- reminder: an active notification at a time;
- a schedule may contain an advance reminder.
History:
- device stores current and recent necessary information;
- companion application stores complete history.
Reason:
This covers normal personal use without implementing a complete calendar platform.
Alternatives considered:
- one-time reminders only;
- complete calendar recurrence rules;
- permanent full history on device.
Consequences:
- device persistence can remain bounded;
- advanced recurrence and cloud-calendar integration are postponed.
Affected areas:
- data model;
- reminder scheduler;
- companion application;
- synchronization.
---
## DEC-019: Animation implementation and performance
Date:
2026-07-11
Status:
Accepted
Decision:
Use a hybrid animation approach.
- major actions and transitions target 30 FPS;
- idle actions target 15-20 FPS;
- slow sleep actions may use 10-15 FPS;
- boot to interactive home should remain within 5 seconds.
Animation techniques:
- component or skeletal-style transforms for reusable small movements;
- frame-by-frame assets for expressive complex actions.
Reason:
This balances visual quality, Flash use, memory use and runtime load.
Alternatives considered:
- all frame-by-frame;
- all skeletal;
- universal 20 FPS;
- universal 30 FPS.
Consequences:
- animation assets require a common manifest;
- performance must be measured on actual hardware;
- large full-screen uncompressed assets require explicit analysis.
Affected areas:
- renderer;
- asset pipeline;
- PSRAM;
- Flash partitions;
- performance tests.
---
## DEC-020: First animation scope
Date:
2026-07-11
Status:
Accepted
Decision:
The first version targets approximately 18 core action categories:
1. idle breathing;
2. blinking;
3. tail movement;
4. looking around;
5. sitting;
6. lying down;
7. sleeping;
8. standing;
9. entering;
10. exiting;
11. happiness;
12. confusion;
13. head petting;
14. sustained petting;
15. stretching;
16. listening;
17. narration;
18. reminder or ear-raising reaction.
Reason:
A coherent base action set is more valuable than a large collection of incomplete animations.
Alternatives considered:
- dozens of animations immediately;
- only idle and blink for the first release.
Consequences:
- advanced jumping, rolling and chase play are postponed;
- each action may later gain randomized variants.
Affected areas:
- animation assets;
- behavior engine;
- acceptance tests.
---
## DEC-021: Idle, sleep and display behavior
Date:
2026-07-11
Status:
Accepted
Decision:
Use progressive inactivity behavior:
- normal idle;
- lower activity;
- lying down;
- sleep;
- reduced brightness.
Initial timing proposal:
- 5 minutes: reduce activity;
- 15 minutes: lie down or sleep;
- 30 minutes: low-brightness state.
Display setting:
- user can choose always-on with dimming;
- or automatic complete screen-off.
Computer disconnected:
- daytime: normal offline-pet mode;
- nighttime: faster sleep, reduced brightness and reduced sound.
Reason:
The cat should remain present while reducing unnecessary display load and disturbance.
Alternatives considered:
- permanently active;
- always turn the screen off;
- immediate power saving after disconnect.
Consequences:
- inactivity and time theme must be separate state inputs;
- touch, wake word and reminders can wake the character.
Affected areas:
- state machine;
- display brightness;
- audio level;
- time themes.
---
## DEC-022: Development methodology
Date:
2026-07-11
Status:
Accepted
Decision:
Use lightweight specification-driven development.
Current required documents:
- project constitution;
- product specification;
- hardware profile;
- decision log;
- Codex instructions;
- later architecture, protocol, test and feature specifications.
Full Spec Kit integration is postponed until the overall design and first development stages are stable.
Reason:
Clear specifications are necessary, but installing a full workflow framework too early would interrupt product and hardware preparation.
Alternatives considered:
- rely only on conversational prompts;
- install and enforce full Spec Kit immediately;
- begin coding without written specifications.
Consequences:
- Codex must work from repository documents;
- feature work will later receive individual specifications and acceptance criteria;
- Spec Kit may be introduced without rewriting the core decisions.
Affected areas:
- project workflow;
- documentation;
- Codex usage;
- roadmap.
---
## DEC-023: First Flash validation sequence
Date:
2026-07-11
Status:
Accepted
Decision:
Before formal cat-firmware development:
1. compile ESP-IDF Hello World without Flashing;
2. build a PCB V1.0-specific smoke-test firmware;
3. review its binaries and Flash layout;
4. perform the first controlled Flash write;
5. verify serial, display, screen touch and top touch;
6. restore the complete original Xiaozhi image;
7. verify the original system recovery;
8. only then begin formal custom firmware Flashing.
Reason:
The first Flash operation and the recovery mechanism must be validated before relying on them during larger development.
Alternatives considered:
- directly Flash the full cat firmware;
- Flash generic Hello World;
- rely on backups without performing a recovery rehearsal.
Consequences:
- motor, audio, Wi-Fi and other high-uncertainty modules are excluded from the smoke test;
- restoration rehearsal is mandatory.
Affected areas:
- development sequence;
- smoke-test specification;
- recovery testing;
- project constitution.
---
## DEC-024: Close device operations for the PCB V1.0 First Flash Change and separate retry from recovery verification
Date:
2026-07-27
Status:
Accepted
Context:
The 2026-07-26 First Flash attempt produced a range-scope deviation: the ROM
no-stub transport sent 960 bytes of `0xFF` beyond the authorized candidate
semantic range. A later independently authorized observation passed startup
and runtime validation for the bounded minimal smoke test, but the First Flash
write result remains **STOPPED / INCONCLUSIVE — RANGE-SCOPE DEVIATION**.
Every one-time device authorization has been consumed and closed. Continuing
device work in the current Change would mix the historical attempt, new
authority, and the separate recovery path.
Decision:
- stop all device operations in the current Change and limit it to
  documentation closure;
- assign any First Flash retry to an independent successor Change with fresh
  review and authorization;
- assign recovery verification to a separate successor Change with its own
  review, authorization, startup observation, and original-function evidence;
- perform no immediate rollback and no retry in the current Change;
- reuse no prior authorization;
- preserve DEC-023 and the complete original-recovery requirement before
  formal custom-cat firmware work.
Alternatives considered:
- retry the write in the current Change;
- perform an immediate rollback solely to close unchecked Tasks;
- combine retry and recovery verification in one successor Change;
- treat the bounded startup PASS as exact-range write acceptance.
Consequences:
- the current black screen does not establish display health or display
  failure because the minimal smoke-test firmware did not initialize the
  display;
- the current Change remains at `42/48` and is **NOT ARCHIVE-READY —
  DOCUMENTATION CLOSURE PENDING**;
- every future device operation requires a new review and exact
  authorization;
- future retry or recovery evidence must not change the historical First
  Flash attempt result;
- the Level 2 32 MiB recovery milestone remains independently gated and is
  not waived.
Affected areas:
- OpenSpec Change `prepare-pcb-v1-first-flash-smoke-test`;
- First Flash retry planning;
- recovery verification planning;
- device-authorization boundaries;
- DEC-023 recovery sequence.

Controlling disposition:
[`PCB V1.0 First-Flash Change Closure Disposition`](hardware/pcb-v1-first-flash-change-closure-disposition.md).
---
# Pending decisions
The following topics remain open:
- Windows companion application technology stack;
- USB protocol framing and serialization;
- firmware foundation and source-reuse strategy;
- display and animation asset format;
- custom partition layout;
- local schedule-storage format;
- exact fixed voice-command list;
- concrete wake word;
- weather source;
- Codex-usage retrieval source;
- dynamic speech-generation engine;
- source-localization feasibility;
- rotating-base UART verification;
- animation-production workflow;
- memory and performance acceptance thresholds;
- settings-screen detailed layout;
- development phases and milestone definitions.
---
# Decision update template
Use the following structure for future decisions:
## DEC-XXX: Title
Date:
YYYY-MM-DD
Status:
Proposed / Accepted / Superseded / Rejected
Decision:
State the selected approach.
Reason:
Explain why it was selected.
Alternatives considered:
List relevant alternatives.
Consequences:
List technical and product implications.
Affected areas:
List documents, modules and tests affected.
Supersedes:
Reference an older decision when applicable.
