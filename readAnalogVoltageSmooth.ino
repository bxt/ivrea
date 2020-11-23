/*
  ReadAnalogVoltage, but smooth

  Reads an analog input on pin 0, and averages over the last 32 read values.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  Based on:
  http://www.arduino.cc/en/Tutorial/ReadAnalogVoltage
*/

#define sensorSmoothPoints 32

int sensorSmoothIndex = 0;
int sensorSmoothValueSum = 0;
int sensorValues[sensorSmoothPoints];

void setup() {
  Serial.begin(9600);
}

void loop() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);

  sensorSmoothIndex++;
  if(sensorSmoothIndex >= sensorSmoothPoints) sensorSmoothIndex = 0;

  sensorSmoothValueSum -= sensorValues[sensorSmoothIndex];
  sensorValues[sensorSmoothIndex] = sensorValue;
  sensorSmoothValueSum += sensorValues[sensorSmoothIndex];

  int smoothSensorValue = sensorSmoothValueSum / sensorSmoothPoints;
  
  Serial.println(smoothSensorValue);
}
