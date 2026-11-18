#include "config.h"
#include "sensor.h"
#include "communication.h"
#include "led.h"
#include "diag.h"

TelemetryData lastTelemetry;
unsigned long lastReadMs = 0;
unsigned long lastLogMs = 0;
char telemetryPayload[256];

void setup() {
  pinMode(WIFI_LED_PIN, OUTPUT);
  pinMode(SERVER_LED_PIN, OUTPUT);

  sensorBegin();
  diagSetup();
  communicationSetup();
  ledSetup();
  diagPrintBoot();

  lastTelemetry.temperatureC = 0.0f;
  lastTelemetry.humidityRH = 0.0f;
  lastTelemetry.valid = false;
  memset(telemetryPayload, 0, sizeof(telemetryPayload));
}

void loop() {
  unsigned long now = millis();

  communicationLoop();

  if (now - lastReadMs >= SAMPLE_INTERVAL_MS) {
    lastReadMs = now;

    TelemetryData data;
    bool readable = readSensorTelemetry(data);
    if (readable) {
      lastTelemetry = data;
      snprintf(telemetryPayload, sizeof(telemetryPayload),
               "[{\"id\":\"%s\",\"v\":%.2f},{\"id\":\"%s\",\"v\":%.2f}]",
               TAG_TEMPERATURE, data.temperatureC, TAG_HUMIDITY, data.humidityRH);

      if (isWifiConnected()) {
        bool sent = tryPostTelemetry(telemetryPayload);
        if (sent) {
          diagPrintInfo("Dữ liệu gửi lên KEPServerEX thành công");
          updateServerLed(true, true);
        } else {
          diagPrintError("Gửi dữ liệu lên KEPServerEX thất bại");
          updateServerLed(false, false);
        }
      } else {
        diagPrintError("WiFi chưa kết nối, bỏ qua gửi dữ liệu");
      }
    } else {
      diagPrintError("Cảm biến SHT31-D không hợp lệ, bỏ qua gói tin");
      updateServerLed(false, false);
    }
  }

  if (now - lastLogMs >= 2000UL) {
    lastLogMs = now;
    NetworkState state = getNetworkState();
    const char* wifiState = wifiStateToString(state.wifiState);
    const char* serverState = serverStateToString(state.serverState);
    diagPrintStatus(wifiState, serverState, state.ipAddress, state.rssiDbm, lastTelemetry.temperatureC, lastTelemetry.humidityRH, lastTelemetry.valid);
  }

  if (isWifiConnected()) {
    updateWifiLed(true, false);
  } else {
    updateWifiLed(false, true);
  }

  if (!isWifiConnected()) {
    ensureConnection();
  }
}
