# Data Model: ESP32 KEPServerEX Telemetry

## Overview

This design defines the canonical data structures used by the ESP32 firmware to acquire, validate, package, and publish telemetry to KEPServerEX. The model is intentionally compact and aligned with the requirement for a single, centralized configuration and non-blocking processing.

## Entity: TelemetryData

Represents a single measurement set captured from the SHT31-D sensor at one timestamp.

| Field | Type | Description | Validation / Rules |
| --- | --- | --- | --- |
| timestampMs | uint32_t | Millis timestamp of the measurement capture | Must be non-zero after boot; monotonic within a single session |
| temperatureC | float | Temperature value in °C | Must be finite and within realistic range; reject values outside sensor/physics guard limits |
| humidityRH | float | Relative humidity in %RH | Must be finite and within 0–100% RH; reject invalid or excessive values |
| isValid | bool | Indicates sensor integrity and sanity-check result | Set to false when sensor read fails or derived values are invalid |
| kepserverPayload | char[256] | JSON payload for the KEPServerEX REST post body | Must be generated using static buffer + `snprintf()`, e.g. `[{"id":"Tag_Temp","v":24.5},{"id":"Tag_Hum","v":51.2}]` |
| statusText | char[64] | Short diagnostic status label | ASCII text in Vietnamese or English, for Serial logs and state reporting |

### Validation rules

- Reject data when the SHT31-D read returns an error or the I2C bus is unavailable.
- Reject non-finite values (NaN/Inf) before transmission.
- Filter out impossible environmental values such as humidity >100% RH or temperature >80°C.
- Do not send invalid payloads to KEPServerEX; preserve local safe state and emit an error alert.

## Entity: DeviceNetworkState

Represents the current networking and server communication state of the ESP32.

| Field | Type | Description | Validation / Rules |
| --- | --- | --- | --- |
| wifiState | enum | WiFi operating state | `CONNECTING`, `CONNECTED`, `LOST` |
| ipAddress | char[16] | IPv4 address assigned by DHCP | Valid IPv4 string, empty only before DHCP lease |
| rssiDbm | int | Received signal strength indicator | Typical range is -90 to -30 dBm; log if weaker than threshold |
| serverState | enum | KEPServerEX communication state | `ONLINE`, `OFFLINE`, `RETRYING` |
| lastSuccessMs | uint32_t | Timestamp of last successful POST | Updated after each valid HTTP 2xx response |
| retryBackoffMs | uint32_t | Delay interval before next reconnect attempt | Must be non-zero and should increase over time under repeated failures |

### State transitions

- `CONNECTING` → `CONNECTED` when WiFi connection succeeds and the device receives an IP.
- `CONNECTED` → `LOST` when the ESP32 detects an outage or disconnect.
- `OFFLINE` → `RETRYING` when a POST fails and auto-reconnect is initiated.
- `RETRYING` → `ONLINE` once an HTTP request returns success.

## Entity: SystemConfiguration

Centralizes all firmware parameters, including hardware mapping, network identity, and timing.

| Field | Type | Description | Validation / Rules |
| --- | --- | --- | --- |
| wifiSsid | char[32] | SSID for the WiFi network | Must not be empty before deployment |
| wifiPassword | char[64] | WiFi password | Stored in config.h only; not hard-coded elsewhere |
| kepserverHost | char[32] | KEPServerEX host/IP | Must be reachable on the OT LAN |
| kepserverPort | uint16_t | HTTP port for the REST agent | Usually 8080 or service-specific port |
| kepserverEndpoint | char[64] | REST path for telemetry post | Must match the configured IoT Gateway endpoint |
| i2cSdaPin | int | SDA GPIO pin for SHT31-D | Must be defined in config.h |
| i2cSclPin | int | SCL GPIO pin for SHT31-D | Must be defined in config.h |
| sensorAddress | uint8_t | I2C address of SHT31-D | Fixed default 0x44 |
| wifiLedPin | int | GPIO for WiFi status LED | Must be independent from server LED |
| serverLedPin | int | GPIO for KEPServerEX status LED | Must have separate indication semantics |
| sampleIntervalMs | uint32_t | Period for reading sensor values | Default 5000ms |
| telemetryPostIntervalMs | uint32_t | Period for HTTP posts | Default 5000ms or aligned with sample trigger |
| reconnectIntervalMs | uint32_t | Interval for WiFi/server retry logic | Must be non-blocking and protected by backoff |
| serialBaud | uint32_t | UART speed for Serial Monitor | Standard value such as 115200 |

### Relationship summary

- `SystemConfiguration` defines hardware and network parameters used by the sensor, WiFi, and KEPServerEX modules.
- `TelemetryData` is created from sensor readings and transformed using the configured KEPServerEX tags and timing settings.
- `DeviceNetworkState` reflects current runtime status derived from WiFi connectivity and HTTP POST results.

## Implementation constraints

- The firmware must only use static buffers and avoid `String` objects in time-critical paths.
- Data payload generation must use fixed-size arrays and `snprintf()` to prevent heap fragmentation.
- The design assumes a single active data stream targeting KEPServerEX in a local LAN and avoids per-message dynamic memory allocations.
