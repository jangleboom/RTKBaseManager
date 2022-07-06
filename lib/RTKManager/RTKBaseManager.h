#ifndef RTK_MANAGER_H
#define RTK_MANAGER_H

#include <Arduino.h>
#include <ESPmDNS.h>
#include <ESPAsyncWebServer.h>
#include <index_html.h>
#include <error_html.h>
#include <reboot_html.h>

#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
  #include <SPIFFS.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
  #include <Hash.h>
  #include <FS.h>
#endif

namespace RTKBaseManager {

const PROGMEM char DEVICE_NAME[] = "rtkbase";
  // WiFi credentials for AP mode
const PROGMEM char SSID_AP[] = "RTK-Base";
const PROGMEM char PASSWORD_AP[] = "12345678";
const PROGMEM char IP_AP[] = "192.168.4.1";
  // WiFi and RTKBase manager
const PROGMEM char PARAM_WIFI_SSID[] = "ssid"; 
const PROGMEM char PARAM_WIFI_PASSWORD[] = "password";
const PROGMEM char PARAM_RTK_LOCATION_METHOD[] = "location_method";
const PROGMEM char PARAM_RTK_SURVEY_ENABLED[] = "survey_enabled";
const PROGMEM char PARAM_RTK_COORDS_ENABLED[] = "coords_enabled";
const PROGMEM char PARAM_RTK_LOCATION_SURVEY_ACCURACY[] = "survey_accuracy";
const PROGMEM char PARAM_RTK_LOCATION_LONGITUDE[] = "longitude";
const PROGMEM char PARAM_RTK_LOCATION_LATITUDE[] = "latitude";
const PROGMEM char PARAM_RTK_LOCATION_HEIGHT[] = "height";
const PROGMEM char PATH_WIFI_SSID[] = "/ssid.txt";
const PROGMEM char PATH_WIFI_PASSWORD[] = "/password.txt";
const PROGMEM char PATH_RTK_LOCATION_METHOD[] = "/location_method.txt";
const PROGMEM char PATH_RTK_LOCATION_SURVEY_ACCURACY[] = "/survey_accuracy.txt";
const PROGMEM char PATH_RTK_LOCATION_LONGITUDE[] = "/longitude.txt";
const PROGMEM char PATH_RTK_LOCATION_LATITUDE[] = "/latitude.txt";
const PROGMEM char PATH_RTK_LOCATION_HEIGHT[] = "/height.txt";


// Wifi
int scanWiFiAPs(String* ssidBuff, int ssidBuffLen);
bool knownNetworkAvailable(const String& ssid, String* ssidBuff, int ssidBuffLen);
// Web server
String processor(const String& var);
void notFound(AsyncWebServerRequest *request);
void actionWipeData(AsyncWebServerRequest *request);
void actionRebootESP32(AsyncWebServerRequest *request);
void actionUpdateData(AsyncWebServerRequest *request);

// SPIFFS
void writeFile(fs::FS &fs, const char* path, const char* message);
String readFile(fs::FS &fs, const char* path);
void listFiles(void);
void wipeSpiffsFiles(void);


}



#endif /*** RTK_MANAGER_H ***/