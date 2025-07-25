// Mojo Wave - Fly Mouse Only Mode (BLE Mouse Adjusted)
// ESP32 + MPU6050 + 2x FSRs | BLE Mouse Control with Gestures

#include <Wire.h>
#include <MPU6050.h>
#include <BleMouse.h>

MPU6050 mpu;
BleMouse bleMouse("Mojo FlyMouse", "Mojo Labs", 100);

// === Pin Configuration ===
const int fsrLeftPin = 34;
const int fsrRightPin = 35;

// === Thresholds ===
const int fsrThreshold = 300;

// === Gesture & Cooldown ===
unsigned long lastGestureTime = 0;
const unsigned long gestureCooldown = 800;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 not connected!");
    while (1);
  }
  Serial.println("MPU6050 ready");

  bleMouse.begin();
  Serial.println("Waiting for BLE mouse connection...");
}

void loop() {
  if (!bleMouse.isConnected()) {
    Serial.println("BLE Mouse not connected.");
    delay(1000);
    return;
  }

  int fsrLeftValue = analogRead(fsrLeftPin);
  int fsrRightValue = analogRead(fsrRightPin);

  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  Serial.print("FSR_L: "); Serial.print(fsrLeftValue);
  Serial.print(" | FSR_R: "); Serial.print(fsrRightValue);
  Serial.print(" | ax: "); Serial.print(ax);
  Serial.print(" | ay: "); Serial.print(ay);
  Serial.print(" | az: "); Serial.println(az);

  if (millis() - lastGestureTime < gestureCooldown) return;

  // === FSR Clicks ===
  if (fsrLeftValue > fsrThreshold) {
    Serial.println("🖱️ Left Click");
    bleMouse.click(MOUSE_LEFT);
    lastGestureTime = millis();
  } else if (fsrRightValue > fsrThreshold) {
    Serial.println("🖱️ Right Click");
    bleMouse.click(MOUSE_RIGHT);
    lastGestureTime = millis();
  }

  // === Motion → Mouse Cursor Movement ===
  int mouseX = ax / 1000;  // Adjust for sensitivity
  int mouseY = ay / 1000;

  bleMouse.move(mouseX, mouseY);
  Serial.print("🖱️ Move X:"); Serial.print(mouseX);
  Serial.print(" Y:"); Serial.println(mouseY);

  delay(20);
}
