#include <Arduino.h>
#include <RTKBaseManager.h>
#include <ManagerConfig.h>

#ifdef DEBUGGING
#include <TestsRTKBaseManager.h>
#endif

AsyncWebServer server(80);
String scannedSSIDs[MAX_SSIDS];

void blinkOneTime(int blinkTime, bool doNotBlock);

void setup() 
{
  #ifdef DEBUGGING
  Serial.begin(BAUD);
  while (!Serial) {};
  #endif
  // Board LED used for error codes (written in README.md)
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

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

  //===============================================================================
  // Wifi setup AP or STATION, depending on data in LittleFS
  setupWiFi(&server);
  delay(1000);

  while (WiFi.getMode() == WIFI_AP) 
  {
    DBG.println(F("Enter Wifi credentials on webform:"));
    DBG.print(F("Connect your computer to SSID: "));
    DBG.println(WiFi.getHostname());
    DBG.print(F("Go with your Browser to IP: "));
    DBG.println(WiFi.softAPIP());
    blinkOneTime(1000, false);
    blinkOneTime(100, false);
  }
  if (WiFi.getMode() == WIFI_STA) 
  {
    while (! WiFi.isConnected())
    {
      DBG.println(F("setup(): Try reconnect to WiFi station"));
      WiFi.reconnect();
      DBG.printf("WiFi state: %s", WiFi.isConnected() ? "connected" : "disconnected");
      blinkOneTime(1000, false);
      blinkOneTime(100, false);
    }
  }
}

unsigned long previousMillis = 0;
const unsigned long RECONNECT_INTERVAL = 10000;

void loop() 
{
  #ifdef DEBUGGING
  aunit::TestRunner::run();
  #endif

  static bool isConnected = false;

  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting every RECONNECT_INTERVAL seconds
  if (currentMillis - previousMillis > RECONNECT_INTERVAL) 
  {
    isConnected = RTKBaseManager::checkConnectionToWifiStation();
    if (! isConnected) 
    {
      blinkOneTime(1000, false);
      blinkOneTime(100, false);
    }
    previousMillis = currentMillis;
  }
  
}

void blinkOneTime(int blinkTime, bool doNotBlock)
{
  digitalWrite(LED_BUILTIN, HIGH);
  doNotBlock ? vTaskDelay(blinkTime) : delay(blinkTime);
  digitalWrite(LED_BUILTIN, LOW);
  doNotBlock ? vTaskDelay(blinkTime) : delay(blinkTime);
}