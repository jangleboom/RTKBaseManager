#include <Arduino.h>
#include <SPIFFS.h>
#include <RTKBaseManager.h>
#include <RTKConfig.h>

#ifdef DEBUGGING
#include <TestsRTKBaseManager.h>
#endif

AsyncWebServer server(80);
String scannedSSIDs[MAX_SSIDS];

void setup() {
  
  #ifdef DEBUGGING
  Serial.begin(BAUD);
  while (!Serial) {};
  #endif
  
  // Initialize SPIFFS
  RTKBaseManager::setupSPIFFS();

  WiFi.setHostname(DEVICE_NAME);
  // Check if we have credentials for a available network
  String lastSSID = RTKBaseManager::readFile(SPIFFS, PATH_WIFI_SSID);
  String lastPassword = RTKBaseManager::readFile(SPIFFS, PATH_WIFI_PASSWORD);

  if (!RTKBaseManager::savedNetworkAvailable(lastSSID) || lastPassword.isEmpty() ) {
    RTKBaseManager::setupAPMode(AP_SSID, AP_PASSWORD);
    delay(500);
  } else {
   RTKBaseManager::setupStationMode(lastSSID.c_str(), lastPassword.c_str(), DEVICE_NAME);
   delay(500);
 }
  RTKBaseManager::startServer(&server);
}

void loop() {
  #ifdef DEBUGGING
  aunit::TestRunner::run();
  #endif
  DEBUG_SERIAL.println(F("Here are your SPIFFS file paths (empty at first run):"))
  RTKBaseManager::listFiles();
  while (true) {};
}