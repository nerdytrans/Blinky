#include <Arduino.h>
#include <Wire.h>

// I2C address of the target device
#define TARGET_ADDRESS 0x08

void setup() {
  Serial.begin(9600);
  Wire.begin(); // Join I2C bus as controller
  Serial.println("Master starting...");
}

void loop() {
  // TODO: Send commands to target via I2C
}
