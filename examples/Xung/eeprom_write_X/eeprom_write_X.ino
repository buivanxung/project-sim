/*
 * EEPROM Write
 *
 * Stores values read from analog input 0 into the EEPROM.
 * These values will stay in the EEPROM when the board is
 * turned off and may be retrieved later by another sketch.
 */

#include <EEPROM.h>

/** the current address in the EEPROM (i.e. which byte we're going to write to next) **/
String seri = "SNS010719V21";
String autho = "7aa4a7d272294f628ee6d51ba8dec55e";

void setup() {
  delay(1000);
  for(int i = 0; i<1024;i++){
    EEPROM.write(i, 0);
  }
  delay(1000);
  for(int i = 0; i< 32;i++){
    EEPROM.write(i, autho[i]);
  }
  delay(1000);
  for (int i= 32 ; i< 44; i++){
    EEPROM.write(i, seri[i-32]);
  }
  
}

void loop() {
  
}
