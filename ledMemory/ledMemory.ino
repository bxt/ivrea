
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
#define PIN_LED_UP 6
#define PIN_LED_RIGHT 5

// Connect the buzzer
#define BUZZER_PIN A0
#define GAME_OVER_FREQUENCY 220

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

void loop() {
  display.clearDisplay();
  display.drawBitmap(16, 8, splash_bmp, SPLASH_BMP_WIDTH, SPLASH_BMP_HEIGHT, 1);
  display.setCursor(13, 55);
  display.println(F("- press any key -"));
  display.display();

  waitForAnyButtonPressed();

  int score = 0;
  uint8_t lastNewSequenceEntry = 5;
  for (; score < 100; score++) {
    display.clearDisplay();
    display.setCursor(19, 18);
    display.println(F("listen & repeat"));
    display.setCursor(48, 32);
    display.println(F("Score: "));
    display.setCursor(59, 41);
    display.println(score);
    display.display();

    uint8_t newSequenceEntry = random(3);
    if (newSequenceEntry >= lastNewSequenceEntry) {
      newSequenceEntry++;
    }
    lastNewSequenceEntry = newSequenceEntry;
    sequence[score / 4] &= ~(3 << ((score % 4) * 2));
    sequence[score / 4] |= newSequenceEntry << ((score % 4) * 2);

    for (int i = 0; i < score + 1; i++) {
      uint8_t sequenceEntry = (sequence[i / 4] >> ((i % 4) * 2)) & 3;

      digitalWrite(leds[sequenceEntry], HIGH);
      tone(BUZZER_PIN, frequencies[sequenceEntry], 500);
      delay(500);
      digitalWrite(leds[sequenceEntry], LOW);
      delay(100);
    }

    bool everythingCorrect = true;
    unsigned long lastButtonTime = 0;
    uint8_t lastButtonLed = leds[0];

    for (int i = 0; i < score + 1; i++) {
      uint8_t sequenceEntry = (sequence[i / 4] >> ((i % 4) * 2)) & 3;

      bool anyButtonPressed = false;
      while (!anyButtonPressed) {
        if (millis() > lastButtonTime + 500) {
          digitalWrite(lastButtonLed, LOW);
        }
        for (int k = 0; k < 4; k++) {
          if (buttons[k].loopAndIsJustPressed()) {
            digitalWrite(lastButtonLed, LOW);
            lastButtonLed = leds[k];
            anyButtonPressed = true;
            if (k != sequenceEntry) {
              everythingCorrect = false;
              break;
            }
            digitalWrite(leds[k], HIGH);
            tone(BUZZER_PIN, frequencies[k], 500);
            lastButtonTime = millis();
          }
        }
      }

      if (!everythingCorrect) {
        break;
      }
    }

    if (everythingCorrect) {
      delay(500);
      for (int k = 0; k < 4; k++) {
        digitalWrite(leds[k], LOW);
      }
      delay(500);
    } else {
      break;
    }
  }

  for (int k = 0; k < 4; k++) {
    digitalWrite(leds[k], HIGH);
  }

  display.clearDisplay();
  display.setCursor(42, 18);
  display.println(F("GAME OVER"));
  display.setCursor(36, 32);
  display.println(F("Final score: "));
  display.setCursor(59, 41);
  display.println(score);
  display.display();

  tone(BUZZER_PIN, GAME_OVER_FREQUENCY, 500);
  delay(500);

  waitForAnyButtonPressed();

  for (int k = 0; k < 4; k++) {
    digitalWrite(leds[k], LOW);
  }
}