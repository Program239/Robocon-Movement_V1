#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const int motorR1 = 19;
const int motorL1 = 18;
const int motorR2 = 26;
const int motorL2 = 27;
const int motorR3 = 13;
const int motorL3 = 12;
String joystickX = "0";
String joystickY = "0";
int slider = 0;
int omniRotation = 0; 

const char *ssid = "OMNIBOT";
const char *password = "1234567890";

const float R = 0.1;

WebServer server(80);

void computeWheelSpeeds(float Vx, float Vy, float omega,float &vB, float &vR, float &vL) {
  // Wheel 1 (Back)
  vB = Vy + R * omega;

  // Wheel 2 (Right)
  vR = -0.866 * Vx + 0.5 * Vy + R * omega;

  // Wheel 3 (Left)
  vL = 0.866 * Vx + 0.5 * Vy + R * omega;

}

void setMotor(int pinA, int pinB, float speed) {
  int pwm = abs(speed * 255.0);
  pwm = constrain(pwm, 0, 255);

  if (speed > 0) {
    analogWrite(pinA, pwm);
    analogWrite(pinB, 0);
  } else if (speed < 0) {
    analogWrite(pinA, 0);
    analogWrite(pinB, pwm);
  } else {
    analogWrite(pinA, 0);
    analogWrite(pinB, 0);
  }
}

void setup() {
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

  pinMode(motorR1, OUTPUT);
  pinMode(motorR2, OUTPUT);
  pinMode(motorR3, OUTPUT);
  pinMode(motorL1, OUTPUT);
  pinMode(motorL2, OUTPUT);
  pinMode(motorL3, OUTPUT);
  
  server.on("/joystick", []() {
    joystickX = server.arg("x");
    joystickY = server.arg("y");
    //Serial.printf("Joystick values X: %s, Y: %s\n", joystickX.c_str(), joystickY.c_str());
    server.send(200, "text/plain", "Joystick values received.");
  });

  server.on("/gameTimer", []() {
    String timer = server.arg("time");
    Serial.printf("Timer: %ss\n" , timer.c_str());
    server.send(200, "text/plain", "Timer value received.");
  });

  server.on("/rotation", []() {
    slider = server.arg("val").toInt();
    Serial.printf("Slider value: %d\n", slider);
    server.send(200, "text/plain", "Slider value received.");

  if (slider == 1) {
    omniRotation = 1; // Right rotation
    Serial.println("Right rotation");

  } else if (slider == -1) {
    omniRotation = 2; // Left rotation
    Serial.println("Left rotation");

  }

  server.send(200, "text/plain", "Slider value processed.");
  });

  server.on("/", []() {
  server.send(200, "text/plain", "ESP32 is running!");
  });


  server.onNotFound([]() {
    Serial.printf("NOT FOUND: %s\n", server.uri().c_str());
    server.send(404, "text/plain", "Not found");
  });

  server.begin();

}

void loop() {

  switch (omniRotation) {
    case 0: // Stop
      setMotor(motorR1, motorL1, 0);
      setMotor(motorR2, motorL2, 0);
      setMotor(motorR3, motorL3, 0);
      break;
    case 1: // Right
      setMotor(motorR1, motorL1, 1);
      setMotor(motorR2, motorL2, 1);
      setMotor(motorR3, motorL3, 1);
      break;
    case 2: // Left
      setMotor(motorR1, motorL1, -1);
      setMotor(motorR2, motorL2, -1);
      setMotor(motorR3, motorL3, -1);
      break;
    default:
      setMotor(motorR1, motorL1, 0.0);
      break;
  }
  
  float stickY = joystickY.toFloat();
  float stickX = -joystickX.toFloat();
  float vB, vR, vL;
  
  computeWheelSpeeds(stickY, stickX, slider, vB, vR, vL);
  setMotor(motorL1, motorR1, vB);
  setMotor(motorL2, motorR2, vR);
  setMotor(motorL3, motorR3, vL);
  //Serial.printf("Motor speeds - B: %.2f, R: %.2f, L: %.2f\n", vB, vR, vL);

  server.handleClient();
}