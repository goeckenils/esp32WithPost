#include <DallasTemperature.h>
#include <OneWire.h>
#include <WiFi.h>
#include <HTTPClient.h>
//#include <string.h>

const char *ssid = "INCOQNITO-HEADQUARTER";
const char *password = "G5iiv3J1";

#define BLUE 17
#define RED 5
#define GREEN 16

OneWire oneWire(4); // pin D6
DallasTemperature sensors(&oneWire);
float temp;

void setup()
{
  Serial.begin(115200);
  delay(4000);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");

  Serial.println("DS18B20 thermometer");

  sensors.setResolution(12);

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  sensors.begin();
}

void loop()
{
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);

  Serial.print("Temp is ");
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

  if (WiFi.status() == WL_CONNECTED)
  {

    HTTPClient http;

    http.begin("http://jsonplaceholder.typicode.com/posts");
    http.addHeader("Content-Type", "text/plain");

    int httpResponseCode = http.POST(String(temp));

    if (httpResponseCode > 0)
    {

      String response = http.getString();

      Serial.println(httpResponseCode);
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

  delay(10000);
}
