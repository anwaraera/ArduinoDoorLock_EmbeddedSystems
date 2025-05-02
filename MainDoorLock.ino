#include <Adafruit_SSD1306.h>
#include <splash.h>
#include <Wire.h>
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

// Display initialization
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Servo Motor initialization
Servo servoMotor;

// 4x4 Keypad initialization
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

// All global variables used throughout
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
  // Begin Serials for communication between ESP and Mega
  Serial.begin(9600);
  Serial1.begin(9600);

  // Servo starts off in locked position
  servoMotor.attach(2);
  servoMotor.write(LOCKED);

  // Oled display set up
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
  char key = keypad.getKey();  // Waits for key to be pushed and stores

  if (key) {

    if (key == 'A' && !isLocked) {  // Pressing 'A' only works when door is unlocked, and locks the door
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
     
      // Send data to Serial1 which is connected to ESP module
      postData = "attempt_pin=A&attempt_status=" + locked;
      Serial1.println(postData);
    }
    else if (isLocked) {
      if (key == '#') {   // When door is locked, '#' is the 'enter' button after putting in a PIN 
        checkPin();
        if (isLocked) {
          locked = "LOCKED";
        } else {
          locked = "UNLOCKED";
        }
       
        // Send data to Serial1 which is connected to ESP module
        postData = "attempt_pin=" + enteredPin + "&attempt_status=" + locked;
        Serial1.println(postData);

        // Clear PIN and display when done
        clearPin();
        display.clearDisplay();
        display.display();
      } else if (key == '*') {   // '*' clears the current PIN entered, allowing users to start again if there was a mistake
        clearPin();
        display.clearDisplay();
        cursor_x = 10;
        display.setCursor(cursor_x, cursor_y);
        display.display();
      } else if (key != 'A') {   // All other keys are saved into 'enteredPin' and displayed as entered
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

// Checks if the user entered PIN and the stored PIN match and locks/unlocks door accordingly
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

// Clears the entered PIN
void clearPin() {
  enteredPin = "";
  if (isLocked) {
    Serial.println("\nEnter PIN and press # to unlock: ");
  }
}

// Unlocks the door - turns the servo motor
void unlock() {
  Serial.println("Correct PIN - Unlocking");
  servoMotor.write(UNLOCKED);
  isLocked = false;
}

// Locks the door - turns the servo motor
void lock() {
  Serial.println("Locking system");
  servoMotor.write(LOCKED);
  isLocked = true;
}

// Outputs into the Serial Monitor for debugging purposes
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
