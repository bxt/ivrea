
uint8_t buzzerPin = 9;

int speed = 500;

char names[] = "cdefgah";
int frequencies[3][7] = {
  {139, 147, 165, 185, 208, 220, 247},
  {277, 294, 330, 370, 415, 440, 494},
  {554, 587, 659, 740, 831, 880, 988},
};

char melody[] = "fZggga.aa fZgggaha fAdAHged eGCGA#fdc defHHgah aaaagg   ";

void setup() {
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  for(int i = 0; i < sizeof(melody) - 3; i++) {
    char toneLetter = melody[i];
    if (toneLetter == ' ') continue;
    if (toneLetter == '#') continue;
    if (toneLetter == '.') {
      noTone(buzzerPin);
      delay(25);
      continue;
    }

    int octave = 1;
    if (toneLetter == 'Z') {
      octave++;
      toneLetter = 'c';
    }
    if (toneLetter >= 'A' && toneLetter <= 'Z') {
      octave--;
      toneLetter -= 'A';
      toneLetter += 'a';
    }

    int frequencyIndex = 0;
    for (int k = 0; k < sizeof(names); k++) {
      if (names[k] == toneLetter) {
        frequencyIndex = k;
      }
    }
    int frequency = frequencies[octave][frequencyIndex];

    if(melody[i + 1] == '#') {
      frequency = 233;
    }

    tone(buzzerPin, frequency);
    delay(speed + (melody[i + 1] == '.' ? -25 : 0));
  }
}