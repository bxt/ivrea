
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Run:
// for i in wrodShuflfe/*.png; do go run transformBitmap.go $i; done
#include "splash.h"
#include "ongoing.h"
#include "success.h"

#include "debugWords.h"

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

#define CHARACTER_WIDTH 5
#define CHARACTER_HEIGHT 7
#define CHARACTER_GAP 1

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

// See https://de.wikipedia.org/wiki/Frequenzen_der_gleichstufigen_Stimmung
int winFrequencies[4] = {262, 330, 392, 523};

int wordIndex = 0;
uint8_t cursorPosition = 0;
bool pickedUp = false;

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
    digitalWrite(leds[i], buttons[i].getLastReading() == LOW ? HIGH : LOW);
  }
}

void exchange(char * start, size_t position1, size_t position2) {
  if(position1 != position2) {
    // Switch in-place:
    start[position1] ^= start[position2];
    start[position2] ^= start[position1];
    start[position1] ^= start[position2];
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

  while(strcmp(currentWord, shuffledWord) == 0) {
    // See https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle#The_modern_algorithm
    for(int i = 0; i < currentWordLength - 2; i++) {
      int j = random(i, currentWordLength);
      exchange(shuffledWord, i, j);
      if(i != j) {
        shuffledWord[i] ^= shuffledWord[j];
        shuffledWord[j] ^= shuffledWord[i];
        shuffledWord[i] ^= shuffledWord[j];
      }
    }
  }

  bool everythingCorrect = false;

  while (!everythingCorrect) {
    makeLedsMirrorButtons();

    if(buttons[0].loopAndIsJustPressed()) { // LEFT
      uint8_t previousCursorPosition = cursorPosition;
      cursorPosition = (currentWordLength + cursorPosition - 1) % currentWordLength;
      if(pickedUp) {
        exchange(shuffledWord, cursorPosition, previousCursorPosition);
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
      cursorPosition = (cursorPosition + 1) % currentWordLength;
      if(pickedUp) {
        exchange(shuffledWord, cursorPosition, previousCursorPosition);
      }
    }

    if (!pickedUp) {
      everythingCorrect = strcmp(currentWord, shuffledWord) == 0;
    }

    display.clearDisplay();

    // debug:
    // display.drawLine(0, SCREEN_HEIGHT/2, SCREEN_WIDTH, SCREEN_HEIGHT/2, 1);
    // display.drawLine(SCREEN_WIDTH/2, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT, 1);

    uint8_t textSize = 1;
    if (currentWordLength < 7) {
      textSize++;
    }
    if (currentWordLength < 10) {
      textSize++;
    }
    display.setTextSize(textSize);

    display.drawBitmap((SCREEN_WIDTH - ONGOING_BMP_WIDTH) / 2, (SCREEN_HEIGHT - ONGOING_BMP_HEIGHT) / 2, ongoing_bmp, ONGOING_BMP_WIDTH, ONGOING_BMP_HEIGHT, 1);
    int startX = (SCREEN_WIDTH - currentWordLength * (CHARACTER_WIDTH + CHARACTER_GAP) * textSize) / 2 + 1;
    uint8_t startY = (SCREEN_HEIGHT - CHARACTER_HEIGHT * textSize) / 2;
    display.setCursor(startX, startY);
    display.println(shuffledWord);

    uint8_t cursorX = startX + cursorPosition * (CHARACTER_WIDTH + CHARACTER_GAP) * textSize;
    if (pickedUp) {
      // erase original char:
      display.fillRect(cursorX, startY, CHARACTER_WIDTH * textSize, CHARACTER_HEIGHT * textSize, 0);
      display.setCursor(cursorX, startY - 3 * textSize);
      display.print(shuffledWord[cursorPosition]);
      display.fillRect(cursorX, startY + (CHARACTER_HEIGHT - 2) * textSize, CHARACTER_WIDTH * textSize, 1 + textSize, 1);
    } else {
      display.fillRect(cursorX, startY + (CHARACTER_HEIGHT + 1) * textSize, CHARACTER_WIDTH * textSize, 1 + textSize, 1);
    }
    display.setTextSize(1);

    display.display();
  }

  delay(200);

  displaySuccessScreen();

  for (int i = 0; i < 4; i++) {
    tone(BUZZER_PIN, winFrequencies[i], 100);
    digitalWrite(leds[0], HIGH);
    delay(100);
  }

  delay(900);

  turnAllLeds(LOW);

  wordIndex = (wordIndex+1) % WORD_COUNT;
  cursorPosition = 0;
  pickedUp = false;
}