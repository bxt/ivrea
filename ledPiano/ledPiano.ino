
#define START_MODE 0

int leds[] = {3, 5, 6, 9, 10, 11}; // the pins the LEDs are attached to
int ledCount = 6;                  // how many LEDs there are, max. 10

char mode = START_MODE;

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < ledCount; i++) {
    pinMode(leds[i], OUTPUT);
  }
}

void loop() {
  if (Serial.available() > 0) {
    int incomingByte = Serial.read();

    // silently ignore space and new line
    if (incomingByte == ' ' || incomingByte == 10) {
      return;
    }

    if (mode == START_MODE) {
      if (incomingByte == 'H') {
        mode = 'H';
      } else if (incomingByte == 'L') {
        mode = 'L';
      } else {
        Serial.print(F("Ignoring invalid byte, expected 'H' or 'L', got "));
        Serial.println(incomingByte, DEC);
      }
    } else if (mode == 'H' || mode == 'L') {
      if (incomingByte >= '0' && incomingByte < '0' + ledCount) {
        int i = incomingByte - '0';
        digitalWrite(leds[i], mode == 'H' ? HIGH : LOW);
        mode = START_MODE;
      } else {
        Serial.print(F("Ignoring invalid byte, expected pin digit, got "));
        Serial.println(incomingByte, DEC);
      }
    }
  }
}
