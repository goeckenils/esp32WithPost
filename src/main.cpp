#include <DallasTemperature.h>
#include <OneWire.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32Ping.h>

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

  Serial.println("-------------------");
  IPAddress Ip (172,16,210,124);
  bool ret = Ping.ping(Ip);

  if(ret > 0) {
    Serial.println("ip is available");
  } else {
    Serial.println("ip adress is not available");
  }

  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);

  Serial.print("Temp is ");
  Serial.print(temp, 1);
  Serial.println();

  String ip = String(WiFi.localIP());

  String databuf;

  const size_t CAPACITY = JSON_OBJECT_SIZE(300);
  StaticJsonDocument<CAPACITY> doc;

  JsonObject object = doc.to<JsonObject>();
  object["tempC"] = temp;
  object["ip"] = ip;

  serializeJson(doc, databuf);

  Serial.println(databuf);

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

    http.begin("http://172.16.210.124/api/POST");
    http.addHeader("content-type", "application/json");

    int httpResponseCode = http.POST(databuf);

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

  delay(3000);
}
