#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>

const char *ssid = "MFI-Master";
const char *password = "MFIRobocon2025";

WebServer server(80);

int gameTime = 0;
int TimerStart = 0;
unsigned long startMillis = 0; // To store the start time

void requestSlaveCoords() {
  HTTPClient http;
  String url = "http://192.168.4.101/camera"; // Replace with the slave's IP
  http.begin(url);
  int httpCode = http.GET();
  if (httpCode == 200) {
    String payload = http.getString();
    Serial.println("Received from slave: " + payload);

    // Parse parameters from payload
    int x = 0, y = 0, width = 0, height = 0;
    int idx;

    idx = payload.indexOf("x=");
    if (idx != -1) x = payload.substring(idx + 2, payload.indexOf('&', idx)).toInt();

    idx = payload.indexOf("y=");
    if (idx != -1) y = payload.substring(idx + 2, payload.indexOf('&', idx)).toInt();

    idx = payload.indexOf("width=");
    if (idx != -1) width = payload.substring(idx + 6, payload.indexOf('&', idx)).toInt();

    idx = payload.indexOf("height=");
    if (idx != -1) height = payload.substring(idx + 7).toInt();

    Serial.printf("Parsed: x=%d, y=%d, width=%d, height=%d\n", x, y, width, height);

    String response = "/camera?x=" + String(x) + "&y=" + String(y) + "&width=" + String(width) + "&height=" + String(height);
    server.sendHeader("Location", response);
    server.send(200, "text/plain", response);

    // Now you can use x, y, width, height as needed
  } else {
    Serial.println("Failed to get coords from slave");
  }
  http.end();
}

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

  server.on("/camera", HTTP_GET, []() {
    Serial.print("Camera request received\n");
    requestSlaveCoords();
    server.send(200, "text/plain", "Camera request processed");
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
