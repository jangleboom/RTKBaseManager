#include <Arduino.h>
#include <SPIFFS.h>
#include <RTKBaseManager.h>

using namespace RTKBaseManager;
AsyncWebServer server(80);
String scannedSSIDs[MAX_SSIDS];

void setup() {
  
  Serial.begin(115200);
  delay(500);
  // Initialize SPIFFS
  setupSPIFFS();
  double latitude = 52.601311471;
  double longitude = 12.601311471;
  double altitude = 35.555;
  high_precision_location_t baseLocation;
  convertDoubleCoordsToIntLocation(latitude, longitude, altitude, &baseLocation);
  writeIntCoordsToSPIFFS(&baseLocation, PATH_RTK_BASE_LOCATION);
  printLocation(&baseLocation);
  baseLocation.lat_hp = 66;
  baseLocation.lon_hp = 66;
  baseLocation.alt_hp = 66;
  printLocation(&baseLocation);
  delay(500);
  readIntCoordsFromSPIFFS(&baseLocation, PATH_RTK_BASE_LOCATION);
  printLocation(&baseLocation);

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