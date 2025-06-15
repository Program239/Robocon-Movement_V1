#include <Wire.h>
#include <HUSKYLENS.h>
#include <WiFi.h>
#include <WebServer.h>

WebServer server(80);

const char *ssid = "CameraBot";
const char *password = "1234567890";

const int GreenLED = 25; // GPIO pin for Green LED
const int RedLED = 26;   // GPIO pin for Red LED

String heightMin = "0";
String heightMax = "0";
String widthMin  = "0";
String widthMax  = "0";
String xMin      = "0";
String xMax      = "0";
String yMin      = "0";
String yMax      = "0";

HUSKYLENS huskylens;

int lastX = -1;
int lastY = -1;
int lastHeight = -1;
int lastWidth = -1;

void setup() {

  Serial.begin(115200);
  Wire.begin(21, 22);  // ESP32 default pins
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
  
  delay(100); // Let everything stabilize

  while (!huskylens.begin(Wire)) {
    Serial.println("Begin failed");
    delay(1000);  // Prevent watchdog resets
  }

  delay(1000); // Allow HuskyLens to initialize

  server.on("/camera", HTTP_GET, []() {
      String response = "/camera?x=" + String(lastX) + "&y=" + String(lastY) + "&width=" + String(lastWidth) + "&height=" + String(lastHeight);
      server.sendHeader("Location", response);
      server.send(200, "text/plain", response);
    });
  
  server.on("/", []() {
    server.send(200, "text/plain", "ESP32 is running!");
  });

  server.on("/net", []() {
    int netDirection = server.arg("val").toInt();
    if (netDirection == 1) {
      Serial.println("Net direction: Up");
    } else if (netDirection == -1) {
      Serial.println("Net direction: Down");
    } else {
      Serial.println("Net direction: Stop");
    }
  });

  server.on("/shooter", []() {
    int shooterDirection = server.arg("direction").toInt();
    if (shooterDirection == 1) {
      Serial.println("Shooter direction: Right");
    } else if (shooterDirection == -1) {
      Serial.println("Shooter direction: Left");
    } else {
      Serial.println("Shooter direction: Stop");
    }
  });

  server.on("/motor", []() { 
    int motorState = server.arg("value").toInt();
    if (motorState == 1) {
      Serial.println("Motor state: ON");
      // Add code to turn on the motor
    } else if (motorState == 0) {
      Serial.println("Motor state: OFF");
      // Add code to turn off the motor
    } else {
      Serial.println("Invalid motor state");
    }
  });

  server.on("/shoot", []() {
    Serial.println("Shoot command received");      
  });

  server.on("/set_params", HTTP_GET, []() {
  heightMin = server.arg("heightMin");
  heightMax = server.arg("heightMax");
  widthMin  = server.arg("widthMin");
  widthMax  = server.arg("widthMax");
  xMin      = server.arg("xMin");
  xMax      = server.arg("xMax");
  yMin      = server.arg("yMin");
  yMax      = server.arg("yMax");

  /*// Example: Print to Serial for debugging
  Serial.println("Received parameters:");
  Serial.println("heightMin: " + heightMin);
  Serial.println("heightMax: " + heightMax);
  Serial.println("widthMin: "  + widthMin);
  Serial.println("widthMax: "  + widthMax);
  Serial.println("xMin: "      + xMin);
  Serial.println("xMax: "      + xMax);
  Serial.println("yMin: "      + yMin);
  Serial.println("yMax: "      + yMax);
  */

  // TODO: Use these values in your logic

  server.send(200, "text/plain", "Parameters received");
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
        lastHeight = result.height;
        lastWidth = result.width;
      } else {
        //Serial.println("Same position, maybe not tracking?");
      }
    }
  } else {
  Serial.println("No data from HuskyLens");
  }

  if (lastHeight <= heightMax.toInt() && lastHeight >= heightMin.toInt() && lastWidth <= widthMax.toInt() && lastWidth >= widthMin.toInt()) {
    digitalWrite(GreenLED, HIGH);
  } else {
    digitalWrite(GreenLED, LOW);
  }

  if (lastX <= xMax.toInt() && lastX >= xMin.toInt() && lastY <= yMax.toInt() && lastY >= yMin.toInt()) {
    digitalWrite(RedLED, HIGH);
  } else {
    digitalWrite(RedLED, LOW);  
  }

  server.handleClient();
}
