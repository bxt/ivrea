
#define LED_SIZE 8

#define LEFT_POTENTIOMETER_PIN A6
#define RIGHT_POTENTIOMETER_PIN A7

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

void setup() {
  cleanBuffer();

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
  cleanBuffer();

  int leftPaddleReading = analogRead(LEFT_POTENTIOMETER_PIN);
  int rightPaddleReading = analogRead(RIGHT_POTENTIOMETER_PIN);

  buffer[0] = (leftPaddleReading) % 256;
  buffer[1] = (leftPaddleReading / 256) % 256;

  buffer[LED_SIZE - 1] = (rightPaddleReading) % 256;
  buffer[LED_SIZE - 2] = (rightPaddleReading / 256) % 256;

  displayBuffer();
}