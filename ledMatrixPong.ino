
#define LED_SIZE 8

#define PADDLE_SIZE 3
#define INITIAL_BALL_TICKS 1200
#define STEP_BALL_TICKS 300

#define WINNING_TICKS 450
#define SCORES_TICKS 1500

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

enum Player { none, left, right };

class Paddle {
private:
  uint8_t pin;
  int reference;
  uint8_t positionX;
  uint8_t positionY;

public:
  Paddle(uint8_t pin, uint8_t positionX) {
    this->pin = pin;
    this->reference = 0;
    this->positionX = positionX;
    this->positionY = (LED_SIZE - PADDLE_SIZE) / 2 + 1;
  }

  void setup() { this->reference = analogRead(this->pin); }

  void loop() {
    int reading = analogRead(this->pin);

    if (reading < this->reference - POTI_THRESHOLD) {
      this->reference = reading;
      if (this->positionY > 0) {
        this->positionY--;
      }
    }

    if (reading > this->reference + POTI_THRESHOLD) {
      this->reference = reading;
      if (this->positionY < LED_SIZE - PADDLE_SIZE) {
        this->positionY++;
      }
    }

    for (int i = 0; i < PADDLE_SIZE; i++) {
      buffer[this->positionY + i] |= 1 << this->positionX;
    }
  }

  bool isCovering(uint8_t positionY) {
    return positionY >= this->positionY &&
           positionY < (this->positionY + PADDLE_SIZE);
  }
};

Paddle leftPaddle(LEFT_POTENTIOMETER_PIN, 0);
Paddle rightPaddle(RIGHT_POTENTIOMETER_PIN, (LED_SIZE - 1));

class Ball {
private:
  int8_t positionX;
  int8_t positionY;
  int8_t speedX;
  int8_t speedY;
  int ticksLeft;
  int ticks;

  void mirrorXAt(int8_t x) {
    this->speedX = -this->speedX;
    this->positionX = x - (this->positionX - x);
  }
  void mirrorYAt(int8_t y) {
    this->speedY = -this->speedY;
    this->positionY = y - (this->positionY - y);
  }

public:
  Ball() {
    this->ticks = STEP_BALL_TICKS;
    this->reset(1);
  }

  void reset(int directionX) {
    int r = random(0, 100);
    this->positionX = (LED_SIZE * 2 - directionX) / 4;
    this->positionY = (LED_SIZE * 2 + (r > 50 ? 1 : -1)) / 4;
    this->speedX = directionX;
    this->speedY = (r > 75 || r < 25 ? 1 : -1);
    if (this->ticks > 30) {
      this->ticks -= 3;
    }
    this->ticksLeft = INITIAL_BALL_TICKS;
  }

  Player loopAndWinner() {
    this->ticksLeft--;
    if (this->ticksLeft < 0) {
      this->ticksLeft = this->ticks;

      this->positionX += this->speedX;

      if (this->positionX < 1) {
        if (leftPaddle.isCovering(this->positionY)) {
          this->mirrorXAt(1);
        } else {
          return right;
        }
      }
      if (this->positionX > LED_SIZE - 2) {
        if (rightPaddle.isCovering(this->positionY)) {
          this->mirrorXAt(LED_SIZE - 2);
        } else {
          return left;
        }
      }

      this->positionY += this->speedY;

      if (this->positionY < 0) {
        this->mirrorYAt(0);
      }
      if (this->positionY > LED_SIZE - 1) {
        this->mirrorYAt(LED_SIZE - 1);
      }
    }

    buffer[this->positionY] |= 1 << (this->positionX);

    return none;
  }
};

Ball ball;

class Scores {
private:
  Player player;
  int ticksLeft;
  int scoreLeft;
  int scoreRight;

public:
  Scores() {
    this->player = none;
    this->ticksLeft = -1;
  }

  void setScorer(Player player) {
    if (player == none)
      return;

    this->player = player;
    this->ticksLeft = SCORES_TICKS;
  }

  bool loopAndIsEnabled() {
    if (this->ticksLeft < 0)
      return false;

    for (int i = 0; i < this->scoreLeft; i++) {
      buffer[i % LED_SIZE] |= 1 << (i / LED_SIZE);
    }

    for (int i = 0; i < this->scoreRight; i++) {
      buffer[i % LED_SIZE] |= 1 << (LED_SIZE - 1 - i / LED_SIZE);
    }

    int bars = max(2 * (this->ticksLeft - SCORES_TICKS / 2) * LED_SIZE / SCORES_TICKS / 3, 0);
    int x, y;
    if (this->player == left) {
      x = min(this->scoreLeft / LED_SIZE + bars, LED_SIZE);
      y = this->scoreLeft % LED_SIZE;
    } else {
      x = max(LED_SIZE - 1 - this->scoreRight / LED_SIZE - bars, 0);
      y = this->scoreRight % LED_SIZE;
    }
    buffer[y] |= 1 << x;

    this->ticksLeft--;

    if (this->ticksLeft < 0) {
      if (this->player == left) {
        this->scoreLeft++;
        ball.reset(1);
      } else {
        this->scoreRight++;
        ball.reset(-1);
      }
    }

    return true;
  }
};

Scores scores;

class Winning {
private:
  Player player;
  int ticksLeft;

public:
  Winning() {
    this->player = none;
    this->ticksLeft = -1;
  }

  void setWinner(Player player) {
    if (player == none)
      return;

    this->player = player;
    this->ticksLeft = WINNING_TICKS;
  }

  bool loopAndIsEnabled() {
    if (this->ticksLeft < 0)
      return false;

    for (int i = 0; i < LED_SIZE; i++) {
      int bars = this->ticksLeft * LED_SIZE / WINNING_TICKS;
      for (int k = 0; k < bars; k++) {
        int xPosition = this->player == left ? k : (LED_SIZE - 1 - k);
        buffer[i] |= 1 << xPosition;
      }
    }

    this->ticksLeft--;

    if (this->ticksLeft < 0) {
      scores.setScorer(this->player);
    }

    return true;
  }
};

Winning winning;

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

  leftPaddle.setup();
  rightPaddle.setup();
}

void loop() {
  cleanBuffer();

  if (!scores.loopAndIsEnabled()) {
    leftPaddle.loop();
    rightPaddle.loop();

    if (!winning.loopAndIsEnabled()) {
      Player maybeWinner = ball.loopAndWinner();
      winning.setWinner(maybeWinner);
    }
  }

  displayBuffer();
}