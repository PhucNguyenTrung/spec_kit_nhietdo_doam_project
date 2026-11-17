# Tasks: ESP32 Temperature & Humidity Telemetry to KEPServerEX

**Input**: Design documents from `/specs/001-esp32-kepserverex-telemetry/`

**Prerequisites**: plan.md (required), spec.md (required for user stories), research.md, data-model.md, contracts/

**Tests**: Validation tasks are included where required by the user stories and quickstart scenarios; no separate unit test suite is mandated for this project.

**Organization**: Tasks are grouped by user story to enable independent implementation and testing of each story.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2, US3)
- Include exact file paths in descriptions

---

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Create the Arduino-compatible firmware project skeleton and shared module layout required by the plan.

- [x] T001 Create the ESP32 Arduino project scaffold in `test_spec_kit.ino` and `config.h` with the required setup/loop structure and central configuration placeholders.
- [x] T002 [P] Create `sensor.h` and `sensor.cpp` with the SHT31-D I2C module interface and read function declarations for temperature/humidity capture.
- [x] T003 [P] Create `communication.h`, `communication.cpp`, `led.h`, `led.cpp`, `diag.h`, and `diag.cpp` with empty module interfaces for WiFi, KEPServerEX HTTP, LED status, and diagnostic logging.

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Establish the reusable core that all stories depend on before any story-specific feature work begins.

**鈿狅笍 CRITICAL**: No user story work can begin until this phase is complete.

- [x] T004 Implement the centralized hardware and network configuration in `config.h`, including GPIO pins, WiFi SSID/password, KEPServerEX host/port/endpoint, I2C address `0x44`, and all timing intervals (`sampleIntervalMs`, `telemetryPostIntervalMs`, `reconnectIntervalMs`).
- [x] T005 [P] Implement the SHT31-D I2C initialization and measurement routine in `sensor.cpp` with `Wire.begin()`/`Wire.requestFrom()` style logic and value validation for `temperatureC` and `humidityRH`.
- [x] T006 [P] Implement static JSON payload generation in `communication.cpp` to build the KEPServerEX array format `[{"id":"<Tag_Name>","v":<Value>}]` using `char[]` buffers and `snprintf()` without `String` objects.
- [x] T007 [P] Implement a non-blocking WiFi connection manager in `communication.cpp` that handles connecting, checking `WiFi.status()`, and scheduling reconnect attempts with backoff while keeping the main loop free of blocking waits.
- [x] T008 Implement the KEPServerEX HTTP POST state machine in `communication.cpp` with timeout handling, HTTP status checking, and safe retry behavior when the server is unavailable or disconnected.
- [x] T009 [P] Implement LED state helper functions in `led.cpp` for WiFi connected/connecting and KEPServerEX online/offline behaviors using independent GPIO pins as specified in `config.h`.
- [x] T010 Implement diagnostic logging in `diag.cpp` and `diag.h` to emit Vietnamese status messages for IP address, RSSI, sensor values, HTTP results, and error states without flooding the Serial output.

**Checkpoint**: Foundation ready - user story implementation can now begin in parallel.

---

## Phase 3: User Story 1 - Thu th岷璸 v脿 膽岷﹜ d峄?li峄噓 m么i tr瓢峄漬g th峄漣 gian th峄眂 l锚n KEPServerEX (Priority: P1) 馃幆 MVP

**Goal**: Collect valid SHT31-D temperature and humidity samples on a fixed schedule and publish them to KEPServerEX with the exact REST JSON contract.

**Independent Test**: Power the ESP32, connect to WiFi and KEPServerEX, wait for the configured sample interval, and confirm KEPServerEX receives valid temperature and humidity values without invalid sensor data being published.

### Implementation for User Story 1

- [x] T011 [P] [US1] Implement sensor validity checks in `sensor.cpp` to reject NaN/Inf values, humidity values outside 0鈥?00% RH, and unrealistic temperatures outside the allowed physical range before the data is transmitted.
- [x] T012 [US1] Add the non-blocking sample scheduler in `test_spec_kit.ino` using `millis()` so the sensor reads occur at the configured interval without using `delay()` or blocking loops.
- [x] T013 [US1] Complete the payload generation and HTTP call path in `communication.cpp` so each measurement produces a KEPServerEX-compatible JSON array and sends it via HTTP POST to the configured endpoint.
- [x] T014 [US1] Implement server success handling in `communication.cpp` and `diag.cpp` so successful `2xx` responses update the server state, log the result, and trigger the KEPServerEX LED success pulse.
- [x] T015 [US1] Add sensor-error branch handling in `sensor.cpp` and `diag.cpp` to log a clear diagnosis and skip transmission when the sensor read fails or output is invalid.
- [x] T016 [US1] Validate end-to-end telemetry publishing against `contracts/kepserver-rest-contract.md` and `quickstart.md` Scenario A to confirm the payload shape, HTTP status behavior, and KEPServerEX acceptance criteria.

**Checkpoint**: At this point, User Story 1 should be fully functional and testable independently.

---

## Phase 4: User Story 2 - T峄?膽峄檔g ph峄 h峄搃 k岷縯 n峄慽 m岷g v脿 m谩y ch峄?khi g岷穚 s峄?c峄?gi谩n 膽o岷 (Priority: P2)

**Goal**: Restore WiFi and KEPServerEX connectivity automatically without blocking the main telemetry loop or requiring a manual reset.

**Independent Test**: Disconnect WiFi or stop the KEPServerEX service, wait for the reconnect interval, and confirm the firmware reconnects automatically and resumes telemetry transmission without freezing.

### Implementation for User Story 2

- [x] T017 [P] [US2] Implement a backoff-based WiFi reconnect loop in `communication.cpp` that transitions the firmware to `CONNECTING`/`CONNECTED`/`LOST` states without using blocking calls.
- [x] T018 [US2] Implement server outage detection and retry logic in `communication.cpp` for failed HTTP POSTs, including reconnect attempts after timeout, service unavailability, or transport disconnects.
- [x] T019 [US2] Ensure the sensor read loop remains active during outages by separating telemetry sampling from the connection retry path in `test_spec_kit.ino` and the communication module.
- [x] T020 [US2] Update `diag.cpp` to print Vietnamese logs for WiFi loss, IP acquisition, RSSI changes, KEPServerEX retry attempts, and reconnect success events.
- [x] T021 [US2] Validate recovery behavior against `quickstart.md` Scenario B and Scenario C, confirming the device resumes data publishing within the configured retry window.

**Checkpoint**: At this point, User Stories 1 and 2 should both work independently.

---

## Phase 5: User Story 3 - Ch峄?b谩o tr峄眂 quan tr岷g th谩i v岷璶 h脿nh qua 2 膽猫n LED ri锚ng bi峄噒 (Priority: P3)

**Goal**: Provide field-friendly indicator states for WiFi connection and KEPServerEX server health using two independent LEDs.

**Independent Test**: Observe LED behavior during WiFi connection, stable WiFi, successful HTTP POST, and failed transmission conditions.

### Implementation for User Story 3

- [x] T022 [P] [US3] Implement WiFi LED behavior in `led.cpp` so the LED blinks while connecting and remains solid when the device is connected to WiFi.
- [x] T023 [US3] Implement KEPServerEX LED behavior in `led.cpp` so a successful POST triggers a short pulse and a failed or lost connection triggers a warning/off state.
- [x] T024 [US3] Wire LED updates into the runtime state transitions in `test_spec_kit.ino` and `communication.cpp` so the indicators reflect the actual WiFi/server state at all times.
- [x] T025 [US3] Validate the LED hardware behavior against the acceptance scenarios in the feature specification and `quickstart.md` to ensure the two indicators are visually distinct and clearly diagnostic.

**Checkpoint**: The device can now communicate state visually without a laptop connection.

---

## Phase 6: User Story 4 - Xu岷 th么ng tin ch岷﹏ 膽o谩n qua Serial Monitor (Priority: P4)

**Goal**: Provide clear Vietnamese diagnostic output on Serial Monitor for monitoring, debugging, and fault isolation during integration and operation.

**Independent Test**: Open Serial Monitor at the configured baud rate and confirm boot, WiFi/IP/RSSI, sensor reading, and HTTP status messages appear in a readable and non-noisy format.

### Implementation for User Story 4

- [x] T026 [P] [US4] Add boot and live-status Serial messages in `diag.cpp` for device startup, WiFi connection state, IP address, RSSI, and successful or failed telemetry transmission.
- [x] T027 [US4] Add error messages in `diag.cpp` for invalid sensor values, I2C bus failure, bad HTTP status, and retry attempts using clear Vietnamese wording without excessive flooding.
- [x] T028 [US4] Format the periodic diagnostic output in `diag.cpp` to include temperature, humidity, WiFi state, and latest HTTP result at the configured status cadence.
- [x] T029 [US4] Validate Serial output against User Story 4 acceptance scenarios and ensure the logs remain concise, useful, and free of noisy or repeated garbage text.

**Checkpoint**: All required user stories are now independently functional and diagnosable.

---

## Phase 7: Polish & Cross-Cutting Concerns

**Purpose**: Final validation and compliance review across the full implementation.

- [x] T030 [P] Compile the firmware with `arduino-cli` for the ESP32 target and confirm there are no syntax or board compatibility errors.
- [x] T031 [P] Review the codebase against the constitution in `.specify/memory/constitution.md` to verify no `delay()` calls remain in the main execution path, `config.h` is the single source of configuration, and the code uses static buffers with `snprintf()` rather than `String` objects.
- [x] T032 Update project documentation references so `quickstart.md` and `contracts/kepserver-rest-contract.md` remain aligned with the final code and configuration names.
- [x] T033 Run the final end-to-end smoke test for the MVP scope: User Story 1 telemetry read + publish flow, then confirm the device stays responsive and logs cleanly while network and server conditions are stable.

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies - can start immediately
- **Foundational (Phase 2)**: Depends on Setup completion - BLOCKS all user stories
- **User Stories (Phase 3+)**: All depend on Foundational completion
  - User Story 1 (P1): Must be complete before final deployment of MVP
  - User Story 2 (P2): Can proceed after the foundation and in parallel with US1 if needed
  - User Story 3 (P3): Can proceed after the foundation and independently validate LED state
  - User Story 4 (P4): Can proceed after the foundation and independent logging validation
- **Polish (Final Phase)**: Depends on all desired user stories being complete

### Parallel Opportunities

- Setup tasks T002 and T003 can run in parallel.
- Foundational tasks T005, T006, T007, T009, and T010 can be implemented concurrently where different module files are involved.
- User Story 1 tasks T011, T012, T013, and T014 can be executed in overlapping order with independent module work.
- User Story 2 tasks T017 and T018 are parallelizable after the foundational communication layer is in place.
- User Story 3 tasks T022 and T023 can be developed in parallel because they target separate LED behaviors.
- Final compliance tasks T030 and T031 can run together.

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Complete Phase 1: Setup
2. Complete Phase 2: Foundational (CRITICAL - blocks all stories)
3. Complete Phase 3: User Story 1
4. Stop and validate the telemetry path independently
5. Deploy or demo only after the publish path is verified

### Incremental Delivery

1. Setup + Foundational 鈫?create a stable, non-blocking firmware skeleton
2. User Story 1 鈫?deploy telemetry collection and transmission
3. User Story 2 鈫?add resilient connection recovery
4. User Story 3 鈫?add visual state indication
5. User Story 4 鈫?add Serial diagnostics and troubleshooting support
6. Final Polish 鈫?compile, compliance, and smoke testing

### Suggested MVP Scope

The recommended MVP is User Story 1 only. It delivers the core value of the project: valid SHT31-D telemetry collected on an interval and transmitted to KEPServerEX over HTTP REST without sacrificing the real-time constraints imposed by the constitution.

---

## Notes

- [P] tasks = different files or independent components, no direct dependency on incomplete work.
- [Story] labels map each task to the user story it serves for traceability and implementation planning.
- The generated tasks are specific enough to be executed without further architectural clarification.
- Validation tasks are written for the project鈥檚 stated operational workflow and do not introduce a separate testing framework beyond the required hardware and compile checks.

