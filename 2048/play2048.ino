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

DirectionButton leftDirectionButton(PIN_LEFT);
DirectionButton downDirectionButton(PIN_DOWN);
DirectionButton upDirectionButton(PIN_UP);
DirectionButton rightDirectionButton(PIN_RIGHT);

class Coord {
public:
  int8_t x;
  int8_t y;
  Coord(int8_t x, int8_t y): x(x), y(y) { }

  Coord& operator+=(const Coord& r) {
    x += r.x;
    y += r.y;
    return *this;
  }
};

Coord operator*(Coord l, const int8_t r) {
    return Coord(l.x * r, l.y * r);
}

Coord operator+(Coord l, const Coord r) {
    return Coord(l.x + r.x, l.y + r.y);
}

bool operator==(Coord const & l, Coord const & r) {return l.x == r.x && l.y == r.y;}
bool operator!=(Coord const & l, Coord const & r) {return !(l == r);}

unsigned long score = 0;

uint8_t grid[4][4] = {
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
};

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

void moveTile(Coord from, Coord to) {
  uint8_t value = grid[from.y][from.x];
  grid[from.y][from.x] = 0;
  grid[to.y][to.x] = value;
}

void calculateGridMovement(Coord originPosition, Coord secondaryDirection, Coord primaryDirection) {
  bool didAnyMove = false;

  for(int i = 0; i < 4; i++) {
    uint8_t prevValue = 0xFF;
    Coord startPosition = originPosition + secondaryDirection * i;
    Coord prevPosition = startPosition + primaryDirection * -1;
    for(int k = 0; k < 4; k++) {
      Coord position = startPosition + primaryDirection * k;
      uint8_t value = grid[position.y][position.x];
      if (value == 0) continue; // skip empty
      if (value == prevValue) { // merging
        moveTile(position, prevPosition);
        didAnyMove = true;
        // increase numbers:
        grid[prevPosition.y][prevPosition.x]++;
        score += 1 << (value + 1);
        // can not merge twice, so move on:
        prevValue = 0;
        prevPosition += primaryDirection;
      } else if (prevValue == 0) { // moving to an empty spot
        moveTile(position, prevPosition);
        didAnyMove = true;
        prevValue = value; // for next merge check
      } else { // can not merge, closing the gap if any
        prevPosition += primaryDirection;
        if(prevPosition != position) {
          moveTile(position, prevPosition);
          didAnyMove = true;
        }
        prevValue = value;
      }
    }
  }

  if (didAnyMove) {
    fillRandomEmptySpot();
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
    calculateGridMovement(Coord(0, 0), Coord(0, 1), Coord(1, 0));
  }

  if (downDirectionButton.loopAndIsJustPressed()) {
    calculateGridMovement(Coord(0, 3), Coord(1, 0), Coord(0, -1));
  }

  if (upDirectionButton.loopAndIsJustPressed()) {
    calculateGridMovement(Coord(0, 0), Coord(1, 0), Coord(0, 1));
  }

  if (rightDirectionButton.loopAndIsJustPressed()) {
    calculateGridMovement(Coord(3, 0), Coord(0, 1), Coord(-1, 0));
  }

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(70, 6);
  display.println(F("Score:"));
  display.setCursor(70, 15);
  display.println(score);

  display.display();

  delay(20);
}