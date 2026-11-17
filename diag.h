#ifndef DIAG_H
#define DIAG_H

#include <Arduino.h>

void diagSetup();
void diagPrintBoot();
void diagPrintStatus(const char* wifiState, const char* serverState, const char* ipAddress, int rssiDbm, float temperatureC, float humidityRH, bool valid);
void diagPrintError(const char* message);
void diagPrintInfo(const char* message);

#endif
