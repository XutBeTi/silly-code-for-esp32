#include "esp_task_wdt.h"

#define HEARTBEAT_LED 2 // LED tích hợp

void setup() {
  pinMode(HEARTBEAT_LED, OUTPUT);

  Serial.begin(115200);

  // Cấu hình watchdog mới
  esp_task_wdt_config_t wdt_config = {
    .timeout_ms = 5000,
    .trigger_panic = true
  };
  esp_task_wdt_init(&wdt_config);
  esp_task_wdt_add(NULL); // thêm loop() task
}

long long counting=0;

void loop() {
  esp_task_wdt_reset(); // báo watchdog rằng task vẫn sống
  digitalWrite(HEARTBEAT_LED, !digitalRead(HEARTBEAT_LED)); // nhấp nháy LED
  Serial.print("ESP32 vẫn hoạt động bình thường ping lần thứ : ");
  Serial.println(++counting);

}
