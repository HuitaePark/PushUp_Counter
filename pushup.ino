#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define TRIG 9
#define ECHO 8
int buzzerPin = 2;
int startButton = 3;
int modeButton1 = 4; // Mode button 1
int modeButton2 = 5; // Mode button 2
int mode = -1; // Initialize mode as -1 (waiting to start)
int counter = 0; // Push-up count
unsigned long startTime = 0; // Timer start time

LiquidCrystal_I2C lcd(0x27, 16, 2); // Update with your I2C address if different

void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(startButton, INPUT_PULLUP);
  pinMode(modeButton1, INPUT_PULLUP); // Mode button 1 setup
  pinMode(modeButton2, INPUT_PULLUP); // Mode button 2 setup

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Push Start!");
}

void loop() {
  long duration, distance;
  int start = digitalRead(startButton);
  int modeSelect1 = digitalRead(modeButton1);
  int modeSelect2 = digitalRead(modeButton2);

  // Waiting for start button to be pressed to enter mode selection
  if (mode == -1 && start == LOW) { 
    mode = 0; // Change to mode selection
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Select Mode...");
    while (digitalRead(startButton) == LOW); // Wait for button release
  }

  if (mode == 0) { // In select mode
    if (modeSelect1 == LOW) { // Mode button 1 pressed
      mode = 1; // Speed mode selected
      lcd.clear();
      lcd.print("Speed Mode");
      counter = 0; // Reset push-up count
      startTime = millis(); // Start timer for speed mode
      while (digitalRead(modeButton1) == LOW); // Wait for button release
    } else if (modeSelect2 == LOW) { // Mode button 2 pressed
      mode = 2; // Normal mode selected
      lcd.clear();
      lcd.print("Normal Mode");
      counter = 0; // Reset push-up count
      while (digitalRead(modeButton2) == LOW); // Wait for button release
    }
  }

  if (mode == 1 && millis() - startTime > 60000) { // In speed mode and 60 seconds passed
    mode = 0; // Switch to select mode
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Done! Count: ");
    lcd.print(counter);
    delay(5000); // Show result for 5 seconds then clear
    lcd.clear();
    lcd.print("Push Start!");
    mode = -1; // Return to initial waiting state
  }

  if (mode == 2 && start == LOW) { // In normal mode and start button pressed
    mode = 0; // Switch to select mode
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Done! Count: ");
    lcd.print(counter);
    delay(5000); // Show result for 5 seconds then clear
    lcd.clear();
    lcd.print("Push Start!");
    mode = -1; // Return to initial waiting state
    while (digitalRead(startButton) == LOW); // Wait for button release
  }

  if (mode != 0 && mode != -1) { // If not in select mode or waiting to start, detect push-ups
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);

    duration = pulseIn(ECHO, HIGH);
    distance = duration * 17 / 1000;

    if (distance < 10) { // Push-up detected
      tone(buzzerPin, 131);
      delay(300);
      noTone(buzzerPin);
      counter++; // Increase count
      delay(500); // Delay to prevent continuous detection
    }
  }
}
