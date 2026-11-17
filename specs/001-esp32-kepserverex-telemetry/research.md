# Research: ESP32 Temperature & Humidity Telemetry to KEPServerEX

## Decision 1: Use ESP32 + SHT31-D over I2C with a REST JSON payload to KEPServerEX

- Decision: The firmware will read environmental data from Sensirion SHT31-D via I2C address 0x44 and publish a standardized KEPServerEX JSON array through HTTP POST to the IoT Gateway REST Server Agent.
- Rationale: The feature specification explicitly resolved the transport and sensor protocol, and it matches the industrial requirement for direct LAN-based data ingestion without authentication. The array payload `[{"id":"<Tag_Name>","v":<Value>}]` is compatible with KEPServerEX IoT Gateway expectations and keeps the implementation simple and reliable in a local OT network.
- Alternatives considered: MQTT broker, Modbus TCP, and raw TCP streaming. These were rejected because the requirement was explicitly defined as REST/HTTP to KEPServerEX and the project goal is straightforward telemetry publishing without a broker architecture.

## Decision 2: Use a non-blocking, millis()-driven state machine

- Decision: The firmware will perform periodic sampling, WiFi retry, and server retry logic with a non-blocking scheduler based on `millis()` and static buffers.
- Rationale: The constitution requires non-blocking execution and forbids `delay()` in the main loop to avoid watchdog resets and preserve real-time responsiveness. Using a state machine also supports independent handling of WiFi and server recovery without blocking sensor reads.
- Alternatives considered: `delay()`, blocking `WiFiClient.connect()`, and blocking HTTP waits. These are explicitly disallowed by the constitution and would increase the risk of system hangs and watchdog resets under unstable links.

## Decision 3: Centralize all configuration in `config.h`

- Decision: All GPIO pins, network settings, KEPServerEX endpoint configuration, and timing thresholds are defined in a single `config.h` file.
- Rationale: This aligns with the constitution and reduces the chance of configuration drift or hard-coded values across modules. The same structure also makes field deployment easier and keeps hardware pin mapping consistent.
- Alternatives considered: per-module constant definitions and compile-time flags scattered across source. Rejected because they break maintainability and violate the project requirements.

## Decision 4: Use modular Arduino-compatible architecture

- Decision: The firmware will follow Arduino IDE compatibility with `test_spec_kit.ino` as the coordinator and separate `.h/.cpp` modules for sensors, connectivity, diagnostics, and LED control.
- Rationale: The constitution requires this structure and ensures the project remains buildable in Arduino IDE or `arduino-cli` without special build steps.
- Alternatives considered: single-file firmware or PlatformIO-only architecture. Rejected because the project explicitly requires Arduino IDE compatibility and modular separation.

## Decision 5: Handle diagnostics with Serial and two LED indicators

- Decision: One LED indicates WiFi state; one LED indicates KEPServerEX communication state. Serial Monitor emits Vietnamese diagnostic messages for IP, RSSI, sensor values, and connection outcomes.
- Rationale: This matches the specified operational workflows and makes troubleshooting fast at the plant floor without requiring a laptop connection. The separate LED behavior provides immediate fault isolation between WiFi and server availability.
- Alternatives considered: single shared LED or logging only over Serial. Rejected because the specification requires independent hardware indicators and direct field diagnosis.

## Open technical considerations resolved by design

- Sensor accuracy target is satisfied by SHT31-D, which provides ±0.2°C and ±2% RH under normal conditions.
- HTTP connectivity uses no Authorization header, matching the KEPServerEX no-auth REST server requirement in the isolated OT LAN.
- Data validity checks will reject impossible values such as humidity >100% RH or temperature outside physical plausibility, and those readings will not be transmitted.
- Recovery policy uses exponential retry/backoff and continues local sampling even while disconnected to avoid blocking the main loop.

## Implementation impact

The design choices above produce an architecture that satisfies the functional and constitutional requirements without introducing blocking code paths or memory-unsafe patterns. The final implementation will be a modular, Arduino-compatible ESP32 firmware that reads SHT31-D telemetry, validates it, uses a WiFi + HTTP retry loop for KEPServerEX, and emits deterministic diagnostics to both LEDs and Serial.
