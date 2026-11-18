#include "communication.h"
#include "config.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>

namespace {
  NetworkState gNetworkState = {WifiState::CONNECTING, ServerState::OFFLINE, "", -999, 0};
  unsigned long gLastWifiAttemptMs = 0;
  unsigned long gLastServerAttemptMs = 0;
  bool gWifiConnected = false;

  void updateWifiState() {
    if (WiFi.status() == WL_CONNECTED) {
      if (gNetworkState.wifiState != WifiState::CONNECTED) {
        gNetworkState.wifiState = WifiState::CONNECTED;
        gWifiConnected = true;
        snprintf(gNetworkState.ipAddress, sizeof(gNetworkState.ipAddress), "%s", WiFi.localIP().toString().c_str());
      }
      gNetworkState.rssiDbm = WiFi.RSSI();
    } else {
      gNetworkState.wifiState = WifiState::LOST;
      gNetworkState.rssiDbm = -999;
      gWifiConnected = false;
    }
  }

  void updateServerState(bool isSuccess) {
    if (isSuccess) {
      gNetworkState.serverState = ServerState::ONLINE;
      gNetworkState.lastSuccessMs = millis();
    } else if (gNetworkState.serverState != ServerState::RETRYING) {
      gNetworkState.serverState = ServerState::RETRYING;
    }
  }
}

void communicationSetup() {
  WiFi.mode(WIFI_STA);
  gNetworkState.wifiState = WifiState::CONNECTING;
  gNetworkState.serverState = ServerState::OFFLINE;
  memset(gNetworkState.ipAddress, 0, sizeof(gNetworkState.ipAddress));
  gNetworkState.rssiDbm = -999;
}

void communicationLoop() {
  unsigned long now = millis();

  if (WiFi.status() != WL_CONNECTED) {
    if (now - gLastWifiAttemptMs >= WIFI_RETRY_INTERVAL_MS) {
      gLastWifiAttemptMs = now;
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    }
  } else {
    updateWifiState();
    if (gNetworkState.serverState == ServerState::OFFLINE || gNetworkState.serverState == ServerState::RETRYING) {
      if (now - gLastServerAttemptMs >= SERVER_RETRY_INTERVAL_MS) {
        gLastServerAttemptMs = now;
        gNetworkState.serverState = ServerState::RETRYING;
      }
    }
  }

  updateWifiState();
}

bool isWifiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

void ensureConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  }
}

bool tryPostTelemetry(const char* payload) {
  if (payload == nullptr || payload[0] == '\0') {
    return false;
  }

  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }

  HTTPClient client;
  client.setTimeout(HTTP_TIMEOUT_MS / 1000);
  String url = String("http://") + KEP_SERVER_HOST + ":" + String(KEP_SERVER_PORT) + KEP_SERVER_ENDPOINT;

  if (!client.begin(url)) {
    updateServerState(false);
    return false;
  }

  client.addHeader("Content-Type", "application/json");
  int httpCode = client.POST(payload);
  bool ok = (httpCode >= 200 && httpCode < 300);
  client.end();

  updateServerState(ok);
  return ok;
}

const char* wifiStateToString(WifiState state) {
  switch (state) {
    case WifiState::CONNECTING: return "DANG_KET_NOI";
    case WifiState::CONNECTED: return "DA_KET_NOI";
    case WifiState::LOST: return "MAT_KET_NOI";
    default: return "KHONG_XAC_DINH";
  }
}

const char* serverStateToString(ServerState state) {
  switch (state) {
    case ServerState::ONLINE: return "TRUC_TUYEN";
    case ServerState::OFFLINE: return "NGOAI_TUYEN";
    case ServerState::RETRYING: return "DANG_THU_LAI";
    default: return "KHONG_XAC_DINH";
  }
}

NetworkState getNetworkState() {
  return gNetworkState;
}
