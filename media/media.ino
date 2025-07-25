// Mojo Wave - Gauntlet Media Control Only
// ESP32 + MPU6050 + 2x FSRs | Media Control via Gestures + Taps

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
const int liftThreshold = -9000;
const int dropThreshold = 9000;
const int swipeRightThresh = 9000;
const int swipeLeftThresh = -7000;
const int deadZoneMin = -3000;
const int deadZoneMax = 3000;

// === Gesture & Cooldown ===
unsigned long lastGestureTime = 0;
const unsigned long gestureCooldown = 800;
int16_t last_ax = 0, last_ay = 0, last_az = 0;

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
  int fsrLeftValue = analogRead(fsrLeftPin);
  int fsrRightValue = analogRead(fsrRightPin);

  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);
  ax = (ax + last_ax) / 2;
  ay = (ay + last_ay) / 2;
  az = (az + last_az) / 2;
  last_ax = ax;
  last_ay = ay;
  last_az = az;

  Serial.print("FSR_L: "); Serial.print(fsrLeftValue);
  Serial.print(" | FSR_R: "); Serial.print(fsrRightValue);
  Serial.print(" | ax: "); Serial.print(ax);
  Serial.print(" | ay: "); Serial.print(ay);
  Serial.print(" | az: "); Serial.println(az);

  if (millis() - lastGestureTime < gestureCooldown) return;

  // === FSR Media Taps ===
  if (fsrLeftValue > fsrThreshold) {
    Serial.println("⏯️ Play/Pause (FSR Left)");
    bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
    lastGestureTime = millis();
  } else if (fsrRightValue > fsrThreshold) {
    Serial.println("🔇 Mute (FSR Right)");
    bleKeyboard.write(KEY_MEDIA_MUTE);
    lastGestureTime = millis();
  }

  // === Gesture Media Control ===
  else if (abs(ay) > abs(ax) && abs(ay) > abs(az)) {
    if (ay < liftThreshold) {
      Serial.println("🡹 Volume Up (Lift)");
      bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
      lastGestureTime = millis();
    } else if (ay > dropThreshold) {
      Serial.println("🡻 Volume Down (Drop)");
      bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
      lastGestureTime = millis();
    }
  } else if (abs(ax) > abs(ay) && abs(ax) > abs(az)) {
    if (ax > swipeRightThresh) {
      Serial.println("➡️ Next Track");
      bleKeyboard.write(KEY_MEDIA_NEXT_TRACK);
      lastGestureTime = millis();
    } else if (ax < swipeLeftThresh) {
      Serial.println("⬅️ Previous Track");
      bleKeyboard.write(KEY_MEDIA_PREVIOUS_TRACK);
      lastGestureTime = millis();
    }
  }

  delay(20);
}
