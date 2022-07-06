#include <RtkWifiManager.h>

const char DEVICE_NAME[8] = "rtkbase";
// WiFi credentials for AP mode
const char SSID_AP[9] = "RTK-Base";
const char PASSWORD_AP[9] = "12345678";
const char IP_AP[12] = "192.168.4.1";
// WiFi and RTKBase manager SPIFFS parameter and paths
const char PARAM_WIFI_SSID[5] = "ssid";
const char PARAM_WIFI_PASSWORD[9] = "password";
const char PARAM_RTK_LOCATION_METHOD[16] = "location_method";
const char PARAM_RTK_SURVEY_ENABLED[15] = "survey_enabled";
const char PARAM_RTK_COORDS_ENABLED[15] = "coords_enabled";
const char PARAM_RTK_LOCATION_SURVEY_ACCURACY[16] = "survey_accuracy";
const char PARAM_RTK_LOCATION_LONGITUDE[10] = "longitude";
const char PARAM_RTK_LOCATION_LATITUDE[9] = "latitude";
const char PARAM_RTK_LOCATION_HEIGHT[7] = "height";
const char PATH_WIFI_SSID[10] = "/ssid.txt";
const char PATH_WIFI_PASSWORD[14] = "/password.txt";
const char PATH_RTK_LOCATION_METHOD[22] = "/location_method.txt";
const char PATH_RTK_LOCATION_SURVEY_ACCURACY[21] = "/survey_accuracy.txt";
const char PATH_RTK_LOCATION_LONGITUDE[15] = "/longitude.txt";
const char PATH_RTK_LOCATION_LATITUDE[14] = "/latitude.txt";
const char PATH_RTK_LOCATION_HEIGHT[12] = "/height.txt";
// WiFi

AsyncWebServer server(80);

RtkWifiManager::RtkWifiManager() {
  Serial.println("INIT");
}
/********************************************************************************
*                             WiFi
* ******************************************************************************/
bool RtkWifiManager::knownNetworkAvailable(const String& ssid) {
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

int RtkWifiManager::scanWiFiAPs() {
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
*                             Web
* ******************************************************************************/

// Replaces placeholder with stored values
String RtkWifiManager::processor(const String& var) {
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
*                             SPIFFS
* ******************************************************************************/

String RtkWifiManager::readFile(fs::FS &fs, const char* path) {
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

void RtkWifiManager::writeFile(fs::FS &fs, const char* path, const char* message) {
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

void RtkWifiManager::listAllFiles() {
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

void RtkWifiManager::wipeAllSpiffsFiles() {
  File root = SPIFFS.open("/");
  File file = root.openNextFile();

  Serial.println(F("Wiping: "));

  while (file) {
    Serial.print("FILE: ");
    Serial.println(file.path());
    SPIFFS.remove(file.path());
    file = root.openNextFile();
  }
}

void RtkWifiManager::actionUpdateData(AsyncWebServerRequest *request) {
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

/********************************************************************************
*                             Web server
* ******************************************************************************/

void RtkWifiManager::notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void  RtkWifiManager::actionRebootESP32(AsyncWebServerRequest *request) {
  Serial.println("ACTION 3!");
  request->send_P(200, "text/html", REBOOT_HTML, processor);
  delay(3000);
  ESP.restart();
}

void RtkWifiManager::actionWipeData(AsyncWebServerRequest *request) {
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