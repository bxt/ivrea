
#define LED_SIZE 8

#define PADDLE_SIZE 3
#define INITIAL_BALL_TICKS 600
#define STEP_BALL_TICKS 300

#define SCORE_TICKS 500

// Connect the potis to that the top pin is ground,
// bottom bin is 5V and middle pin goes to:
#define LEFT_POTENTIOMETER_PIN A6
#define RIGHT_POTENTIOMETER_PIN A7

// Higher value means you have to turn more
#define POTI_THRESHOLD 10

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

int leftPaddleReference = 0;
int rightPaddleReference = 0;
int leftPaddlePosition = (LED_SIZE - PADDLE_SIZE)/2 + 1;
int rightPaddlePosition = (LED_SIZE - PADDLE_SIZE)/2 + 1;

int ballPositionX = (LED_SIZE - PADDLE_SIZE)/2;
int ballPositionY = (LED_SIZE - PADDLE_SIZE)/2;
int ballSpeedX = 1;
int ballSpeedY = 1;
int ballTicksLeft = INITIAL_BALL_TICKS;

int scoreLeftTicksLeft = -1;
int scoreRightTicksLeft = -1;

void startGame() {
  ballPositionX = (LED_SIZE - PADDLE_SIZE)/2;
  ballPositionY = (LED_SIZE - PADDLE_SIZE)/2;
  ballSpeedX = 1;
  ballSpeedY = 1;
  ballTicksLeft = INITIAL_BALL_TICKS;
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

  int leftPaddleReference = analogRead(LEFT_POTENTIOMETER_PIN);
  int rightPaddleReference = analogRead(RIGHT_POTENTIOMETER_PIN);
}

void loop() {
  cleanBuffer();

  int leftPaddleReading = analogRead(LEFT_POTENTIOMETER_PIN);
  int rightPaddleReading = analogRead(RIGHT_POTENTIOMETER_PIN);

  if (leftPaddleReading < leftPaddleReference - POTI_THRESHOLD) {
    leftPaddleReference = leftPaddleReading;
    if(leftPaddlePosition > 0) {
      leftPaddlePosition--;
    }
  }

  if (leftPaddleReading > leftPaddleReference + POTI_THRESHOLD) {
    leftPaddleReference = leftPaddleReading;
    if(leftPaddlePosition < LED_SIZE - PADDLE_SIZE) {
      leftPaddlePosition++;
    }
  }

  if (rightPaddleReading < rightPaddleReference - POTI_THRESHOLD) {
    rightPaddleReference = rightPaddleReading;
    if(rightPaddlePosition > 0) {
      rightPaddlePosition--;
    }
  }

  if (rightPaddleReading > rightPaddleReference + POTI_THRESHOLD) {
    rightPaddleReference = rightPaddleReading;
    if(rightPaddlePosition < LED_SIZE - PADDLE_SIZE) {
      rightPaddlePosition++;
    }
  }

  for (int i = 0; i < PADDLE_SIZE; i++) {
    buffer[leftPaddlePosition + i] |= 1;
    buffer[rightPaddlePosition + i] |= 1 << (LED_SIZE - 1);
  }

  if(scoreLeftTicksLeft > 0) {
    for (int i = 0; i < LED_SIZE; i++) {
      int bars = scoreLeftTicksLeft*LED_SIZE/SCORE_TICKS;
      for (int k = 0; k < bars; k++) {
        buffer[i] |= 1 << k;
      }
    }
    scoreLeftTicksLeft--;
  } else if(scoreRightTicksLeft > 0) {
    for (int i = 0; i < LED_SIZE; i++) {
      int bars = scoreRightTicksLeft*LED_SIZE/SCORE_TICKS;
      for (int k = 0; k < bars; k++) {
        buffer[i] |= 1 << (LED_SIZE - k - 1);
      }
    }
    scoreRightTicksLeft--;
  } else {
    ballTicksLeft--;
    if(ballTicksLeft < 0) {
      ballTicksLeft = STEP_BALL_TICKS;

      ballPositionX += ballSpeedX;

      if(ballPositionX < 1) {
        if (ballPositionY >= leftPaddlePosition && ballPositionY < (leftPaddlePosition + PADDLE_SIZE)) {
          ballSpeedX = -ballSpeedX;
          ballPositionX = 1 - (ballPositionX - 1);
        } else {
          scoreRightTicksLeft = SCORE_TICKS;
          startGame();
        }
      }
      if(ballPositionX > LED_SIZE - 2) {
        if (ballPositionY >= rightPaddlePosition && ballPositionY < (rightPaddlePosition + PADDLE_SIZE)) {
          ballSpeedX = -ballSpeedX;
          ballPositionX = LED_SIZE - 2 - (ballPositionX - (LED_SIZE - 2));
        } else {
          scoreLeftTicksLeft = SCORE_TICKS;
          startGame();
        }
     }

     ballPositionY += ballSpeedY;

      if(ballPositionY < 0) {
        ballSpeedY = -ballSpeedY;
        ballPositionY = -ballPositionY;
      }
      if(ballPositionY > LED_SIZE - 1) {
        ballSpeedY = -ballSpeedY;
        ballPositionY = LED_SIZE - 1 - (ballPositionY - (LED_SIZE - 1));
     }
    }

    buffer[ballPositionY] |= 1 << (ballPositionX);
  }

  displayBuffer();
}