#include <Arduino.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>

#define USE_SERIAL Serial
#define BLUE 17
#define RED 5
#define GREEN 16

const char *ssid = "Die Box";
const char *password = "7E43B05B8";

WiFiMulti wifiMulti;
OneWire oneWire(4);
DallasTemperature sensors(&oneWire);
float temp;

void setup()
{
  USE_SERIAL.begin(115200);
  sensors.setResolution(12);

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  sensors.begin();

  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  for (uint8_t t = 4; t > 0; t--)
  {
    USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

  wifiMulti.addAP(ssid, password);
}

void loop()
{

  Serial.println("-------------------");

  String databuf;

  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);

  Serial.print("[TEMP] ");
  Serial.print(temp, 1);
  Serial.println();

  if (temp < 30.0)
  {
    digitalWrite(GREEN, LOW);
    digitalWrite(RED, HIGH);
    digitalWrite(BLUE, LOW);
  }
  else if (temp > 30.0)
  {
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, HIGH);
    digitalWrite(BLUE, HIGH);
  }

  String mac = String(WiFi.macAddress());

  StaticJsonDocument<200> doc;
  doc["tempC"] = temp;
  doc["ip"] = WiFi.localIP().toString();
  doc["mac"] = mac;

  serializeJson(doc, databuf);

  Serial.print("[JSON] ");
  Serial.print(databuf);
  Serial.println();
  // wait for WiFi connection
  if ((wifiMulti.run() == WL_CONNECTED))
  {

    HTTPClient http;

    USE_SERIAL.print("[HTTP] begin...\n");

    http.begin("http://172.16.210.124:8080/temperature");

    http.addHeader("content-type", "application/json");
    USE_SERIAL.print("[HTTP] POST...\n");
    int httpCode = http.POST(databuf);

    // httpCode will be negative on error
    if (httpCode > 0)
    {
      // HTTP header has been send and Server response header has been handled
      USE_SERIAL.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK)
      {
        String payload = http.getString();
        USE_SERIAL.print("[MSG] ");
        USE_SERIAL.print(payload);
        Serial.println();
      }
    }
    else
    {
      USE_SERIAL.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }

  delay(5000);
}