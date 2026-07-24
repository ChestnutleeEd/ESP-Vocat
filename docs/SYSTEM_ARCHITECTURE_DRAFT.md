# ESP-VoCat System Architecture Draft
- Document status: Draft
- Version: 0.1
- Updated: 2026-07-25
- Target hardware: ESP-VoCat PCB V1.0
- Current stage: architecture definition before implementation
This document defines the initial system boundaries for the ESP-VoCat embodied desktop cat assistant.
It does not authorize device flashing, Flash erasure, motor activation or irreversible hardware operations.
The following documents take precedence or provide complementary requirements:
1. `PROJECT_CONSTITUTION.md`
2. `docs/HARDWARE_PROFILE.md`
3. `docs/PRODUCT_SPEC_DRAFT.md`
4. `docs/DECISION_LOG.md`
5. `AGENTS.md`
---
## 1. Architecture goals
The architecture must prioritize:
1. responsive local cat interaction;
2. smooth animation;
3. reliable touch and reminders;
4. safe hardware control;
5. graceful computer disconnection;
6. clear responsibility boundaries;
7. incremental implementation and testing;
8. future expansion without a complete rewrite.
The device must continue to feel like a living desktop cat when the Windows companion application is unavailable.
---
## 2. System overview
The product contains two primary runtime systems.
```text
Windows computer
└── ESP-VoCat Companion Application
    ├── device discovery
    ├── USB serial communication
    ├── weather retrieval
    ├── Codex usage retrieval or aggregation
    ├── schedule and reminder editing
    ├── complex reminder parsing
    ├── dynamic speech generation
    ├── complete history storage
    └── device configuration
USB connection
└── versioned bidirectional protocol
ESP-VoCat device
└── ESP32-S3 firmware
    ├── board support
    ├── display and rendering
    ├── cat animation
    ├── behavior state machine
    ├── screen touch
    ├── top petting
    ├── local audio playback
    ├── microphone modes
    ├── local fixed commands
    ├── schedules and reminders
    ├── cached external information
    ├── settings persistence
    ├── USB protocol endpoint
    └── rotating-base control after verification
```
The companion application is responsible for data aggregation and configuration.
The ESP-VoCat device is responsible for the real-time embodied experience.
---
## 3. Core architecture principles
### 3.1 Local-first interaction
The following functions must not require a computer round trip:
- idle cat animation;
- breathing and blinking;
- screen-touch reactions;
- top petting;
- local cat sounds;
- local menu navigation;
- viewing cached information;
- locally stored reminders;
- supported local fixed commands;
- sleep and wake behavior;
- settings required for normal operation.
A computer delay or disconnection must not freeze the cat.
### 3.2 Asynchronous external data
Weather, Codex usage and companion-generated speech arrive asynchronously.
They must update local cached state rather than block the interface.
The device must not wait for the computer before:
- rendering a frame;
- processing touch;
- triggering a reminder;
- returning to the home screen;
- selecting a local cat action.
### 3.3 Failure isolation
Examples:
- weather retrieval failure must not stop animation;
- Codex data failure must not stop reminders;
- companion disconnection must not stop touch;
- voice recognition failure must not stop menu navigation;
- base-control failure must not stop screen-based gaze reactions;
- dynamic-speech failure must fall back to text or fixed local speech.
### 3.4 Explicit data freshness
Externally supplied data must include:
- data type;
- generation or retrieval time;
- receipt time where useful;
- validity information where applicable;
- cached or live state.
Stale data must not be presented as current data.
---
## 4. Firmware layer model
```text
Product layer
├── cat behavior
├── interaction rules
├── information pages
├── reminders
├── voice-command actions
└── navigation
Service layer
├── animation scheduler
├── audio service
├── schedule service
├── persistence service
├── connection service
├── external-data cache
└── diagnostics
Hardware abstraction layer
├── board configuration
├── display
├── screen touch
├── top capacitive touch
├── audio codec
├── microphone ADC
├── base UART
└── system timing
ESP-IDF and reviewed third-party components
```
Higher-level modules must not directly manipulate hardware registers or GPIOs.
Hardware drivers must not contain cat personality or product-navigation logic.
---
## 5. Proposed firmware modules
### 5.1 Board Support Package
Responsibilities:
- define ESP-VoCat PCB V1.0 hardware;
- contain verified GPIO mapping;
- create supported peripherals;
- apply safe initial states;
- expose board identity and capabilities.
It must not contain:
- cat behavior;
- menu navigation;
- reminder logic;
- weather or Codex business logic.
Potential location:
```text
firmware/components/board_esp_vocat_v1/
```
### 5.2 Display Driver
Responsibilities:
- initialize the ST77916 display;
- configure orientation and pixel format;
- transfer image data;
- control brightness;
- report display errors.
It must not contain:
- cat-state decisions;
- weather formatting;
- reminder scheduling.
### 5.3 Renderer
Responsibilities:
- compose backgrounds;
- draw the cat;
- draw lightweight information;
- draw menus and reminder overlays;
- manage clipping and screen layers;
- expose frame-time measurements.
Suggested conceptual layers:
```text
Background
Cat
Transient effects
Lightweight information
Menu or function page
Reminder overlay
Optional diagnostics
```
### 5.4 Animation System
Responsibilities:
- load animation metadata;
- play frame-based animations;
- play component-based transformations;
- control frame timing;
- provide completion events;
- support safe interruption and transition.
It must not decide why an animation is played.
The cat behavior engine makes that decision.
### 5.5 Cat Behavior Engine
Responsibilities:
- select idle actions;
- apply personality parameters;
- handle temporary happiness state;
- process touch and petting reactions;
- manage inactivity progression;
- coordinate listening, narration and reminders;
- control transitions between cat states.
Initial conceptual states:
```text
BOOTING
IDLE
IDLE_ACTION
TOUCH_REACT
PETTING
LISTENING
PROCESSING_COMMAND
SPEAKING
INFO_TRANSITION
INFO_MODE
ALERTING
SLEEPING
TURNING
ERROR_RECOVERY
```
The final state-transition table requires a dedicated OpenSpec change.
### 5.6 Screen Touch Service
Responsibilities:
- receive CST816S touch events;
- normalize coordinate orientation;
- report touch start, movement and release;
- measure touch duration;
- support long-press cancellation.
Product-level code maps coordinates to:
- cat head;
- cat body;
- tail;
- empty background;
- radial-menu icons;
- page controls.
### 5.7 Top Petting Service
Responsibilities:
- read the PCB V1.0 capacitive-touch channel;
- identify touch start and release;
- measure duration;
- debounce repeated touches;
- report stable petting events.
It must not directly start animations.
The cat behavior engine interprets petting events.
### 5.8 Audio Service
Responsibilities:
- play local cat sounds;
- play fixed local speech;
- receive or cache generated narration;
- manage volume categories;
- manage night volume;
- provide completion and interruption events.
Logical categories:
- cat interaction;
- UI feedback;
- reminder alert;
- information narration;
- voice-assistant response.
### 5.9 Microphone and Voice Service
Responsibilities:
- implement wake-word mode;
- implement touch-to-listen mode;
- implement microphone-disabled mode;
- recognize supported local fixed commands;
- delegate complex processing to the companion when connected.
First-release language:
- Mandarin Chinese.
Open-ended AI conversation is not a first-release dependency.
### 5.10 Reminder and Schedule Service
Responsibilities:
- store bounded current and upcoming data;
- trigger reminders reliably;
- support recurrence:
  - once;
  - daily;
  - weekdays;
  - selected weekdays;
- support snooze;
- detect missed reminders;
- queue simultaneous reminders;
- expose schedules for display.
Complete history remains on the Windows companion application.
### 5.11 External Data Cache
Responsibilities:
- cache weather data;
- cache Codex usage;
- store timestamps;
- determine stale state;
- expose validated snapshots to the UI;
- survive temporary computer disconnection.
The cache does not directly retrieve Internet data in the first version.
### 5.12 USB Communication Service
Responsibilities:
- device handshake;
- protocol-version negotiation;
- framed-message parsing;
- payload-length validation;
- acknowledgements and errors;
- inbound update dispatch;
- outbound status reporting;
- malformed-message protection.
The protocol must not use unbounded text parsing.
### 5.13 Settings Service
Responsibilities:
- persist validated settings;
- apply companion updates;
- provide defaults;
- validate ranges;
- support settings-schema versions.
Initial settings may include:
- cat display name;
- personality;
- volume categories;
- mute;
- narration detail;
- microphone mode;
- brightness;
- screen-off mode;
- day, dusk and night times;
- automatic turning;
- inactivity timings.
### 5.14 Base Control Service
Current status:
- architecture placeholder;
- blocked until UART wiring and protocol are verified.
Future responsibilities:
- safe initialization;
- bounded movement;
- stop command;
- movement cooldown;
- repeated-command suppression;
- parameter validation;
- state reporting.
Voice recognition must not directly issue raw motor commands.
### 5.15 Diagnostics Service
Responsibilities:
- startup diagnostics;
- module-health reporting;
- frame timing;
- heap and PSRAM status;
- task-stack margins;
- connection state;
- error counters;
- controlled serial logging.
Diagnostics must not expose private NVS data, credentials or tokens.
---
## 6. Concurrency model
Exact FreeRTOS task allocation remains undecided.
Conceptual execution groups:
### High-priority interaction
- touch event capture;
- audio input buffering where enabled;
- reminder timer events;
- display-transfer completion.
### Regular interactive work
- renderer;
- animation timing;
- behavior state machine;
- UI navigation.
### Background work
- USB message processing;
- storage writes;
- asset loading;
- diagnostics;
- external-cache updates.
Rules:
1. Host communication must not run inside rendering callbacks.
2. Storage writes must not block touch or animation.
3. Asset decoding should be incremental or buffered.
4. Reminder deadlines must not depend on a low-priority UI loop.
5. Shared state should use bounded synchronization or message passing.
6. Large global locks should be avoided.
The final task model must be validated on actual hardware.
---
## 7. Device boot sequence
Target:
```text
Power-on to interactive home within 5 seconds
```
Proposed logical sequence:
```text
Bootloader
→ minimal board initialization
→ display initialization
→ boot animation begins
→ local settings load
→ touch services start
→ reminder state loads
→ USB connection service starts
→ nonessential services initialize
→ cat home becomes interactive
```
Nonessential initialization may continue after the visible boot animation begins.
Fallback behavior:
- companion unavailable:
  - continue offline;
- weather or Codex unavailable:
  - use cache or empty state;
- voice unavailable:
  - continue touch interaction;
- base unavailable:
  - disable physical turning;
- cat asset failure:
  - display a minimal safe fallback screen.
---
## 8. Runtime event model
The firmware should use validated events rather than arbitrary direct calls between modules.
Example events:
```text
TOUCH_DOWN
TOUCH_MOVE
TOUCH_UP
SCREEN_LONG_PRESS
TOP_PET_START
TOP_PET_DURATION
TOP_PET_END
WAKE_WORD_DETECTED
FIXED_COMMAND_RECOGNIZED
REMINDER_DUE
REMINDER_SNOOZED
COMPANION_CONNECTED
COMPANION_DISCONNECTED
WEATHER_UPDATED
CODEX_USAGE_UPDATED
SETTINGS_UPDATED
ANIMATION_FINISHED
AUDIO_FINISHED
BASE_MOVEMENT_FINISHED
INACTIVITY_THRESHOLD_REACHED
```
Events must contain bounded, validated data.
---
## 9. Priority and interruption policy
Conceptual priority order:
1. hardware safety and critical error;
2. due reminder;
3. active user touch and navigation;
4. wake word and command interaction;
5. information narration;
6. deliberate cat interaction;
7. random idle behavior;
8. background-data updates.
Examples:
- touch may interrupt a random idle animation;
- a reminder may interrupt normal narration;
- weather arrival must not cancel an active touch gesture;
- conflicting base movement must stop or finish safely;
- an unsafe-to-display reminder must enter a queue.
A detailed interruption table requires a dedicated specification.
---
## 10. Windows companion architecture
The first companion application is a lightweight Windows configuration application with optional system-tray operation.
Conceptual modules:
```text
User interface
├── connection status
├── settings
├── schedules and reminders
├── data timestamps
└── manual synchronization
Application services
├── device discovery
├── synchronization
├── weather provider
├── Codex usage provider
├── reminder parser
├── speech generator
└── history store
Infrastructure
├── serial transport
├── local storage
├── Windows startup integration
├── logging
└── secret storage
```
The programming language and UI framework remain undecided.
### 10.1 Device Discovery
Requirements:
- scan relevant serial ports;
- do not permanently assume COM7;
- perform a nondestructive handshake;
- identify:
  - product;
  - PCB revision;
  - firmware version;
  - protocol version;
  - device identifier;
- reject unrelated serial devices safely.
### 10.2 Synchronization Service
Responsibilities:
- initial synchronization;
- immediate updates after settings changes;
- periodic weather updates;
- periodic Codex updates;
- time synchronization;
- reconnect recovery;
- acknowledgement handling.
Initial proposed intervals:
- weather: every 30 minutes;
- Codex usage: every 10 minutes;
- time: at connection and every hour;
- schedules and settings: after changes.
### 10.3 Weather Provider
Responsibilities:
- retrieve weather from the selected source;
- normalize provider-specific data;
- reduce data to the device schema;
- retain the last successful result;
- expose timestamps and errors.
The provider remains undecided.
### 10.4 Codex Usage Provider
Responsibilities:
- obtain or derive supported Codex usage information;
- distinguish ChatGPT-plan usage from OpenAI API usage;
- avoid transferring account credentials to the ESP32;
- provide normalized values and timestamps;
- report transparently when no reliable source is available.
The concrete source remains unresolved.
### 10.5 Reminder Parser
Responsibilities:
- parse complex natural-language reminders;
- return structured time and recurrence data;
- provide validation or confidence information;
- avoid inventing ambiguous dates or times.
The ESP32 retains a smaller deterministic parser for simple supported phrases.
### 10.6 Speech Generator
Responsibilities:
- generate dynamic Mandarin narration;
- use one gentle and natural voice in the first version;
- support short and detailed narration modes;
- cache output where useful;
- provide a fallback when generation fails.
Technology and audio format remain undecided.
### 10.7 History Store
The computer stores:
- complete reminder history;
- completed reminders;
- dismissed reminders;
- synchronization metadata;
- nonsecret diagnostic records.
The device stores only bounded current and near-future data.
---
## 11. USB protocol boundaries
The exact encoding remains undecided.
The protocol should support these categories.
### Device identity
- hello;
- device information;
- firmware version;
- PCB revision;
- protocol version;
- capabilities.
### Connection control
- heartbeat;
- time synchronization;
- acknowledgement;
- error response.
### External information
- weather update;
- Codex usage update;
- freshness metadata.
### Schedules and reminders
- schedule snapshot;
- reminder add or update;
- reminder delete;
- snooze event;
- completion event.
### Settings
- settings snapshot;
- setting update;
- settings acknowledgement.
### Audio
- audio metadata;
- bounded audio transfer;
- playback request;
- playback state.
### Diagnostics
- health summary;
- error report;
- explicitly requested performance summary.
The protocol must not expose:
- arbitrary GPIO control;
- arbitrary memory access;
- eFuse writes;
- full Flash erase;
- unrestricted command execution.
---
## 12. Data ownership
### Device authority
The device is authoritative for:
- current interaction state;
- current animation;
- touch state;
- active reminder presentation;
- local playback state;
- current base state after verification;
- local runtime state not yet synchronized.
### Companion authority
The companion is authoritative for:
- complete reminder history;
- user-edited schedules;
- weather results;
- Codex usage results;
- dynamic speech generation;
- Windows auto-start preference;
- desktop-side configuration.
### Shared synchronized state
Shared state includes:
- cat display name;
- personality;
- audio settings;
- brightness;
- microphone mode;
- time-theme ranges;
- automatic turning;
- schedules;
- upcoming reminders.
Conflict-resolution rules remain open.
---
## 13. Persistence boundaries
### Device persistence
May contain:
- validated settings;
- upcoming schedules;
- upcoming reminders;
- snooze state;
- cached weather;
- cached Codex usage;
- update timestamps;
- bounded diagnostic counters;
- schema versions.
Must not contain:
- ChatGPT browser cookies;
- reusable account sessions;
- passwords;
- unnecessary private tokens;
- original private NVS values;
- complete long-term history.
### Companion persistence
May contain:
- full schedules and reminder history;
- provider configuration;
- generated-audio cache;
- device registration metadata;
- desktop preferences.
Secrets must use suitable Windows secret storage and must not be committed to Git.
---
## 14. Offline and reconnection behavior
### Companion disconnected
The device continues:
- cat behavior;
- screen touch;
- top petting;
- local sounds;
- reminders;
- cached information;
- local navigation;
- device settings.
After approximately 10 seconds:
- connection indicator becomes inactive;
- no disruptive pop-up is required.
External-data pages display:
- cached state;
- last update time.
### Companion reconnects
Proposed sequence:
1. handshake;
2. protocol and capability comparison;
3. time synchronization;
4. settings synchronization;
5. schedule and reminder synchronization;
6. weather update if due;
7. Codex update if due;
8. synchronization-complete report.
The device must not reset simply because the computer reconnects.
---
## 15. Security boundaries
1. Only known message types are accepted.
2. All message lengths are bounded.
3. Audio and text payloads have explicit limits.
4. Account credentials never need to reach the ESP32.
5. Logs redact secrets.
6. Build files and secrets are excluded from Git.
7. Raw private NVS backups remain outside the project repository.
8. No generic remote-command execution is exposed over USB.
9. No USB command may write eFuses or erase the full Flash.
10. Physical movement must pass safety validation.
---
## 16. Asset architecture
Initial conceptual structure:
```text
assets/
├── cat/
│   └── default-orange/
│       ├── manifest
│       ├── idle
│       ├── walk
│       ├── sleep
│       ├── petting
│       ├── listening
│       ├── speaking
│       └── transitions
└── audio/
    ├── cat
    ├── ui
    ├── reminder
    └── fixed-speech
```
A future cat manifest should define:
- character identifier;
- supported actions;
- frame rate;
- dimensions;
- anchor points;
- touch hit regions;
- transition compatibility;
- sound mappings;
- personality parameters;
- resource-schema version.
Concrete asset formats remain undecided.
---
## 17. Testing architecture
### Host-only tests
Examples:
- USB parser;
- reminder recurrence;
- settings validation;
- freshness calculation;
- companion-provider normalization;
- animation-manifest validation;
- state-machine transitions.
### Build validation
Examples:
- ESP32-S3 target;
- compiler warnings;
- binary sizes;
- partition fit;
- dependency versions.
### Initial device smoke tests
Allowed scope:
- serial output;
- display colors;
- display text;
- screen touch;
- top capacitive touch.
### Later device feature tests
Added only after the recovery rehearsal:
- animation;
- audio;
- microphone;
- reminders;
- USB communication;
- rotating base.
### Recovery tests
- recovery-image hash validation;
- controlled full restoration;
- original-firmware behavior validation.
Simulation does not constitute physical hardware verification.
---
## 18. Repository boundaries
Current structure:
```text
custom-vocat/
├── AGENTS.md
├── PROJECT_CONSTITUTION.md
├── assets/
├── companion-app/
├── docs/
├── firmware/
├── references/
├── specs/
├── tests/
└── tools/
```
Responsibilities:
### `firmware/`
ESP-IDF firmware and reviewed board components.
### `companion-app/`
Windows companion application.
### `assets/`
Project-created cat and audio assets.
### `docs/`
Product, hardware, architecture and decision documentation.
### `specs/`
OpenSpec feature changes and acceptance criteria.
### `tests/`
Host tests, hardware test records and recovery records.
### `tools/`
Asset conversion, protocol inspection and development utilities.
### `references/`
Source provenance, licenses and technical reference notes.
Immutable recovery backups must remain outside this repository.
---
## 19. Open architecture decisions
The following topics remain unresolved:
1. Windows companion language and UI framework;
2. companion storage technology;
3. serial communication library;
4. USB framing and serialization;
5. acknowledgement and retry model;
6. dynamic audio-transfer method;
7. display and rendering framework;
8. animation asset format;
9. image compression;
10. custom Flash partition layout;
11. device storage schema;
12. firmware source-reuse strategy;
13. wake-word engine;
14. fixed-command recognition engine;
15. dynamic speech engine;
16. weather provider;
17. Codex usage source;
18. sound-direction feasibility;
19. base UART protocol;
20. detailed FreeRTOS task allocation;
21. memory and performance thresholds;
22. firmware update and rollback strategy.
These decisions must be addressed through focused OpenSpec changes rather than being locked prematurely.
---
## 20. Architecture readiness criteria
This draft is ready for Git and OpenSpec initialization when:
- firmware and companion responsibilities are explicit;
- offline behavior is explicit;
- credential boundaries are explicit;
- major firmware modules are identified;
- unresolved decisions remain visible;
- no untested behavior is described as verified.
This document remains a draft until the first implementation roadmap is reviewed.
