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


#define PIN_LEFT 7
#define PIN_DOWN 8
#define PIN_UP 9
#define PIN_RIGHT 10

void setup() {
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for this one!
    Serial.println(F("SSD1306 allocation failed"));
    while(1) {} // Infinite loop so we don't go to the loop() function
  }

  pinMode(PIN_LEFT, INPUT);
  pinMode(PIN_DOWN, INPUT);
  pinMode(PIN_UP, INPUT);
  pinMode(PIN_RIGHT, INPUT);
}

int posX = 0;
int posY = 0;

void loop() {
  for (int i = 0; i < 16; i++) {
    display.clearDisplay();

    display.drawBitmap(posX, posY,  tile_bmps[i], TILE_BMP_WIDTH, TILE_BMP_HEIGHT, 1);
    display.display();


    if (digitalRead(PIN_LEFT) == LOW && posX > 0) {
      posX--;
    }

    if (digitalRead(PIN_RIGHT) == LOW && posX < (display.width() - TILE_BMP_WIDTH )) {
      posX++;
    }

    if (digitalRead(PIN_UP) == LOW && posY > 0) {
      posY--;
    }

    if (digitalRead(PIN_DOWN) == LOW && posY < (display.height() - TILE_BMP_HEIGHT)) {
      posY++;
    }

    delay(20);
  }
}