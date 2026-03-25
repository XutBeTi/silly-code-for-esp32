#include <WiFi.h>
#include <WiFiManager.h>
#include <Espalexa.h>

#define LED1 16
#define LED2 17

Espalexa espalexa;

void firstLightChanged(uint8_t brightness) {
  digitalWrite(LED1, brightness > 0 ? HIGH : LOW);
  Serial.printf("LED1: %d\n", brightness);
}

void secondLightChanged(uint8_t brightness) {
  digitalWrite(LED2, brightness > 0 ? HIGH : LOW);
  Serial.printf("LED2: %d\n", brightness);
}

void setup() {
  Serial.begin(115200);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  WiFiManager wm;

  if (!wm.autoConnect("ESP32_Alexa_Setup")) {
    Serial.println("Failed to connect WiFi. Restarting...");
    delay(3000);
    ESP.restart();
  }

  Serial.println("WiFi connected!");
  Serial.println(WiFi.localIP());

  espalexa.addDevice("Light A", firstLightChanged);
  espalexa.addDevice("Light B", secondLightChanged);

  espalexa.begin();

  Serial.println("Espalexa started");
}

void loop() {
  espalexa.loop();
}