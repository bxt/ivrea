#include <OSFS.h>
#include <EEPROM.h>
#include "osfsEeprom.h"

void setup() {
  Serial.begin(9600);

  Serial.println(F("Current EEPROM contents:"));
  // Prints something like 42 A4 EA 0 0 FF...
	for (uint16_t i = 0; i < EEPROM.length(); i++) {
		uint8_t value = EEPROM.read(i);
    Serial.print(value, HEX);
    if((i % 16) == 15) {
      Serial.println();
    } else {
      Serial.print(F(" "));
    }
	}

  OSFS::result r;

  r = OSFS::checkLibVersion();

  if (r == OSFS::result::UNFORMATTED) {
  	Serial.println(F("Not formatted, would go on and format..."));
    r = OSFS::format();
    Serial.print(F("Formatting Error: "));
  	Serial.println( (int) r );
  } else if (r == OSFS::result::NO_ERROR) {
  	Serial.println(F("OSFS already set up and working"));
  } else {
  	Serial.print(F("Error: "));
  	Serial.println( (int) r );
  }
}

void loop() {

}