
#define LED_SIZE 8

// On one side there's "1588BS" written on my LED matrix
// If you look at the modules form this side the pinout seems to be:
// HG1A3FD0 (the side without the "1588BS")
// 46BC7E52 (the side you're looking at with the "1588BS")
// Letters are the columns and the cathodes/ground:
uint8_t ledCathodes[] = {A0, A1, A2, A3, A4, A5, A6, A7};
// Numbers are the rows and the anodes:
uint8_t ledAnodes[] = {2, 3, 4, 5, 6, 7, 8, 9};

void setup() {
  for(int i = 0; i < LED_SIZE; i++) {
    digitalWrite(ledCathodes[i], LOW);
    pinMode(ledCathodes[i], INPUT);
  }
    for(int i = 0; i < LED_SIZE; i++) {
    digitalWrite(ledAnodes[i], LOW);
    pinMode(ledAnodes[i], OUTPUT);
  }
}

void loop() {
  for(int x = 0; x < LED_SIZE; x++) {
    pinMode(ledCathodes[x], OUTPUT);
    for(int y = 0; y < LED_SIZE; y++) {
      digitalWrite(ledAnodes[y], HIGH);
      delay(300);
      digitalWrite(ledAnodes[y], LOW);
    }
    pinMode(ledCathodes[x], INPUT);
  }
}