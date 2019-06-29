/*
 * EEPROM Write
 *
 * Stores values read from analog input 0 into the EEPROM.
 * These values will stay in the EEPROM when the board is
 * turned off and may be retrieved later by another sketch.
 */

#include <EEPROM.h>

String seri = "SNE050519V10";

void setup()
{
  EEPROM.begin(512);
  for(uint8_t i = 128; i<140; i++) {
    EEPROM.write(i, seri[i-128]);
  }
  
  EEPROM.commit();
}

void loop()
{
}
