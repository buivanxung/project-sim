
#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>
#include <ESP8266WebServer.h>
#include "EEPROM.h"
#include <BlynkSimpleEsp8266.h>
#include <ESP8266HTTPClient.h>

#define ESP_X

ESP8266WebServer server(80);
 
char*     ssid           = "WT_";
String          eseri = "";
const char*     passphrase     = "88888888";
String          eblynk          = "";
String          st;
String          content;
uint8_t         statusCode;

HTTPClient http;

bool testWifi();
void checkSeri();
BLYNK_CONNECTED(){
  Blynk.syncAll();
}
void setup() {
    String esid = "";
    String epass = "";
    EEPROM.begin(512);
    delay(10);

    for (uint8_t i = 0; i < 32; ++i) {
        esid += char(EEPROM.read(i));
    }
    esid.trim();

    for (uint8_t i = 32; i < 96; ++i) {
        epass += char(EEPROM.read(i));
    }
    epass.trim();

    for (uint8_t i = 96; i < 128; ++i) {
        eblynk += char(EEPROM.read(i));
    }
    eblynk.trim();
     for (uint8_t i = 128; i < 140; ++i) {
        eseri += char(EEPROM.read(i));
    }
    eseri.trim();
    EEPROM.end();
    if ( esid.length() > 1 ) {
        WiFi.begin(esid.c_str(), epass.c_str());
        if (testWifi()) {
            checkSeri();
            char * auth_ = new char[eblynk.length() + 1];
            eblynk.toCharArray(auth_, eblynk.length() + 1);
            Blynk.begin(auth_, esid.c_str(), epass.c_str(),"wirelesstech.online", 8082);
            return;
        }
    }
}

void checkSeri(){
   http.begin("http://wirelesstech.online:5060/check?" + eseri);
   int httpCode = http.GET();
    if(httpCode == HTTP_CODE_OK) {
     String payload = http.getString();
        if (payload[0] == 'O') {
            EEPROM.begin(512);
            for (uint8_t i = 0; i < 32; ++i) {
                EEPROM.write(96 + i, payload[i+1]);
            }
            for (uint8_t i = 96; i < 128; ++i) {
                eblynk += char(EEPROM.read(i));
            }
            eblynk.trim();
            EEPROM.commit();
            EEPROM.end();
            http.begin("http://wirelesstech.online:5060/reques?" + eseri);
            httpCode = http.GET();
            ESP.restart();
        }
      }
      http.end();
}

bool testWifi() {
     int c = 0;
     while ( c < 20 ) {
          if (WiFi.status() == WL_CONNECTED) {
              if (Ping.ping("www.google.com",5)) {
                return true;
              }
            }
          delay(1000);
          c++;
     }

    setupAP();
    while ( c < 15000 ) {
        server.handleClient();
        delay(50);
        c++;
    }
    ESP.restart();
}

void setupAP() {
    WiFi.disconnect();
    delay(100);
    WiFi.mode(WIFI_STA);
    delay(100);
    String temp = (String)ssid + eseri;
    uint8_t templ = temp.length() + 1;
    char char_array[templ];
    temp.toCharArray(char_array, templ);
    uint8_t n = WiFi.scanNetworks();
    if (n == 0) {
        ESP.restart();
    }
    st = "<select\rname=\"ssid\">\n";
    for (int i = 0; i < n; ++i) {
        // Print SSID and RSSI for each network found
        st += "<option value=\""+WiFi.SSID(i)+"\">"+WiFi.SSID(i)+"</option>\n";
    }

    WiFi.softAP(char_array, passphrase, 6);
    createWebServer();
    server.begin();
}

void createWebServer() {
    server.on("/", []() {
        content = "<!DOCTYPE HTML>\r\n<html>";
        content += "\n<head>\n<meta\rcharset=\"utf-8\">\n</head>\n";
        content += "<body\rstyle=\"margin:\auto;text-align:\rcenter\">\n";
        content += "<form method=\"get\" action=\"setting\">\n";
        content += "<p> SSID:</p>\n";
        content += st;
        content += "</select>";
        content += "\n<p>Password:</p>\n";
        content += "<input\rtype=\"text\"\rname=\"pwd\">\n";
        content += "<p>Key:</p>\n";
        content += "<input\rtype=\"text\"\rname=\"blyn\">\n";
        content += "<div><input\rtype=\"submit\"\rvalue=\"Send\">\n";
        content += "</p>";
        content += "</body>\n</html>\n";
        server.send(200, "text/html", content);
    });
    server.on("/setting", []() {
        String qsid = server.arg("ssid");
        String qpass = server.arg("pwd");
        String qblynk = server.arg("blyn");

        if (qsid.length() > 0) {
            EEPROM.begin(512);
            for (uint8_t i = 0; i < 128; ++i) {
                EEPROM.write(i, 0);
            }
            EEPROM.commit();
            for (uint8_t i = 0; i < qsid.length(); ++i) {
                EEPROM.write(i, qsid[i]);
            }
            for (uint8_t i = 0; i < qpass.length(); ++i) {
                EEPROM.write(32 + i, qpass[i]);
            }
            for (uint8_t i = 0; i < qblynk.length(); ++i) {
                EEPROM.write(96 + i, qblynk[i]);
            }
            EEPROM.commit();
            EEPROM.end();

            content = "{\"Success\":\"Luu vao he thong. Khoi dong lai ten wifi moi\"}";
            server.send(200, "application/json", content);
            delay(5000);
            ESP.restart();
        } else {
            content = "{\"Error\":\"404 not found\"}";
            server.send(404, "application/json", content);
        }
    });
}

void loop() {
    Blynk.run();
}
