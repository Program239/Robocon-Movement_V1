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
const int Shooter = 14; 
const int Tolak = 32; 
String joystickX = "0";
String joystickY = "0";
int s = 2;
int shooterDirection = 0;
bool shooterActive = false;
unsigned long shooterStartTime = 0;
int shooterStep = 0;


const char *ssid = "MFI mechanum";
const char *password = "mechanumm111";

const float df = 0.1;

const int MAX_PWM = 200;

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

  WiFi.setTxPower(WIFI_POWER_19_5dBm); // Set WiFi power to maximum

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

  server.on("/shoot", []() {
    if(!shooterActive){
      shooterActive = true;
      shooterStartTime = millis();
      shooterStep = 0;
      Serial.println("Shooter sequence started");
      server.send(200, "text/plain", "shoot value received.");
    } else {
      Serial.println("Shooter sequence already active");
      server.send(400, "text/plain", "Shooter sequence already active.");
      return;
    }
  });

  server.on("/rotation", []() {   
    int shooterDirection = server.arg("val").toInt();
    Serial.printf("Tolak is pressed\n");
    server.send(200, "text/plain", "rotation value received.");
    digitalWrite(Tolak,HIGH);
    delay(4000);
    digitalWrite(Tolak,LOW);
  });


  server.begin(); // <-- This line was misplaced before. Now it's correctly placed outside the handler.

}
void loop() {
   float stickY = joystickY.toFloat();
   float stickX = joystickX.toFloat();
   float stickRx = shooterDirection - 0.1;
   float vFR, vFL, vBR, vBL;

    computeWheelSpeeds(stickY, stickX, stickRx, vFR, vFL, vBR, vBL);
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

    if (shooterActive) {
      unsigned long currentTime = millis();
      switch (shooterStep) {
        case 0: // Initial step
          digitalWrite(Shooter, HIGH);
          Serial.println("Shooter motor started");
          shooterStep = 1;
          shooterStartTime = currentTime; // Reset the start time
          break;
        case 1: // Wait for 6 seconds
          if (currentTime - shooterStartTime >= 6000) {
            digitalWrite(Tolak, HIGH);
            Serial.println("Shooter motor stopped after 6 seconds");
            shooterStep = 2;
            shooterStartTime = currentTime; // Reset the start time for the next step
          }
          break;
        case 2: // Final step
          if (currentTime - shooterStartTime >= 2000) {
            digitalWrite(Tolak, LOW);
            digitalWrite(Shooter, LOW);
            Serial.println("Shooter sequence completed, motors stopped");
            shooterActive = false; // Reset the state
            shooterStep = 3; // Move to the next step
          }
          break;
      }
    }

 server.handleClient();

}