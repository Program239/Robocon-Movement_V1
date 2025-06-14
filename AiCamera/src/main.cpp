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
const int limitTolak1 = 34;
const int limitTolak2 = 35;
const int limitNet1 = 32;
const int limitNet2 = 33;
String slider = "0";
int turnDirection = 0;
int shooterDirection = 0;
int netDirection = 0;
int tolakDirection = 0;
int button1State = 0;
int button2State = 0;
bool netMotorRightActive = false;
bool netMotorLeftActive = false;
bool tolakExtendActive = false;
bool tolakRetractActive = false;

WebServer server(80);

const char *ssid = "ESP32-TOP";
const char *password = "1234567890";

/*HUSKYLENS huskylens;

int lastX = -1, lastY = -1;*/

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

  
  /*while (!huskylens.begin(Wire)) {
    Serial.println("Begin failed");
    delay(1000);  // Prevent watchdog resets
  }*/

  delay(1000); // Allow HuskyLens to initialize

  // Initialize pins
  pinMode(rightTurn, OUTPUT);
  pinMode(leftTurn, OUTPUT);
  pinMode(Shooter, OUTPUT);
  pinMode(Tolak1, OUTPUT);
  pinMode(Tolak2, OUTPUT);
  pinMode(rightNet, OUTPUT);
  pinMode(leftNet, OUTPUT);
  pinMode(limitTolak1, INPUT_PULLUP);
  pinMode(limitTolak2, INPUT_PULLUP);
  pinMode(limitNet1, INPUT_PULLUP);
  pinMode(limitNet2, INPUT_PULLUP);

  /*server.on("/camera", HTTP_GET, []() {
      String response = "/camera?x=" + String(lastX) + "&y=" + String(lastY);
      server.sendHeader("Location", response);
      server.send(200, "text/plain", response);
    });/*
  
  server.on("/", []() {
    server.send(200, "text/plain", "ESP32 is running!");
  });

  /*server.on("/button3", []() {
    
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
    digitalWrite(Tolak1,HIGH);
    delay(3000);
    digitalWrite(Tolak1,LOW);
  });

  server.on("/rotation", []() {
  slider = server.arg("val");  
  int rotation = slider.toInt();  

  if (rotation == 1) {
    digitalWrite(leftTurn,LOW);
    digitalWrite(rightTurn,HIGH);
    Serial.println("Right rotation");
  } else if (rotation == -1) {
    digitalWrite(leftTurn,HIGH);
    digitalWrite(rightTurn,LOW);
    Serial.println("Left rotation");
  } else if (rotation == 0) {
    digitalWrite(rightTurn,LOW);
    digitalWrite(leftTurn,LOW);
    Serial.println("Stop rotation");
  }

  server.send(200, "text/plain", "Slider value processed.");
});

server.on("/right", []() {
    if (digitalRead(limitNet1) == LOW) {
    shooterDirection = 1; // Right
    netMotorRightActive = true;
    Serial.println("Net UP");
    }
  });

  server.on("/left", []() {
    if (digitalRead(limitNet2) == LOW) {
    shooterDirection = 2; // Right
    netMotorLeftActive = true;
    Serial.println("Net DOWN");
    }
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
  });*/

  server.on("/shooter", []() { //turntable
    int shooterDirection = server.arg("direction").toInt();
    if (shooterDirection == 1) {
      Serial.println("Shooter direction: Right");
      turnDirection = 1; // Right
    } else if (shooterDirection == -1) {
      Serial.println("Shooter direction: Left");
      turnDirection = 2; // Left
    } else {
      Serial.println("Shooter direction: Stop");
      turnDirection = 0; // Stop
    }
  });

  server.on("/motor", []() {  //net
    int motorState = server.arg("value").toInt();
    if (motorState == 1) 
    if (digitalRead(limitNet1) == LOW) {
    netDirection= 1; // Right
    netMotorRightActive = true;
    Serial.println("Net UP");
    }
  });

/*  server.on("/motor", []() {  //tolak
    int motorState = server.arg("value").toInt();
    if (motorState == 1) 
    if (digitalRead(limitTolak1) == LOW) {
    tolakDirection= 1; // Right
    netMotorRightActive = true;
    Serial.println("Net UP");
    }
  });*/

  server.on("/shoot", []() { //shooter
    Serial.println("Shoot command received");  
    shooterDirection = 1; // Activate shooter    
  });

  /*server.begin();
  huskylens.writeAlgorithm(ALGORITHM_OBJECT_TRACKING);*/
}

void loop() {

 /* if (huskylens.request()) {
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
  }*/

  if  (netMotorRightActive && digitalRead(limitNet1) == HIGH) {  // LOW = pressed
    shooterDirection = 0;
    netMotorRightActive = false;
    Serial.println("Limit1 triggered while moving — motor stopped.");
  }
  if  (netMotorLeftActive && digitalRead(limitNet2) == HIGH) {  // LOW = pressed
    shooterDirection = 0;
    netMotorLeftActive = false;
    Serial.println("Limit2 triggered while moving — motor stopped.");
  }
  
  if  (tolakRetractActive && digitalRead(limitTolak1) == HIGH) {  // LOW = pressed
    tolakDirection = 0;
    tolakRetractActive = false;
    Serial.println("Limit3 triggered while moving — motor stopped.");
  }

  if  (tolakExtendActive && digitalRead(limitTolak2) == HIGH) {  // LOW = pressed
    tolakDirection = 0;
    tolakExtendActive = false;
    Serial.println("Limit4 triggered while moving — motor stopped.");
  }
  switch (netDirection) {
    case 0: // Stop
      setMotor(rightNet, leftNet, 0.0);
      break;
    case 1: // Right
      setMotor(rightNet, leftNet, 1);
      break;
    case 2: // Left
      setMotor(rightNet, leftNet, -1);
      break;
    default:
      setMotor(rightNet, leftNet, 0.0);
      break;
  }

  switch (tolakDirection) {
    case 0: // Stop
      setMotor(Tolak1, Tolak2, 0.0);
      break;
    case 1: // Right
      setMotor(Tolak1, Tolak2, 1);
      break;
    case 2: // Left
      setMotor(Tolak1, Tolak2, -1);
      break;
    default:
      setMotor(Tolak1, Tolak2, 0.0);
      break;
  }

  switch (turnDirection) {
    case 0: // Stop
      setMotor(rightTurn, leftTurn, 0.0);
      break;
    case 1: // Right
      setMotor(rightTurn, leftTurn, 1);
      break;
    case 2: // Left
      setMotor(rightTurn, leftTurn, -1);
      break;
    default:
      setMotor(rightTurn, leftTurn, 0.0);
      break;
  }

  switch (shooterDirection) {
    case 0: // Stop
      digitalWrite (Shooter, LOW);
      break;
    case 1: // Right
      digitalWrite (Shooter, HIGH);
      break;
    default:
      digitalWrite (Shooter, LOW);
      break;
  }

  server.handleClient();
}
