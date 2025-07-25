// Mojo Wave - Gauntlet Function Keyboard Only
// ESP32 + MPU6050 + 2x FSRs | Tap + Motion Shortcuts via BLE Keyboard

#include <Wire.h>
#include <MPU6050.h>
#include <BleKeyboard.h>

MPU6050 mpu;
BleKeyboard bleKeyboard;

// === Pin Configuration ===
const int fsrLeftPin = 34;
const int fsrRightPin = 35;

// === Thresholds ===
const int fsrThreshold = 300;
const int swipeRightThresh = 9000;
const int swipeLeftThresh = -7000;
const int tiltUpThresh = 20000;
const int thrustDownThresh = 18000;

unsigned long lastTriggerTime = 0;
const unsigned long triggerCooldown = 600;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 not connected!");
    while (1);
  }
  Serial.println("MPU6050 ready");

  bleKeyboard.begin();
}

void loop() {
  if (!bleKeyboard.isConnected()) {
    delay(500);
    return;
  }

  // Read FSR
  int fsrLeftValue = analogRead(fsrLeftPin);
  int fsrRightValue = analogRead(fsrRightPin);

  // Read MPU Motion
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  Serial.print("FSR_L: "); Serial.print(fsrLeftValue);
  Serial.print(" | FSR_R: "); Serial.print(fsrRightValue);
  Serial.print(" | gx: "); Serial.print(gx);
  Serial.print(" | gy: "); Serial.print(gy);
  Serial.print(" | az: "); Serial.println(az);

  if (millis() - lastTriggerTime < triggerCooldown) return;

  // === Tap + Motion Combos ===

  if (fsrLeftValue > fsrThreshold) {
    if (gx > swipeRightThresh) {
      Serial.println("➡️ Ctrl + C (Copy)");
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press('c');
      bleKeyboard.releaseAll();
    }
    else if (gx < swipeLeftThresh) {
      Serial.println("⬅️ Ctrl + V (Paste)");
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press('v');
      bleKeyboard.releaseAll();
    }
    else if (gy > tiltUpThresh) {
      Serial.println("⤴️ Ctrl + Z (Undo)");
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press('z');
      bleKeyboard.releaseAll();
    }
    else if (az > thrustDownThresh) {
      Serial.println("⏎ Enter");
      bleKeyboard.write(KEY_RETURN);
    }

    lastTriggerTime = millis(); // Cooldown
  }

  delay(20);
}
