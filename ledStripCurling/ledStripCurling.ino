#include <limits.h>
#include <FastLED.h>

// Hook up the LED strip
// How many LEDS do we have? 5m with 60 per meter:
#define NUM_LEDS 300
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

// How many rounds are in a game
#define ENDS_COUNT 4

// How many shots a player has
#define SHOT_COUNT 3

// How big the target size is
#define HOUSE_SIZE 10

// State of the LEDs, basically our "VRAM":
CRGB leds[NUM_LEDS];

void waitForButtonDown() {
  while(digitalRead(BUTTON_PIN) != BUTTON_ACTIVE);
}
void waitForButtonUp() {
  while(digitalRead(BUTTON_PIN) == BUTTON_ACTIVE);
}
void waitForButtonPress() {
  waitForButtonDown();
  delay(DEBOUNCE_DELAY);
  waitForButtonUp();
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

CRGB playerColors[] = {
  0xFF5500,
  0x00FF00,
  // 0x5500FF,
  // 0xFF0055,
};

const int playerCount = sizeof(playerColors) / sizeof(CRGB);

uint8_t playerScores[playerCount] = {0};

int shotsFired[playerCount][SHOT_COUNT] = {0};

void displayTarget() {
  if(target + 1 < NUM_LEDS) {
    leds[target + 1] = CRGB::Red;
  }
  if(target - 1 >= 0) {
    leds[target - 1] = CRGB::Red;
  }
  if(target + HOUSE_SIZE < NUM_LEDS) {
    leds[target + HOUSE_SIZE] = CRGB::Blue;
  }
  if(target - HOUSE_SIZE >= 0) {
    leds[target - HOUSE_SIZE] = CRGB::Blue;
  }
}

void displayShots() {
  for(int playerIndex = 0; playerIndex < playerCount; playerIndex++) {
    for(int shotIndex = 0; shotIndex < SHOT_COUNT; shotIndex++) {
      int shotAt = shotsFired[playerIndex][shotIndex];
      if(shotAt >= 0) {
        leds[shotAt] = playerColors[playerIndex];
      }
    }
  }
}

void resetShots() {
  for(int playerIndex = 0; playerIndex < playerCount; playerIndex++) {
    for(int shotIndex = 0; shotIndex < SHOT_COUNT; shotIndex++) {
      shotsFired[playerIndex][shotIndex] = -1;
    }
  }
}

void resetScores() {
  for(int playerIndex = 0; playerIndex < playerCount; playerIndex++) {
    playerScores[playerIndex] = 0;
  }
}

void renderGame() {
  allLedsTo(CRGB::Black);
  displayTarget();
  displayShots();
  FastLED.show();
}

int performShot() {
  waitForButtonDown();

  delay(DEBOUNCE_DELAY);

  int shotAt = 0;
  for(; shotAt < NUM_LEDS; shotAt++) {
    if(digitalRead(BUTTON_PIN) != BUTTON_ACTIVE) {
      break;
    }
    tone(BUZZER_PIN, 220 + shotAt);
    delay(8);
  }

  if(digitalRead(BUTTON_PIN) == BUTTON_ACTIVE) { // messed up, out of bounds:
    shotAt = -1;

    for(int i = 0; i < 100; i++) {
      tone(BUZZER_PIN, 220 + NUM_LEDS + i * 4);
      delay(1);
    }
    noTone(BUZZER_PIN);
  } else {
    for(int i = shotAt; i >= 0; i--) {
      tone(BUZZER_PIN, 220 + i);
      delay(1);
    }
  }

  delay(DEBOUNCE_DELAY);
  waitForButtonUp();
  noTone(BUZZER_PIN);

  return shotAt;
}

int getShotDistance(int playerIndex, int shotIndex) {
  int shotAt = shotsFired[playerIndex][shotIndex];
  if(shotAt < 0) return INT_MAX;
  return abs(target - shotAt);
}

void calculateScores() {
  int closestShotDistance = INT_MAX;
  int closestPlayerIndex = -1;
  for(int playerIndex = 0; playerIndex < playerCount; playerIndex++) {
    for(int shotIndex = 0; shotIndex < SHOT_COUNT; shotIndex++) {
      int shotDistance = getShotDistance(playerIndex, shotIndex);
      if (shotDistance < closestShotDistance) {
        closestShotDistance = shotDistance;
        closestPlayerIndex = playerIndex;
      }
    }
  }

  int closestShotDistanceOtherPlayers = INT_MAX;
  for(int playerIndex = 0; playerIndex < playerCount; playerIndex++) {
    if(playerIndex == closestPlayerIndex) continue;
    for(int shotIndex = 0; shotIndex < SHOT_COUNT; shotIndex++) {
      int shotDistance = getShotDistance(playerIndex, shotIndex);
      if (shotDistance < closestShotDistanceOtherPlayers) {
        closestShotDistanceOtherPlayers = shotDistance;
      }
    }
  }

  int endScore = 0;
  for(int shotIndex = 0; shotIndex < SHOT_COUNT; shotIndex++) {
    int shotDistance = getShotDistance(closestPlayerIndex, shotIndex);
    if (shotDistance < closestShotDistanceOtherPlayers) {
      endScore++;
    }
  }

  playerScores[closestPlayerIndex] += endScore;
}

void renderScores() {
  allLedsTo(CRGB::Black);

  int dot = NUM_LEDS;
  for(int playerIndex = 0; playerIndex < playerCount; playerIndex++) {
    for(int i = 0; i < playerScores[playerIndex]; i++) {
      leds[--dot] = playerColors[playerIndex];
    }
  }

  FastLED.show();
}

void winningCelebration() {
  int maxScore = 0;
  int winningPlayerIndex = 0;
  for(int playerIndex = 0; playerIndex < playerCount; playerIndex++) {
    int playerScore = playerScores[playerIndex];
    if(playerScore > maxScore) {
      winningPlayerIndex = playerIndex;
      maxScore = playerScore;
    }
  }

  CRGB playerColor = playerColors[winningPlayerIndex];

  int i = 0;
  while(i < NUM_LEDS || digitalRead(BUTTON_PIN) != BUTTON_ACTIVE) {
    for(int k = 0; k < min(i, NUM_LEDS); k++) {
      CRGB ledColor = playerColor;
      ledColor %= -((NUM_LEDS + i + k) * 10);
      leds[k] = ledColor;
    }

    FastLED.show();
    i++;
  }
  delay(DEBOUNCE_DELAY);
  waitForButtonUp();
  delay(DEBOUNCE_DELAY);
}

bool isMultipleWinners() {
  int maxScore = 0;
  for(int playerIndex = 0; playerIndex < playerCount; playerIndex++) {
    int playerScore = playerScores[playerIndex];
    if(playerScore > maxScore) {
      maxScore = playerScore;
    }
  }

  int winnerCount = 0;
  for(int playerIndex = 0; playerIndex < playerCount; playerIndex++) {
    int playerScore = playerScores[playerIndex];
    if(playerScore == maxScore) {
      winnerCount++;
    }
  }

  return winnerCount != 1;
}

void pushAway(int lastShotAt) {
  if (lastShotAt < 0) return;

  for(int playerIndex = 0; playerIndex < playerCount; playerIndex++) {
    for(int shotIndex = 0; shotIndex < SHOT_COUNT; shotIndex++) {
      int shotAt = shotsFired[playerIndex][shotIndex];
      if (shotAt < 0) continue;
      if (shotAt == lastShotAt) {
        int newShotAt;
        if(shotAt < target) {
          newShotAt = shotAt - 1;
        } else {
          newShotAt = shotAt + 1;
        }
        pushAway(newShotAt);
        shotsFired[playerIndex][shotIndex] = newShotAt;
      }
    }
  }
}

void loop() {
  for(int endIndex = 0; endIndex < ENDS_COUNT || isMultipleWinners(); endIndex++) {
    resetShots();
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
        int shotAt = performShot();

        pushAway(shotAt);
        shotsFired[playerIndex][shotIndex] = shotAt;

        renderGame();
      }
    }

    delay(1000);

    renderScores();
    delay(500);
    calculateScores();
    renderScores();
    delay(1500);
  }

  winningCelebration();

  resetScores();
}