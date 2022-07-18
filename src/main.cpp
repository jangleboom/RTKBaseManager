#include <Arduino.h>
#include <SPIFFS.h>
#include <RTKBaseManager.h>
#include <config.h>

using namespace RTKBaseManager;
AsyncWebServer server(80);
String scannedSSIDs[MAX_SSIDS];

void setup() {
  
  #ifdef DEBUGGING
  Serial.begin(BAUD);
  while (!Serial) {};
  #endif
  
  // Initialize SPIFFS
  setupSPIFFS();

  delay(500);
  double test_val = 11.123456789;
  Serial.print(F("Test test_val: ")); Serial.println(test_val, 9);
  Serial.print(F("Test getLowerPrecisionPartFromDouble: "));Serial.println(getLowerPrecisionPartFromDouble(test_val));
  Serial.print(F("Test getHighPrecisionPartFromDouble: "));Serial.println(getHighPrecisionPartFromDouble(test_val));
  delay(1000);

 
  
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
  // TODO: #Serial and this only #ifdef DEBUG
  aunit::TestRunner::run();
}