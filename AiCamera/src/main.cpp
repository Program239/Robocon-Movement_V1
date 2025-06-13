#include <Wire.h>
#include <Arduino.h>
#include <HUSKYLENS.h>
#include <WiFi.h>
#include <WebServer.h>

// Pin definitions (same as yours)
const int Shooter = 32; 
const int Tolak = 14; 
const int Rturn = 25;
const int Lturn = 33;
const int Rnet = 26;
const int Lnet = 27;
const int Limit1 = 34;
const int Limit2 = 35;
String slider = "0";
int shooterDirection = 0;
int shooter1Direction = 0;
int netDirection = 0;
int button1State = 0;
int button2State = 0;
bool netMotorRightActive = false;
bool netMotorLeftActive = false;

WebServer server(80);

const char *ssid = "ESP32-TOP";
const char *password = "1234567890";

HUSKYLENS huskylens;

int lastX = -1, lastY = -1;

void setMotor(int pinA, int pinB, float speed) {
  int pwm = abs(speed * 255.0);
  pwm = constrain(pwm, 0, 255);
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);  // ESP32 default pins

  // put your setup code here, to run once:
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

  // Initialize pins
  pinMode(Rturn, OUTPUT);
  pinMode(Lturn, OUTPUT);
  pinMode(Shooter, OUTPUT);
  pinMode(Tolak, OUTPUT);
  pinMode(Rnet, OUTPUT);
  pinMode(Lnet, OUTPUT);
  pinMode(Limit1, INPUT_PULLUP);
  pinMode(Limit2, INPUT_PULLUP);

  server.on("/camera", HTTP_GET, []() {
      String response = "/camera?x=" + String(lastX) + "&y=" + String(lastY);
      server.sendHeader("Location", response);
      server.send(200, "text/plain", response);
    });
  
  server.on("/", []() {
    server.send(200, "text/plain", "ESP32 is running!");
  });

  server.on("/button3", []() {
    
    button1State = 1;
    Serial.printf("Shooter is pressed\n");
    server.send(200, "text/plain", "button1 value received.");
    digitalWrite(Shooter,HIGH);
    delay(9000);
    digitalWrite(Shooter,LOW);

  });

  server.on("/button2", []() {
    button2State = 1;
    Serial.printf("Tolak is pressed\n");
    server.send(200, "text/plain", "button2 value received.");
    digitalWrite(Tolak,HIGH);
    delay(3000);
    digitalWrite(Tolak,LOW);
  });

  server.on("/rotation", []() {
  slider = server.arg("val");  
  int rotation = slider.toInt();  

  if (rotation == 1) {
    digitalWrite(Lturn,LOW);
    digitalWrite(Rturn,HIGH);
    Serial.println("Right rotation");
  } else if (rotation == -1) {
    digitalWrite(Lturn,HIGH);
    digitalWrite(Rturn,LOW);
    Serial.println("Left rotation");
  } else if (rotation == 0) {
    digitalWrite(Rturn,LOW);
    digitalWrite(Lturn,LOW);
    Serial.println("Stop rotation");
  }

  server.send(200, "text/plain", "Slider value processed.");
});

server.on("/right", []() {
    if (digitalRead(Limit1) == LOW) {
    shooterDirection = 1; // Right
    netMotorRightActive = true;
    Serial.println("Net UP");
    }
  });

  server.on("/left", []() {
    if (digitalRead(Limit2) == LOW) {
    shooterDirection = 2; // Right
    netMotorLeftActive = true;
    Serial.println("Net DOWN");
    }
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

  if  (netMotorRightActive && digitalRead(Limit1) == HIGH) {  // LOW = pressed
    shooterDirection = 0;
    netMotorRightActive = false;
    Serial.println("Limit1 triggered while moving — motor stopped.");
  }
  if  (netMotorLeftActive && digitalRead(Limit2) == HIGH) {  // LOW = pressed
    shooterDirection = 0;
    netMotorLeftActive = false;
    Serial.println("Limit2 triggered while moving — motor stopped.");
  }

    switch (shooterDirection) {
    case 0: // Stop
      setMotor(Rnet, Lnet, 0.0);
      break;
    case 1: // Right
      setMotor(Rnet, Lnet, 1);
      break;
    case 2: // Left
      setMotor(Rnet, Lnet, -1);
      break;
    default:
      setMotor(Rnet, Lnet, 0.0);
      break;
  }

  server.handleClient();
}
