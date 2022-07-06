#include <Arduino.h>
#include <SPIFFS.h>
#include <RTKBaseManager.h>

using namespace RTKBaseManager;
AsyncWebServer server(80);

#define MAX_SSIDS 10 // Space to scan and remember SSIDs
String seenSSIDs[MAX_SSIDS];

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
  

  listFiles();
  Serial.print(readFile(SPIFFS, PATH_WIFI_SSID));
  WiFi.setHostname(DEVICE_NAME);

  // Check if we have credentials for a available network
  String clientSSID = readFile(SPIFFS, PATH_WIFI_SSID);
  String clientPassword = readFile(SPIFFS, PATH_WIFI_PASSWORD);

  if (!knownNetworkAvailable(clientSSID, seenSSIDs, MAX_SSIDS) || clientPassword.isEmpty()) {
    int foundAPs = scanWiFiAPs(seenSSIDs, MAX_SSIDS);
    for (int i=0; i<foundAPs; i++) {
      Serial.printf("%d %s\n", i+1, seenSSIDs[i].c_str());
    }
    delay(1000);
    Serial.print("Setting soft-AP ... ");
    WiFi.mode(WIFI_AP);
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
