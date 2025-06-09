#include <Wire.h>
#include <HUSKYLENS.h>

HUSKYLENS huskylens;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);  // ESP32 default pins

  delay(100); // Let everything stabilize

  while (!huskylens.begin(Wire)) {
    Serial.println("Begin failed");
    delay(1000);  // Prevent watchdog resets
  }

  huskylens.writeAlgorithm(ALGORITHM_OBJECT_TRACKING);
}

void loop() {
  static int lastX = -1, lastY = -1;

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
}
