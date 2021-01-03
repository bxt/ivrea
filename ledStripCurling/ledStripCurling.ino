#include <FastLED.h>

// Hook up the LED strip
// How many LEDS do we have? 5m with 60 per meter:
#define NUM_LEDS 300
// Save some cycles...:
#define NUM_LEDSTIMES2 600
// Connect the (middle) data via some 150-400 Ohms resistor:
#define DATA_PIN 6
// ... plus connect red and black wires to 5V and GND of the Arduiono
// ... and put a 1000uF capacitor to catch power spikes

// And connect a button, active low:
#define BUTTON_PIN 10
// ... and it should have a pull-up and be active low:
#define BUTTON_ACTIVE LOW
#define DEBOUNCE_DELAY 50ul

// And connect a button, active low:
#define BUZZER_PIN A0

// How many shots a player has
#define SHOT_COUNT 3

// State of the LEDs, basically our "VRAM":
CRGB leds[NUM_LEDS];

void waitForButtonPress() {
  while(digitalRead(BUTTON_PIN) != BUTTON_ACTIVE);
  delay(DEBOUNCE_DELAY);
  while(digitalRead(BUTTON_PIN) == BUTTON_ACTIVE);
  delay(DEBOUNCE_DELAY);
}

void allLedsTo(CRGB crgb) {
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = crgb;
  }
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  // To not hurt the eyes:
  FastLED.setBrightness(4);

  // We work with 5V LEDs. And since the power comes from USB (500mA) and we
  // want to have a reserve of 200mA for powering the Arduino itself and some
  // excess just in case, let's limit to 300mA:
  FastLED.setMaxPowerInVoltsAndMilliamps(5,300);

  allLedsTo(CRGB::Black);
  FastLED.show();

  waitForButtonPress();
  randomSeed(micros());
}

int target = 0;

CRGB playerColors[2] = {
  0xFF8800,
  0x00FF00,
};

const int playerCount = sizeof(playerColors) / sizeof(CRGB);

int shotsFired[playerCount][SHOT_COUNT] = {0};

void displayTarget() {
  leds[target] = CRGB::White;
  if(target + 10 < NUM_LEDS) {
    leds[target + 10] = CRGB::DarkSlateGrey;
  }
  if(target - 10 >= 0) {
    leds[target - 10] = CRGB::DarkSlateGrey;
  }
}

void displayShots() {
  for(int playerIndex = 0; playerIndex < playerCount; playerIndex++) {
    for(int shotIndex = 0; shotIndex < SHOT_COUNT; shotIndex++) {
      int shotAt = shotsFired[playerIndex][shotIndex];
      if(shotAt > 0) {
        leds[shotAt] = playerColors[playerIndex];
      }
    }
  }
}

void resetShots() {
  for(int playerIndex = 0; playerIndex < playerCount; playerIndex++) {
    for(int shotIndex = 0; shotIndex < SHOT_COUNT; shotIndex++) {
      shotsFired[playerIndex][shotIndex] = 0;
    }
  }
}

void renderGame() {
  allLedsTo(CRGB::Black);
  displayTarget();
  displayShots();
  FastLED.show();
}

void loop() {
  target = random(50, NUM_LEDS);

  renderGame();

  for(int playerIndex = 0; playerIndex < playerCount; playerIndex++) {
    CRGB playerColor = playerColors[playerIndex];

    delay(1000);

    allLedsTo(playerColor);
    FastLED.show();
    delay(1000);
    renderGame();

    for(int shotIndex = 0; shotIndex < SHOT_COUNT; shotIndex++) {
      while(digitalRead(BUTTON_PIN) != BUTTON_ACTIVE);

      delay(DEBOUNCE_DELAY);
      int shotAt = 0;
      for(; shotAt < NUM_LEDS; shotAt++) {
        if(digitalRead(BUTTON_PIN) != BUTTON_ACTIVE) {
          break;
        }
        tone(BUZZER_PIN, 220 + shotAt);
        delay(8);
      }
      for(int i = shotAt; i >= 0; i--) {
        tone(BUZZER_PIN, 220 + i);
        delay(1);
      }
      noTone(BUZZER_PIN);

      shotsFired[playerIndex][shotIndex] = shotAt;

      renderGame();
    }
  }

  delay(500);
  waitForButtonPress();
  resetShots();
}