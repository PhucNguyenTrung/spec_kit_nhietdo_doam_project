#include "sensor.h"
#include "config.h"
#include <Wire.h>

static bool readSht31Raw(float& temperature, float& humidity) {
  const uint8_t command[] = {0x2C, 0x06};

  Wire.beginTransmission(SHT31_I2C_ADDRESS);
  if (Wire.endTransmission() != 0) {
    return false;
  }

  Wire.beginTransmission(SHT31_I2C_ADDRESS);
  Wire.write(command, sizeof(command));
  if (Wire.endTransmission() != 0) {
    return false;
  }

  unsigned long startedAt = millis();
  while ((millis() - startedAt) < 20UL) {
    // Wait for the SHT31-D conversion window without blocking the surrounding runtime loop.
  }

  if (Wire.requestFrom((int)SHT31_I2C_ADDRESS, (int)6) != 6) {
    return false;
  }

  uint8_t buffer[6] = {0};
  for (uint8_t i = 0; i < 6 && Wire.available(); ++i) {
    buffer[i] = Wire.read();
  }

  if (buffer[0] == 0 && buffer[1] == 0) {
    return false;
  }

  uint16_t rawTemperature = (static_cast<uint16_t>(buffer[0]) << 8) | buffer[1];
  uint16_t rawHumidity = (static_cast<uint16_t>(buffer[3]) << 8) | buffer[4];

  temperature = -45.0f + (175.0f * static_cast<float>(rawTemperature) / 65535.0f);
  humidity = (100.0f * static_cast<float>(rawHumidity) / 65535.0f);
  return true;
}

bool sensorBegin() {
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  return true;
}

bool readSensorTelemetry(TelemetryData& data) {
  data.temperatureC = 0.0f;
  data.humidityRH = 0.0f;
  data.valid = false;

  float temperature = 0.0f;
  float humidity = 0.0f;
  if (!readSht31Raw(temperature, humidity)) {
    return false;
  }

  data.temperatureC = temperature;
  data.humidityRH = humidity;
  data.valid = isValidTelemetry(data);
  return data.valid;
}

bool isValidTelemetry(const TelemetryData& data) {
  if (!isfinite(data.temperatureC) || !isfinite(data.humidityRH)) {
    return false;
  }

  if (data.temperatureC < -40.0f || data.temperatureC > 80.0f) {
    return false;
  }

  if (data.humidityRH < 0.0f || data.humidityRH > 100.0f) {
    return false;
  }

  return true;
}
