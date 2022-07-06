/**
 * @file    RtkWifiManager.h
 * @author  jangleboom
 * @link    https://github.com/audio-communication-group/rwaht_esp_wifi_manager
 * <br>
 * @brief   This is part of a distributed software, here: the web interface to config 
 *          the realtime kinematics base station
 * <br>
 * @todo    - a simular version for the head tracker
 *          - a check for special characters in the form
 *          - a check of the number of decimal places in the input of the geo-coordinates 
 *            with regard to a suitable level of accuracy
 *          - upload html and (separated css and js) to SPIFFS 
 * @note    
 */

#pragma once

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

#include <Arduino.h>
#include <ESPmDNS.h>
#include <ESPAsyncWebServer.h>
#include <index_html.h>
#include <error_html.h>
#include <reboot_html.h>

#define MAX_SSIDS 10 // Space to scan and remember SSIDs

class RtkWifiManager {
  private:

  public:
  static const char DEVICE_NAME[8];
  // WiFi credentials for AP mode
  static const char SSID_AP[9];
  static const char PASSWORD_AP[9];
  static const char IP_AP[12];
  // WiFi and RTKBase manager
  static const char PARAM_WIFI_SSID[5];
  static const char PARAM_WIFI_PASSWORD[9];
  static const char PARAM_RTK_LOCATION_METHOD[16];
  static const char PARAM_RTK_SURVEY_ENABLED[15];
  static const char PARAM_RTK_COORDS_ENABLED[15];
  static const char PARAM_RTK_LOCATION_SURVEY_ACCURACY[16];
  static const char PARAM_RTK_LOCATION_LONGITUDE[10];
  static const char PARAM_RTK_LOCATION_LATITUDE[9];
  static const char PARAM_RTK_LOCATION_HEIGHT[7];
  static const char PATH_WIFI_SSID[10];
  static const char PATH_WIFI_PASSWORD[14];
  static const char PATH_RTK_LOCATION_METHOD[22];
  static const char PATH_RTK_LOCATION_SURVEY_ACCURACY[21];
  static const char PATH_RTK_LOCATION_LONGITUDE[15];
  static const char PATH_RTK_LOCATION_LATITUDE[14];
  static const char PATH_RTK_LOCATION_HEIGHT[12];
  
  static String seenSSIDs[MAX_SSIDS];
  // Wifi
  RtkWifiManager();
  static int scanWiFiAPs(void);
  static bool knownNetworkAvailable(const String& ssid);
  // Web server
  static String processor(const String& var);
  static void notFound(AsyncWebServerRequest *request);
  static void actionRebootESP32(AsyncWebServerRequest *request);
  static void actionWipeData(AsyncWebServerRequest *request);
  static void actionUpdateData(AsyncWebServerRequest *request);
  // SPIFFS
  static void listAllFiles(void);
  static void wipeAllSpiffsFiles(void);
  static String readFile(fs::FS &fs, const char* path);
  static void writeFile(fs::FS &fs, const char* path, const char* message);
};
