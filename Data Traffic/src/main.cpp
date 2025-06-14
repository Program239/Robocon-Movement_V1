#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char *ssid = "MFI-Master";
const char *password = "1234567890";

WebServer server(80);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  Serial.print("Initiating WiFi AP: ");
  Serial.print(ssid);
  while (WiFi.softAPgetStationNum() == 0) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("\nWiFi AP started successfully");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  server.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
}
