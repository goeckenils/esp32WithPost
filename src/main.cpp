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

const char *ssid = "INCOQNITO-HEADQUARTER";
const char *password = "G5iiv3J1";

WiFiMulti wifiMulti;
OneWire oneWire(4);
DallasTemperature sensors(&oneWire);
float temp;


const char *root_ca =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIJEzCCBvugAwIBAgITLQALhFkkcMjGVaDdXQAAAAuEWTANBgkqhkiG9w0BAQsF\n"
    "ADCBizELMAkGA1UEBhMCVVMxEzARBgNVBAgTCldhc2hpbmd0b24xEDAOBgNVBAcT\n"
    "B1JlZG1vbmQxHjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3JhdGlvbjEVMBMGA1UE\n"
    "CxMMTWljcm9zb2Z0IElUMR4wHAYDVQQDExVNaWNyb3NvZnQgSVQgVExTIENBIDUw\n"
    "HhcNMTkwOTI0MDIxODU2WhcNMjEwOTI0MDIxODU2WjAeMRwwGgYDVQQDDBMqLmF6\n"
    "dXJld2Vic2l0ZXMubmV0MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA\n"
    "pz6lyA7hEmKqc2dwyLFwruhPgV/bIN+QJ3pUC8iGq3VnT47skYUu7AqGleEhOVOC\n"
    "ZhF7ISkGlODntcHMcCqUnVbWhbekluP240GF0w4DZC49hcZZbHbAPAv/IcDqnwBv\n"
    "aTaiZ/l+fHIkie72AWqftAn8Ip5OO7uSY1OY86mMOBCfLENXQgbUP4YS581Z8zSE\n"
    "tsjniPO++/OwwbyAK7j3yfLw0rlUKEYioEDEk6fMf+ufL6Tio8lFNzl78ceeZoz+\n"
    "x9Re6wWkqCVMjx9bKecrMkEAESvi+SYMtphfxXsrt/VOj2wJI7VhULWQbqudV2RD\n"
    "95x8MkXpGT9USoM/dllJqwIDAQABo4IE2jCCBNYwggH5BgorBgEEAdZ5AgQCBIIB\n"
    "6QSCAeUB4wB3AO5Lvbd1zmC64UJpH6vhnmajD35fsHLYgwDEe4l6qP3LAAABbWEZ\n"
    "wEwAAAQDAEgwRgIhAIHOLcPfiTtrFzVLwbr0fFj2qcxbguhS9Rk4k6QsaQVbAiEA\n"
    "xpz8Hfc3shYfveV/75g3Kc5+JRw4YUJjIo3BvKFPUhgAdwBc3EOS/uarRUSxXprU\n"
    "VuYQN/vV+kfcoXOUsl7m9scOygAAAW1hGb8eAAAEAwBIMEYCIQCLcrLZNBYkXJi5\n"
    "frzmmwq6Bvsbl7atld3AYRWc0Tox4AIhALBeLpk8P5sOVqSSlhTSyg1/tIDv53I7\n"
    "E4KM5azF1ZEFAHYARJRlLrDuzq/EQAfYqP4owNrmgr7YyzG1P9MzlrW2gagAAAFt\n"
    "YRm/PQAABAMARzBFAiEA+aR5wON30hj40P2qnJk4uJTeGtcuKboKNsPcRp+AOtgC\n"
    "ICy5jCAUf2dG0S7ABdmxvXhmaEBaBgKo82VEraj5VXIYAHcAVYHUwhaQNgFK6gub\n"
    "VzxT8MDkOHhwJQgXL6OqHQcT0wwAAAFtYRnAEQAABAMASDBGAiEA8WMTsShVWa0c\n"
    "R3JvxLj27EmwKN2a1SufWl32yx3YQeMCIQDnoomX1neD/rNm8gBc23X5zu3CSM7B\n"
    "kk6eq72N1sabMjAnBgkrBgEEAYI3FQoEGjAYMAoGCCsGAQUFBwMCMAoGCCsGAQUF\n"
    "BwMBMD4GCSsGAQQBgjcVBwQxMC8GJysGAQQBgjcVCIfahnWD7tkBgsmFG4G1nmGF\n"
    "9OtggV2E0t9CgueTegIBZAIBHTCBhQYIKwYBBQUHAQEEeTB3MFEGCCsGAQUFBzAC\n"
    "hkVodHRwOi8vd3d3Lm1pY3Jvc29mdC5jb20vcGtpL21zY29ycC9NaWNyb3NvZnQl\n"
    "MjBJVCUyMFRMUyUyMENBJTIwNS5jcnQwIgYIKwYBBQUHMAGGFmh0dHA6Ly9vY3Nw\n"
    "Lm1zb2NzcC5jb20wHQYDVR0OBBYEFMCvcFYiJ7SwEyi2FVzBBRhjIKR0MAsGA1Ud\n"
    "DwQEAwIEsDB8BgNVHREEdTBzghMqLmF6dXJld2Vic2l0ZXMubmV0ghcqLnNjbS5h\n"
    "enVyZXdlYnNpdGVzLm5ldIISKi5henVyZS1tb2JpbGUubmV0ghYqLnNjbS5henVy\n"
    "ZS1tb2JpbGUubmV0ghcqLnNzby5henVyZXdlYnNpdGVzLm5ldDCBrAYDVR0fBIGk\n"
    "MIGhMIGeoIGboIGYhktodHRwOi8vbXNjcmwubWljcm9zb2Z0LmNvbS9wa2kvbXNj\n"
    "b3JwL2NybC9NaWNyb3NvZnQlMjBJVCUyMFRMUyUyMENBJTIwNS5jcmyGSWh0dHA6\n"
    "Ly9jcmwubWljcm9zb2Z0LmNvbS9wa2kvbXNjb3JwL2NybC9NaWNyb3NvZnQlMjBJ\n"
    "VCUyMFRMUyUyMENBJTIwNS5jcmwwTQYDVR0gBEYwRDBCBgkrBgEEAYI3KgEwNTAz\n"
    "BggrBgEFBQcCARYnaHR0cDovL3d3dy5taWNyb3NvZnQuY29tL3BraS9tc2NvcnAv\n"
    "Y3BzMB8GA1UdIwQYMBaAFAj+JZ906ocEwry7jqg4XzPG0WxlMB0GA1UdJQQWMBQG\n"
    "CCsGAQUFBwMCBggrBgEFBQcDATANBgkqhkiG9w0BAQsFAAOCAgEAThfa7DRA8Eyi\n"
    "zAzX4HqntgVaw2DE0508I1aseAb+x9sRDmaDKucHbWyc46WjbBysy7vOI1+2PTal\n"
    "ovI8g2n15zasa8odM0vfhQW9goS3Z/5bJGuwKzlW3tL8wuBh/QPKNPJ0mGyQU2kG\n"
    "XFg2bo0PuBaa2tsiH2EIZilEomq8RS+4nMObpegEkW/Vf8IKdUI9AY1PIC9axbbn\n"
    "h9q8jyHr6AzCuwZOwtuCjcoUzbEHUuTJOk/ZbXAgUY+r8AVKIYVQfjDgKa7VbKf8\n"
    "pLqbHJEt8cT/Vbje1ZOTrJ2fV8L2udq73AAid6n2rbVRrwfeUEcGQmSxABXPJRpX\n"
    "O0TvD22E+y4YfKvsFGz8sE6WOxIYKCklS0+tuF/bNlH7i1zLSygVn3NbUuo3p0q+\n"
    "xI8ngrrw7IpvuLb/KeGTR2tQADMqFzfPGrtgv3wabS44FItpks3ZaPrcrrgLTj7G\n"
    "Pv0mtaK34cMiUd5exKCbbL6Kt1PVhczUaKr87RgbvxQBqIghFqwas36KMDNQH5Z8\n"
    "Z9nD5uV7Iaqcm9Om3g0bll84iEeZsx5UWUvQu0az96TK0gL7Ds8rt8amMWi0dIDl\n"
    "CigvhGM12gzJpM43qhyjBqcFlRPjAROyK7iwR/xGnjApqabLPq+UI7iPFcSwx1Ea\n"
    "5QI647alyLpIiln/4mYqxDDFr4kE5iY=\n"
    "-----END CERTIFICATE-----\n";


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
  doc["ip"] =  WiFi.localIP().toString();
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

    http.begin("https://esp32-iot.azurewebsites.net/api");

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

  delay(60000);
}