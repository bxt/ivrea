
#define PIN_LED_LEFT 2
#define PIN_LED_DONW 3
#define PIN_LED_UP 4
#define PIN_LED_RIGHT 5

void setup() {
  pinMode(PIN_LED_LEFT, OUTPUT);
  pinMode(PIN_LED_DONW, OUTPUT);
  pinMode(PIN_LED_UP, OUTPUT);
  pinMode(PIN_LED_RIGHT, OUTPUT);
}

void loop() {
  digitalWrite(PIN_LED_LEFT, HIGH);
  delay(500);
  digitalWrite(PIN_LED_LEFT, LOW);

  digitalWrite(PIN_LED_DONW, HIGH);
  delay(500);
  digitalWrite(PIN_LED_DONW, LOW);

  digitalWrite(PIN_LED_UP, HIGH);
  delay(500);
  digitalWrite(PIN_LED_UP, LOW);

  digitalWrite(PIN_LED_RIGHT, HIGH);
  delay(500);
  digitalWrite(PIN_LED_RIGHT, LOW);

}