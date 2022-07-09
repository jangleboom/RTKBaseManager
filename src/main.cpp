#include <Arduino.h>
#include <SPIFFS.h>
#include <RTKBaseManager.h>

using namespace RTKBaseManager;
AsyncWebServer server(80);
String scannedSSIDs[MAX_SSIDS];

void setup() {
  Serial.begin(115200);
  // Initialize SPIFFS
  #ifdef ESP32
    if (!SPIFFS.begin(true)) {
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }
  #else
    if (!SPIFFS.begin()) {
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }
  #endif
  
  WiFi.setHostname(DEVICE_NAME);
  // Check if we have credentials for a available network
  String lastSSID = readFile(SPIFFS, PATH_WIFI_SSID);
  String lastPassword = readFile(SPIFFS, PATH_WIFI_PASSWORD);

  if (!savedNetworkAvailable(lastSSID) || lastPassword.isEmpty() ) {
    setupAPMode(AP_SSID, AP_PASSWORD);
    delay(500);
  } else {
   setupStationMode(lastSSID.c_str(), lastPassword.c_str(), DEVICE_NAME);
   delay(500);
 }
  startServer(&server);
}

void loop() {
  
}