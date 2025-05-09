#include <Arduino.h>
#include <WiFi.h>

// Pin definitions (same as yours)
const int gripperRY = 30;
const int gripperLY = 32;
const int motorR1 = 34;
const int motorL1 = 36;
const int motorR2 = 38;
const int motorL2 = 40;
const int motorR3 = 42;
const int motorL3 = 44;
const int gripperRX = 46;
const int gripperLX = 48;
int s = 2;

const char *ssid = "MyRobot";
const char *password = "12345678";

WiFiServer server(80);

// put function definitions here:
void RUNRIGHT(int Speed) {
  digitalWrite(motorL1, 0);
  digitalWrite(motorR1, Speed);
  digitalWrite(motorR2, 0);
  digitalWrite(motorL2, Speed);
  digitalWrite(motorR3, Speed);
  digitalWrite(motorL3, 0);
}

void RUNLEFT(int Speed) {
  digitalWrite(motorR1, 0);
  digitalWrite(motorL1, Speed);
  digitalWrite(motorR2, Speed);
  digitalWrite(motorL2, 0);
  digitalWrite(motorR3, 0);
  digitalWrite(motorL3, Speed);
}

void RUNSTRAIGHT(int Speed) {
  digitalWrite(motorR1, Speed);
  digitalWrite(motorL1, 0);
  digitalWrite(motorR2, Speed);
  digitalWrite(motorL2, 0);
  digitalWrite(motorR3, 0);
  digitalWrite(motorL3, 0);
}

void RUNBACK(int Speed) {
  digitalWrite(motorR1, 0);
  digitalWrite(motorL1, Speed);
  digitalWrite(motorR2, 0);
  digitalWrite(motorL2, Speed);
  digitalWrite(motorR3, 0);
  digitalWrite(motorL3, 0);
}

void STOP() {
  digitalWrite(motorR1, 0);
  digitalWrite(motorL1, 0);
  digitalWrite(motorR2, 0);
  digitalWrite(motorL2, 0);
  digitalWrite(motorR3, 0);
  digitalWrite(motorL3, 0);
}

void GRAB() {
  digitalWrite(gripperRX, HIGH);
  digitalWrite(gripperLX, LOW);
}

void RELEASE() {
  digitalWrite(gripperRX, HIGH);
  digitalWrite(gripperLX, HIGH);
}

void uP() {
  digitalWrite(gripperRY, HIGH);
  digitalWrite(gripperLY, LOW);
}

void dowN() {
  digitalWrite(gripperRY, HIGH);
  digitalWrite(gripperLY, HIGH);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  IPAddress IP = WiFi.localIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  server.begin();
  Serial.println("Robot started. Waiting for connection...");

  // Initialize pins
  pinMode(motorR1, OUTPUT);
  pinMode(motorR2, OUTPUT);
  pinMode(motorR3, OUTPUT);
  pinMode(motorL1, OUTPUT);
  pinMode(motorL2, OUTPUT);
  pinMode(motorL3, OUTPUT);
  pinMode(gripperRX, OUTPUT);
  pinMode(gripperLX, OUTPUT);
  pinMode(gripperRY, OUTPUT);
  pinMode(gripperLY, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:

  WiFiClient client = server.available();
  if (client) {
    String command = client.readStringUntil('\n');
    command.trim();
    Serial.println("Received: " + command);

    if (command.startsWith("LSTICK:")) {
      // Example: LSTICK:100,-50
      int commaIndex = command.indexOf(',');
      int x = command.substring(7, commaIndex).toInt();
      int y = command.substring(commaIndex + 1).toInt();

      // Process stick values
      //Serial.print("Joystick X:%d Y:%d\n", x, y);
      if (x > 20 && abs(y) < 20) RUNRIGHT(x);
      else if (x < -20 && abs(y) < 20) RUNLEFT(x);
      else if (y > 20 && abs(x) < 20) RUNBACK(y);
      else if (y < -20 && abs(x) < 20) RUNSTRAIGHT(y);
      else STOP();
    }
    //else if (command == "UP") ;
    //else if (command == "DOWN") ;
    //else if (command == "LEFT") ;
    //else if (command == "RIGHT") ;
    //else if (command == "TRIANGLE") ;
    //else if (command == "CIRCLE") ;
    //else if (command == "CROSS") ;
    //else if (command == "SQUARE") ;
  }
}
