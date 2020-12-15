
#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "splash.h"

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
#define GAME_OVER_FREQUENCY 220

// How fast the game runs
#define DELAY_TIME_MS 500

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
};

uint8_t leds[4] = {PIN_LED_LEFT, PIN_LED_DOWN, PIN_LED_UP, PIN_LED_RIGHT};
DirectionButton buttons[4] = {
  DirectionButton(PIN_LEFT),
  DirectionButton(PIN_DOWN),
  DirectionButton(PIN_UP),
  DirectionButton(PIN_RIGHT),
};
int frequencies[4] = {330, 370, 415, 440};

uint8_t sequence[100] = {0};

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
}

void waitForAnyButtonPressed() {
  bool anyButtonPressed = false;
  while (!anyButtonPressed) {
    for (int i = 0; i < 4; i++) {
      anyButtonPressed = anyButtonPressed || buttons[i].loopAndIsJustPressed();
    }
  }
}

void displaySplashScreen() {
  display.clearDisplay();
  display.drawBitmap(16, 8, splash_bmp, SPLASH_BMP_WIDTH, SPLASH_BMP_HEIGHT, 1);
  display.setCursor(13, 55);
  display.println(F("- press any key -"));
  display.display();
}

void displayGameInProgressScreen(int score) {
  display.clearDisplay();
  display.setCursor(19, 18);
  display.println(F("listen & repeat"));
  display.setCursor(48, 32);
  display.println(F("Score: "));
  display.setCursor(59, 41);
  display.println(score);
  display.display();
}

void displayGameOverScreen(int score) {
  display.clearDisplay();
  display.setCursor(42, 18);
  display.println(F("GAME OVER"));
  display.setCursor(36, 32);
  display.println(F("Final score: "));
  if (score > 9) {
    display.setCursor(58, 41);
  } else {
    display.setCursor(61, 41);
  }
  display.println(score);
  display.display();
}

void turnAllLeds(uint8_t toLevel) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(leds[i], toLevel);
  }
}

void setSequence(uint8_t index, uint8_t value) {
  uint8_t sequenceIndex = index / 4;
  uint8_t bitIndex = (index % 4) * 2;
  sequence[sequenceIndex] &= ~(3 << bitIndex);
  sequence[sequenceIndex] |= value << bitIndex;
}

uint8_t getSequence(uint8_t index) {
  uint8_t sequenceIndex = index / 4;
  uint8_t bitIndex = (index % 4) * 2;
  return (sequence[sequenceIndex] >> bitIndex) & 3;
}

void loop() {
  displaySplashScreen();

  waitForAnyButtonPressed();

  int score = 0;
  uint8_t lastNewSequenceEntry = 5; // sentinel, but means we never start with last one...
  for (; score < 100; score++) {
    displayGameInProgressScreen(score);

    // Pick, and make sure we don't pick the same entry twice in a row:
    uint8_t newSequenceEntry = random(3);
    if (newSequenceEntry >= lastNewSequenceEntry) {
      newSequenceEntry++;
    }
    lastNewSequenceEntry = newSequenceEntry;

    setSequence(score, newSequenceEntry);

    // Play back the sequence including the new value:
    for (int i = 0; i < score + 1; i++) {
      uint8_t sequenceEntry = getSequence(i);
      digitalWrite(leds[sequenceEntry], HIGH);
      tone(BUZZER_PIN, frequencies[sequenceEntry], DELAY_TIME_MS);
      delay(DELAY_TIME_MS);

      digitalWrite(leds[sequenceEntry], LOW);
      delay(100);
    }

    // Read back the sequence from the user:
    bool everythingCorrect = true;
    unsigned long lastButtonTime = 0;
    uint8_t lastButtonLed = leds[0];
    for (int i = 0; i < score + 1; i++) {
      uint8_t sequenceEntry = getSequence(i);

      bool anyButtonPressed = false;
      while (!anyButtonPressed) { // loop until any button was pressed
        if (millis() > lastButtonTime + DELAY_TIME_MS) {
          digitalWrite(lastButtonLed, LOW);
        }
        for (int k = 0; k < 4; k++) {
          if (buttons[k].loopAndIsJustPressed()) {
            digitalWrite(lastButtonLed, LOW);
            lastButtonLed = leds[k];
            anyButtonPressed = true;
            if (k != sequenceEntry) { // wrong button!
              everythingCorrect = false;
              break;
            }
            digitalWrite(leds[k], HIGH);
            tone(BUZZER_PIN, frequencies[k], DELAY_TIME_MS);
            lastButtonTime = millis();
          }
        }
      }

      if (!everythingCorrect) {
        break;
      }
    }

    if (everythingCorrect) {
      delay(DELAY_TIME_MS);
      turnAllLeds(LOW);
      delay(DELAY_TIME_MS);
    } else {
      break;
    }
  }

  turnAllLeds(HIGH);
  tone(BUZZER_PIN, GAME_OVER_FREQUENCY, DELAY_TIME_MS);
  displayGameOverScreen(score);
  delay(DELAY_TIME_MS);

  waitForAnyButtonPressed();

  turnAllLeds(LOW);
}