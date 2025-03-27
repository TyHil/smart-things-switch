#include "secrets.h"

#include <WiFiNINA.h>

#define ON_PIN 2
#define OFF_PIN 10

#define ST_SERVER "api.smartthings.com"
#define SSL_PORT 443

#define CONNECT_WAIT_SECONDS 10
uint64_t timer = 0;

WiFiSSLClient client;

const char ALL_ON[] = "93c62696-9736-46a2-b7f9-80743479a8d6";
const char ALL_OFF[] = "0ed3da72-dc00-484a-b960-429ff357e949";

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ON_PIN, INPUT_PULLUP);
  pinMode(OFF_PIN, INPUT_PULLUP);
  //Serial.begin(9600); //Serial.println();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void toggleDevice(const char * device) {
  client.connect(ST_SERVER, SSL_PORT);
  client.print("POST /v1/devices/");
  client.print(device);
  client.println("/commands HTTP/1.1");
  client.print("Accept: ");
  client.println("application/json");
  client.println("Content-Type: application/json");
  client.print("Content-Length: ");
  client.println(72);
  client.print("Host: ");
  client.println(ST_SERVER);
  client.println("User-Agent: Arduino/1.0");
  client.print("Authorization: Bearer ");
  client.println(SMARTTHINGS_TOKEN);
  client.println("Connection: close");
  client.println();
  client.println("{\"commands\":[{\"component\":\"main\",\"capability\":\"switch\",\"command\":\"on\"}]}");
  client.println();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    if (millis() - timer >= 1000 * CONNECT_WAIT_SECONDS) {
      timer = millis();
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    }
    digitalWrite(LED_BUILTIN, !(digitalRead(LED_BUILTIN)));
    delay(250);
  } else {
    digitalWrite(LED_BUILTIN, false);
    if (!digitalRead(ON_PIN)) {
      digitalWrite(LED_BUILTIN, true);
      toggleDevice(ALL_ON);
    }
    if (!digitalRead(OFF_PIN)) {
      digitalWrite(LED_BUILTIN, true);
      toggleDevice(ALL_OFF);
    }
    /*while (client.available()) {
      char c = client.read();
      Serial.write(c);
    }*/
    delay(100);
  }
}
