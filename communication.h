#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <Arduino.h>

enum class WifiState {
  CONNECTING,
  CONNECTED,
  LOST
};

enum class ServerState {
  ONLINE,
  OFFLINE,
  RETRYING
};

struct NetworkState {
  WifiState wifiState;
  ServerState serverState;
  char ipAddress[16];
  int rssiDbm;
  unsigned long lastSuccessMs;
};

void communicationSetup();
void communicationLoop();
void ensureConnection();
bool tryPostTelemetry(const char* payload);
const char* wifiStateToString(WifiState state);
const char* serverStateToString(ServerState state);
NetworkState getNetworkState();

#endif
