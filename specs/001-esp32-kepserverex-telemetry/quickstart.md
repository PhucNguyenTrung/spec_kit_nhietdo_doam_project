# Quickstart: ESP32 KEPServerEX Telemetry

## Prerequisites

- ESP32 development board with Arduino ESP32 Core 3.x installed
- Arduino IDE 2.x or `arduino-cli`
- Sensirion SHT31-D sensor connected via I2C to the ESP32
- WiFi access on the same OT LAN as KEPServerEX
- KEPServerEX IoT Gateway REST Server Agent configured and reachable
- USB cable for serial/log inspection

## Hardware wiring

- SHT31-D SDA -> ESP32 GPIO configured in `config.h`
- SHT31-D SCL -> ESP32 GPIO configured in `config.h`
- SHT31-D VDD -> 3.3V rail
- SHT31-D GND -> GND
- WiFi LED -> configured GPIO pin
- KEPServerEX LED -> configured GPIO pin

## Configuration

1. Open `config.h`.
2. Set the WiFi SSID and password.
3. Set the KEPServerEX host, port, and endpoint.
4. Set the I2C pins and sensor address (default `0x44`).
5. Set the sample interval, retry interval, and serial baud rate.

## Building and uploading

```bash
arduino-cli compile --fqbn esp32:esp32:esp32 .
arduino-cli upload --fqbn esp32:esp32:esp32 --port COMx .
```

If using Arduino IDE:

1. Open `test_spec_kit.ino`.
2. Select the ESP32 board target.
3. Compile and upload.
4. Open Serial Monitor at the configured baud rate.

## Validation scenarios

### Scenario A: Successful telemetry publish

1. Power on the ESP32.
2. Confirm the WiFi LED transitions from blinking to steady on.
3. Observe the Serial Monitor for IP and RSSI output.
4. Wait for the configured sample interval.
5. Confirm data is posted to KEPServerEX with HTTP `200 OK`.
6. Confirm the KEPServerEX LED blinks once for each successful payload.

Expected outcome: sensor values appear in KEPServerEX with updated timestamps and no error logs.

### Scenario B: WiFi outage recovery

1. Disconnect the WiFi AP while the device is running.
2. Confirm the WiFi LED begins blinking.
3. Confirm the firmware continues to read local sensor values without blocking.
4. Restore WiFi.
5. Verify the ESP32 reconnects automatically and resumes POST operations.

Expected outcome: reconnect occurs within the configured retry window and without manual reset.

### Scenario C: KEPServerEX outage recovery

1. Stop the KEPServerEX server or disconnect the service.
2. Confirm that the KEPServerEX LED enters an error or off state.
3. Confirm that the firmware continues local processing and attempts retries.
4. Restart KEPServerEX.
5. Verify the device re-establishes communication automatically.

Expected outcome: the ESP32 resumes successful HTTP posts without freezing or requiring a reboot.

### Scenario D: Invalid sensor data handling

1. Simulate an invalid or disconnected SHT31-D condition.
2. Confirm the sensor read fails gracefully.
3. Verify no invalid values are posted to KEPServerEX.
4. Confirm the error is logged over Serial and flagged by LED state.

Expected outcome: only valid sensor values are transmitted; invalid values are rejected and logged.

## Troubleshooting

- If the WiFi LED never stabilizes, confirm SSID/password and signal quality.
- If the KEPServerEX LED never blinks, confirm host, port, and endpoint values in `config.h`.
- If Serial logs are empty, verify the baud rate and the board connection.
- If the device resets unexpectedly, check for blocking operations, incorrect retry loops, and stack-heavy logging.
