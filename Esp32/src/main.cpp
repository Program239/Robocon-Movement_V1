#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// Pin definitions (same as yours)
const int motorR1 = 19;
const int motorL1 = 18;
const int motorR2 = 26;
const int motorL2 = 27;
const int motorR3 = 13;
const int motorL3 = 12;
const int Rturn = 25;
const int Lturn = 33;
const int Shooter = 35; 
const int Tolak = 14; 
String joystickX = "0";
String joystickY = "0";
String slider = "0";
int shooterDirection = 0;
int s=2;
//float slide = slider.toFloat() * 0.5;

const char *ssid = "ESP32-ROBOT";
const char *password = "1234567890";

const float R = 0.1;

WebServer server(80);

// put function definitions here:
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

  // Initialize pins
  pinMode(motorR1, OUTPUT);
  pinMode(motorR2, OUTPUT);
  pinMode(motorR3, OUTPUT);
  pinMode(motorL1, OUTPUT);
  pinMode(motorL2, OUTPUT);
  pinMode(motorL3, OUTPUT);
  pinMode(Rturn, OUTPUT);
  pinMode(Lturn, OUTPUT);

  server.on("/joystick", []() {
    joystickX = server.arg("x");
    joystickY = server.arg("y");
    //Serial.printf("Joystick values X: %s, Y: %s\n", joystickX.c_str(), joystickY.c_str());
    server.send(200, "text/plain", "Joystick values received.");
  });

  server.on("/rotation", []() {
    slider = server.arg("val");
    Serial.printf("Slider value: %s\n", slider.c_str());
    server.send(200, "text/plain", "Slider value received.");
  });
  
  server.on("/right", []() {
    shooterDirection = 1;
    Serial.printf("Right is pressed\n");
    server.send(200, "text/plain", "Right value received.");

  });

  server.on("/left", []() {
    shooterDirection = 2;
    Serial.printf("Left is pressed\n");
    server.send(200, "text/plain", "Left value received.");

  });

  server.on("/stop", []() {
    shooterDirection = 0;
    Serial.printf("Stop is pressed\n");
    server.send(200, "text/plain", "Stop value received.");

  });

  server.on("/", []() {
  server.send(200, "text/plain", "ESP32 is running!");
  });


  server.onNotFound([]() {
    Serial.printf("NOT FOUND: %s\n", server.uri().c_str());
    server.send(404, "text/plain", "Not found");
  });

  /*server.on("/button1", []() {
    shooterDirection = 1;
    Serial.printf("Shooter is pressed\n");
    server.send(200, "text/plain", "button1 value received.");
    digitalWrite(Shooter,HIGH);
    delay(5000);
    digitalWrite(Tolak,LOW);

  });


  server.on("/button2", []() {
    shooterDirection = 2;
    Serial.printf("Tolak is pressed\n");
    server.send(200, "text/plain", "button2 value received.");
    digitalWrite(Tolak,HIGH);
    delay(4000);
    digitalWrite(Tolak,LOW);
  });*/

  server.begin();

}

void loop() {

  setMotor(Rturn, Lturn, 0.5);
  setMotor(Rturn, Lturn, -0.5);
  setMotor(Rturn, Lturn, 0.0); 

  switch (shooterDirection) {
    case 0: // Stop
      setMotor(Rturn, Lturn, 0.0);
      break;
    case 1: // Right
      setMotor(Rturn, Lturn, 0.5);
      break;
    case 2: // Left
      setMotor(Rturn, Lturn, -0.5);
      break;
    default:
      setMotor(Rturn, Lturn, 0.0);
      break;
  }


  
  float stickY = joystickY.toFloat();
  float stickX = -joystickX.toFloat();
  float slide = slider.toFloat();
  float vB, vR, vL;
  
  computeWheelSpeeds(stickY, stickX, slide, vB, vR, vL);
  setMotor(motorL1, motorR1, vB);
  setMotor(motorL2, motorR2, vR);
  setMotor(motorL3, motorR3, vL);
  //Serial.printf("Motor speeds - B: %.2f, R: %.2f, L: %.2f\n", vB, vR, vL);

  server.handleClient();
}
