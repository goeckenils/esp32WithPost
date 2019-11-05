#include <DallasTemperature.h>
#include <OneWire.h>
#include <ArduinoJson.h>
#include "ESP8266WiFi.h"
#include <ESP8266HTTPClient.h>

const char *ssid = "DESKTOP-0GAOED8 9425"; //login WLAN
const char *password = "W73303b{";          //login WLAN

#define BLUE 0  //GPIO
#define RED 5   //GPIO
#define GREEN 4 //GPIO

OneWire oneWire(2); // pin D4
DallasTemperature sensors(&oneWire);
float temp;

void setup()
{
  Serial.begin(9600);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");
  Serial.println("DS18B20 thermometer");
  sensors.setResolution(12);
  sensors.begin();

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
}

void loop()
{

  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);

  String databuf;
  StaticJsonDocument<200> doc;
  doc["tempc"] = temp;
  doc["ip"] = WiFi.localIP().toString();
  doc["mac"] = WiFi.macAddress();
  serializeJson(doc, databuf);
  Serial.println(databuf);

  if (temp < 20.0)
  {
    digitalWrite(GREEN, HIGH);
    digitalWrite(RED, HIGH);
    digitalWrite(BLUE, LOW);
  }
  else if (20.0 <= temp && temp <= 40.0)
  {
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, HIGH);
  } else {
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, HIGH);
    digitalWrite(BLUE, HIGH);
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http; 
    //http.begin("https://esp32-iot.azurewebsites.net/temperature","39:8E:01:A5:0C:66:8A:74:F0:10:4A:83:60:15:A2:6E:21:55:4C:CE"); //ONLINE
    http.begin("http://192.168.16.36:5000/temperature"); //LOCAL
    http.addHeader("content-type", "application/json");
    int httpResponseCode = http.POST(databuf);

    if (httpResponseCode > 0)
    {
      Serial.println(httpResponseCode);
    }
    else if (httpResponseCode == 200) 
    {
      String response = http.getString();
      Serial.println(response);
    }
    else
    {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }
  else
  {
    Serial.println("Error in WiFi connection");
  }

  delay(5000); //10 seconds delay
}
