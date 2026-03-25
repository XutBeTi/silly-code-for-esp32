#include <WiFi.h>
#include <Espalexa.h>

const char* ssid = "TC HOUSE";
const char* password = "thuycuong";

#define LED1 16
#define LED2 17

Espalexa espalexa;

void firstLightChanged(uint8_t brightness) {
  digitalWrite(LED1, (brightness > 0) ? HIGH : LOW);
  Serial.printf("LED 1: %d\n", brightness);
}

void secondLightChanged(uint8_t brightness) {
  digitalWrite(LED2, (brightness > 0) ? HIGH : LOW);
  Serial.printf("LED 2: %d\n", brightness);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  espalexa.addDevice("Light A", firstLightChanged);
  delay(500);
  espalexa.addDevice("Light B", secondLightChanged);

  espalexa.begin();
}

void loop() {
  espalexa.loop();
  delay(1);
}