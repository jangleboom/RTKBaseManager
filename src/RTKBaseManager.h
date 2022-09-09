/**
 * @file    RTKBaseManager.h
 * @author  jangleboom
 * @link    https://github.com/audio-communication-group/RTKBaseManager.git
 * <br>
 * @brief   This is part of a distributed software, here: the web interface to config 
 *          the realtime kinematics base station
 * <br>
 * @todo    - a simular version for the head tracker
 *          - a check for special characters in the form
 *          - a check of the number of decimal places in the input of the geo-coordinates 
 *            with regard to a suitable level of accuracy
 *          - upload html and (separated css and js) to SPIFFS 
 *          - refactor func getIntLocationFromSPIFFS (get ist not good)
 * 
 * @note    FYI: A good tutorial about how to transfer input data from a from and save them to SPIFFS
 *          https://medium.com/@adihendro/html-form-data-input-c942ba23224
 */

#ifndef RTK_MANAGER_H
#define RTK_MANAGER_H

#include <Arduino.h>
#include <ESPmDNS.h>
#include <ESPAsyncWebServer.h>
#include <index_html.h>
#include <error_html.h>
#include <reboot_html.h>
#include <ManagerConfig.h>

#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
  #include <SPIFFS.h>
  #include <FS.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
  #include <Hash.h>
  #include <FS.h>
#endif

namespace RTKBaseManager {
  // DEVICE_NAME can be defined in separate config.h file, if not use this here
  #ifndef DEVICE_NAME
  #define DEVICE_NAME "rtkbase"
  #endif
  // WiFi credentials for AP mode
  #define MAX_SSIDS 10 // Space to scan and remember SSIDs
  const char AP_SSID[] PROGMEM = "RTK-Base";
  const char AP_PASSWORD[] PROGMEM = "12345678";
  const char IP_AP[] PROGMEM = "192.168.4.1";
  // Parameters for SPIFFS file management
  const char PARAM_WIFI_SSID[] PROGMEM = "ssid"; 
  const char PARAM_WIFI_PASSWORD[] PROGMEM = "password";
  const char PARAM_RTK_CASTER_HOST[] PROGMEM = "caster_host";
  const char PARAM_RTK_CASTER_PORT[] PROGMEM = "caster_port";
  const char PARAM_RTK_MOINT_POINT[] PROGMEM = "mount_point";
  const char PARAM_RTK_MOINT_POINT_PW[] PROGMEM = "mount_point_pw";
  const char PARAM_RTK_LOCATION_METHOD[] PROGMEM = "location_method";
  const char PARAM_RTK_SURVEY_ENABLED[] PROGMEM = "survey_enabled";
  const char PARAM_RTK_COORDS_ENABLED[] PROGMEM = "coords_enabled";
  const char PARAM_RTK_LOCATION_SURVEY_ACCURACY[] PROGMEM = "survey_accuracy";
  const char PARAM_RTK_LOCATION_LONGITUDE[] PROGMEM = "longitude";
  const char PARAM_RTK_LOCATION_LATITUDE[] PROGMEM = "latitude";
  const char PARAM_RTK_LOCATION_ALTITUDE[] PROGMEM = "altitude";
  // Paths for SPIFFS file management
  const char PATH_WIFI_SSID[] PROGMEM = "/ssid.txt";
  const char PATH_WIFI_PASSWORD[] PROGMEM = "/password.txt";
  const char PATH_RTK_CASTER_HOST[] PROGMEM = "/caster_host";
  const char PATH_RTK_CASTER_PORT[] PROGMEM = "/caster_port";
  const char PATH_RTK_MOINT_POINT[] PROGMEM = "/mount_point";
  const char PATH_RTK_MOINT_POINT_PW[] PROGMEM = "/mount_point_pw";
  const char PATH_RTK_LOCATION_METHOD[] PROGMEM = "/location_method.txt";
  const char PATH_RTK_LOCATION_SURVEY_ACCURACY[] PROGMEM = "/survey_accuracy.txt";
  const char PATH_RTK_LOCATION_LONGITUDE[] PROGMEM = "/longitude.txt";
  const char PATH_RTK_LOCATION_LATITUDE[] PROGMEM = "/latitude.txt";
  const char PATH_RTK_LOCATION_ALTITUDE[] PROGMEM = "/altitude.txt";
  const char SEP = ',';
  const uint8_t LOW_PREC_IDX = 0;
  const uint8_t HIGH_PREC_IDX = 1;
 

// typedef struct {
//   double latitude;
//   double longitude;
//   double altitude;
// } location_double_t;

typedef struct {
  int32_t lat;       // 7 post comma digits latitude
  int8_t  lat_hp;    // high precision extension latitude
  int32_t lon;       // 7 post comma digits longitude
  int8_t  lon_hp;    // high precision extension longitude
  int32_t alt;       // altitude in mm precision
  int8_t  alt_hp;    // altitude in 0.1 mm precision
} location_int_t;

  /*** Wifi ***/

  /**
   * @brief Setup RTK base station in station mode to enter further settings
   * 
   * @param ssid        SSID of the local network
   * @param password    Password of the local network
   * @param deviceName  MDNS name, connect via http://<deviceName>.local
   */
  void setupStationMode(const char* ssid, const char* password, const char* deviceName);

  /**
   * @brief Setup RTK base station in access point mode to enter local network
   *        credentials and other settings
   * 
   * @param apSsid      SSID of the access point
   * @param apPassword  Password of the access point
   */
  void setupAPMode(const char* apSsid, const char* apPassword);

  /**
   * @brief Check possibility of connecting with an availbale network.
   * 
   * @param ssid        SSID of saved network in SPIFFS
   * @return true       If the credentials are complete and the network is available.
   * @return false      If the credentials are incomplete or the network is not available.
   */
  bool savedNetworkAvailable(const String& ssid);

  /*** Web server ***/

  /**
   * @brief         Start web server 
   * 
   * @param server  Pointer to global web server object
   */
  void startServer(AsyncWebServer *server);

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
   * @brief Just init SPIFFS for ESP32 or ESP8266
   * 
   * @param format  True if SPIFFS should formated at start
   * @return true   If SPIFFS is successfully initialized
   *         false  If SPIFFS init failed
   */
  bool setupSPIFFS(bool format);

  /**
   * @brief         Write data to SPIFFS
   * 
   * @param fs      Address of file system
   * @param path    Path to file
   * @param message Content to save in file on path
   * @return  true  If succeed
   *          false If failed
   */
  bool writeFile(fs::FS &fs, const char* path, const char* message);

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

  /**
   * @brief Get the int formated location from SPIFFS 
   * 
   * @param location Address of location_int_t location struct to write to
   * @param pathLat Path to saved latitude
   * @param pathLon Path to saved longitude
   * @param pathAlt Path to saved altitude
   * @return true 
   * @return false 
   */
  bool getIntLocationFromSPIFFS(location_int_t* location, const char* pathLat, const char* pathLon, const char* pathAlt);
  
  /**
   * @brief Print content of location_int_t struct
   * 
   * @param location Pointer to location_int_t var
   */
  void printIntLocation(location_int_t* location);

  /*** Help funcs ***/

  /**
   * @brief Get the lower precision part from double value
   *        like latitude, longitude or altitude
   * 
   * @param input Value in double format
   * @return int32_t Converted value with 7 post dot digits (up tu 1.11 cm)
   */
  int32_t getLowerPrecisionCoordFromDouble(double input);

  /**
   * @brief Get the high precision part from double value
   *        like latitude, longitude or altitude
   * 
   * @param input Value in double format
   * @return int8_t Converted value with 8-9 post dot digits (up tu 1.11 mm, digit 9 is not used)
   */
  int8_t getHighPrecisionCoordFromDouble(double input);

  /**
   * @brief Get the Double From Integer Parts 
   * 
   * @param val int32_t holding die lower precision
   * @param valHp int8_t holding the high precision extension
   * @return double reconstructed value
   */
  double getDoubleCoordFromIntegerParts(int32_t coord, int8_t coordHp);

  /**
   * @brief Get the Double From Integer Parts 
   * 
   * @param val int32_t holding die lower precision
   * @param valHp int8_t holding the high precision extension
   * @return double reconstructed value
   */
  float getFloatAltFromIntegerParts(int32_t alt, int8_t altHp);

  /**
   * @brief Get the deconstructed double val as CSV integer object
   * 
   * @param doubleStr Double coord to deconstruct as String
   * @return String   Deconstructed double coord as CSV
   */
  String getDeconstructedCoordAsCSV(const String& doubleStr);

   /**
   * @brief Get the deconstructed double val as CSV integer object
   * 
   * @param floatStr Float altitude to deconstruct as String
   * @return String   Deconstructed float altitude val as CSV
   */
  String getDeconstructedAltAsCSV(const String& floatStr);

  /**
   * @brief Get the reconstructed coordinate from CSV object
   * 
   * @param csvStr CSV String: <int32_t, int8_t>
   * @param precision Floating point digit num
   * @return String Reconstructed int value as String
   */
  String getFloatingPointStringFromCSV(const String& csvStr, int precision);

  /**
   * @brief Get the reconstructed altitude from CSV object
   * 
   * @param csvStr 
   * @return String Reconstructed int value as String
   */
  String getAltStringFromCSV(const String& csvStr);

  /**
   * @brief Get the Value from CSV object
   * 
   * @param data      CSV String
   * @param separator Separator
   * @param index     Index of val to extract
   * @return String   Extracted val
   */
  String getValueAsStringFromCSV(const String &data, char separator, int index);

  /**
   * @brief Reconnect with last network if not already connected
   * 
   * @return bool   true if connected, false if not
   */
  bool checkConnectionToWifiStation(void);

  /**
   * @brief Set the Location Method in SPIFFS file to Survey
   * 
   */
  void setLocationMethodSurvey(void);

  /**
   * @brief Set the Location Method in SPIFFS file to Coordinates
   * 
   */
  void setLocationMethodCoords(void);

  /**
   * @brief Get the Lower Precision Altitude object
   * 
   * @param alt Altitude (mean sea level or ellipsoid)
   * @return int32_t Mm precision altitude part
   */
  int32_t getLowerPrecisionIntAltitudeFromFloat(float alt);

  /**
   * @brief Get the Higher Precision Altitude object
   * 
   * @param alt Altitude (mean sea level or ellipsoid)
   * @return int8_t int32_t 0.1 mm precision altitude part
   */
  int8_t getHigherPrecisionIntAltitudeFromFloat(float alt);

  /**
   * @brief Get the Digits Count object
   * 
   * @param num Integer number to count 
   * @return int8_t Digits of num
   */
  int8_t getDigitsCount(int32_t num);

  // uint8_t getNumberOfDigits(int32_t i);

  // void getIntAltitudeFromDouble(float alt);

  /**
   * @brief Get the Float Altitude From Int object
   * 
   * @param alt Lower precision altitude part (mm)
   * @param altHp Higher precision altitude part (0.1 mm)
   * @return Float value of altitude 
   */
  float getFloatAltitudeFromInt(int32_t alt, int8_t altHp);

}

#endif /*** RTK_MANAGER_H ***/