
#define sensorSmoothPoints 32

int sensorSmoothIndex = 0;
int sensorSmoothValueSum = 0;
int sensorValues[sensorSmoothPoints];

int leds[] = {3, 5, 6, 9, 10, 11}; // the PWM pins the LEDs are attached to
int ledCount = 6;                  // how many LEDs there are
int maxBrightness = 128;
int msPerLevel = 16;

int offset = 2*maxBrightness/ledCount;

void setup() {
  
  // declare pin 9 to be an output:
  for(int i = 0; i < ledCount; i++) {
    pinMode(leds[i], OUTPUT);
  }
  
}

void loop() {
  int sensorValue = analogRead(A0);
  
  sensorSmoothIndex++;
  if(sensorSmoothIndex >= sensorSmoothPoints) sensorSmoothIndex = 0;

  sensorSmoothValueSum -= sensorValues[sensorSmoothIndex];
  sensorValues[sensorSmoothIndex] = sensorValue;
  sensorSmoothValueSum += sensorValues[sensorSmoothIndex];

  int smoothSensorValue = sensorSmoothValueSum / sensorSmoothPoints;
  
  unsigned long time = millis();
  msPerLevel = map(smoothSensorValue, 0, 1023, 1, 40);
  for(int i = 0; i < ledCount; i++) {
    int step = (i*offset + time/msPerLevel) % (maxBrightness*2);
    int brightness = step > maxBrightness ? maxBrightness*2 - step : step;
    brightness = (brightness*brightness)/(maxBrightness);
    analogWrite(leds[i], brightness + 5);
  }
}
