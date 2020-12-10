
// Connect "+" of the passive buzzer to this pin:
uint8_t buzzerPin = 9;

// Connect the pins of the ultrasonic module accordingly:
uint8_t sr04triggerPin = 2;
uint8_t sr04echoPin = 3;

float distance;

void setup() {
  pinMode(sr04triggerPin, OUTPUT);
  pinMode(sr04echoPin, INPUT);

  Serial.begin(9600);

  digitalWrite(sr04triggerPin, LOW);
}

void loop() {
  digitalWrite(sr04triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(sr04triggerPin, LOW);

  distance = pulseIn(sr04echoPin, HIGH) / 58.0;
  Serial.print(distance);
  Serial.println("cm");
  int frequency = map(distance, 10, 300, 880, 220);
  tone(buzzerPin, frequency);

  delay(200);
}