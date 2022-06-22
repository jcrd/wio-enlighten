#include <rpcWiFi.h>
#include <WiFiClient.h>
#include <TFT_eSPI.h>

#include "env.h"

TFT_eSPI tft;
WiFiServer server(80);

void setup()
{
  tft.begin();
  tft.fillScreen(TFT_BLACK);

  pinMode(WIO_LIGHT, INPUT);

  Serial.begin(115200);
  while (!Serial)
    ;

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  int y = 4;
  tft.setCursor(4, y);
  y += 12;

  Serial.println("Connecting to WiFi...");
  tft.println("Connecting to WiFi...");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);

    tft.setCursor(4, y);
    y += 12;

    Serial.println("Connecting to WiFi...");
    tft.println("Connecting to WiFi...");

    WiFi.begin(ssid, password);
  }

  tft.setCursor(4, y);

  Serial.println(WiFi.localIP());
  tft.println(WiFi.localIP());

  server.begin();
}

void loop()
{
  WiFiClient client = server.available();

  if (client)
  {
    String currentLine = "";
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();

        if (c == '\n')
        {

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0)
          {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:application/json");
            client.println();

            int v = analogRead(WIO_LIGHT);

            client.print("{ \"light\": \"");
            client.print(v);
            client.print("\" }");

            client.println();
            break;
          }
          else
          {
            currentLine = "";
          }
        }
        else if (c != '\r')
        {
          currentLine += c;
        }

        if (currentLine.endsWith("GET /light"))
        {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:application/json");
          client.println();

          int v = analogRead(WIO_LIGHT);
          client.println(v);
          break;
        }
      }
    }

    client.stop();
  }
}
