#include <Arduino.h>
#include <SPIFFS.h>
#include <RTKBaseManager.h>
#include <ManagerConfig.h>

#ifdef DEBUGGING
#include <TestsRTKBaseManager.h>
#endif

AsyncWebServer server(80);
String scannedSSIDs[MAX_SSIDS];

void setup() 
{
  
  #ifdef DEBUGGING
  Serial.begin(BAUD);
  while (!Serial) {};
  #endif
  
  // Initialize SPIFFS, set true for formatting
  bool format = false;
  if (!RTKBaseManager::setupSPIFFS(format)) 
  {
    DEBUG_SERIAL.println(F("setupSPIFFS failed, freezing"));
    while (true) {};
  }

  DEBUG_SERIAL.print(F("Device name: "));DEBUG_SERIAL.println(DEVICE_NAME);

  String locationMethod = readFile(SPIFFS, PATH_RTK_LOCATION_METHOD);
  DEBUG_SERIAL.print(F("Location method: ")); DEBUG_SERIAL.println(locationMethod);
  
  location_t lastLocation;
  if (getIntLocationFromSPIFFS(&lastLocation, PATH_RTK_LOCATION_LATITUDE, PATH_RTK_LOCATION_LONGITUDE, PATH_RTK_LOCATION_ALTITUDE)) 
  {
    printLocation(&lastLocation);
  }

  // Check if we have credentials for a available network
  String lastSSID = RTKBaseManager::readFile(SPIFFS, PATH_WIFI_SSID);
  String lastPassword = RTKBaseManager::readFile(SPIFFS, PATH_WIFI_PASSWORD);

  if (!RTKBaseManager::savedNetworkAvailable(lastSSID) || lastPassword.isEmpty() ) 
  {
    RTKBaseManager::setupAPMode(AP_SSID, AP_PASSWORD);
    delay(500);
  } else 
  {
   RTKBaseManager::setupStationMode(lastSSID.c_str(), lastPassword.c_str(), DEVICE_NAME);
   delay(500);
  }
  RTKBaseManager::startServer(&server);
}

unsigned long previousMillis = 0;
const unsigned long RECONNECT_INTERVAL = 30000;

void loop() 
{
  #ifdef DEBUGGING
  aunit::TestRunner::run();
  #endif

  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting every RECONNECT_INTERVAL seconds
  if (currentMillis - previousMillis > RECONNECT_INTERVAL) 
  {
    RTKBaseManager::checkConnectionToWifiStation();
    previousMillis = currentMillis;
  }
  
}