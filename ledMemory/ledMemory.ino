
#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "sprite.h"
#include "animation.h"

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
#define PIN_LED_UP 4
#define PIN_LED_RIGHT 5

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

uint8_t sequence[100] = {0};

void setup() {
  for (int i = 0; i < 4; i++) {
    pinMode(leds[i], OUTPUT);
  }
  for (int i = 0; i < 4; i++) {
    buttons[i].setup();
  }

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for this one!
    while(1) {} // Infinite loop so we don't go to the loop() function
  }

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

void waitForAnyButtonPressed() {
  bool anyButtonPressed = false;
  while (!anyButtonPressed) {
    for (int i = 0; i < 4; i++) {
      anyButtonPressed ||= buttons[i].loopAndIsJustPressed();
    }
  }
}

void loop() {
  display.clearDisplay();
  display.setCursor(20, 27);
  display.println(F("LED MEMORY"));
  display.setCursor(4, 36);
  display.println(F("- press any key to start -"));
  display.display();

  waitForAnyButtonPressed();

  int score = 0;
  currentGame: for (; score < 100; score++) {
    display.clearDisplay();
    display.setCursor(11, 18);
    display.println(F("- listen and repeat -"));
    display.setCursor(48, 27);
    display.println(F("Score: "));
    display.setCursor(59, 36);
    display.println(score);
    display.display();

    unit8_t newSequenceEntry = random(4);
    sequence[score / 4] &= ~(3 << (score % 4));
    sequence[score / 4] |= newSequenceEntry << (score % 4);

    for (int i = 0; i < score + 1; i++) {
      unit8_t sequenceEntry = (sequence[i/4] >> (i % 4)) & 3;

      digitalWrite(leds[sequenceEntry], HIGH);
      delay(500);
      digitalWrite(leds[sequenceEntry], LOW);
      delay(100);
    }

    sequenceReading: for (int i = 0; i < score + 1; i++) {
      unit8_t sequenceEntry = (sequence[i/4] >> (i % 4)) & 3;

      for (int k = 0; k < 4; k++) {
        if(buttons[k].loopAndIsJustPressed()) {
          if (k == sequenceEntry) {
            continue sequenceReading;
          } else {
            break currentGame;
          }
        }
      }
    }
  }

  display.clearDisplay();
  display.setCursor(42, 18);
  display.println(F("GAME OVER"));
  display.setCursor(36, 27);
  display.println(F("Final score: "));
  display.setCursor(59, 36);
  display.println(score);
  display.display();

  waitForAnyButtonPressed();
}