#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>
#include <BlynkSimpleTinyGSM.h>
#include <EEPROM.h>
#include <ArduinoHttpClient.h>

char seri[15] = "";
char auth[34] = "";
uint8_t csq;
uint8_t dem = 0;
char server[] = "wirelesstech.online";

TinyGsm modem(Serial);
BlynkTimer timer;

TinyGsmClient client(modem);
HttpClient http(client, server, 5060);
void valueSignal()
{
    csq = modem.getSignalQuality();
    Blynk.virtualWrite(V5, csq);
}
void resetGSM(){
  Blynk.disconnect();
  modem.poweroff();
  digitalWrite(13,1);
  delay(5000);
  digitalWrite(13,0);
  delay(5000);
}

BLYNK_CONNECTED(){
  Blynk.syncAll();
}

void setup()
{
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  digitalWrite(13,0);
  for ( uint8_t i = 0; i < 32; i++) {
    auth[i]= char(EEPROM.read(i));
  }
  for (uint8_t j = 32; j < 44; j++) {
      seri[j-32]= char(EEPROM.read(j));
  }
  modem.restart();
  modem.gprsConnect("e-connect", "", "");
  delay(2000);
  http.get("/check?" + String(seri));
  delay(1000);
  String body = http.responseBody();
  http.stop();
  if (body[0] == 'O' ) {
    for ( uint8_t i = 0; i < 32; i++) {
        EEPROM.write(i, body[i+1]);
        auth[i] = body[i+1];
    }
    delay(3000);
    http.get("/reques?" + String(seri));
    delay(2000);
    http.stop();
  }
  delay(2000);
  modem.restart();
  Blynk.begin(auth, modem, "e-connect", "", "",server,8082);
  timer.setInterval(50000L, valueSignal);
}

void loop()
{
   if(!Blynk.run()){
     dem ++;
     if(dem == 10){
       resetGSM();
        modem.restart();
        Blynk.begin(auth, modem, "e-connect", "", "",server,8082);
        dem = 0;
     }else
     {
       dem = 0;
     }
   }
   timer.run();
}
