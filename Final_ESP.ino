
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <SoftwareSerial.h>

/*
SOURCES:
  - https://forum.arduino.cc/t/arduino-mega-2560-serial-communication-to-esp8266-nodemcu/1057321/11
  - https://newbiely.com/tutorials/esp8266/esp8266-mysql
  - https://www.instructables.com/noobs-guide-to-ESP8266-with-Arduino-Mega-2560-or-U/
  
*/

SoftwareSerial SUART(D2, D1);

const char *ssid = "Anwaraiphone";
const char *password = "anwarabera";

String hostName = "elvis.rowan.edu";
String pathName = "/~kalafu84/ESPFinalProject/index.php";

void setup() {
  Serial.begin(9600);
  SUART.begin(9600);
  WiFi.begin(ssid, password);

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.print(++i);
    Serial.println();
  }
  Serial.println(WiFi.localIP());
}

void loop() {

  while (SUART.available() > 0) {
    String postData = SUART.readString();
    Serial.println(postData);

    WiFiClient client;
    HTTPClient http;

    http.begin(client, "http://elvis.rowan.edu/~kalafu84/ESPFinalProject/index.php");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int httpCode = http.POST(postData);
    // if connection established then do this
    if (httpCode == 200) {
      Serial.println("Values uploaded successfully.");
      Serial.println(httpCode);
      String webpage = http.getString();  // Get html webpage output and store it in a string
      Serial.println(webpage + "\n");
    }

    // if failed to connect then return and restart

    else {
      Serial.println(httpCode);
      Serial.println("Failed to upload values. \n");
      return;
    }
    http.end();
  }
}
