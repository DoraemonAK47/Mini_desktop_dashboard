#include <ESP32Servo.h>

Servo servo1;

int servo1Pin = 12;

void setup() {
  servo1.attach(servo1Pin);
	Serial.begin(115200);
  servo1.write(90); delay(100);
}

void loop() {
    servo1.write(90); delay(500);
    servo1.write(45); delay(200);
    servo1.write(0); delay(200);
    servo1.write(45); delay(200);
    servo1.write(90); delay(500);
}
