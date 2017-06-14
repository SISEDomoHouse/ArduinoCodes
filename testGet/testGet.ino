// project: DomoHouse, SISE(Semillero de investigación de sistemas embebidos)
// Adaptation done by: Alexander Acosta - Julian Arango - Yeison Osorio
// github: https://github.com/SISEDomoHouse/ArduinoCodes

// Description: This program test the connection to the server "iotweatherstation" in order to SAVE: idhome, temp, humid and timestamp (yyyy:mm:dd:hh:mm:ss)
// the connection is done throught Wifi network and HTTP Connection. It uses GET Method.

//Libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

ESP8266WiFiMulti WiFiMulti;

const unsigned long SEND_Timer = 5000;  //Delay between SEND updates, 5000 milliseconds
const unsigned int SENTVARS = 1;  //Number of sensor vars sent to REST Web App, (idHome, Temp & Humid, TimeStamp)
const char* SSID = "domohouse"; //WiFi SSID, change nombre-wifi por la red propia
const char* PASSWORD = "1234567890"; //WiFi Pass, coloque el password real
const char* HOST = "http://87380993.ngrok.io";  //REST Web Host, replace by real 'server' url

String appName = "/sensor";
String serviceSaveSensors = "/create";  //Name of the service SAVESENSORS
char* propertyNames[] = {"name"}; //Vector Var names
String propertyValues[SENTVARS]; //Vector for Var values
unsigned long lastConnectionTime = 0; //Last time you connected to the server, in milliseconds


void SEND(int SENTVARS, char* sensorNames[], String values[]) {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    HTTPClient http;
    String url = appName;
    url += serviceSaveSensors;
    url += "?";
    for (int idx = 0; idx < SENTVARS; idx++)
    {
      url += propertyNames[idx];
      url += "=";
      url += values[idx];
      url += "&";
    }

    http.begin(HOST + url);
    Serial.print(HOST + url);

    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
}


void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFiMulti.addAP(SSID, PASSWORD);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() - lastConnectionTime > SEND_Timer) {
    propertyValues[0] = "emontoya";
    SEND(SENTVARS, propertyNames, propertyValues);
    lastConnectionTime = millis();
  }

}
