#define SERIAL_IN 4
#define SERIAL_CLOCK 7
#define R_CLOCK 8
#define LENGTH 16

bool values[LENGTH];

bool showIntermediate = true;

#define DELAY_MODE_MINIMAL 0
#define DELAY_MODE_ANIMATED 1
#define DELAY_MODE_EDUCATIONAL 2

unsigned int delayMode = DELAY_MODE_ANIMATED;

unsigned int resultDelayTime = 500;

void applyDelay() {
  if(delayMode == DELAY_MODE_MINIMAL) {
    delayMicroseconds(1);
  }
  if(delayMode == DELAY_MODE_ANIMATED) {
    delay(20);
  }
  if(delayMode == DELAY_MODE_EDUCATIONAL) {
    delay(300);
  }
}

void writeValues() {
  for (int i = 0; i < LENGTH; i++) {
    bool value =  values[i];
    digitalWrite(SERIAL_IN, value ? HIGH : LOW);
    applyDelay();
    digitalWrite(SERIAL_CLOCK, HIGH);
    applyDelay();
    digitalWrite(SERIAL_IN, LOW);
    digitalWrite(SERIAL_CLOCK, LOW);
    applyDelay();
    if (showIntermediate || i == LENGTH - 1) {
      digitalWrite(R_CLOCK, HIGH);
      applyDelay();
      digitalWrite(R_CLOCK, LOW);
      applyDelay();
    }
  }
  delay(resultDelayTime);
}

void processSerial() {
  while(Serial.available() > 0) {
    int incomingByte = Serial.read();

    if (incomingByte == ' ' || incomingByte == 10) {
      // silently ignore space and new line
      continue;
    }

    if (incomingByte == 'I') {
      showIntermediate = !showIntermediate;
    } else if (incomingByte == 'D') {
       incomingByte = Serial.read();
      if (incomingByte >= '0' && incomingByte < '3') {
        delayMode = incomingByte - '0';
      } else {
        Serial.print(F("Ignoring invalid byte, expected delay mode '0', '1' or '2', got "));
        Serial.println(incomingByte, DEC);
      }
    } else if (incomingByte == 'R') {
      resultDelayTime = 0;
      for (int k = 0; k < 3; k++) {
        incomingByte = Serial.read();
        if (incomingByte >= '0' && incomingByte <= '9') {
          int digit = incomingByte - '0';
          resultDelayTime = resultDelayTime * 10 + digit;
        } else {
          Serial.print(F("Ignoring invalid byte, expected result delay digit, got "));
          Serial.println(incomingByte, DEC);
        }
      }
    } else {
      Serial.print(F("Ignoring invalid byte, expected command 'I', 'D' or 'R', got "));
      Serial.println(incomingByte, DEC);
    }
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(SERIAL_IN, OUTPUT);
  pinMode(SERIAL_CLOCK, OUTPUT);
  pinMode(R_CLOCK, OUTPUT);
}

void loop() {

  for (int k = 0; k < 2; k++) {
    for (int i = 0; i < LENGTH; i++) {
      values[i] = false ^ (k == 0);
    }
    writeValues();
    processSerial();

    for (int i = 0; i < LENGTH; i++) {
      values[i] = (i % 2 >= 1) ^ (k == 0);
    }
    writeValues();
    processSerial();

    for (int i = 0; i < LENGTH; i++) {
      values[i] = (i % 4 >= 2) ^ (k == 0);
    }
    writeValues();
    processSerial();

    for (int i = 0; i < LENGTH; i++) {
      values[i] = (i % 8 >= 4) ^ (k == 0);
    }
    writeValues();
    processSerial();

    for (int i = 0; i < LENGTH; i++) {
      values[i] = (i % 16 >= 8) ^ (k == 0);
    }
    writeValues();
    processSerial();
  }

  if(delayMode == DELAY_MODE_MINIMAL) {
    for (int k = 0; k < LENGTH; k++) {
     for (int i = 0; i < LENGTH; i++) {
        values[i] = i < k;
      }
      writeValues();
      processSerial();
    }
  }

  if(delayMode == DELAY_MODE_MINIMAL) {
    for (int j = 0; j < LENGTH; j++) {
      for (int k = j; k >= 0; k--) {
       for (int i = 0; i < LENGTH; i++) {
          values[i] = i > j || i == k;
        }
        writeValues();
        processSerial();
      }
    }
  }

  for (int k = 0; k < 8; k++) {
    for (int i = 0; i < LENGTH; i++) {
      values[i] = random(2) >= 1;
    }
    writeValues();
    processSerial();
  }
}