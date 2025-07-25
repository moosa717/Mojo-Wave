// Mojo Wave: Mode Switch + Sleep Mode Core


#define MODE_SWITCH_PIN_1 27  // Middle = LOW → Mouse Mode
#define MODE_SWITCH_PIN_2 26  // Side 2 = LOW → Keyboard Mode

#define FSR_LEFT_PIN 34
#define FSR_RIGHT_PIN 35

// === Constants ===
const unsigned long sleepTimeout = 60000;  // 1 min of inactivity
unsigned long lastActiveTime = 0;

int currentMode = 0; // 0 = Media, 1 = Mouse, 2 = Keyboard
bool isSleeping = false;

void setup() {
  Serial.begin(115200);

  pinMode(MODE_SWITCH_PIN_1, INPUT_PULLUP);
  pinMode(MODE_SWITCH_PIN_2, INPUT_PULLUP);

  pinMode(FSR_LEFT_PIN, INPUT);
  pinMode(FSR_RIGHT_PIN, INPUT);

  Serial.println("Booting Mojo Wave Mode Manager...");
}

void loop() {
  handleModeSwitch();
  checkActivity();
  handleSleepLogic();

  delay(100); // Adjust if needed
}

void handleModeSwitch() {
  // Always read mode switch
  if (digitalRead(MODE_SWITCH_PIN_1) == LOW) {
    currentMode = 1;
  } else if (digitalRead(MODE_SWITCH_PIN_2) == LOW) {
    currentMode = 2;
  } else {
    currentMode = 0;
  }

  // Print mode (optional)
  static int lastMode = -1;
  if (currentMode != lastMode) {
    Serial.print("Switched to Mode: ");
    Serial.println(currentMode == 0 ? "Media" : currentMode == 1 ? "Mouse" : "Keyboard");
    lastMode = currentMode;
  }
}

void checkActivity() {
  // Basic activity = FSR tap or motion
  int fsrL = analogRead(FSR_LEFT_PIN);
  int fsrR = analogRead(FSR_RIGHT_PIN);

  if (fsrL > 100 || fsrR > 100) {
    lastActiveTime = millis();
    if (isSleeping) {
      Serial.println(" Woke up from Sleep");
      isSleeping = false;
    }
  }
}

void handleSleepLogic() {
  if (!isSleeping && millis() - lastActiveTime > sleepTimeout) {
    Serial.println("Entering Sleep Mode (No Activity)");
    isSleeping = true;

    // Optional: add real sleep mode (e.g. ESP32 light sleep)
    // esp_light_sleep_start();
  }

  if (isSleeping) {
    // You can skip BLE actions, MPU updates, etc. here
    return;
  }

  // Add your mode logic here (Media, Mouse, Keyboard)
}

