#include <Wire.h>
#include <HUSKYLENS.h>
#include <WiFi.h>
#include <WebServer.h>

WebServer server(80);

const char *ssid = "ESP32-ROBOT";
const char *password = "1234567890";

HUSKYLENS huskylens;

int lastX = -1, lastY = -1;

void setup() {
  // Set your desired static IP, gateway, and subnet
  IPAddress local_IP(192, 168, 4, 2);      // Change to your desired static IP
  IPAddress gateway(192, 168, 4, 1);        // Usually your router or AP IP
  IPAddress subnet(255, 255, 255, 0);       // Common subnet mask

  WiFi.config(local_IP, gateway, subnet);
  Serial.begin(115200);
  Wire.begin(21, 22);  // ESP32 default pins
  WiFi.begin(ssid, password);

  Serial.print("Connecting to " + String(ssid) + "...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\nConnected to WiFi!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  delay(100); // Let everything stabilize

  while (!huskylens.begin(Wire)) {
    Serial.println("Begin failed");
    delay(1000);  // Prevent watchdog resets
  }

  delay(1000); // Allow HuskyLens to initialize



  server.on("/camera", HTTP_GET, []() {
      String response = "/camera?x=" + String(lastX) + "&y=" + String(lastY);
      server.sendHeader("Location", response);
      server.send(200, "text/plain", response);
    });
  
  server.on("/", []() {
    server.send(200, "text/plain", "ESP32 is running!");
  });

  server.begin();
  huskylens.writeAlgorithm(ALGORITHM_OBJECT_TRACKING);
}

void loop() {

  if (huskylens.request()) {
    if (huskylens.available()) {
      HUSKYLENSResult result = huskylens.read();

      if (result.xCenter != lastX || result.yCenter != lastY) {
        Serial.printf("Moved: X=%d, Y=%d\n", result.xCenter, result.yCenter);
        lastX = result.xCenter;
        lastY = result.yCenter;
      } else {
        //Serial.println("Same position, maybe not tracking?");
      }
    }
  } else {
  Serial.println("No data from HuskyLens");
  }

  server.handleClient();
}
