#include <Arduino.h>
#include <SPIFFS.h>
#include <RTKBaseManager.h>

using namespace RTKBaseManager;
AsyncWebServer server(80);
String scannedSSIDs[MAX_SSIDS];

void setup() {
  Serial.begin(115200);
  delay(500);
  double latitude = 52.601311471;
  Serial.print(F("Test Latitude: ")); Serial.println(latitude,9);
  Serial.print(F("Test getIntegerFromDouble: "));Serial.println(getIntegerFromDouble(latitude));
  Serial.print(F("Test getPrecisionExtensionFromDouble: "));Serial.println(getPrecisionExtensionFromDouble(latitude));
  delay(10000);
  // Initialize SPIFFS
  setupSPIFFS();
  
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