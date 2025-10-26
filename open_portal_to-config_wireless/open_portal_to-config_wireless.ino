#include <WiFi.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

void setup() {
  Serial.begin(115200);
  Serial.println("Khoi dong...");

  WiFiManager wifiManager;

  // Xóa cấu hình WiFi cũ (tùy chọn, chỉ dùng để test)
  wifiManager.resetSettings();//neu duoc kich hoat moi lan khoi dong se yeu cau config lai thay vi tu ket noi lai

  // Tạo portal nếu chưa có WiFi
  if (!wifiManager.autoConnect("ESP_Config_AP", "12345678")) {
    Serial.println("Ket noi that bai, reset ESP...");
    delay(3000);
    ESP.restart();
  }

  Serial.println("Da ket noi WiFi thanh cong!");
  Serial.println(WiFi.localIP());
}

void loop() {
  // code chinh o day
}
