#include <Wire.h>
#include <Arduino.h>
#include <HUSKYLENS.h>
#include <WiFi.h>
#include <WebServer.h>

// Pin definitions (same as yours)
const int Shooter = 23;
const int Tolak1 = 18;
const int Tolak2 = 19;
const int rightTurn = 25;
const int leftTurn = 26;
const int rightNet = 27;
const int leftNet = 14;
const int lowerLimitpusher = 34;
const int upperLimitpusher = 35;
const int upperLimitnet = 33;
const int lowerLimitnet = 32;

String slider = "0";
int turnDirection = 0;
int shooterDirection = 0;
int netUPDOWN = 0;
int shootSpeed = 0; 
int tolakDirection = 0;
int netDirection = 0;
bool netMotorRightActive = false;
bool netMotorLeftActive = false;
bool tolakExtendActive = false;
bool tolakRetractActive = false;

WebServer server(80);

const char *ssid = "CameraBot";
const char *ssid = "CameraBot";
const char *password = "1234567890";

const int GreenLED = 5; // GPIO pin for Green LED
const int RedLED = 4;   // GPIO pin for Red LED

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

void setMotor(int pinA, int pinB, float speed) {
  int pwm = abs(speed * 255.0);
  pwm = constrain(pwm, 0, 255);
}

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
  Serial.println("\nWiFi AP started successfully");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  delay(100); // Let everything stabilize

  
  while (!huskylens.begin(Wire)) {
    Serial.println("Begin failed");
    delay(1000);  // Prevent watchdog resets
  }

  delay(1000); // Allow HuskyLens to initialize

  // Initialize pins
  pinMode(rightTurn, OUTPUT);
  pinMode(leftTurn, OUTPUT);
  pinMode(Shooter, OUTPUT);
  pinMode(Tolak1, OUTPUT);
  pinMode(Tolak2, OUTPUT);
  pinMode(rightNet, OUTPUT);
  pinMode(leftNet, OUTPUT);
  pinMode(upperLimitnet, INPUT_PULLUP);
  pinMode(lowerLimitnet, INPUT_PULLUP);
  pinMode(upperLimitpusher, INPUT_PULLUP);
  pinMode(lowerLimitpusher, INPUT_PULLUP);
  digitalWrite(Shooter, LOW);
  digitalWrite(Tolak1, LOW);
  digitalWrite(Tolak2, LOW);
  digitalWrite(rightTurn, LOW);
  digitalWrite(leftTurn, LOW);
  digitalWrite(rightNet, LOW);
  digitalWrite(leftNet, LOW);
  pinMode(GreenLED, OUTPUT);
  pinMode(RedLED, OUTPUT);
  

  server.on("/camera", HTTP_GET, []() {
      String response = "/camera?x=" + String(lastX) + "&y=" + String(lastY) + "&width=" + String(lastWidth) + "&height=" + String(lastHeight);
      server.sendHeader("Location", response);
      server.send(200, "text/plain", response);
    });

  server.on("/", []() {
    server.send(200, "text/plain", "ESP32 is running!");
  });

  server.on("/shooter", []() { //turntable
    int shooterDirection = server.arg("direction").toInt();
    if (shooterDirection == 1) {
      Serial.println("Shooter direction: Right");
      turnDirection = 1;
    } else if (shooterDirection == -1) {
      Serial.println("Shooter direction: Left");
      turnDirection = 2;
    } else if (shooterDirection == 0) {
      Serial.println("Shooter direction: Stop");
      turnDirection = 0;
    }
  });

   server.on("/net", []() { // net direction
    int netDirection = server.arg("val").toInt();
    if (netDirection == 1 && digitalRead(upperLimitnet) == LOW) {
      netUPDOWN = 1;
      netMotorRightActive = true;
      Serial.println("Net direction: Up");
    } else if (netDirection == -1 && digitalRead(lowerLimitnet) == LOW) {
      netUPDOWN = 2;
      netMotorLeftActive = true;
      Serial.println("Net direction: Down");
    } else if (netDirection == 0) {
      netUPDOWN = 0;
      netMotorRightActive = false;
      Serial.println("Net direction: Stop");
    }
  });
  
server.on("/motor", []() {  // shooter
    int motorState = server.arg("value").toInt();
    if (motorState == 1 ) {
      shootSpeed = 1;
      Serial.println("Shooter ON");

    } else if (motorState == 0) {
      shootSpeed = 0;
      Serial.println("Shooter OFF");
    }
  });

  server.on("/shoot", []() {
    Serial.println("Shoot command received");
    if (shooterDirection == 1 && digitalRead(upperLimitpusher) == LOW) {
    //digitalWrite(Shooter, HIGH);
      tolakDirection = 1; // Extend
      tolakExtendActive = true;
      Serial.println("Shooter direction: Extend");
    
    }
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
      } else {
        //Serial.println("Same position, maybe not tracking?");
      }
    }
  } else {
  Serial.println("No data from HuskyLens");
  }

  server.handleClient();
}