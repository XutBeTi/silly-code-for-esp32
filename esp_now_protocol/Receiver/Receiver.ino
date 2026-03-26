#include <esp_now.h>
#include <WiFi.h>

#define L1 18

typedef struct struct_message {
  int value; 
} struct_message;

struct_message incomingData;

void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  memcpy(&incomingData, data, sizeof(incomingData));
  digitalWrite( L1, incomingData.value );
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
  pinMode(L1,OUTPUT);
}

void loop() {
}