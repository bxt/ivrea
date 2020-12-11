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

#define EEPROM_MAGIC_NUMBER 0x42 // Should be unique for this program

#define TILE_MOVEMENT_FACTOR 4 // Animation speeed

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
  Coord(int8_t x, int8_t y) : x(x), y(y) {}

  Coord &operator+=(Coord const &r) {
    x += r.x;
    y += r.y;
    return *this;
  }
};

Coord operator*(Coord const &l, int8_t const &r) { return Coord(l.x * r, l.y * r); }
Coord operator+(Coord const &l, Coord const &r) { return Coord(l.x + r.x, l.y + r.y); }
Coord operator-(Coord const &l, Coord const &r) { return Coord(l.x - r.x, l.y - r.y); }

bool operator==(Coord const &l, Coord const &r) {
  return l.x == r.x && l.y == r.y;
}
bool operator!=(Coord const &l, Coord const &r) { return !(l == r); }

// Game state:

bool gameOver = false;
bool victoryOccured = false;
int victoryCelebrated = false;
unsigned long score = 0;
unsigned long highScore = 0;

// Here 0: empty, 1: "2", 2: "4", 3: "8", 4: "16" etc.
uint8_t grid[4][4] = {
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
};

// Save next tiles in this grid while they are beeing animated
uint8_t nextGrid[4][4] = {
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
};

void loadHighscore() {
  int eepromAddress = 0;
  uint8_t magicNumber;
  EEPROM.get(eepromAddress, magicNumber);
  if (magicNumber == EEPROM_MAGIC_NUMBER) {
    eepromAddress += sizeof(uint8_t);
    EEPROM.get(eepromAddress, highScore);
  } else {
    highScore = 0;
  }
}

void saveHighscore() {
  int eepromAddress = 0;
  uint8_t magicNumber = EEPROM_MAGIC_NUMBER;
  EEPROM.put(eepromAddress, magicNumber);
  eepromAddress += sizeof(uint8_t);
  EEPROM.put(eepromAddress, highScore);
}

void resetGame() {
  if (score > highScore) {
    highScore = score;
    saveHighscore();
  }

  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      nextGrid[y][x] = 0;
    }
  }

  gameOver = false;
  victoryOccured = false;
  victoryCelebrated = false;
  score = 0;

  fillRandomEmptySpot();
  flushTileMovements();
}

void fillRandomEmptySpot() {
  uint8_t emptySpotCount = 0;

  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      uint8_t value = nextGrid[y][x];
      if (value == 0)
        emptySpotCount++;
    }
  }

  uint8_t fillIndex = random(0, emptySpotCount);
  uint8_t fillValue = random(0, 100) < 10 ? 2 : 1;

  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      uint8_t value = nextGrid[y][x];
      if (value == 0) {
        emptySpotCount--;

        if (fillIndex == emptySpotCount) {
          nextGrid[y][x] = fillValue;
          return;
        }
      }
    }
  }
}

int movementTicksLeft = -1;

size_t movementGhostTileCount = 0;
Coord movementGhostTilePositions[12] = {Coord(0, 0), Coord(0, 0), Coord(0, 0),
                                        Coord(0, 0), Coord(0, 0), Coord(0, 0),
                                        Coord(0, 0), Coord(0, 0), Coord(0, 0),
                                        Coord(0, 0), Coord(0, 0), Coord(0, 0)};
Coord movementGhostTileSpeeds[12] = {Coord(0, 0), Coord(0, 0), Coord(0, 0),
                                     Coord(0, 0), Coord(0, 0), Coord(0, 0),
                                     Coord(0, 0), Coord(0, 0), Coord(0, 0),
                                     Coord(0, 0), Coord(0, 0), Coord(0, 0)};
uint8_t movementGhostTileValues[12] = {0};

void moveTile(Coord from, Coord to) {
  uint8_t value = grid[from.y][from.x];
  grid[from.y][from.x] = 0;
  nextGrid[from.y][from.x] = 0;
  nextGrid[to.y][to.x] = value;

  movementTicksLeft = TILE_BMP_WIDTH / TILE_MOVEMENT_FACTOR;
  movementGhostTilePositions[movementGhostTileCount] = from * TILE_BMP_WIDTH;
  movementGhostTileSpeeds[movementGhostTileCount] = (to - from) * TILE_MOVEMENT_FACTOR;
  movementGhostTileValues[movementGhostTileCount] = value;
  movementGhostTileCount++;
}

void flushTileMovements() {
  movementTicksLeft = -1;
  movementGhostTileCount = 0;

  bool canMerge = false;
  bool hasEmtpy = false;
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      grid[y][x] = nextGrid[y][x];
      if (y < 3 && nextGrid[y][x] == nextGrid[y + 1][x]) {
        canMerge = true;
      }
      if (x < 3 && nextGrid[y][x] == nextGrid[y][x + 1]) {
        canMerge = true;
      }
      if (nextGrid[y][x] == 0) {
        hasEmtpy = true;
      }
    }
  }

  gameOver = !canMerge && !hasEmtpy;
}

void calculateGridMovement(Coord originPosition, Coord secondaryDirection,
                           Coord primaryDirection) {
  flushTileMovements();

  if (gameOver) {
    resetGame();
    return;
  }

  bool didAnyMove = false;

  for (int i = 0; i < 4; i++) { // for each row/col
    Coord startPosition = originPosition + secondaryDirection * i;

    // process items in direction of movement:
    uint8_t prevValue = 0xFF; // sentinel
    Coord prevPosition = startPosition - primaryDirection; // just outside the grid
    for (int k = 0; k < 4; k++) {
      Coord position = startPosition + primaryDirection * k;
      uint8_t value = grid[position.y][position.x];

      if (value == 0) { // empty
        continue; // just skip

      } else if (value == prevValue) { // merging
        moveTile(position, prevPosition);
        didAnyMove = true;

        // increase numbers:
        nextGrid[prevPosition.y][prevPosition.x]++;
        score += 1 << (value + 1);
        if (value == 10)
          victoryOccured = true;

        // can not merge same position twice, so move on:
        prevValue = 0;
        prevPosition += primaryDirection;

      } else if (prevValue == 0) { // moving to an empty spot
        moveTile(position, prevPosition);
        didAnyMove = true;
        prevValue = value; // for next merge check

      } else { // can not merge, closing the gap if any
        prevPosition += primaryDirection;
        if (prevPosition != position) {
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

  loadHighscore();

  fillRandomEmptySpot();
  flushTileMovements();

  leftDirectionButton.setup();
  downDirectionButton.setup();
  upDirectionButton.setup();
  rightDirectionButton.setup();
}

void loop() {
  // Victory celebration:
  if (victoryOccured && !victoryCelebrated) {
    for (int i = 0; i < ANIMATION_BMP_COUNT; i++) {
      display.clearDisplay();
      display.drawBitmap(0, 0, animation_bmps[i], ANIMATION_BMP_WIDTH,
                         ANIMATION_BMP_HEIGHT, 1);
      display.display();

      delay(32);
    }

    while (!(leftDirectionButton.loopAndIsJustPressed()) &&
           !(downDirectionButton.loopAndIsJustPressed()) &&
           !(upDirectionButton.loopAndIsJustPressed()) &&
           !(rightDirectionButton.loopAndIsJustPressed())) {
      // Wait for user to press any key
    }

    victoryCelebrated = true;
  }

  display.clearDisplay();

  // Display grid:
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      uint8_t value = grid[y][x];
      if (value == 0)
        continue;
      uint8_t posX = x * TILE_BMP_WIDTH;
      uint8_t posY = y * TILE_BMP_HEIGHT;
      display.drawBitmap(posX, posY, tile_bmps[value - 1], TILE_BMP_WIDTH,
                         TILE_BMP_HEIGHT, 1);
    }
  }

  // Display moving tiles:
  if (movementTicksLeft > 0) {
    for (int i = 0; i < movementGhostTileCount; i++) {
      Coord position = movementGhostTilePositions[i];
      uint8_t value = movementGhostTileValues[i];
      display.drawBitmap(position.x, position.y, tile_bmps[value - 1],
                         TILE_BMP_WIDTH, TILE_BMP_HEIGHT, 1);
      movementGhostTilePositions[i] += movementGhostTileSpeeds[i];
    }
    movementTicksLeft--;
  }
  if (movementTicksLeft == 0) {
    flushTileMovements();
    movementTicksLeft--;
  }

  // Handle input:

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

  // Display text:

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  if (gameOver) {
    display.setCursor(70, 6);
    display.println(F("GAME OVER"));
  }

  display.setCursor(70, 15);
  display.println(victoryOccured ? F("Winscore:") : F("Score:"));

  display.setCursor(70, 24);
  display.println(score);

  display.setCursor(70, 33);
  display.println(F("High:"));

  display.setCursor(70, 42);
  display.println(highScore);

  if (gameOver && score > highScore) {
    display.setCursor(70, 51);
    display.println(F("NEW!!!"));
  }

  display.display();
}