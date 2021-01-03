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
#define DIRECTION_BUTTON_ACTIVE LOW
#define DEBOUNCE_DELAY 50ul

// State of the LEDs, basically our "VRAM":
CRGB leds[NUM_LEDS];

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

DirectionButton fireButton(BUTTON_PIN);

const int totalSteps = (NUM_LEDS * 2 - 1);

int zigzag(int i) {
  i = i % totalSteps;
  if (i >= NUM_LEDS) return totalSteps - i;
  return i;
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
}

int rOffset = 0;
int gOffset = 100;
int bOffset = 140;

enum Channel { r, g, b };
Channel currentChannel = r;

void loop() {
  for(int dot = 0; dot < totalSteps; dot++) {
    for(uint8_t i = 0; i < 25; i++) {
      uint8_t v = i * 10;
      leds[zigzag(dot + rOffset + i)].r = v;
      leds[zigzag(dot + gOffset + i)].g = v;
      leds[zigzag(dot + bOffset + i)].b = v;
    }

    if(fireButton.loopAndIsJustPressed()) {
      switch (currentChannel) {
        case r:
          for(uint8_t i = 0; i < 25; i++) {
            leds[zigzag(dot + rOffset + i)].r = 0;
          }
          rOffset = totalSteps - dot;
          currentChannel = g;
          break;
        case g:
          for(uint8_t i = 0; i < 25; i++) {
            leds[zigzag(dot + gOffset + i)].g = 0;
          }
          gOffset = totalSteps - dot;
          currentChannel = b;
          break;
        case b:
          for(uint8_t i = 0; i < 25; i++) {
            leds[zigzag(dot + bOffset + i)].b = 0;
          }
          bOffset = totalSteps - dot;
          currentChannel = r;
          break;
      }
    }

    FastLED.show();

    //delay(30);
  }
}