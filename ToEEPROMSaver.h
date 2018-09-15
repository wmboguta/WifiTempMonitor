#include <EEPROM.h>


char output[256];

void cleanEEPROM() {
  EEPROM.begin(256);
  // cleaning
  for(int i=0; i<256; i++) {
    output[i] = NULL;
    EEPROM.write(i, NULL);
  }
  EEPROM.commit();
  Serial.println("Cleaned");
}

void saveToEEPROM(String text) {
  for (int i = 0; i<text.length(); i++) {
    EEPROM.write(i, text[i]);
  }
  EEPROM.commit();
  Serial.println("Saved");
}

void readFromEEPROM() {
  EEPROM.begin(256);
  char tmp = NULL;
  int i;
  for (i = 0; i < 256; i++) {
    tmp = char(EEPROM.read(i));
    if(tmp == NULL) break;
    output[i] = tmp;
  }
  Serial.println(output);
}

