#include <FastLED.h>

// Hook up the LED strip
// How many LEDS do we have? 5m with 60 per meter:
#define NUM_LEDS 300
// Connect the (middle) data via some 150-400 Ohms resistor:
#define DATA_PIN 6
// ... plus connect red and black wires to 5V and GND of the Arduiono
// ... and put a 1000uF capacitor to catch power spikes

// State of the LEDs, basically our "VRAM":
CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  // To not hurt the eyes:
  FastLED.setBrightness(8);

  // We work with 5V LEDs. And since the power comes from USB (500mA) and we
  // want to have a reserve of 200mA for powering the Arduino itself and some
  // excess just in case, let's limit to 300mA:
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 300);
}

void loop() {
  for (int i = 0; i < NUM_LEDS; i += 3) {
    for (int k = 0; k < NUM_LEDS; k++) {
      // 300*17/20 = 255, and 3*17=51, so 3 rainbows:
      uint8_t hue = (k * 51 / 20) % 255;
      leds[(i + k) % NUM_LEDS] = CHSV(hue, 255, 255);
    }

    FastLED.show();

    // delay(30);
  }
}