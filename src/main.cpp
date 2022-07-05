
#include <Arduino.h>
#include <ESPmDNS.h>

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

#include <ESPAsyncWebServer.h>
#include "index_html.h"
#include "error_html.h"
#include "reboot_html.h"

/** TODO: 
          - upload html and css to SPIFFS 
*/

AsyncWebServer server(80);

const char* DEVICE_NAME = "rtkbase";
// WiFi credentials for AP mode
const char *SSID_AP = "RTK-Base";
const char *PASSWORD_AP = "12345678";
const char *IP_AP = "192.168.4.1";

// WiFi and RTKBase manager
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
const uint8_t MAX_SSIDS = 10;
String seenSSIDs[MAX_SSIDS];
int scanWiFiAPs(void);
int setupWiFiAP(const String& ssid);
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


void setup() {
  Serial.begin(115200);
  // Initialize SPIFFS
  #ifdef ESP32
    if (!SPIFFS.begin(true)) {
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }
  #else
    if (!SPIFFS.begin()) {
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }
  #endif
  
  WiFi.setHostname(DEVICE_NAME);

  // Check if we have credentials for a available network
  String clientSSID = readFile(SPIFFS, PATH_WIFI_SSID);
  String clientPassword = readFile(SPIFFS, PATH_WIFI_PASSWORD);

  if (!knownNetworkAvailable(clientSSID) || clientPassword.isEmpty()) {
    int foundAPs = scanWiFiAPs();
    for (int i=0; i<foundAPs; i++) {
      Serial.printf("%d %s\n", i+1, seenSSIDs[i].c_str());
    }
    delay(1000);
    Serial.print("Setting soft-AP ... ");
    WiFi.mode(WIFI_AP);
    // WiFi.softAP(SSID_AP, PASSWORD_AP);
    Serial.println(WiFi.softAP(SSID_AP, PASSWORD_AP) ? "Ready" : "Failed!");
    Serial.print("Access point started: ");
    Serial.println(SSID_AP);
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());
    delay(1000);
 } else {
  WiFi.mode(WIFI_STA);
  WiFi.begin(clientSSID.c_str(), clientPassword.c_str());
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    // TODO:  - count reboots and stop after 3 times (save in SPIFFS)
    //        - display state
    Serial.println("WiFi Failed! Reboot in 10 s as AP!");
    delay(10000);
    ESP.restart();
  }
  Serial.println();

  if (!MDNS.begin(DEVICE_NAME)) {
      Serial.println("Error starting mDNS, use local IP instead!");
  } else {
    Serial.printf("Starting mDNS, find me under <http://www.%s.local>\n", DEVICE_NAME);
  }

  Serial.print("Wifi client started: ");
  Serial.println(WiFi.getHostname());
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", INDEX_HTML, processor);
  });

  server.on("/actionUpdateData", HTTP_POST, actionUpdateData);
  server.on("/actionWipeData", HTTP_POST, actionWipeData);
  server.on("/actionRebootESP32", HTTP_POST, actionRebootESP32);

  server.onNotFound(notFound);
  server.begin();
}

void loop() {
  
}

/********************************************************************************
*                             WiFi
* ******************************************************************************/
bool knownNetworkAvailable(const String& ssid) {
  if (ssid.isEmpty()) return false;

  int nNetworks = scanWiFiAPs();
  for (int i=0; i<nNetworks; i++) {
    if (ssid.equals(seenSSIDs[i])) {
      Serial.printf("A known network with SSID %s found, connecting...", seenSSIDs[i]);
      return true;
    }
  }
  Serial.printf("No known network with SSID %s found, starting AP to enter new credentials\n", ssid);
  return false;
}

int scanWiFiAPs() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    // Delete old entries
    for (int i = 0; i < MAX_SSIDS; i++) {
        seenSSIDs[i] = "";
    }

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
  
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" network(s) found");
        for (int i = 0; i < n; i++) {
            // Print SSID and RSSI for each network found
            seenSSIDs[i] = String(WiFi.SSID(i));//+ " (" + String(WiFi.RSSI(i)) + ")";
            Serial.println(seenSSIDs[i]);
            delay(10);
            if (n == MAX_SSIDS) break;
        }
    }
    Serial.println();
    return n;
}


/********************************************************************************
*                             Web server
* ******************************************************************************/
void actionUpdateData(AsyncWebServerRequest *request) {
  Serial.println("ACTION 1!");

  int params = request->params();
  for (int i = 0; i < params; i++) {
    AsyncWebParameter* p = request->getParam(i);
    Serial.printf("%d. POST[%s]: %s\n", i+1, p->name().c_str(), p->value().c_str());

    if (strcmp(p->name().c_str(), PARAM_WIFI_SSID) == 0) {
      if (p->value().length() > 0) {
        writeFile(SPIFFS, PATH_WIFI_SSID, p->value().c_str());
     } 
    }

    if (strcmp(p->name().c_str(), PARAM_WIFI_PASSWORD) == 0) {
      if (p->value().length() > 0) {
        writeFile(SPIFFS, PATH_WIFI_PASSWORD, p->value().c_str());
     } 
    }

    if (strcmp(p->name().c_str(), PARAM_RTK_LOCATION_METHOD) == 0) {
      if (p->value().length() > 0) {
        writeFile(SPIFFS, PATH_RTK_LOCATION_METHOD, p->value().c_str());
     } 
    }

    if (strcmp(p->name().c_str(), PARAM_RTK_LOCATION_SURVEY_ACCURACY) == 0) {
      if (p->value().length() > 0) {
        writeFile(SPIFFS, PATH_RTK_LOCATION_SURVEY_ACCURACY, p->value().c_str());
     } 
    }

    if (strcmp(p->name().c_str(), PARAM_RTK_LOCATION_LATITUDE) == 0) {
      if (p->value().length() > 0) {
        writeFile(SPIFFS, PATH_RTK_LOCATION_LATITUDE, p->value().c_str());
     } 
    }

    if (strcmp(p->name().c_str(), PARAM_RTK_LOCATION_LONGITUDE) == 0) {
      if (p->value().length() > 0) {
        writeFile(SPIFFS, PATH_RTK_LOCATION_LONGITUDE, p->value().c_str());
     } 
    }

    if (strcmp(p->name().c_str(), PARAM_RTK_LOCATION_HEIGHT) == 0) {
      if (p->value().length() > 0) {
        writeFile(SPIFFS, PATH_RTK_LOCATION_HEIGHT, p->value().c_str());
     } 
    }

  }
  Serial.println(F("Data saved to SPIFFS!"));
  request->send_P(200, "text/html", INDEX_HTML, processor);
}

void actionWipeData(AsyncWebServerRequest *request) {
  Serial.println("ACTION 2!");
  int params = request->params();
  Serial.printf("params: %d\n", params);
  for (int i = 0; i < params; i++) {
    AsyncWebParameter* p = request->getParam(i);
    Serial.printf("%d. POST[%s]: %s\n", i+1, p->name().c_str(), p->value().c_str());
    if (strcmp(p->name().c_str(), "wipe_button") == 0) {
      if (p->value().length() > 0) {
        Serial.printf("wipe command received: %s",p->value().c_str());
        wipeAllSpiffsFiles();
      } 
     }
    } 

  Serial.print(F("Data in SPIFFS was wiped out!"));
  request->send_P(200, "text/html", INDEX_HTML, processor);
}

void actionRebootESP32(AsyncWebServerRequest *request) {
  Serial.println("ACTION 3!");
  request->send_P(200, "text/html", REBOOT_HTML, processor);
  delay(3000);
  ESP.restart();
}


void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

// Replaces placeholder with stored values
String processor(const String& var) {
  if (var == PARAM_WIFI_SSID) {
    String savedSSID = readFile(SPIFFS, PATH_WIFI_SSID);
    return (savedSSID.isEmpty() ? String(PARAM_WIFI_SSID) : savedSSID);
  }
  else if (var == PARAM_WIFI_PASSWORD) {
    String savedPassword = readFile(SPIFFS, PATH_WIFI_PASSWORD);
    return (savedPassword.isEmpty() ? String(PARAM_WIFI_PASSWORD) : "*******");
  }
  else if (var == PARAM_RTK_LOCATION_METHOD) {
    String savedLocationMethod = readFile(SPIFFS, PATH_RTK_LOCATION_METHOD);
    return (savedLocationMethod.isEmpty() ? String(PARAM_RTK_SURVEY_ENABLED) : savedLocationMethod);
  }
  else if (var == PARAM_RTK_LOCATION_SURVEY_ACCURACY) {
    String savedSurveyAccuracy = readFile(SPIFFS, PATH_RTK_LOCATION_SURVEY_ACCURACY);
    return (savedSurveyAccuracy.isEmpty() ? String(PARAM_RTK_LOCATION_SURVEY_ACCURACY) : savedSurveyAccuracy);
  }
  else if (var == PARAM_RTK_LOCATION_LATITUDE) {
    String savedLatitude = readFile(SPIFFS, PATH_RTK_LOCATION_LATITUDE);
    return (savedLatitude.isEmpty() ? String(PARAM_RTK_LOCATION_LATITUDE) : savedLatitude);
  }
  else if (var == PARAM_RTK_LOCATION_LONGITUDE) {
    String savedLongitude = readFile(SPIFFS, PATH_RTK_LOCATION_LONGITUDE);
    return (savedLongitude.isEmpty() ? String(PARAM_RTK_LOCATION_LONGITUDE) : savedLongitude);
  }
  else if (var == PARAM_RTK_LOCATION_HEIGHT) {
    String savedHeight = readFile(SPIFFS, PATH_RTK_LOCATION_HEIGHT);
    return (savedHeight.isEmpty() ? String(PARAM_RTK_LOCATION_HEIGHT) : savedHeight);
  }
  else if (var == "next_addr") {
    String savedSSID = readFile(SPIFFS, PATH_WIFI_SSID);
    String savedPW = readFile(SPIFFS, PATH_WIFI_PASSWORD);
    if (savedSSID.isEmpty() || savedPW.isEmpty()) {
      return String(IP_AP);
    } else {
      String clientAddr = String(DEVICE_NAME);
      clientAddr += ".local";
      return clientAddr;
    }
  }
  else if (var == "next_ssid") {
    String savedSSID = readFile(SPIFFS, PATH_WIFI_SSID);
    return (savedSSID.isEmpty() ? String(SSID_AP) : savedSSID);
  }
  return String();
}

/********************************************************************************
*                             Data storage (SPIFFS)
* ******************************************************************************/
void writeFile(fs::FS &fs, const char* path, const char* message) {
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, "w");
  if (!file) {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
  file.close();
}

String readFile(fs::FS &fs, const char* path) {
  Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "r");

  if (!file || file.isDirectory()) {
    Serial.println("- empty file or failed to open file");
    return String();
  }
  Serial.println("- read from file:");
  String fileContent;

  while (file.available()) {
    fileContent += String((char)file.read());
  }
  file.close();
  Serial.println(fileContent);

  return fileContent;
}

void listAllFiles() {
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
 
  while (file) {
      Serial.print("FILE: ");
      Serial.println(file.name());
      file = root.openNextFile();
  }
  file.close();
  root.close();
}

void wipeAllSpiffsFiles() {
  File root = SPIFFS.open("/");
  File file = root.openNextFile();

  Serial.println(F("Wiping: "));

  while (file) {
    Serial.print("FILE: ");
    Serial.println(file.path());
    SPIFFS.remove(file.path());
    file = root.openNextFile();
  }
  //  file.close();
  listAllFiles();

}
