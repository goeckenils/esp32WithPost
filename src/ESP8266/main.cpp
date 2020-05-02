#include <DallasTemperature.h>
#include <OneWire.h>
#include <ArduinoJson.h>
#include "ESP8266WiFi.h"
#include <ESP8266HTTPClient.h>
#define BLUE 0                            
#define RED 5                             
#define GREEN 4                          
const char *ssid = "Martin Router King";
const char *password = "WxqhLb6rKpbOaVzlPx4lJgbAPTvkV5Jo";                
OneWire oneWire(2);                       
DallasTemperature sensors(&oneWire);
float temp;

void Failed() {

  digitalWrite(RED, LOW);
  digitalWrite(GREEN, HIGH);
  digitalWrite(BLUE, HIGH);

}

void SendingData() {

  digitalWrite(RED, HIGH);
  digitalWrite(GREEN, HIGH);
  digitalWrite(BLUE, LOW);

}

void Connected() {

  digitalWrite(RED, HIGH);
  digitalWrite(GREEN, LOW);
  digitalWrite(BLUE, HIGH);

}

void setup() {

  Serial.begin(9600);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

    delay(1000);
    Serial.println("Connecting to WiFi..");
    Failed();

  }

  Connected();
  Serial.println("Connected to the WiFi network");
  Serial.println("DS18B20 thermometer");
  sensors.setResolution(12);
  sensors.begin();

}

void loop() {

  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);
  String databuf;
  StaticJsonDocument<200> doc;
  doc["tempc"] = temp;
  doc["ip"] = WiFi.localIP().toString();
  doc["mac"] = WiFi.macAddress();
  serializeJson(doc, databuf);
  Serial.println(databuf);

  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http; 
    http.begin("http://192.168.0.113:5000/temperature");
    http.addHeader("content-type", "application/json");
    int httpResponseCode = http.POST(databuf);

    if (httpResponseCode > 0) {

      Serial.println(httpResponseCode);

    } else if (httpResponseCode == 200) {

      String response = http.getString();
      Serial.println(response);
      SendingData();

    } else {

      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
      Failed();

    }

    http.end();

  } else {

    Serial.println("Error in WiFi connection");
    Failed();

  }

  delay(10000);

}