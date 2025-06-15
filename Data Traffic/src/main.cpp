#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char *ssid = "MFI-Master";
const char *password = "MFIRobocon2025";

WebServer server(80);

int gameTime = 0;
int TimerStart = 0;
unsigned long startMillis = 0; // To store the start time

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

  server.on("/gameTimer", []() {
    String response = "/gameTimer?time=" + String(gameTime);
    server.sendHeader("GameTime", response);
    Serial.print("Current game time: " + String(gameTime) + "s\n");
  // TODO: Use these values in your logic
    server.send(200, "text/plain", response);
  });

  server.on("/gameTimer/start", HTTP_GET, []() {
    Serial.print("Timer started\n");
    TimerStart = 1; // Start the timer
    server.send(200, "text/plain", String(gameTime));
  });

  server.on("/gameTimer/stop", HTTP_GET, []() {
    Serial.print("Timer stopped\n");
    TimerStart = 0; // Stop the timer
    startMillis = 0; // Reset start time
    gameTime = 0; // Reset game time
    server.send(200, "text/plain", String(gameTime));
  });

  server.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  if (TimerStart == 1) {
    if (startMillis == 0) {
      startMillis = millis(); // Store the start time only once
      Serial.println("Game started!");
    }
    gameTime = (millis() - startMillis) / 1000; // Elapsed time in seconds
  } else {
    startMillis = 0;// Reset if TimerStart is not 1
  }
  server.handleClient();
}
