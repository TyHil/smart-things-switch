#include "secrets.h"

#include <WiFiNINA.h>

#define ON_PIN 2
#define OFF_PIN 10

#define ST_SERVER "api.smartthings.com"
#define SSL_PORT 443

WiFiSSLClient SSLClient;

const char ALL_ON[] = "93c62696-9736-46a2-b7f9-80743479a8d6";
const char ALL_OFF[] = "0ed3da72-dc00-484a-b960-429ff357e949";

unsigned long onTimer;
unsigned long offTimer;

void connectWiFI() {
  uint8_t connectAttempts = 15;
  if (WiFi.status() == WL_CONNECTED) return;
  //Serial.println("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while ((WiFi.status() != WL_CONNECTED) && (connectAttempts)) {
    //Serial.println("Connecting to WiFi");
    connectAttempts--;
    delay(500);
  }
  if (!connectAttempts) {
    //Serial.print("Unable to connect to "); Serial.println(WIFI_SSID);
    while (true) {
      digitalWrite(LED_BUILTIN, !(digitalRead(LED_BUILTIN)));
      delay(100);
    }
  }
  //Serial.print("Connected to WiFi "); Serial.println(WiFi.SSID());
  digitalWrite(LED_BUILTIN, false);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, true);
  pinMode(ON_PIN, INPUT_PULLUP);
  pinMode(OFF_PIN, INPUT_PULLUP);
  //Serial.begin(9600); //Serial.println();
  connectWiFI();
}

void toggleDevice(const char * device) {
  SSLClient.connect(ST_SERVER, SSL_PORT);
  SSLClient.print("POST /v1/devices/");
  SSLClient.print(device);
  SSLClient.println("/commands HTTP/1.1");
  SSLClient.print("Accept: ");
  SSLClient.println("application/json");
  SSLClient.println("Content-Type: application/json");
  SSLClient.print("Content-Length: ");
  SSLClient.println(72);
  SSLClient.print("Host: ");
  SSLClient.println(ST_SERVER);
  SSLClient.println("User-Agent: Arduino/1.0");
  SSLClient.print("Authorization: Bearer ");
  SSLClient.println(smartThingsToken);
  SSLClient.println("Connection: close");
  SSLClient.println();
  SSLClient.println("{\"commands\":[{\"component\":\"main\",\"capability\":\"switch\",\"command\":\"on\"}]}");
  SSLClient.println();
}

void loop() {
  digitalWrite(LED_BUILTIN, false);
  if (millis() - onTimer >= 250 && !digitalRead(ON_PIN)) {
    onTimer = millis();
    digitalWrite(LED_BUILTIN, true);
    toggleDevice(ALL_ON);
  }
  if (millis() - offTimer >= 250 && !digitalRead(OFF_PIN)) {
    offTimer = millis();
    digitalWrite(LED_BUILTIN, true);
    toggleDevice(ALL_OFF);
  }
  delay(100);
}
