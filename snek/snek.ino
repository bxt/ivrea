#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OSFS.h>
#include <EEPROM.h>
#include "splash.h"
#include "game_over.h"
#include "osfsEeprom.h"

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

// How fast the game runs:
#define FRAME_DELAY 100ul

#define FIELD_WIDTH 21  // field width, in snake piece sizes
#define FIELD_HEIGHT 12 // field height, in snake piece sizes
#define FIELD_UNIT 4    // snake piece size

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
};

DirectionButton leftDirectionButton(PIN_LEFT);
DirectionButton downDirectionButton(PIN_DOWN);
DirectionButton upDirectionButton(PIN_UP);
DirectionButton rightDirectionButton(PIN_RIGHT);

void displaySplashScreen() {
  display.clearDisplay();
  display.drawBitmap(16, 2, splash_bmp, SPLASH_BMP_WIDTH, SPLASH_BMP_HEIGHT, 1);
  display.setCursor(13, 55);
  display.println(F("- press any key -"));
  display.display();
}

void waitForAnyButtonPressed() {
  while (!(leftDirectionButton.loopAndIsJustPressed()) &&
         !(downDirectionButton.loopAndIsJustPressed()) &&
         !(upDirectionButton.loopAndIsJustPressed()) &&
         !(rightDirectionButton.loopAndIsJustPressed())) {
    // Wait for user to press any key
  }
}

uint8_t highScore = 0;
bool gameOver = false;
uint8_t snekLength = 3;
uint8_t snekCounts[FIELD_WIDTH * FIELD_HEIGHT] = {0};
uint8_t directionX = 1;
uint8_t directionY = 0;
uint8_t positionX = 0;
uint8_t positionY = 0;

uint8_t mouseX = 0;
uint8_t mouseY = 0;

void loadHighscore() {
  OSFS::getFile("snekhiscore", highScore);
}

void saveHighscore() {
  OSFS::newFile("snekhiscore", highScore, true);
}

void setMousePosition() {
  do {
    mouseX = random(0, FIELD_WIDTH);
    mouseY = random(0, FIELD_HEIGHT);
  } while (snekCounts[mouseY * FIELD_WIDTH + mouseX] > 0);
}

void initializeGame() {
  gameOver = false;
  snekLength = 3;
  for (uint8_t *snekCount = &snekCounts[FIELD_WIDTH * FIELD_HEIGHT - 1];
       snekCount >= snekCounts; snekCount--) {
    *snekCount = 0;
  }
  uint8_t *snekCount = &snekCounts[(FIELD_HEIGHT + 1) * FIELD_WIDTH / 2];
  *snekCount++ = 1;
  *snekCount++ = 2;
  *snekCount++ = 3;
  directionX = 1;
  directionY = 0;
  positionX = FIELD_WIDTH / 2 + 2;
  positionY = FIELD_HEIGHT / 2;

  mouseX = FIELD_WIDTH / 2 + 6;
  mouseY = FIELD_HEIGHT / 2;
}

void setup() {
  Serial.begin(9600);

  leftDirectionButton.setup();
  downDirectionButton.setup();
  upDirectionButton.setup();
  rightDirectionButton.setup();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for this one!
    Serial.println(F("SSD1306 allocation failed"));
    while(1) {} // Infinite loop so we don't go to the loop() function
  }

  loadHighscore();

  display.setTextColor(SSD1306_WHITE);

  displaySplashScreen();
  waitForAnyButtonPressed();
  randomSeed(micros());
  initializeGame();
}

bool processInput() {
  if (directionX == 0) {
    if (leftDirectionButton.loopAndIsJustPressed()) {
      directionX = -1;
      directionY = 0;
      return true;
    }
    if (rightDirectionButton.loopAndIsJustPressed()) {
      directionX = 1;
      directionY = 0;
      return true;
    }
  } else if (directionY == 0) {
    if (downDirectionButton.loopAndIsJustPressed()) {
      directionX = 0;
      directionY = 1;
      return true;
    }
    if (upDirectionButton.loopAndIsJustPressed()) {
      directionX = 0;
      directionY = -1;
      return true;
    }
  }
  return false;
}

void advanceGameState() {
  positionX += directionX;
  positionY += directionY;

  if (positionX >= FIELD_WIDTH || positionY >= FIELD_HEIGHT) { // out of bounds
    gameOver = true;
  } else if (snekCounts[positionY * FIELD_WIDTH + positionX] > 0) { // tail bite
    gameOver = true;
  } else {
    bool eaten = positionX == mouseX && positionY == mouseY;
    if (eaten) {
      snekLength++;
    } else {
      for (uint8_t *snekCount = &snekCounts[FIELD_WIDTH * FIELD_HEIGHT - 1];
           snekCount >= snekCounts; snekCount--) {
        if (*snekCount > 0) {
          (*snekCount)--;
        }
      }
    }

    snekCounts[positionY * FIELD_WIDTH + positionX] = snekLength;

    if (eaten) {
      setMousePosition();
    }
  }
}

void renderGame() {
  display.clearDisplay();

  int screenXOffset = (SCREEN_WIDTH - FIELD_WIDTH * FIELD_UNIT) / 2;
  int screenYOffset = (SCREEN_HEIGHT - FIELD_HEIGHT * FIELD_UNIT) / 2 + 3;

  // snake:
  for (uint8_t y = 0; y < FIELD_HEIGHT; y++) {
    for (uint8_t x = 0; x < FIELD_WIDTH; x++) {
      uint8_t snekCount = snekCounts[y * FIELD_WIDTH + x];
      if (snekCount > 0) {
        int8_t additionalWidth = -1;
        if (x < FIELD_WIDTH - 1) {
          uint8_t leftSnekCount = snekCounts[y * FIELD_WIDTH + x + 1];
          if (leftSnekCount == snekCount + 1 || (leftSnekCount == snekCount - 1 && leftSnekCount != 0)) {
            additionalWidth = 0;
          }
        }
        int8_t additionalHeight = -1;
        if (y < FIELD_HEIGHT - 1) {
          uint8_t bottomSnekCount = snekCounts[(y + 1) * FIELD_WIDTH + x];
          if (bottomSnekCount == snekCount + 1 || (bottomSnekCount == snekCount - 1 && bottomSnekCount != 0)) {
            additionalHeight = 0;
          }
        }
        display.fillRect(x * FIELD_UNIT + screenXOffset, y * FIELD_UNIT + screenYOffset, FIELD_UNIT + additionalWidth, FIELD_UNIT + additionalHeight, 1);
      }
    }
  }

  // eye:
  display.drawPixel(positionX * FIELD_UNIT + screenXOffset + 1, positionY * FIELD_UNIT + screenYOffset + 1, 0);

  // food:
  display.fillCircle(mouseX * FIELD_UNIT + screenXOffset + 1, mouseY * FIELD_UNIT + screenYOffset + 1, FIELD_UNIT/2 - 1, 1);

  // bounds:
  display.drawRect(screenXOffset - 2, screenYOffset - 2, FIELD_UNIT * FIELD_WIDTH + 3, FIELD_UNIT * FIELD_HEIGHT + 3, 1);

  // current score:
  display.setCursor(screenXOffset - 2, 0);
  display.println(snekLength - 3);

  display.display();
}

void handleGameOver() {
  if (gameOver) {
    delay(750);

    uint8_t score = snekLength - 3;

    display.clearDisplay();
    display.drawBitmap(16, 2, game_over_bmp, GAME_OVER_BMP_WIDTH, GAME_OVER_BMP_HEIGHT, 1);
    display.setCursor(13, 55);
    display.print(F("Sc. "));
    display.print(score);
    display.setCursor(61, 55);
    display.print(F("High "));
    if (score > highScore) {
      display.print(F("NEW"));
    } else {
      display.print(highScore);
    }
    display.display();

    if (score > highScore) {
      highScore = score;
      saveHighscore();
    }

    delay(250);
    waitForAnyButtonPressed();

    initializeGame();
  }
}

void loop() {
  unsigned long pollingEnd = millis() + FRAME_DELAY;
  uint8_t didProcessInputTimes = 0;
  while (!gameOver && didProcessInputTimes < 3 && millis() < pollingEnd) {
    if (processInput()) {
      advanceGameState();
      didProcessInputTimes++;
    }
  }

  if (!gameOver && didProcessInputTimes == 0) {
    advanceGameState();
  }

  renderGame();

  handleGameOver();
}
