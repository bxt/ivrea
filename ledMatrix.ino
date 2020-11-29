
#define LED_SIZE 8

// On one side there's "1588BS" written on my LED matrix
// If you look at the modules form this side the pinout seems to be:
// HG1A3FD0 (the side without the "1588BS")
// 46BC7E52 (the side you're looking at with the "1588BS")
// Letters are the columns and the cathodes/ground:
//       Matrix col:     A    B  C   D   E   F   G   H
uint8_t ledCathodes[] = {A0, A1, A2, A3, A4, A5, 11, 12};
// Numbers are the rows and the anodes:
//       Matrix row:   0  1  2  3  4  5  6  7
uint8_t ledAnodes[] = {2, 3, 4, 5, 6, 7, 8, 9};

uint8_t buffer[LED_SIZE];

void cleanBuffer() {
  for (int i = 0; i < LED_SIZE; i++) {
    buffer[i] = 0;
  }
}

void displayBuffer() {
  for (int x = 0; x < LED_SIZE; x++) {
    pinMode(ledCathodes[x], OUTPUT);
    uint8_t columnData = buffer[x];
    for (int y = 0; y < LED_SIZE; y++) {
      if (columnData & 0x80) {
        digitalWrite(ledAnodes[y], HIGH);
      }
      columnData <<= 1;
    }
    for (int y = 0; y < LED_SIZE; y++) {
      digitalWrite(ledAnodes[y], LOW);
    }
    pinMode(ledCathodes[x], INPUT);
  }
}

void drawBitmap(const uint8_t *data, int xOffset, int yOffset) {
  for (int i = 0; i < LED_SIZE; i++) {
    uint8_t datum = pgm_read_byte(data + i);
    uint8_t shiftedDatum = yOffset < 0 ? (datum << -yOffset) : (datum >> yOffset);
    buffer[(i + xOffset) % LED_SIZE] |= shiftedDatum;
  }
}

typedef void (*effectFunction)(const int t);

struct effect {
  effectFunction function;
  unsigned int length;
};

void lightAllOnce(const int t) {
  buffer[(t / 8) % LED_SIZE] = 1 << (t % LED_SIZE);
}

void lightRows(const int t) {
  buffer[t % LED_SIZE] = 0xFF;
}

void lightColumns(const int t) {
  for (int i = 0; i < LED_SIZE; i++) {
    buffer[i] = 1 << (t % LED_SIZE);
  }
}

void binaryCounting(const int t) {
  for (int i = 0; i < LED_SIZE; i++) {
    if (t + i > 8) {
      buffer[i] = (t + i - 8) % 256;
    }
  }
}

void squares(const int t) {
  int t2 = t / 4;
  int tl = t2 % 8 < 4 ? t2 % 4 : 3 - (t2 % 4);
  for (int x = 0; x < LED_SIZE; x++) {
    for (int y = 0; y < LED_SIZE; y++) {
      int xDist = (abs(x * 2 - 7) - 1) / 2;
      int yDist = (abs(y * 2 - 7) - 1) / 2;
      if ((xDist == tl && yDist <= tl) || (yDist == tl && xDist <= tl)) {
        buffer[x] = buffer[x] | (1 << y);
      }
    }
  }
}

const uint8_t PROGMEM heart[] = {
  B01101100,
  B11111110,
  B11111110,
  B01111100,
  B00111000,
  B00010000,
  B00000000,
  B00000000,
};

void hearts(const int t) {
  int xOffset = (t >> 4) % 3;
  int yOffset = (t >> 3) & 1;
  drawBitmap(heart, xOffset, yOffset);
}

const uint8_t PROGMEM letterH[] = {
  B00000000,
  B1001000,
  B1001000,
  B1111000,
  B1001000,
  B1001000,
  B00000000,
  B00000000,
};
const uint8_t PROGMEM letterI[] = {
  B00000000,
  B01000000,
  B00000000,
  B01000000,
  B01000000,
  B11100000,
  B00000000,
  B00000000,
};
const uint8_t PROGMEM exclamationMark[] = {
  B00000000,
  B11100000,
  B11100000,
  B01000000,
  B00000000,
  B01000000,
  B00000000,
  B00000000,
};
const uint8_t PROGMEM letterS[] = {
  B00000000,
  B11110000,
  B00001000,
  B00110000,
  B01000000,
  B00111000,
  B00000000,
  B00000000,
};
const uint8_t PROGMEM letterU[] = {
  B00000000,
  B00000000,
  B00000000,
  B10100000,
  B10100000,
  B01000000,
  B00000000,
  B00000000,
};
const uint8_t PROGMEM letterP[] = {
  B00000000,
  B00000000,
  B00000000,
  B01100000,
  B10100000,
  B01100000,
  B00100000,
  B00100000,
};

const uint8_t PROGMEM questionMark[] = {
  B00000000,
  B01100000,
  B10010000,
  B01000000,
  B00000000,
  B01000000,
  B00000000,
  B00000000,
};

void letters(const int t) {
  int len = 34;
  int yOffset = (t / 4) % len - len;

  if (yOffset < -26)
    drawBitmap(letterH, 0, yOffset + 34);
  if (yOffset < -23 && yOffset > -35)
    drawBitmap(letterI, 0, yOffset + 31);
  if (yOffset < -19 && yOffset > -31)
    drawBitmap(exclamationMark, 0, yOffset + 27);

  if (yOffset < -12 && yOffset > -24)
    drawBitmap(letterS, 0, yOffset + 20);
  if (yOffset < -9 && yOffset > -21)
    drawBitmap(letterU, 0, yOffset + 17);
  if (yOffset < -5 && yOffset > -17)
    drawBitmap(letterP, 0, yOffset + 13);
  if (yOffset < -1 && yOffset > -13)
    drawBitmap(questionMark, 0, yOffset + 9);

  if (yOffset > -8)
    drawBitmap(letterH, 0, yOffset);
}

void checkerboard(const int t) {
  for (int i = 0; i < LED_SIZE; i++) {
    buffer[i] = B10101010 ^ ((t / 8 % 2 > 0) == (i & 1 > 0) ? 0xFF : 0);
  }
}

const uint8_t PROGMEM smiley[] = {
  B00000000,
  B00000000,
  B00101000,
  B00000000,
  B01000100,
  B00111000,
  B00000000,
  B00000000,
};

void smile(const int t) {
  drawBitmap(smiley, 0, 0);
}

void sierpinsky(const int t) {
  int t2 = t / 8;
  for (int x = 0; x < LED_SIZE; x++) {
    for (int y = 0; y < LED_SIZE; y++) {
      if ((x & y & t2) > 0) {
        buffer[x] = buffer[x] | (1 << y);
      }
    }
  }
}

void fill1(const int t) {
  for (int i = 0; i < LED_SIZE; i++) {
    int shift = 8 - t + i;
    buffer[i] = 0xFF << (shift > 0 ? shift : 0);
  }
}

void empty1(const int t) {
  for (int i = 0; i < LED_SIZE; i++) {
    buffer[i] = 0xFF << t + i;
  }
}

void fill2(const int t) {
  for (int i = 0; i < LED_SIZE; i++) {
    int shift = 8 - t + i;
    buffer[7 - i] = 0xFF << (shift > 0 ? shift : 0);
  }
}

void empty2(const int t) {
  for (int i = 0; i < LED_SIZE; i++) {
    buffer[7 - i] = 0xFF << t + i;
  }
}

struct effect effects[] = {
  {lightAllOnce, 64},
  {fill1, 16},
  {empty2, 16},
  {fill2, 16},
  {empty1, 16},
  {smile, 24},
  {checkerboard, 32},
  {sierpinsky, 64},
  {squares, 96},
  {letters, 256},
  {hearts, 96},
  {binaryCounting, 256},
  {lightRows, 8},
  {lightColumns, 8},
};
unsigned long effectsLengthSum = 0;

void setup() {
  cleanBuffer();

  for (int i = 0; i < sizeof(effects) / sizeof(struct effect); i++) {
    effectsLengthSum += effects[i].length;
  }

  for (int i = 0; i < LED_SIZE; i++) {
    digitalWrite(ledCathodes[i], LOW);
    pinMode(ledCathodes[i], INPUT);
  }
  for (int i = 0; i < LED_SIZE; i++) {
    digitalWrite(ledAnodes[i], LOW);
    pinMode(ledAnodes[i], OUTPUT);
  }
}

void loop() {
  unsigned long t = (millis() / 32ul) % effectsLengthSum;
  cleanBuffer();

  for (int i = 0; true; i++) {
    if (t < effects[i].length) {
      (*(effects[i].function))(t);
      break;
    } else {
      t -= effects[i].length;
    }
  }

  displayBuffer();
}