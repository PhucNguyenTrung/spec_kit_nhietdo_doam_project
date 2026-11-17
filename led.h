#ifndef LED_H
#define LED_H

#include <Arduino.h>

void ledSetup();
void updateWifiLed(bool isConnected, bool isConnecting);
void updateServerLed(bool isOnline, bool isSuccessPulse);
void blinkServerLedWarning();
void setLedStates(bool wifiConnected, bool serverOnline);

#endif
