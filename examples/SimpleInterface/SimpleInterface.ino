#include <Arduino.h>
#include <SPIFFS.h>
#include <RTKBaseManager.h>
#include <ManagerConfig.h>

#ifdef DEBUGGING
#include <TestsRTKBaseManager.h>
#endif

AsyncWebServer server(80);
String scannedSSIDs[MAX_SSIDS];

bool deleteFilesystem = false;

void setup() 
{
  #ifdef DEBUGGING
  Serial.begin(BAUD);
  while (!Serial) {};
  #endif
  
  // Initialize SPIFFS
  if (!RTKBaseManager::setupSPIFFS(FORMAT_SPIFFS_IF_FAILED)) 
  {
    DBG.println(F("setupSPIFFS failed, freezing"));
    while (true) {};
  }

  // If you want to clean the whole filesystem set deleteFilesystem true, run 1x and set it false again
  if (deleteFilesystem) 
  {
    wipeSpiffsFiles();
    while (true) {};
  }

  DBG.print(F("Device name: ")); DBG.println(DEVICE_TYPE);

  String locationMethod = readFile(SPIFFS, PATH_RTK_LOCATION_METHOD);
  DBG.print(F("Location method: ")); DBG.println(locationMethod);
  
  location_t lastLocation;
  if (getLocationFromSPIFFS(&lastLocation, PATH_RTK_LOCATION_LATITUDE, PATH_RTK_LOCATION_LONGITUDE, PATH_RTK_LOCATION_ALTITUDE, PATH_RTK_LOCATION_COORD_ACCURACY)) 
  {
    printLocation(&lastLocation);
  }

  setupWiFi(&server);
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