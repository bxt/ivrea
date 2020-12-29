
#include <SPI.h>
#include <Wire.h>
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

class DirectionButton {
private:
  uint8_t pin;
  uint8_t lastReading;
  unsigned long lastChangeTime;

public:
  DirectionButton(uint8_t pin) {
    this->pin = pin;
    this->lastReading = HIGH;
    this->lastChangeTime = 0;
  }

  void setup() {
    pinMode(this->pin, INPUT_PULLUP);
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

DirectionButton buttons[4] = {
  DirectionButton(PIN_LEFT),
  DirectionButton(PIN_DOWN),
  DirectionButton(PIN_UP),
  DirectionButton(PIN_RIGHT),
};

void displaySplashScreen() {
  display.clearDisplay();
  display.drawBitmap(16, 2, splash_bmp, SPLASH_BMP_WIDTH, SPLASH_BMP_HEIGHT, 1);
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
    buttons[i].setup();
  }

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for this one!
    Serial.println(F("SSD1306 allocation failed"));
    while(1) {} // Infinite loop so we don't go to the loop() function
  }

  display.setTextColor(SSD1306_WHITE);

  displaySplashScreen();
  waitForAnyButtonPressed();
  randomSeed(micros());
}

uint8_t backBuffer[1024] = {0};
uint8_t frontBuffer[1024] = {0};

void loop() {
  for(int y = 0; y < SCREEN_HEIGHT; y++) {
    for(int x = 0; x < SCREEN_WIDTH; x++) {
      bool currentValue = (backBuffer[(x/8) + y*SCREEN_WIDTH/8] << (x % 8)) & 0x80 > 1;
      uint8_t neighborCount = 0;
      for (int yOffs = -1; yOffs <= 1; yOffs++) {
        for (int xOffs = -1; xOffs <= 1; xOffs++) {
          if(xOffs != 0 && yOffs != 0) {
            int neighborX = (x + xOffs + SCREEN_WIDTH) % SCREEN_WIDTH;
            int neighborY = (y + yOffs + SCREEN_HEIGHT) % SCREEN_HEIGHT;
            bool neighborValue = (backBuffer[(neighborX/8) + neighborY*SCREEN_WIDTH/8] >> (neighborX % 8)) & 0x01 > 1;
            if(neighborValue) {
              neighborCount++;
            }
          }
        }
      }

      if((currentValue && (neighborCount == 2 || neighborCount == 3))
       || (!currentValue && neighborCount == 3)) {
        frontBuffer[(x/8) + y*SCREEN_WIDTH/8] |= 0x80 >> (x % 8);
      } else {
        frontBuffer[(x/8) + y*SCREEN_WIDTH/8] &= ~(0x80 >> (x % 8));
      }
    }
  }

  if (buttons[0].loopAndIsJustPressed()) { // LEFT
    frontBuffer[ 0] = B00100000;
    frontBuffer[16] = B00010000;
    frontBuffer[32] = B01110000;
  }
  if (buttons[1].loopAndIsJustPressed()) { // DOWN
  }
  if (buttons[2].loopAndIsJustPressed()) { // UP
  }
  if (buttons[3].loopAndIsJustPressed()) { // RIGHT
  }

  uint8_t *backBufferPointer = backBuffer;
  uint8_t *frontBufferPointer = frontBuffer;
  uint8_t *tmp = backBufferPointer;
  backBufferPointer = frontBufferPointer;
  frontBufferPointer = tmp;

  display.clearDisplay();

  display.drawBitmap(0, 0, frontBuffer, SCREEN_WIDTH, SCREEN_HEIGHT, 1);
  display.display();

  delay(200);
}