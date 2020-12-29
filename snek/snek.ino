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

#define FIELD_WIDTH 21 // field width, in snake piece sizes
#define FIELD_HEIGHT 12 // field height, in snake piece sizes
#define FIELD_UNIT 4 // snake piece size

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

uint8_t snekLength = 3;
uint8_t snekCounts[FIELD_WIDTH * FIELD_HEIGHT] = {0};
uint8_t directionX = 1;
uint8_t directionY = 0;
uint8_t positionX = 0;
uint8_t positionY = 0;

uint8_t mouseX = 0;
uint8_t mouseY = 0;

void setMousePosition() {
  do {
    mouseX = random(0, FIELD_WIDTH);
    mouseY = random(0, FIELD_HEIGHT);
  } while (snekCounts[mouseY * FIELD_WIDTH + mouseX] > 0);
}

void initializeGame() {
  snekLength = 3;
  for(uint8_t *snekCount = &snekCounts[FIELD_WIDTH * FIELD_HEIGHT - 1]; snekCount > snekCounts; snekCount--) {
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
  initializeGame();
}

void loop() {
  if (buttons[0].loopAndIsJustPressed()) { // LEFT
    if(directionX == 0) {
      directionX = -1;
      directionY = 0;
    }
  }
  if (buttons[1].loopAndIsJustPressed()) { // DOWN
    if(directionY == 0) {
      directionX = 0;
      directionY = 1;
    }
  }
  if (buttons[2].loopAndIsJustPressed()) { // UP
    if(directionY == 0) {
      directionX = 0;
      directionY = -1;
    }
  }
  if (buttons[3].loopAndIsJustPressed()) { // RIGHT
    if(directionX == 0) {
      directionX = 1;
      directionY = 0;
    }
  }

  positionX += directionX;
  positionY += directionY;

  if (positionX >= FIELD_WIDTH || positionY >= FIELD_HEIGHT) { // out of bounds
    // Game over
    initializeGame();
  } else if (snekCounts[positionY * FIELD_WIDTH + positionX] > 0) { // tail bite
    // Game over
    initializeGame();
  } else {
    bool eaten = positionX == mouseX && positionY == mouseY;
    if (eaten) {
      snekLength++;
    } else {
      for(uint8_t *snekCount = &snekCounts[FIELD_WIDTH * FIELD_HEIGHT - 1]; snekCount > snekCounts; snekCount--) {
        if(*snekCount > 0) {
          (*snekCount)--;
        }
      }
    }

    snekCounts[positionY * FIELD_WIDTH + positionX] = snekLength;

    if(eaten) {
      setMousePosition();
    }
  }

  display.clearDisplay();

  int screenXOffset = (SCREEN_WIDTH - FIELD_WIDTH * FIELD_UNIT) / 2;
  int screenYOffset = (SCREEN_HEIGHT - FIELD_HEIGHT * FIELD_UNIT) / 2 + 3;

  for (uint8_t y = 0; y < FIELD_HEIGHT; y++) {
    for (uint8_t x = 0; x < FIELD_WIDTH; x++) {
      if(snekCounts[y * FIELD_WIDTH + x] > 0) {
        display.fillRect(x * FIELD_UNIT + screenXOffset, y * FIELD_UNIT + screenYOffset, FIELD_UNIT - 1, FIELD_UNIT - 1, 1);
      }
    }
  }

  display.fillRect(positionX * FIELD_UNIT + screenXOffset, positionY * FIELD_UNIT + screenYOffset, FIELD_UNIT - 1, FIELD_UNIT - 1, 1);

  display.fillCircle(mouseX * FIELD_UNIT + screenXOffset + 1, mouseY * FIELD_UNIT + screenYOffset + 1, FIELD_UNIT/2 - 1, 1);

  display.drawRect(screenXOffset - 2, screenYOffset - 2, FIELD_UNIT * FIELD_WIDTH + 4, FIELD_UNIT * FIELD_HEIGHT + 4, 1);

  display.setCursor(screenXOffset - 2, 0);
  display.println(snekLength - 3);

  display.display();

  delay(200);
}