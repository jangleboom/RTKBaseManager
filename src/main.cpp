#include <Arduino.h>
#include <SPIFFS.h>
#include <RTKBaseManager.h>
#include "../test/test.h"

using namespace RTKBaseManager;
AsyncWebServer server(80);
String scannedSSIDs[MAX_SSIDS];

void setup() {
  
  Serial.begin(115200);
  delay(500);
  // Initialize SPIFFS
  setupSPIFFS();

  delay(500);
  high_precision_location_t baseLocation;
  readIntCoordsFromSPIFFS(&baseLocation, PATH_RTK_BASE_LOCATION);
  double height = getDoubleFromIntegerParts(baseLocation.alt, baseLocation.alt_hp);
  Serial.print(F("Read Altitude from location struct: ")); Serial.println(height, 9);
  // printLocation(&baseLocation);

 
  // Serial.print(F("Test Latitude: ")); Serial.println(latitude, 9);
  // Serial.print(F("Test getCommonPrecisionPartFromDouble: "));Serial.println(getCommonPrecisionPartFromDouble(latitude));
  // Serial.print(F("Test getHighPrecisionPartFromDouble: "));Serial.println(getHighPrecisionPartFromDouble(latitude));
  delay(10000);

 
  
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