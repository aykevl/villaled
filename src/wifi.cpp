#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

#include <esp_wifi.h>
#include <mdns.h>

#include "secrets.h"

static AsyncWebServer server(80);

void initWiFi() {
  // Start WiFi (with powersaving enabled).
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  esp_wifi_set_ps(WIFI_PS_MAX_MODEM);

  // Add a dummy root page.
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "VillaLED kelder");
  });

  // Add OTA updater.
  AsyncElegantOTA.begin(&server, "technicie", OTA_PASS);

  // Start the HTTP server.
  server.begin();
}
