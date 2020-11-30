#include <LiquidCrystal.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// LCD:
// If you put the LDC with the pins on top and look at it left to right the pins are:
// VSS, VDD, VO, RS, RW, E, D0 - D7, A, K.
// Connect VSS, RW and K to GND; VO with 1.5kO to GND.
// Connect VDD to 5V; A with 220O to 5V.
// D0 - D3 can float, other pins according to this config:
LiquidCrystal lcd(
 12, // RS
 11, // Enable
 5,  // D4
 4,  // D5
 3,  // D5
 2  // D7
);

#define TMP_ROW 0
#define HUM_ROW 1
#define DATA_COL 7

// DHT11 Temp probe:
// If you look at the DHT 11 from the side with the holes the pins are:
// 5V, DATA, NC, GND.
// Wire up 5V and GND, leave NC alone, and put DATA into the pin here:
#define DHTPIN 8 // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11 // I have the cheap one

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayForSensor;

#define CAPRI_SUN (byte)0
uint8_t capriSun[] = {
  B00000,
  B10101,
  B01110,
  B11111,
  B01110,
  B10101,
  B00000,
  B00000,
};

#define ENTERPRISE_CLOUD (byte)1
uint8_t enterpriseCloud[] = {
  B00000,
  B00000,
  B00110,
  B11111,
  B01110,
  B00000,
  B00000,
  B00000,
};

#define SPECIAL_SNOWFLAKE (byte)2
uint8_t specialSnowflake[] = {
  B00100,
  B10101,
  B01110,
  B11111,
  B01110,
  B10101,
  B00100,
  B01110,
};

#define DROPLET_1 (byte)3
uint8_t droplet1[] = {
  B00000,
  B00000,
  B00000,
  B00100,
  B00100,
  B00000,
  B00000,
  B00000,
};

#define DROPLET_2 (byte)4
uint8_t droplet2[] = {
  B00000,
  B00100,
  B00110,
  B01110,
  B01110,
  B00100,
  B00000,
  B00000,
};

#define DROPLET_3 (byte)5
uint8_t droplet3[] = {
  B00100,
  B01110,
  B01110,
  B11111,
  B11111,
  B01110,
  B00000,
  B00000,
};

void setup() {
  lcd.begin(16/*columns*/, 2/*rows*/);

  lcd.createChar(CAPRI_SUN, capriSun);
  lcd.createChar(ENTERPRISE_CLOUD, enterpriseCloud);
  lcd.createChar(SPECIAL_SNOWFLAKE, specialSnowflake);
  lcd.createChar(DROPLET_1, droplet1);
  lcd.createChar(DROPLET_2, droplet2);
  lcd.createChar(DROPLET_3, droplet3);

  lcd.setCursor(0, TMP_ROW);
  lcd.print("Temp.: ??.??");
  lcd.setCursor(0, HUM_ROW);
  lcd.print("Hum.:  ??.??");

  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  delayForSensor = sensor.min_delay / 1000;
}

void loop() {
  delay(delayForSensor);

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  lcd.setCursor(DATA_COL, TMP_ROW);
  if (isnan(event.temperature)) {
    lcd.print("EE.EE");
  } else {
    lcd.print(event.temperature);
    lcd.print(" C ");
    if (event.temperature > 20) {
      lcd.write(CAPRI_SUN);
    } else if (event.temperature < 15) {
      lcd.write(SPECIAL_SNOWFLAKE);
    } else {
      lcd.write(ENTERPRISE_CLOUD);
    }
  }

  dht.humidity().getEvent(&event);
  lcd.setCursor(DATA_COL, HUM_ROW);
  if (isnan(event.relative_humidity)) {
    lcd.print("EE.EE");
  } else {
    lcd.print(event.relative_humidity);
    lcd.print(" % ");
    if (event.relative_humidity > 55) {
      lcd.write(DROPLET_3);
    } else if (event.relative_humidity < 40) {
      lcd.write(DROPLET_1);
    } else {
      lcd.write(DROPLET_2);
    }
  }
}