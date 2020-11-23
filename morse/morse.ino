#define MORSE_LED LED_BUILTIN

#define WPM 10

#define skip 0
#define dit 1
#define dah 2

const PROGMEM uint8_t morseLookupTable[] = {
  dit, dah, skip, skip,    // A
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

unsigned long ditLength = 1200/WPM;

void setup() {
  Serial.begin(9600);
  pinMode(MORSE_LED, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    incomingByte = Serial.read();

    // 4 extra dits for space, makes 7
    if (incomingByte == 32) {
      delay(ditLength * 4);
      return;
    }
    
    // Convert to upper case
    if (incomingByte >= 97 && incomingByte <= 122) {
      incomingByte -= 32;
    }

    // Handle letter    
    if (incomingByte >= 65 && incomingByte <= 90) {
      incomingByte -= 65;

      for(int i = 0; i = 4; i++) {
        uint8_t sign = pgm_read_byte_near(morseLookupTable + incomingByte + i);

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
          Serial.print(F("Okay, strange table says sign is: "));
          Serial.println(sign, DEC);
        }
        
      }

      // 2 extra dits after letter, makes 3
      delay(ditLength * 2);

      return;
    }

    // We got something other than a letter or a space:
    Serial.print(F("Well, that's weird, I received incomingByte: "));
    Serial.println(incomingByte, DEC);
  }
}
