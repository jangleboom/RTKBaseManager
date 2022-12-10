#include <Arduino.h>
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

  //===============================================================================
  // Initialize LittleFS
  // Use board_build.partitions in platformio.ini
  if (!setupLittleFS()) 
  {
    formatLittleFS();
    if (!setupLittleFS()) while (true) {};
  }

  // Uncomment if you want to format (e. g after changing partition sizes)
  // (And dont forget to comment this again after one run ;)
  //formatLittleFS();

  //wipeLittleFSFiles();  // Use this for deleting all data
#ifdef DEBUGGING
  listFiles();
  delay(3000);
#endif
  //===============================================================================
  
  DBG.print(F("Device name: ")); DBG.println(DEVICE_TYPE);

  String locationMethod = readFile(LittleFS, getPath(PARAM_RTK_LOCATION_METHOD).c_str());
  DBG.print(F("Location method: ")); DBG.println(locationMethod);
  
  location_t lastLocation;
  if (getLocationFromLittleFS(&lastLocation, \
      getPath(PARAM_RTK_LOCATION_LATITUDE).c_str(), \
      getPath(PARAM_RTK_LOCATION_LONGITUDE).c_str(), \
      getPath(PARAM_RTK_LOCATION_ALTITUDE).c_str(), \
      getPath(PARAM_RTK_LOCATION_COORD_ACCURACY).c_str() ) )
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