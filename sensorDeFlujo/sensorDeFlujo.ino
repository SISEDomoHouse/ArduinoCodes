// project: DomoHouse, SISE(Semillero de investigación de sistemas embebidos)
// Adaptation done by: Alexander Acosta - Julian Arango - Yeison Osorio
// github: https://github.com/SISEDomoHouse/ArduinoCodes

// Description: This program do the connection to the server in ngrok in order to SAVE: id_sensor, id_variable and the value from the sensor
// the connection is done throught Wifi network and HTTP Connection. It uses GET Method.

//Libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

ESP8266WiFiMulti WiFiMulti;

#define sensorDeFlujo D1 // Pin al que esta conectado el sensor
unsigned int litrosPorMinuto; // Calculated litres/hour
unsigned long pulsosAcumulados; // Pulsos acumulados
volatile int pulsos; // Cantidad de pulsos del sensor. Como se usa dentro de una interrupcion debe ser volatile
float litros[2]; // Litros acumulados

const unsigned long SEND_Timer = 5000;  //Delay between SEND updates, 5000 milliseconds
const unsigned int SENTVARS = 3;  //Number of variables (sensor_id, variable_id & value)
const char* SSID = "domohouse"; //WiFi SSID, change nombre-wifi por la red propia
const char* PASSWORD = "1234567890"; //WiFi Pass, coloque el password real
const char* HOST = "http://959a5423.ngrok.io";  //REST Web Host, replace by real 'server' url

String appName = "/register";
String serviceSaveRegister = "/create";  //Name of the service to create registers
char* propertyNames[] = {"sensor", "variable", "value"}; //Vector Var names
String propertyValues[SENTVARS]; //Vector for Var values
unsigned long lastConnectionTime = 0; //Last time you connected to the server, in milliseconds



void SEND(int SENTVARS, char* sensorNames[], String values[]) {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    HTTPClient http;
    String url = appName;
    url += serviceSaveRegister;
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
    litros[1] = values[2].toFloat();
    http.end();
  }
}

void flujo () {
  pulsos++;
}


void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  propertyValues[0] = "1"; // sensor_id
  propertyValues[1] = "1"; // variable_id
  litros[1] = -1;
  pinMode(sensorDeFlujo, INPUT);

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  attachInterrupt(digitalPinToInterrupt(sensorDeFlujo), flujo, RISING);
  interrupts(); // Habilitar interrupciones

  WiFiMulti.addAP(SSID, PASSWORD);

}

void loop() {
  // put your main code here, to run repeatedly:

  litros[0] = pulsos * 1.0 / 400;

  if (millis() - lastConnectionTime > SEND_Timer) {
    propertyValues[0] = "1";
    propertyValues[1] = "1";
    propertyValues[2] = litros[0];

    if (litros[0] > litros[1]) {
      SEND(SENTVARS, propertyNames, propertyValues);
    }
    lastConnectionTime = millis();
  }





}
