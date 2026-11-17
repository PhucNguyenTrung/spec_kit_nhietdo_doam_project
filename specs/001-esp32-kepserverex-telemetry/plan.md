# Implementation Plan: ESP32 Temperature & Humidity Telemetry to KEPServerEX

**Branch**: `001-esp32-kepserverex-telemetry` | **Date**: 2026-11-17 | **Spec**: [spec.md](./spec.md)

**Input**: Feature specification from `/specs/001-esp32-kepserverex-telemetry/spec.md`

## Summary

The feature deploys an ESP32 firmware that samples temperature and humidity from an SHT31-D sensor over I2C, validates the telemetry, and publishes it in KEPServerEX-compatible JSON to the KEPServerEX REST Server Agent through HTTP POST over the OT LAN. The firmware is designed around non-blocking `millis()` scheduling, centralized configuration in `config.h`, and resilient WiFi/server reconnection behavior without watchdog-triggering blocking delays.

## Technical Context

**Language/Version**: C++ (Arduino ESP32 Core 3.x)

**Primary Dependencies**: ESP32 Arduino Core 3.x, WiFi client stack, HTTP client library, SHT31-D sensor driver (Adafruit Sensirion-compatible library)

**Storage**: N/A (local runtime state only; no database or persistent storage required)

**Testing**: `arduino-cli` compile/upload, Serial Monitor diagnostics, WiFi interruption and KEPServerEX outage fault injection

**Target Platform**: ESP32 microcontroller in an industrial OT LAN with WiFi access and a reachable KEPServerEX IoT Gateway REST server

**Project Type**: Embedded firmware / industrial telemetry

**Performance Goals**: 5s sample interval, <1s telemetry delivery latency on a healthy LAN, loop execution under 10ms with no blocking waits

**Constraints**: No `delay()` in main execution path; static-only buffers; single centralized `config.h`; non-blocking auto-reconnect and retry logic; maintain operation during WiFi/server failure

**Scale/Scope**: Single-node ESP32 telemetry device publishing two process variables (temperature and humidity) to KEPServerEX with embedded diagnostic logging

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

- PASS: Principle I — Arduino IDE compatibility and modular architecture are satisfied by `test_spec_kit.ino` plus separate `*.h/.cpp` modules.
- PASS: Principle II — The implementation will use `millis()`-based scheduling and avoid blocking waits in `loop()`.
- PASS: Principle III — All GPIOs, network settings, KEPServerEX endpoints, and timing values are centralized in `config.h`.
- PASS: Principle IV — Data payloads will use static `char[]` buffers and `snprintf()`, with no dynamic `String` usage in the runtime loop.
- PASS: Principle V — WiFi and KEPServerEX recovery are handled by non-blocking auto-reconnect and retry logic.
- PASS: Principle VI — All source comments, logs, and documentation are produced in Vietnamese to comply with project standards.

## Project Structure

### Documentation (this feature)

```text
specs/001-esp32-kepserverex-telemetry/
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── contracts/
│   └── kepserver-rest-contract.md
└── spec.md
```

### Source Code (repository root)

```text
test_spec_kit.ino
config.h
sensor.h
sensor.cpp
communication.h
communication.cpp
diag.h
diag.cpp
led.h
led.cpp
```

**Structure Decision**: The implementation is a single-target embedded firmware project for Arduino IDE compatibility, with the main sketch as `test_spec_kit.ino` and runtime logic split into sensor, communication, LED, and diagnostic modules. This structure keeps hardware configuration centralized and preserves non-blocking real-time operation.

## Complexity Tracking

> **Fill ONLY if Constitution Check has violations that must be justified**

No violations identified; no complexity exceptions are required.
