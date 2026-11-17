#include "led.h"
#include "config.h"

namespace {
  bool gWifiState = false;
  bool gServerState = false;
  unsigned long gLastServerPulseMs = 0;
}

void ledSetup() {
  pinMode(WIFI_LED_PIN, OUTPUT);
  pinMode(SERVER_LED_PIN, OUTPUT);
  digitalWrite(WIFI_LED_PIN, LOW);
  digitalWrite(SERVER_LED_PIN, LOW);
}

void updateWifiLed(bool isConnected, bool isConnecting) {
  gWifiState = isConnected;
  if (isConnecting) {
    digitalWrite(WIFI_LED_PIN, (millis() / 500) % 2 == 0 ? HIGH : LOW);
  } else if (isConnected) {
    digitalWrite(WIFI_LED_PIN, HIGH);
  } else {
    digitalWrite(WIFI_LED_PIN, LOW);
  }
}

void updateServerLed(bool isOnline, bool isSuccessPulse) {
  gServerState = isOnline;
  if (isSuccessPulse) {
    digitalWrite(SERVER_LED_PIN, HIGH);
    gLastServerPulseMs = millis();
    return;
  }

  if (isOnline) {
    digitalWrite(SERVER_LED_PIN, LOW);
  } else {
    digitalWrite(SERVER_LED_PIN, (millis() / 250) % 2 == 0 ? HIGH : LOW);
  }
}

void blinkServerLedWarning() {
  digitalWrite(SERVER_LED_PIN, HIGH);
}

void setLedStates(bool wifiConnected, bool serverOnline) {
  updateWifiLed(wifiConnected, !wifiConnected);
  updateServerLed(serverOnline, false);
}
