#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// Pin definitions (same as yours)
const int motorFR_R1 = 19; 
const int motorFR_L1 = 18; 
const int motorFL_R2 = 26; 
const int motorFL_L2 = 27; 
const int motorBR_R3 = 13; 
const int motorBR_L3 = 12;
const int motorBL_R4 = 25;
const int motorBL_L4 = 33; 
const int Shooter = 35; 
const int Tolak = 14; 
String joystickX = "0";
String joystickY = "0";
int s = 2;
int shooterDirection = 0;


const char *ssid = "mechanum";
const char *password = "mechanum123";

const float df = 0.1;

const int MAX_PWM = 230;

WebServer server(80);

// put function definitions here:
void computeWheelSpeeds(float Vx, float Vy, float omega,float &vFR, float &vFL, float &vBR, float &vBL) {
  
   vFR = Vy - Vx + df * omega;

   vFL = Vy + Vx - df * omega;
  
   vBR = -Vx - Vy + df * omega;

   vBL =  Vx - Vy - df * omega;
  
}

void setMotor(int pinA, int pinB, float speed) {
  int pwm = abs(speed * MAX_PWM);
  pwm = constrain(pwm, 0, MAX_PWM);

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
  pinMode(motorFR_R1, OUTPUT);
  pinMode(motorFR_L1, OUTPUT);
  pinMode(motorFL_R2, OUTPUT);
  pinMode(motorFL_L2, OUTPUT);
  pinMode(motorBR_R3, OUTPUT);
  pinMode(motorBR_L3, OUTPUT);
  pinMode(motorBL_R4, OUTPUT);
  pinMode(motorBL_L4, OUTPUT);
  pinMode(Shooter, OUTPUT);
  pinMode(Tolak, OUTPUT);

 

  server.on("/joystick", []() {
    joystickX = server.arg("x");
    joystickY = server.arg("y");
    //Serial.printf("Joystick values — X: %s, Y: %s\n", joystickX.c_str(), joystickY.c_str());
    server.send(200, "text/plain", "Joystick values received.");
  });

  server.on("/", []() {
  server.send(200, "text/plain", "ESP32 is running!");
  });

  server.onNotFound([]() {
  Serial.printf("NOT FOUND: %s\n", server.uri().c_str());
  server.send(404, "text/plain", "Not found");
  });

  server.on("/button1", []() {
    shooterDirection = 1;
    Serial.printf("Shooter is pressed\n");
    server.send(200, "text/plain", "button1 value received.");
    digitalWrite(Shooter,HIGH);
    delay(7000);
    digitalWrite(Tolak,LOW);

  });

  server.on("/button2", []() {
    shooterDirection = 2;
    Serial.printf("Tolak is pressed\n");
    server.send(200, "text/plain", "button2 value received.");
    digitalWrite(Tolak,HIGH);
    delay(4000);
    digitalWrite(Tolak,LOW);
  });

   /*server.on("/stop", []() {
    shooterDirection = 1;
    Serial.printf("Stop is pressed\n");
    server.send(200, "text/plain", "Stop value received.");
    digitalWrite(Shooter, LOW);
    digitalWrite(Tolak, LOW);
  });*/

  server.begin(); // <-- This line was misplaced before. Now it's correctly placed outside the handler.

}
void loop() {
   float stickY = joystickY.toFloat();
   float stickX = joystickX.toFloat();
   //float stickRx = joystick.toFloat();
   float vFR, vFL, vBR, vBL;

  
    computeWheelSpeeds(stickY, stickX, 0, vFR, vFL, vBR, vBL);
    setMotor(motorFR_L1, motorFR_R1, vFR);
    setMotor(motorFL_L2, motorFL_R2, vFL);
    setMotor(motorBR_L3, motorBR_R3, vBR);
    setMotor(motorBL_L4, motorBL_R4, vBL);

    /*Serial.print(F("\tvFR: "));
    Serial.print(vFR);
    Serial.print(F("\tvFL: "));
    Serial.print(vFL);
    Serial.print(F("\tvBR: "));
    Serial.print(vBR);
    Serial.print(F("\tvBL: \n"));
    Serial.print(vBL);*/
    //Serial.print(F("\tSpeed: "));

 server.handleClient();

}