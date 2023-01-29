#include <RTKBaseManager.h>

/*
=================================================================================
                                WiFi
=================================================================================
*/

#pragma region: WIFI

bool RTKBaseManager::setupWiFi(AsyncWebServer* server)
{
  bool success = false;

  String deviceName = getDeviceName(DEVICE_TYPE);

  WiFi.softAPdisconnect(true); // AP  if connected
  WiFi.disconnect(true);       // STA if connected
  WiFi.setHostname(deviceName.c_str());

  // Check if we have credentials for a available network
  String lastSSID = readFile(LittleFS, getPath(PARAM_WIFI_SSID).c_str());
  String lastPassword = readFile(LittleFS, getPath(PARAM_WIFI_PASSWORD).c_str());

  if (lastSSID.isEmpty() || lastPassword.isEmpty() ) 
  {
    setupAPMode(deviceName.c_str(), AP_PASSWORD);
    delay(500);
    success = true;
  } 
  else
  {
    while (! savedNetworkAvailable(lastSSID)) 
    {
      DBG.print(F("Waiting for HotSpot "));
      DBG.print(lastSSID);
      DBG.println(F(" to appear..."));
      vTaskDelay(1000/portTICK_RATE_MS);
    }
    success = setupStationMode(lastSSID.c_str(), lastPassword.c_str(), deviceName.c_str());
    delay(500);
  }

  startServer(server);

  return success;
}

bool RTKBaseManager::setupStationMode(const char* ssid, const char* password, const char* deviceName) 
{
  bool success = false;

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, password);
  WiFi.waitForConnectResult();

  if (! WiFi.isConnected() ) 
  {
    DBG.println("WiFi Failed! Reboot in 10 s as AP!");
    success = false;
  }
  else 
  {
    DBG.print(F("WiFi connected to SSID: "));
    DBG.println(WiFi.SSID());

    success = true;
  }

  

  return success;
}

void RTKBaseManager::setupAPMode(const char* apSsid, const char* apPassword) 
{
  DBG.print("Setting soft-AP ... ");
  WiFi.mode(WIFI_AP);
  bool result = WiFi.softAP(apSsid, apPassword);
  DBG.println(result ? "Ready" : "Failed!");
  DBG.print("Access point started: ");
  DBG.println(apSsid);
  DBG.print("IP address: ");
  DBG.println(WiFi.softAPIP());
  DBG.print(F("AP Password: "));
  DBG.println(AP_PASSWORD);
}

bool RTKBaseManager::checkConnectionToWifiStation() 
{ 
  bool isConnectedToStation = WiFi.isConnected();

  if (WiFi.getMode() == WIFI_MODE_STA)
  {
    if ( ! isConnectedToStation) 
    {
      WiFi.disconnect();
      DBG.println("Try reconnect to access point.");
      isConnectedToStation = WiFi.reconnect();
    } 
    else 
    {
      DBG.print(F("WiFi connected to SSID: "));
      DBG.println(WiFi.SSID());
      DBG.print(F("WiFi client name: "));
      DBG.println(WiFi.getHostname());
      DBG.print(F("IP Address: "));
      DBG.println(WiFi.localIP());

      String deviceName = getDeviceName(DEVICE_TYPE);

      if ( ! MDNS.begin(deviceName.c_str()) ) 
      {
        DBG.println("Error starting mDNS, use local IP instead!");
      } 
      else 
      {
        DBG.print(F("Starting mDNS, find me under <http://"));
        DBG.print(deviceName);
        DBG.println(F(".local>"));
      }
    }
  }

  return isConnectedToStation;
}

bool RTKBaseManager::savedNetworkAvailable(const String& ssid) 
{
  if (ssid.isEmpty()) return false;

  uint8_t nNetworks = (uint8_t) WiFi.scanNetworks();
  DBG.print(nNetworks);  
  DBG.println(F(" networks found."));

  for (uint8_t i=0; i<nNetworks; i++) 
  {
    if (ssid.equals(String(WiFi.SSID(i)))) 
    {
      DBG.print(F("A known network with SSID found: ")); 
      DBG.print(WiFi.SSID(i));
      DBG.print(F(" (")); 
      DBG.print(WiFi.RSSI(i)); 
      DBG.println(F(" dB), connecting..."));
      return true;
    }
  }
  return false;
}

/*
=================================================================================
                                Web server
=================================================================================
*/
void RTKBaseManager::startServer(AsyncWebServer *server) 
{
  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) 
  {
    request->send_P(200, "text/html", INDEX_HTML, processor);
  });

  server->on("/actionUpdateData", HTTP_POST, actionUpdateData);
  server->on("/actionWipeData", HTTP_POST, actionWipeData);
  server->on("/actionRebootESP32", HTTP_POST, actionRebootESP32);

  server->onNotFound(notFound);
  server->begin();
}
  
void RTKBaseManager::notFound(AsyncWebServerRequest *request) 
{
  request->send(404, "text/plain", "Not found");
}

void RTKBaseManager::actionRebootESP32(AsyncWebServerRequest *request) 
{
  DBG.println("ACTION actionRebootESP32!");
  request->send_P(200, "text/html", REBOOT_HTML, RTKBaseManager::processor);
  delay(3000);
  ESP.restart();
}

void RTKBaseManager::actionWipeData(AsyncWebServerRequest *request) 
{
  DBG.println(F("ACTION actionWipeData!"));

  int params = request->params();
  DBG.print(F("params: "));
  DBG.println(params);

  for (int i = 0; i < params; i++) 
  {
    AsyncWebParameter* p = request->getParam(i);
    DBG.printf("%d. POST[%s]: %s\n", i+1, p->name().c_str(), p->value().c_str());
    if (strcmp(p->name().c_str(), "wipe_button") == 0) 
    {
      if (p->value().length() > 0) 
      {
        DBG.printf("wipe command received: %s\n",p->value().c_str());
        wipeLittleFSFiles();
      } 
     }
    } 
  request->send_P(200, "text/html", INDEX_HTML, processor);
}

void RTKBaseManager::actionUpdateData(AsyncWebServerRequest *request) 
{

  DBG.println("ACTION: actionUpdateData!");

  int params = request->params();
  for (int i = 0; i < params; i++) 
  {
    AsyncWebParameter* p = request->getParam(i);
    DBG.printf("%d. POST[%s]: %s\n", i+1, p->name().c_str(), p->value().c_str());

    if (strcmp(p->name().c_str(), PARAM_DEVICE_NAME) == 0) 
    {
      if (p->value().length() > 0) 
      {
        String newName = p->value();
        newName.toLowerCase();
        writeFile(LittleFS, getPath(PARAM_DEVICE_NAME).c_str(), newName.c_str());
      } 
    }

    if (strcmp(p->name().c_str(), PARAM_WIFI_SSID) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, getPath(PARAM_WIFI_SSID).c_str(), p->value().c_str());
      } 
    }

    if (strcmp(p->name().c_str(), PARAM_WIFI_PASSWORD) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, getPath(PARAM_WIFI_PASSWORD).c_str(), p->value().c_str());
      } 
    }

    if (strcmp(p->name().c_str(), PARAM_RTK_CASTER_HOST) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, getPath(PARAM_RTK_CASTER_HOST).c_str(), p->value().c_str());
      } 
    }

    if (strcmp(p->name().c_str(), PARAM_RTK_CASTER_PORT) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, getPath(PARAM_RTK_CASTER_PORT).c_str(), p->value().c_str());
      } 
    }

    if (strcmp(p->name().c_str(), PARAM_RTK_MOINT_POINT) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, getPath(PARAM_RTK_MOINT_POINT).c_str(), p->value().c_str());
      } 
    }

    if (strcmp(p->name().c_str(), PARAM_RTK_MOINT_POINT_PW) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, getPath(PARAM_RTK_MOINT_POINT_PW).c_str(), p->value().c_str());
      } 
    }

    if (strcmp(p->name().c_str(), PARAM_RTK_LOCATION_METHOD) == 0) 
    {
      if (p->value().length() > 0) 
      {
        String pValue = p->value();
        if (pValue.equals("survey_enabled")) 
        {
          // Clear old coords values
          LittleFS.remove(getPath(PARAM_RTK_LOCATION_ALTITUDE).c_str());
          LittleFS.remove(getPath(PARAM_RTK_LOCATION_LATITUDE).c_str());
          LittleFS.remove(getPath(PARAM_RTK_LOCATION_LONGITUDE).c_str());
          LittleFS.remove(getPath(PARAM_RTK_LOCATION_COORD_ACCURACY).c_str());
        }
        writeFile(LittleFS, getPath(PARAM_RTK_LOCATION_METHOD).c_str(), p->value().c_str());
     } 
    }

    if (strcmp(p->name().c_str(), PARAM_RTK_LOCATION_SURVEY_ACCURACY) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, getPath(PARAM_RTK_LOCATION_SURVEY_ACCURACY).c_str(), p->value().c_str());
      } 
    }

    if (strcmp(p->name().c_str(), PARAM_RTK_LOCATION_COORD_ACCURACY) == 0) 
    {
      if (p->value().length() > 0)
      {
        writeFile(LittleFS, getPath(PARAM_RTK_LOCATION_COORD_ACCURACY).c_str(), p->value().c_str());
      } 
    }

    if (strcmp(p->name().c_str(), PARAM_RTK_LOCATION_LATITUDE) == 0) 
    {
      if (p->value().length() > 0) 
      {
        String deconstructedValAsCSV = getDeconstructedCoordAsCSV(p->value());
        writeFile(LittleFS, getPath(PARAM_RTK_LOCATION_LATITUDE).c_str(), deconstructedValAsCSV.c_str());
     } 
    }

    if (strcmp(p->name().c_str(), PARAM_RTK_LOCATION_LONGITUDE) == 0) 
    {
      if (p->value().length() > 0) 
      {
        String deconstructedValAsCSV = getDeconstructedCoordAsCSV(p->value());
        writeFile(LittleFS, getPath(PARAM_RTK_LOCATION_LONGITUDE).c_str(), deconstructedValAsCSV.c_str());
     } 
    }

    if (strcmp(p->name().c_str(), PARAM_RTK_LOCATION_ALTITUDE) == 0) 
    {
      if (p->value().length() > 0) 
      {
        String deconstructedAltAsCSV = getDeconstructedAltAsCSV(p->value());
        // int32_t elipsoid = String(p->value()).toInt()*1000;
        writeFile(LittleFS, getPath(PARAM_RTK_LOCATION_ALTITUDE).c_str(), deconstructedAltAsCSV.c_str());
      } 
    }
  }

  DBG.println(F("Data saved to LittleFS!"));
  request->send_P(200, "text/html", INDEX_HTML, RTKBaseManager::processor);
}

String RTKBaseManager::getDeconstructedCoordAsCSV(const String& doubleStr) 
{
  double dVal = doubleStr.toDouble();
  int32_t lowerPrec = getLowerPrecisionCoordFromDouble(dVal);
  int8_t highPrec = getHighPrecisionCoordFromDouble(dVal);
  String deconstructedCSV = String(lowerPrec) + SEP + String(highPrec);
  return deconstructedCSV;
}

String RTKBaseManager::getDeconstructedAltAsCSV(const String& floatStr) 
{
  float alt = floatStr.toFloat();
  int32_t lowerPrec = getLowerPrecisionIntAltitudeFromFloat(alt);
  int8_t highPrec = getHigherPrecisionIntAltitudeFromFloat(alt);
  String deconstructedCSV = String(lowerPrec) + SEP + String(highPrec);
  return deconstructedCSV;
}

String RTKBaseManager::getFloatingPointStringFromCSV(const String& csvStr, int precision) 
{ 
  if (csvStr.isEmpty()) return String();
  int32_t lowerPrec = (int32_t)getValueAsStringFromCSV(csvStr, SEP, 0).toInt();
  int8_t highPrec = (int8_t)getValueAsStringFromCSV(csvStr, SEP, 1).toInt();
  String reconstructedValStr = "";
  if (precision > ALT_PRECISION) 
  {
    double reconstructedVal = getDoubleCoordFromIntegerParts(lowerPrec, highPrec);
    reconstructedValStr = String(reconstructedVal, precision);
  } else 
  {
    float reconstructedVal = getFloatAltFromIntegerParts(lowerPrec, highPrec);
    reconstructedValStr = String(reconstructedVal, precision);
  }
  
  return reconstructedValStr;
}

// Replaces placeholder with stored values
String RTKBaseManager::processor(const String& var) 
{
  if (var == PARAM_DEVICE_NAME) 
  {
    String savedDeviceName = readFile(LittleFS, getPath(PARAM_DEVICE_NAME).c_str());
    return (savedDeviceName.isEmpty() ? getDeviceName(DEVICE_TYPE) : savedDeviceName);
  }

  else if (var == PARAM_WIFI_SSID) 
  {
    String savedSSID = readFile(LittleFS, getPath(PARAM_WIFI_SSID).c_str());
    return (savedSSID.isEmpty() ? "" : savedSSID);
  }
  else if (var == PARAM_WIFI_PASSWORD) 
  {
    String savedPassword = readFile(LittleFS, getPath(PARAM_WIFI_PASSWORD).c_str());
    return (savedPassword.isEmpty() ? "" : "*******");
  }

  else if (var == PARAM_RTK_CASTER_HOST) 
  {
    String savedCaster = readFile(LittleFS, getPath(PARAM_RTK_CASTER_HOST).c_str());
    return (savedCaster.isEmpty() ? "" : savedCaster);
  }

  else if (var == PARAM_RTK_CASTER_PORT) 
  {
    String savedPort = readFile(LittleFS, getPath(PARAM_RTK_CASTER_PORT).c_str());
    return (savedPort.isEmpty() ? "" : savedPort);
  }

  else if (var == PARAM_RTK_MOINT_POINT) 
  {
    String savedMointPoint = readFile(LittleFS, getPath(PARAM_RTK_MOINT_POINT).c_str());
    return (savedMointPoint.isEmpty() ? "" : savedMointPoint);
  }

  else if (var == PARAM_RTK_MOINT_POINT_PW) 
  {
    String savedMointPointPW = readFile(LittleFS, getPath(PARAM_RTK_MOINT_POINT_PW).c_str());
    return (savedMointPointPW.isEmpty() ? "" : "*******");
  }

  else if (var == PARAM_RTK_LOCATION_METHOD) 
  {
    String savedLocationMethod = readFile(LittleFS, getPath(PARAM_RTK_LOCATION_METHOD).c_str());
    return (savedLocationMethod.isEmpty() ? "" : savedLocationMethod);
  }

  else if (var == PARAM_RTK_LOCATION_SURVEY_ACCURACY) 
  {
    String savedSurveyAccuracy = readFile(LittleFS, getPath(PARAM_RTK_LOCATION_SURVEY_ACCURACY).c_str());
    return (savedSurveyAccuracy.isEmpty() ? "" : savedSurveyAccuracy);
  }

  else if (var == PARAM_RTK_LOCATION_COORD_ACCURACY) 
  {
    String savedCoordAccuracy = readFile(LittleFS, getPath(PARAM_RTK_LOCATION_COORD_ACCURACY).c_str());
    return (savedCoordAccuracy.isEmpty() ? "" : savedCoordAccuracy);
  }

  else if (var == PARAM_RTK_LOCATION_LATITUDE) 
  {
    String savedLatitude = readFile(LittleFS, getPath(PARAM_RTK_LOCATION_LATITUDE).c_str());
    String savedLatitudeStr = getFloatingPointStringFromCSV(savedLatitude, COORD_PRECISION);
    return (savedLatitude.isEmpty() ? "" : savedLatitudeStr);
  }

  else if (var == PARAM_RTK_LOCATION_LONGITUDE) 
  {
    String savedLongitude = readFile(LittleFS, getPath(PARAM_RTK_LOCATION_LONGITUDE).c_str());
    String savedLongitudeStr = getFloatingPointStringFromCSV(savedLongitude, COORD_PRECISION);
    return (savedLongitude.isEmpty() ? "" : savedLongitudeStr);
  }

  else if (var == PARAM_RTK_LOCATION_ALTITUDE) 
  {
    String savedAltitude = readFile(LittleFS, getPath(PARAM_RTK_LOCATION_ALTITUDE).c_str());
    String savedAltitudeStr = getFloatingPointStringFromCSV(savedAltitude, ALT_PRECISION);
    // float altitude = savedAltitude.toFloat() / 1000.0;
    return (savedAltitude.isEmpty() ? "" : savedAltitudeStr);
  }
  else if (var == "next_addr") 
  {
    String savedSSID = readFile(LittleFS, getPath(PARAM_WIFI_SSID).c_str());
    String savedPW = readFile(LittleFS, getPath(PARAM_WIFI_PASSWORD).c_str());
    if (savedSSID.isEmpty() || savedPW.isEmpty()) 
    {
      return String(IP_AP);
    } 
    else 
    {
      String clientAddr = getDeviceName(DEVICE_TYPE);
      clientAddr += ".local";

      return clientAddr;
    }
  }
  else if (var == "next_ssid") 
  {
    String savedSSID = readFile(LittleFS, getPath(PARAM_WIFI_SSID).c_str());
    String apHostName = getDeviceName(DEVICE_TYPE);
    return (savedSSID.isEmpty() ? apHostName : savedSSID);
  }
  return String();
}

/*
=================================================================================
                                LittleFS
=================================================================================
*/

bool RTKBaseManager::setupLittleFS() 
{
  bool isMounted = false; 

  if ( !LittleFS.begin() ) 
  {
    Serial.println(F("An Error has occurred while mounting LittleFS"));
    return isMounted;
  } 
  else
  {
    DBG.println("LittleFS mounted");
    isMounted = true;
  }

  return isMounted;
}

bool RTKBaseManager::formatLittleFS()
{
  DBG.println("Formatting file system, please wait...");
  bool formatted = LittleFS.format();
 
  if (formatted) 
  {
    DBG.println("\n\nSuccess formatting");
  }
  else
  {
    DBG.println("\n\nError formatting");
  }

  return formatted;
}

String RTKBaseManager::readFile(fs::FS &fs, const char* path) 
{
  String fileContent;

  DBG.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, FILE_READ);

  if ( !file || file.isDirectory() ) 
  {
    DBG.println("- empty file or failed to open file");
    return String();
  }

  DBG.println("- read from file:");

  while ( file.available() ) 
  {
    fileContent += String( (char) file.read() );
  }
  file.close();
  DBG.println(fileContent);

  return fileContent;
}

bool RTKBaseManager::writeFile(fs::FS &fs, const char* path, const char* message) 
{ 
  bool success = false;
  DBG.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) 
  {
    DBG.println("- failed to open file for writing");
    return success;
  }
  if (file.print(message)) 
  {
    DBG.println("- file written");
    success = true;
  } else 
  {
    DBG.println("- write failed");
    success = false;
  }
  file.close();

  return success;
}

void RTKBaseManager::listFiles() 
{
  File root = LittleFS.open("/", FILE_READ);
  File file = root.openNextFile();
 
  while (file) 
  {
    DBG.print("FILE: ");
    DBG.println(file.name());
    file = root.openNextFile();
  }
  file.close();
  root.close();
}

void RTKBaseManager::clearPath(const char* path)
{
  if ( LittleFS.exists(path) )
  {
    if ( ! LittleFS.remove(path) )
    {
        assert("Failed to remove LittleFS path.");
    }
  }
}

void RTKBaseManager::wipeLittleFSFiles() 
{
  File root = LittleFS.open("/", FILE_WRITE);
  File file = root.openNextFile();

  DBG.println(F("Wiping: "));

  while (file) 
  {
      const char* pathStr = strdup(file.path());
      file.close();
      LittleFS.remove(pathStr);
      file = root.openNextFile();
  }

  DBG.println(F("Data in LittleFS wiped out!"));
}

String RTKBaseManager::getPath(const char* fileName)
{
  String path = "/";
  path += fileName;
  path += ".txt";

  return path;
}

bool RTKBaseManager::getLocationFromLittleFS(location_t* location, const char* pathLat, const char* pathLon, const char* pathAlt, const char* pathAcc) 
{
  bool success = false;
  String latStr = readFile(LittleFS, pathLat);
  String lonStr = readFile(LittleFS, pathLon);
  String altStr = readFile(LittleFS, pathAlt);
  String accStr = readFile(LittleFS, pathAcc);

  if (!latStr.isEmpty() && !lonStr.isEmpty() && !altStr.isEmpty()) 
  {
    location->lat =  (int32_t)getValueAsStringFromCSV(latStr, SEP, LOW_PREC_IDX).toInt();
    location->lat_hp = (int8_t)getValueAsStringFromCSV(latStr, SEP, HIGH_PREC_IDX).toInt();
    location->lon =  (int32_t)getValueAsStringFromCSV(lonStr, SEP, LOW_PREC_IDX).toInt();
    location->lon_hp = (int8_t)getValueAsStringFromCSV(lonStr, SEP, HIGH_PREC_IDX).toInt();
    location->alt =  (int32_t)getValueAsStringFromCSV(altStr, SEP, LOW_PREC_IDX).toInt();
    location->alt_hp = (int8_t)getValueAsStringFromCSV(altStr, SEP, HIGH_PREC_IDX).toInt();
    location->acc = accStr.toFloat();
    success = true;
  } 
  
  return success;
}

void RTKBaseManager::printLocation(location_t* location) 
{
  DBG.print(F("Lat: ")); DBG.print(location->lat, DEC); DBG.print(SEP); DBG.println(location->lat_hp, DEC);
  DBG.print(F("Lon: ")); DBG.print(location->lon, DEC); DBG.print(SEP); DBG.println(location->lon_hp, DEC);
  DBG.print(F("Alt: ")); DBG.print(location->alt, DEC); DBG.print(SEP); DBG.println(location->alt_hp, DEC);
  DBG.print(F("Acc: ")); DBG.println(location->acc);
}
/*** Help Functions ***/
// TODO: make this privat

int32_t RTKBaseManager::getLowerPrecisionCoordFromDouble(double input) 
{
 // We work with 7 + 2 post dot places, (max 0.11 mm accuracy)
  double intp, fracp;
  fracp = modf(input, &intp);
  String output = String((int)intp);
  String fracpStr = (fracp < 0) ? String(abs(fracp), 9) : String(fracp, 9);
  output += fracpStr.substring(2,9);
  return atoi(output.c_str());
}

int8_t RTKBaseManager::getHighPrecisionCoordFromDouble(double input) 
{
  // We work with 7 + 2 post dot places, (max 0.11 mm accuracy)
  double intp, fracp;
  
  fracp = abs(modf(input, &intp));
  String fracpStr = String(fracp, 9);
  String outputStr = fracpStr.substring(9, 11);
  int8_t output = outputStr.toInt();
  DBG.println("getHighPrecisionCoordFromDouble: ");
  DBG.print("intp: ");
  DBG.print(intp, 9);  
  DBG.print(", fracp: ");
  DBG.print(fracp, 9);
  DBG.print(", outputStr: ");
  DBG.println(outputStr.c_str());
  return output;
}

double RTKBaseManager::getDoubleCoordFromIntegerParts(int32_t coord, int8_t coordHp) 
{
  double d_coord;
  d_coord = (double)coord * 1e-7;
  d_coord += (double)coordHp * 1e-9;

  return d_coord;
}

float RTKBaseManager::getFloatAltFromIntegerParts(int32_t alt, int8_t altHp) 
{
  float f_alt;
  f_alt = (float)alt * 1e-3;      // mm to m
  f_alt += (float)altHp * 1e-4;   // add the 0.1 mm part

  // Works for height above elipsoid and mean sea level too
  
  // Calculate the altitude in mm * 10^-1
  f_alt = (alt * 10) + altHp;
  // Now convert to m
  f_alt = f_alt / 10000.0; // Convert from mm * 10^-1 to m

  return f_alt;
}

String RTKBaseManager::getValueAsStringFromCSV(const String &data, char separator, int col)
{
  int i = data.indexOf(separator);
  int len = data.length();
  String result = (col==0) ? data.substring(0, i) : data.substring(i+1, len);
  return  result;
}

void RTKBaseManager::setLocationMethodCoords() 
{
  writeFile(LittleFS, getPath(PARAM_RTK_LOCATION_METHOD).c_str(), "coords_enabled");
}

void RTKBaseManager::setLocationMethodSurvey() 
{
  writeFile(LittleFS, getPath(PARAM_RTK_LOCATION_METHOD).c_str(), "survey_enabled");
}

int32_t RTKBaseManager::getLowerPrecisionIntAltitudeFromFloat(float alt) 
{
  // Get cm part (cut mm digit)
  return (int32_t)(alt * 1000.0); // cm accuracy
}

int8_t RTKBaseManager::getHigherPrecisionIntAltitudeFromFloat(float alt) 
{
  // Get the 0.1 mm part
  int8_t alt_100nm = (((int32_t)(alt * 10000)) % 10);
  return alt_100nm;
}

int8_t RTKBaseManager::getDigitsCount(int32_t num)
{
   return (1 + log10( num ));
}

float RTKBaseManager::getFloatAltitudeFromInt(int32_t alt, int8_t altHp) 
{
    float f_alt;
    // Calculate the height above elipsoid in mm * 10^-1
    f_alt = (alt * 10) + altHp;
    // Now convert to m
    f_alt /= 10000.0; // Convert from mm * 10^-1 to m

    return f_alt;
}

String RTKBaseManager::getDeviceName(const String& prefix) 
  {
    String deviceName = readFile(LittleFS, getPath(PARAM_DEVICE_NAME).c_str());

    if (deviceName.isEmpty())
    {
      String suffix = String(getChipId(), HEX);

      deviceName += prefix;
      deviceName += "-";
      deviceName += suffix;
    }

   return deviceName; 
  }

uint32_t RTKBaseManager::getChipId() 
  {
    uint32_t chipId = 0;

    for(int i=0; i<17; i=i+8) {
      chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
    }
      return chipId;
  }