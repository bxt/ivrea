#define MORSE_LED LED_BUILTIN

#define WPM 10

#define skip 0
#define dit 1
#define dah 2

const PROGMEM uint8_t morseLookupTable[] = {
  dit, dah, skip, skip,   // A
  dah, dit, dit, dit,     // B
  dah, dit, dah, dit,     // C
  dah, dit, dit, skip,    // D
  dit, skip, skip, skip,  // E
  dit, dit, dah, dit,     // F
  dah, dah, dit, skip,    // G
  dit, dit, dit, dit,     // H
  dit, dit, skip, skip,   // I
  dit, dah, dah, dah,     // J
  dah, dit, dah, skip,    // K
  dit, dah, dit, dit,     // L
  dah, dah, skip, skip,   // M
  dah, dit, skip, skip,   // N
  dah, dah, dah, skip,    // O
  dit, dah, dah, dit,     // P
  dah, dah, dit, dah,     // Q
  dit, dah, dit, skip,    // R
  dit, dit, dit, skip,    // S
  dah, skip, skip, skip,  // T
  dit, dit, dah, skip,    // U
  dit, dit, dit, dah,     // V
  dit, dah, dah, skip,    // W
  dah, dit, dit, dah,     // X
  dah, dit, dah, dah,     // Y
  dah, dah, dit, dit,     // Z
};

int incomingByte = 0;

unsigned long ditLength = 1200 / WPM;

void sendSignAndADit(uint8_t sign) {
  if (sign == dit) {
    digitalWrite(MORSE_LED, HIGH);
    delay(ditLength);
    digitalWrite(MORSE_LED, LOW);
    delay(ditLength);
  } else if (sign == dah) {
    digitalWrite(MORSE_LED, HIGH);
    delay(ditLength * 3);
    digitalWrite(MORSE_LED, LOW);
    delay(ditLength);
  } else if (sign == skip) {
    // no-op, skip is just here for memory alignment
  } else {
    Serial.print(F("Tried to send invalid sign: "));
    Serial.println(sign, DEC);
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(MORSE_LED, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    incomingByte = Serial.read();

    // 4 extra dits for space, makes 7
    if (incomingByte == ' ') {
      delay(ditLength * 4);
      return;
    }

    // 11 extra dits for new line, makes 14
    if (incomingByte == '\n') {
      delay(ditLength * 11);
      return;
    }

    // Convert to upper case
    if (incomingByte >= 'a' && incomingByte <= 'z') {
      incomingByte -= 32;
    }

    // Handle letter
    if (incomingByte >= 'A' && incomingByte <= 'Z') {
      incomingByte -= 'A';

      for (int i = 0; i < 4; i++) {
        uint8_t sign =
            pgm_read_byte_near(morseLookupTable + incomingByte * 4 + i);
        sendSignAndADit(sign);
      }

      // 2 extra dits after letter, makes 3
      delay(ditLength * 2);

      return;
    }

    // Handle number
    if (incomingByte >= '0' && incomingByte <= '9') {
      incomingByte -= '0';

      for (int i = 0; i < 5; i++) {
        uint8_t sign = incomingByte > 5
          ? (i >= incomingByte - 5 ? dit : dah)
          : (i >= incomingByte ? dah : dit);
        sendSignAndADit(sign);
      }

      // 2 extra dits after number, makes 3
      delay(ditLength * 2);

      return;
    }

    // We got something other than a letter or a space:
    Serial.print(F("Ignoring unknown byte "));
    Serial.println(incomingByte, DEC);
  }
}
