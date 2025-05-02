#include <Adafruit_SSD1306.h>
#include <splash.h>
#include <Wire.h>
#include <Time.h>
#include <Keypad.h>
#include <Servo.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET 4

/*
SOURCES:
 -  https://www.youtube.com/watch?v=bg-HJ54Cw64 (Servo Motor set up)
 -  https://www.instructables.com/Connecting-a-4-x-4-Membrane-Keypad-to-an-Arduino/ (Membrane Switch set up)
 -  https://www.instructables.com/Arduino-and-the-SSD1306-OLED-I2C-128x64-Display/ (setting up display)
 -  https://playground.arduino.cc/Main/I2cScanner/ (I2C Scanner)
*/

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Servo servoMotor;

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 10, 9, 8, 7 };
byte colPins[COLS] = { 6, 5, 4, 3 };
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String correctPin = "1234";
String enteredPin = "";
bool isLocked = true;
const int LOCKED = 0;
const int UNLOCKED = 90;
int cursor_x = 10;
int cursor_y = 20;
String postData = "";
String locked = "";
String timestamp = "";

void setup() {
  servoMotor.attach(2);
  servoMotor.write(LOCKED);
  Serial.begin(9600);
  Serial1.begin(9600);
  displayStatus();
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0X3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(cursor_x, cursor_y);
  display.display();
}

void loop() {
  char key = keypad.getKey();

  if (key) {

    if (key == 'A' && !isLocked) {
      lock();
      display.clearDisplay();
      display.setCursor(20, 20);
      display.println("LOCKED");
      display.display();
      cursor_x = 10;
      delay(5000);
      display.clearDisplay();
      display.display();
      if (isLocked) {
          locked = "LOCKED";
        } else {
          locked = "UNLOCKED";
        }
      postData = "attempt_pin=A&attempt_status=" + locked;
      Serial1.println(postData);
    }

    else if (isLocked) {
      if (key == '#') {
        checkPin();
        if (isLocked) {
          locked = "LOCKED";
        } else {
          locked = "UNLOCKED";
        }
        postData = "attempt_pin=" + enteredPin + "&attempt_status=" + locked;
        Serial1.println(postData);

        clearPin();
        display.clearDisplay();
        display.display();
      } else if (key == '*') {
        clearPin();
        display.clearDisplay();
        cursor_x = 10;
        display.setCursor(cursor_x, cursor_y);
        display.display();
      } else if (key != 'A') {
        enteredPin += key;
        Serial.print(key);
        display.setCursor(cursor_x, cursor_y);
        display.print(key);
        cursor_x += 20;
        display.display();
      }
    }
  }
}

void checkPin() {
  Serial.println();

  if (enteredPin == correctPin) {
    unlock();
    display.clearDisplay();
    display.setCursor(5, 20);
    display.println("UNLOCKED");
    display.display();
    cursor_x = 10;
    delay(5000);
  } else {
    Serial.println("Incorrect PIN");
    servoMotor.write(LOCKED);
    isLocked = true;
    display.clearDisplay();
    display.setCursor(5, 20);
    display.println("Incorrect PIN");
    display.display();
    cursor_x = 10;
    delay(5000);
  }

  displayStatus();
}

void clearPin() {
  enteredPin = "";
  if (isLocked) {
    Serial.println("\nEnter PIN and press # to unlock: ");
  }
}

void unlock() {
  Serial.println("Correct PIN - Unlocking");
  servoMotor.write(UNLOCKED);
  isLocked = false;
}

void lock() {
  Serial.println("Locking system");
  servoMotor.write(LOCKED);
  isLocked = true;
}

void displayStatus() {
  Serial.println();
  Serial.print("Current Status: ");
  if (isLocked) {
    Serial.println("LOCKED");
    Serial.println("Enter PIN and press # to unlock: ");
  } else {
    Serial.println("UNLOCKED");
    Serial.println("Press A to lock: ");
  }
}