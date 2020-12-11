#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "sprite.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET 4 // Reset pin, no idea what for...
// SSD1306 display connected to I2C, on nano SDA = A4, SCL = A5 pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


#define DEBOUNCE_DELAY 50ul
#define DIRECTION_BUTTON_ACTIVE LOW

#define PIN_LEFT 7
#define PIN_DOWN 8
#define PIN_UP 9
#define PIN_RIGHT 10

enum Direction { left, down, up, right };

class DirectionButton {
private:
  uint8_t pin;
  uint8_t lockedReading;
  uint8_t lastReading;
  unsinged long lastChangeTime;
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
      if ((milis() - this->lastChangeTime) > DEBOUNCE_DELAY &&
          reading == DIRECTION_BUTTON_ACTIVE) {
        returnValue = true;
      }

      this->lastChangeTime = milis();
    }

    this->lastReading = reading;

    return returnValue;
  }
};

DirectionButton leftDirectionButton(PIN_LEFT);
DirectionButton downDirectionButton(PIN_DOWN);
DirectionButton upDirectionButton(PIN_UP);
DirectionButton rightDirectionButton(PIN_RIGHT);

unsigned long score = 0;

uint8_t grid[4][4] = {
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
}

void fillRandomEmptySpot() {
  uint8_t emptySpotCount = 0;

  for(int y = 0; y < 4; y++) {
    for(int x = 0; x < 4; x++) {
      uint8_t value = grid[y][x];
      if (value == 0) emptySpotCount++;
    }
  }

  uint8_t fillIndex = random(0, emptySpotCount);
  uint8_t fillValue = random(0, 100) < 10 ? 2 : 1;

  for(int y = 0; y < 4; y++) {
    for(int x = 0; x < 4; x++) {
      uint8_t value = grid[y][x];
      if (value == 0) {
        emptySpotCount--;

        if (fillIndex == emptySpotCount) {
          grid[y][x] = fillValue;
          return;
        }
      }
    }
  }
}

void setup() {
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for this one!
    Serial.println(F("SSD1306 allocation failed"));
    while(1) {} // Infinite loop so we don't go to the loop() function
  }

  fillRandomEmptySpot();

  leftDirectionButton.setup();
  downDirectionButton.setup();
  upDirectionButton.setup();
  rightDirectionButton.setup();
}

void loop() {
  display.clearDisplay();

  for(int y = 0; y < 4; y++) {
    for(int x = 0; x < 4; x++) {
      uint8_t value = grid[y][x];
      if (value == 0) continue;
      uint8_t posX = x * TILE_BMP_WIDTH;
      uint8_t posY = y * TILE_BMP_HEIGHT;
      display.drawBitmap(posX, posY, tile_bmps[value - 1], TILE_BMP_WIDTH, TILE_BMP_HEIGHT, 1);
    }
  }

  if (leftDirectionButton.loopAndIsJustPressed()) {
    for(int y = 0; y < 4; y++) {
      uint8_t prevValue = 0xFF;
      int8_t prevPosition = -1;
      for(int x = 0; x < 4; x++) {
        uint8_t value = grid[y][x];
        if (value == 0) continue;
        if (value == prevValue) {
          grid[y][x] = 0;
          grid[y][prevPosition]++;
          score += 1 << (value + 1);
          prevValue = 0;
          prevPosition++;
        } else {
          grid[y][x] = 0;
          prevPosition++;
          grid[y][prevPosition] = value;
          prevValue = value;
        }
      }
    }

    fillRandomEmptySpot();
  }

  if (downDirectionButton.loopAndIsJustPressed()) {
    for(int x = 0; x < 4; x++) {
      uint8_t prevValue = 0xFF;
      int8_t prevPosition = -1;
      for(int y = 4 - 1; y >= 0; y--) {
        uint8_t value = grid[y][x];
        if (value == 0) continue;
        if (value == prevValue) {
          grid[y][x] = 0;
          grid[prevPosition][x]++;
          score += 1 << (value + 1);
          prevValue = 0;
          prevPosition--;
        } else {
          grid[y][x] = 0;
          prevPosition--;
          grid[prevPosition][x] = value;
          prevValue = value;
        }
      }
    }

    fillRandomEmptySpot();
  }

  if (upDirectionButton.loopAndIsJustPressed()) {
    for(int x = 0; x < 4; x++) {
      uint8_t prevValue = 0xFF;
      int8_t prevPosition = -1;
      for(int y = 0; y < 4; y++) {
        uint8_t value = grid[y][x];
        if (value == 0) continue;
        if (value == prevValue) {
          grid[y][x] = 0;
          grid[prevPosition][x]++;
          score += 1 << (value + 1);
          prevValue = 0;
          prevPosition++;
        } else {
          grid[y][x] = 0;
          prevPosition++;
          grid[prevPosition][x] = value;
          prevValue = value;
        }
      }
    }

    fillRandomEmptySpot();
  }

  if (rightDirectionButton.loopAndIsJustPressed()) {
    for(int y = 0; y < 4; y++) {
      uint8_t prevValue = 0xFF;
      int8_t prevPosition = 4;
      for(int x = 4 - 1; x >= 0; x--) {
        uint8_t value = grid[y][x];
        if (value == 0) continue;
        if (value == prevValue) {
          grid[y][x] = 0;
          grid[y][prevPosition]++;
          score += 1 << (value + 1);
          prevValue = 0;
          prevPosition--;
        } else {
          grid[y][x] = 0;
          prevPosition--;
          grid[y][prevPosition] = value;
          prevValue = value;
        }
      }
    }

    fillRandomEmptySpot();
  }

  display.display();

  delay(20);
}