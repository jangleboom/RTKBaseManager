#include <Arduino.h>
#include <RtkWifiManager.h>



/********************************************************************************
*                             WiFi
* ******************************************************************************/

const uint8_t MAX_SSIDS = 10;                      // Max wifi ssid scan 
String seenSSIDs[MAX_SSIDS];                       // Holds the wifi ssids at your place

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
