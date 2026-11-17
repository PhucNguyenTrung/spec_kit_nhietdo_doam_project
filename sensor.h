#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>

struct TelemetryData {
  float temperatureC;
  float humidityRH;
  bool valid;
};

bool sensorBegin();
bool readSensorTelemetry(TelemetryData& data);
bool isValidTelemetry(const TelemetryData& data);

#endif
