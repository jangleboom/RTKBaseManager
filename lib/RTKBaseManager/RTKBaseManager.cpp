#include <RTKBaseManager.h>

/********************************************************************************
*                             WiFi
* ******************************************************************************/

void RTKBaseManager::setupStationMode(const char* ssid, const char* password, const char* deviceName) {
  WiFi.mode(WIFI_STA);
  WiFi.begin( ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    // TODO:  - count reboots and stop after 3 times (save in SPIFFS)
    //        - display state
    Serial.println("WiFi Failed! Reboot in 10 s as AP!");
    delay(10000);
    ESP.restart();
  }
  Serial.println();

  if (!MDNS.begin(deviceName)) {
      Serial.println("Error starting mDNS, use local IP instead!");
  } else {
    Serial.printf("Starting mDNS, find me under <http://www.%s.local>\n", DEVICE_NAME);
  }

  Serial.print("Wifi client started: ");
  Serial.println(WiFi.getHostname());
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void RTKBaseManager::setupAPMode(const char* apSsid, const char* apPassword) {
    Serial.print("Setting soft-AP ... ");
    WiFi.mode(WIFI_AP);
    Serial.println(WiFi.softAP(apSsid, apPassword) ? "Ready" : "Failed!");
    Serial.print("Access point started: ");
    Serial.println(AP_SSID);
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());
}

bool RTKBaseManager::savedNetworkAvailable(const String& ssid) {
  if (ssid.isEmpty()) return false;

  uint8_t nNetworks = (uint8_t) WiFi.scanNetworks();
  Serial.print(nNetworks);  Serial.println(F(" networks found."));
    for (uint8_t i=0; i<nNetworks; i++) {
    if (ssid.equals(String(WiFi.SSID(i)))) {
      Serial.print(F("A known network with SSID found: ")); 
      Serial.print(WiFi.SSID(i));
      Serial.print(F(" (")); 
      Serial.print(WiFi.RSSI(i)); 
      Serial.println(F(" dB), connecting..."));
      return true;
    }
  }
  return false;
}

/********************************************************************************
*                             Web server
* ******************************************************************************/

void RTKBaseManager::startServer(AsyncWebServer *server) {
  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", INDEX_HTML, processor);
  });

  server->on("/actionUpdateData", HTTP_POST, actionUpdateData);
  server->on("/actionWipeData", HTTP_POST, actionWipeData);
  server->on("/actionRebootESP32", HTTP_POST, actionRebootESP32);

  server->onNotFound(notFound);
  server->begin();
}
  
void RTKBaseManager::notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void RTKBaseManager::actionRebootESP32(AsyncWebServerRequest *request) {
  Serial.println("ACTION 3!");
  request->send_P(200, "text/html", REBOOT_HTML, RTKBaseManager::processor);
  delay(3000);
  ESP.restart();
}

void RTKBaseManager::actionWipeData(AsyncWebServerRequest *request) {
  Serial.println("ACTION 2!");
  int params = request->params();
  Serial.printf("params: %d\n", params);
  for (int i = 0; i < params; i++) {
    AsyncWebParameter* p = request->getParam(i);
    Serial.printf("%d. POST[%s]: %s\n", i+1, p->name().c_str(), p->value().c_str());
    if (strcmp(p->name().c_str(), "wipe_button") == 0) {
      if (p->value().length() > 0) {
        Serial.printf("wipe command received: %s",p->value().c_str());
        wipeSpiffsFiles();
      } 
     }
    } 

  Serial.print(F("Data in SPIFFS was wiped out!"));
  request->send_P(200, "text/html", INDEX_HTML, processor);
}

void RTKBaseManager::actionUpdateData(AsyncWebServerRequest *request) {
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
  request->send_P(200, "text/html", INDEX_HTML, RTKBaseManager::processor);
}

// Replaces placeholder with stored values
String RTKBaseManager::processor(const String& var) {
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
    return (savedSSID.isEmpty() ? String(AP_SSID) : savedSSID);
  }
  return String();
}

/********************************************************************************
*                             SPIFFS
* ******************************************************************************/

void RTKBaseManager::setupSPIFFS(void) {
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
}

String RTKBaseManager::readFile(fs::FS &fs, const char* path) {
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

void RTKBaseManager::writeFile(fs::FS &fs, const char* path, const char* message) {
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
void RTKBaseManager::listFiles() {
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

void RTKBaseManager::wipeSpiffsFiles() {
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

uint8_t RTKBaseManager::getDecimalPlacesFromCoord(int32_t num) {
  if ((num % 100) > 0) {
    return 3;
  } else if ((num % 10) > 0) {
     return 2;
  } else  {
    return 1;
  }
}

uint32_t RTKBaseManager::getIntegerFromDouble(double input) {
 // We work with 7 + 2 post dot places, (max 0.11 mm accuracy)
  double intp, fracp;
  fracp = modf(input,&intp);
  // Serial.printf("%.9f %.9f\n", intp,fracp);
  String output = String((int)intp);
  // Serial.print("(int)intp: "); Serial.println(output.c_str());
  String fracpStr = String(abs(fracp),9);
  // Serial.print("String(postDot): "); Serial.println(fracpStr.c_str());
  output += fracpStr.substring(2,9);
  // Serial.print("output: "); Serial.println(output);
  return atol(output.c_str());
}

uint8_t RTKBaseManager::getPrecisionExtensionFromDouble(double input) {
  // We work with 7 + 2 post dot places, (max 0.11 mm accuracy)
  double intp, fracp;
  fracp = abs(modf(input,&intp));
  String fracpStr = String(fracp,9);
  String output = fracpStr.substring(9,11);
  return atol(output.c_str());
}