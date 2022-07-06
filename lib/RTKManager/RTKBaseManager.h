/**
 * @file    RTKBaseManager.h
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
// Parameters for SPIFFS file management
const PROGMEM char PARAM_WIFI_SSID[] = "ssid"; 
const PROGMEM char PARAM_WIFI_PASSWORD[] = "password";
const PROGMEM char PARAM_RTK_LOCATION_METHOD[] = "location_method";
const PROGMEM char PARAM_RTK_SURVEY_ENABLED[] = "survey_enabled";
const PROGMEM char PARAM_RTK_COORDS_ENABLED[] = "coords_enabled";
const PROGMEM char PARAM_RTK_LOCATION_SURVEY_ACCURACY[] = "survey_accuracy";
const PROGMEM char PARAM_RTK_LOCATION_LONGITUDE[] = "longitude";
const PROGMEM char PARAM_RTK_LOCATION_LATITUDE[] = "latitude";
const PROGMEM char PARAM_RTK_LOCATION_HEIGHT[] = "height";
// Paths for SPIFFS file management
const PROGMEM char PATH_WIFI_SSID[] = "/ssid.txt";
const PROGMEM char PATH_WIFI_PASSWORD[] = "/password.txt";
const PROGMEM char PATH_RTK_LOCATION_METHOD[] = "/location_method.txt";
const PROGMEM char PATH_RTK_LOCATION_SURVEY_ACCURACY[] = "/survey_accuracy.txt";
const PROGMEM char PATH_RTK_LOCATION_LONGITUDE[] = "/longitude.txt";
const PROGMEM char PATH_RTK_LOCATION_LATITUDE[] = "/latitude.txt";
const PROGMEM char PATH_RTK_LOCATION_HEIGHT[] = "/height.txt";

/*** Wifi ***/

/**
 * @brief Scan available Wifi SSIDs
 * 
 * @param ssidBuff    String array holding scanned SSIDs
 * @param ssidBuffLen Max count of strings in array
 * @return int        Number of available Wifi networks
 */
int scanWiFiAPs(String* ssidBuff, int ssidBuffLen);

/**
 * @brief Check possibility of connecting with an availbale network.
 * 
 * @param ssid        SSID of saved network in SPIFFS
 * @param ssidBuff    Buffer with scanned SSIDs 
 * @param ssidBuffLen Buffer length
 * @return true       If the credentials are complete and the network is available.
 * @return false      If the credentials are incomplete or the network is not available.
 */
bool knownNetworkAvailable(const String& ssid, String* ssidBuff, int ssidBuffLen);


/*** Web server ***/

/**
 * @brief Relaces placeholders in HTML code
 * 
 * @param var Placeholder
 * @return String Text to replace the placeholder
 */
String processor(const String& var);

/**
 * @brief Request not found handler
 * 
 * @param request Request
 */
void notFound(AsyncWebServerRequest *request);

/**
 * @brief Action to handle wipe SPIFFS button
 * 
 * @param request Request
 */
void actionWipeData(AsyncWebServerRequest *request);

/**
 * @brief Action to handle Reboot button
 * 
 * @param request Request
 */
void actionRebootESP32(AsyncWebServerRequest *request);

/**
 * @brief Action to handle Save button
 * 
 * @param request Request
 */
void actionUpdateData(AsyncWebServerRequest *request);


/*** SPIFFS ***/

/**
 * @brief         Write data to SPIFFS
 * 
 * @param fs      Address of file system
 * @param path    Path to file
 * @param message Content to save in file on path
 */
void writeFile(fs::FS &fs, const char* path, const char* message);

/**
 * @brief           Read data from SPIFFS
 * 
 * @param fs        Address of file system
 * @param path      Path to file
 * @return String   Content saved in file on path
 */
String readFile(fs::FS &fs, const char* path);

/**
 * @brief List all saved SPIFFS files 
 * 
 */
void listFiles(void);

/**
 * @brief Delete all saved SPIFFS files 
 * 
 */
void wipeSpiffsFiles(void);


}



#endif /*** RTK_MANAGER_H ***/