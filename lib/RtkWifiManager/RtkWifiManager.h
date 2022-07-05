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

#ifndef RTK_WIFI_MANAGER_H
#define RTK_WIFI_MANAGER_H

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

class RtkWifiManager {
  private:

  public:
  AsyncWebServer server(80);
  RtkWifiManager();
}

const char* DEVICE_NAME = "rtkbase";
// WiFi credentials for AP mode
const char *SSID_AP = "RTK-Base";
const char *PASSWORD_AP = "12345678";
const char *IP_AP = "192.168.4.1";

// WiFi and RTKBase SPIFFS parameters and paths 
const char* PARAM_WIFI_SSID = "ssid";
const char* PARAM_WIFI_PASSWORD = "password";
const char* PARAM_RTK_LOCATION_METHOD = "location_method";
const char* PARAM_RTK_SURVEY_ENABLED = "survey_enabled";
const char* PARAM_RTK_COORDS_ENABLED = "coords_enabled";
const char* PARAM_RTK_LOCATION_SURVEY_ACCURACY = "survey_accuracy";
const char* PARAM_RTK_LOCATION_LONGITUDE = "longitude";
const char* PARAM_RTK_LOCATION_LATITUDE = "latitude";
const char* PARAM_RTK_LOCATION_HEIGHT = "height";
const char PATH_WIFI_SSID[] = "/ssid.txt";
const char PATH_WIFI_PASSWORD[] = "/password.txt";
const char PATH_RTK_LOCATION_METHOD[] = "/location_method.txt";
const char PATH_RTK_LOCATION_SURVEY_ACCURACY[] = "/survey_accuracy.txt";
const char PATH_RTK_LOCATION_LONGITUDE[] = "/longitude.txt";
const char PATH_RTK_LOCATION_LATITUDE[] = "/latitude.txt";
const char PATH_RTK_LOCATION_HEIGHT[] = "/height.txt";

// WiFi
const uint8_t MAX_SSIDS;                      // Max wifi ssid scan 
String seenSSIDs[MAX_SSIDS];                       // Holds the wifi ssids at your place

/**
 * @brief Scan for Wifi SSIDs and collect MAX_SSIDS of them into the seenSSIDs array
 * 
 * @return int Number of found SSIDs
 */
int scanWiFiAPs(void);

/**
 * @brief           Check whether a known network can be reached with the 
 *                  saved access data (if they are saved).
 * 
 * @param ssid      SSID of the saved network
 * @return true     If credentials (incl. PW) are saved and the network is
 *                  reachable
 * @return false    If the access data is incomplete or the network 
 *                  cannot be reached with the specified Ssid 
 */
bool knownNetworkAvailable(const String& ssid);

// Web server
String processor(const String& var);
void notFound(AsyncWebServerRequest *request);
void actionRebootESP32(AsyncWebServerRequest *request);
void actionWipeData(AsyncWebServerRequest *request);
void actionUpdateData(AsyncWebServerRequest *request);
// SPIFFS
void writeFile(fs::FS &fs, const char* path, const char* message);
String readFile(fs::FS &fs, const char* path);
void listAllFiles(void);
void wipeAllSpiffsFiles(void);


#endif /*** RTK_WIFI_MANAGER_H ***/