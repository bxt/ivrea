
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Run:
// for i in wrodShuflfe/*.png; do go run transformBitmap.go $i; done
#include "splash.h"
#include "ongoing.h"
#include "success.h"

#include "wordsDe.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET 4 // Reset pin, no idea what for...
// SSD1306 display connected to I2C, on nano SDA = A4, SCL = A5 pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Connect momentary tact pushbutton to those pins:
#define PIN_LEFT 7
#define PIN_DOWN 8
#define PIN_UP 9
#define PIN_RIGHT 10
// ... and they should have a pull-up and be active low:
#define DIRECTION_BUTTON_ACTIVE LOW
#define DEBOUNCE_DELAY 50ul

// Connect the corresponding LEDs to those pins:
#define PIN_LED_LEFT 2
#define PIN_LED_DOWN 3
// This pin is out of order because otherwise the OLED reset makes it light up:
#define PIN_LED_UP 6
#define PIN_LED_RIGHT 5

// Connect the buzzer
#define BUZZER_PIN A0

class DirectionButton {
private:
  uint8_t pin;
  uint8_t lockedReading;
  uint8_t lastReading;
  unsigned long lastChangeTime;

public:
  DirectionButton(uint8_t pin) {
    this->pin = pin;
    this->lastReading = HIGH;
    this->lastChangeTime = 0;
  }

  void setup() {
    pinMode(this->pin, INPUT);
    this->lastReading = digitalRead(this->pin);
  }

  bool loopAndIsJustPressed() {
    int reading = digitalRead(this->pin);

    bool returnValue = false;

    if (this->lastReading != reading) {
      if ((millis() - this->lastChangeTime) > DEBOUNCE_DELAY &&
          reading == DIRECTION_BUTTON_ACTIVE) {
        returnValue = true;
      }

      this->lastChangeTime = millis();
    }

    this->lastReading = reading;

    return returnValue;
  }

  uint8_t getLastReading() {
    return this->lastReading;
  }
};

uint8_t leds[4] = {PIN_LED_LEFT, PIN_LED_DOWN, PIN_LED_UP, PIN_LED_RIGHT};
DirectionButton buttons[4] = {
  DirectionButton(PIN_LEFT),
  DirectionButton(PIN_DOWN),
  DirectionButton(PIN_UP),
  DirectionButton(PIN_RIGHT),
};
int frequencies[4] = {330, 370, 415, 440};

uint8_t cursorPosition = 0;
int wordIndex = 0;
bool pickedUp = false;

void displaySplashScreen() {
  display.clearDisplay();
  display.drawBitmap(16, 8, splash_bmp, SPLASH_BMP_WIDTH, SPLASH_BMP_HEIGHT, 1);
  display.setCursor(13, 55);
  display.println(F("- press any key -"));
  display.display();
}

void waitForAnyButtonPressed() {
  bool anyButtonPressed = false;
  while (!anyButtonPressed) {
    for (int i = 0; i < 4; i++) {
      anyButtonPressed = anyButtonPressed || buttons[i].loopAndIsJustPressed();
    }
  }
}

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < 4; i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }
  for (int i = 0; i < 4; i++) {
    buttons[i].setup();
  }

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for this one!
    Serial.println(F("SSD1306 allocation failed"));
    while(1) {} // Infinite loop so we don't go to the loop() function
  }

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  displaySplashScreen();
  waitForAnyButtonPressed();
  randomSeed(micros());
}

void displaySuccessScreen() {
  display.clearDisplay();
  display.drawBitmap(0, 0, success_bmp, SUCCESS_BMP_WIDTH, SUCCESS_BMP_HEIGHT, 1);
  display.setCursor(73, 47);
  display.setTextSize(2);
  display.println(wordIndex);
  display.setTextSize(1);
  display.display();
}

void turnAllLeds(uint8_t toLevel) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(leds[i], toLevel);
  }
}

void makeLedsMirrorButtons() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(leds[i], buttons[i].getLastReading());
  }
}

void loop() {
  char currentWord[MAX_WORD_LENGTH];
  char shuffledWord[MAX_WORD_LENGTH];
  int currentWordLength = strlen_P(words[wordIndex]);
  for(int i = 0; i < currentWordLength; i++) {
    currentWord[i] = pgm_read_byte_near(words[wordIndex] + i);
    shuffledWord[i] = pgm_read_byte_near(words[wordIndex] + i);
  }
  currentWord[currentWordLength] = '\0';
  shuffledWord[currentWordLength] = '\0';

  // See https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle#The_modern_algorithm
  for(int i = 0; i < currentWordLength - 1; i++) {
    int j = random(i, currentWordLength);
    // Switch in-place:
    shuffledWord[i] ^= shuffledWord[j];
    shuffledWord[j] ^= shuffledWord[i];
    shuffledWord[i] ^= shuffledWord[j];
  }

  bool everythingCorrect = false;

  while (!everythingCorrect) {
    makeLedsMirrorButtons();

    if(buttons[0].loopAndIsJustPressed()) { // LEFT
      uint8_t previousCursorPosition = cursorPosition;
      cursorPosition = (cursorPosition + 1) % currentWordLength;
      if(pickedUp) {
        shuffledWord[previousCursorPosition] ^= shuffledWord[cursorPosition];
        shuffledWord[cursorPosition] ^= shuffledWord[previousCursorPosition];
        shuffledWord[previousCursorPosition] ^= shuffledWord[cursorPosition];
      }
    }
    if(buttons[1].loopAndIsJustPressed()) { // DOWN
      pickedUp = false;
    }
    if(buttons[2].loopAndIsJustPressed()) { // UP
      pickedUp = true;
    }
    if(buttons[3].loopAndIsJustPressed()) { // RIGHT
      uint8_t previousCursorPosition = cursorPosition;
      cursorPosition = (currentWordLength + cursorPosition - 1) % currentWordLength;
      if(pickedUp) {
        shuffledWord[previousCursorPosition] ^= shuffledWord[cursorPosition];
        shuffledWord[cursorPosition] ^= shuffledWord[previousCursorPosition];
        shuffledWord[previousCursorPosition] ^= shuffledWord[cursorPosition];
      }
    }

    everythingCorrect = true;
    for(int i = 0; i < currentWordLength; i++) {
      if(currentWord[i] != shuffledWord[i]) {
        everythingCorrect = false;
        break;
      }
    }

    display.clearDisplay();
    display.drawBitmap(32, 8, ongoing_bmp, ONGOING_BMP_WIDTH, ONGOING_BMP_HEIGHT, 1);
    display.setCursor((128 - currentWordLength * 6) / 2, 18);
    display.println(currentWord);

    display.setCursor(48, 46);
    display.println(F("Score: "));
    display.setCursor(59, 55);
    display.println(wordIndex);
    display.display();
  }

  displaySuccessScreen();

  // See https://de.wikipedia.org/wiki/Frequenzen_der_gleichstufigen_Stimmung
  tone(BUZZER_PIN, 262, 100);
  digitalWrite(leds[0], HIGH);
  delay(100);

  tone(BUZZER_PIN, 330, 100);
  digitalWrite(leds[1], HIGH);
  delay(100);

  tone(BUZZER_PIN, 392, 100);
  digitalWrite(leds[2], HIGH);
  delay(100);

  tone(BUZZER_PIN, 523, 300);
  digitalWrite(leds[3], HIGH);
  delay(500);

  turnAllLeds(LOW);

  wordIndex = (wordIndex+1) % WORD_COUNT;
}
