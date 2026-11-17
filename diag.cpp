#include "diag.h"
#include "config.h"

void diagSetup() {
  Serial.begin(SERIAL_BAUD);
}

void diagPrintBoot() {
  Serial.println("[BOOT] Khởi động ESP32 telemetry KEPServerEX");
  Serial.println("[BOOT] Cấu hình I2C SHT31-D: SDA=21, SCL=22, dia chi=0x44");
  Serial.println("[BOOT] Cấu hình WiFi: SSID=ESP32_OT_LAN");
}

void diagPrintStatus(const char* wifiState, const char* serverState, const char* ipAddress, int rssiDbm, float temperatureC, float humidityRH, bool valid) {
  Serial.print("[STATUS] WiFi=");
  Serial.print(wifiState);
  Serial.print(" | KEPServer=");
  Serial.print(serverState);
  Serial.print(" | IP=");
  Serial.print(ipAddress);
  Serial.print(" | RSSI=");
  Serial.print(rssiDbm);
  Serial.print(" dBm | Temp=");
  Serial.print(temperatureC, 2);
  Serial.print("C | Hum=");
  Serial.print(humidityRH, 2);
  Serial.print("%RH | Data=");
  Serial.println(valid ? "HOP_LE" : "KHONG_HOP_LE");
}

void diagPrintError(const char* message) {
  Serial.print("[LOI] ");
  Serial.println(message);
}

void diagPrintInfo(const char* message) {
  Serial.print("[INFO] ");
  Serial.println(message);
}
