#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif

#include <WiFiManager.h> // Thư viện này tự nhận diện chip bên trong

void setup() {
  Serial.begin(115200);
  delay(1000); // Đợi Serial ổn định
  Serial.println("\n--- He thong bat dau khoi dong ---");

  // Khởi tạo WiFiManager
  WiFiManager wm;

  /* * LƯU Ý: Dòng resetSettings() bên dưới sẽ xóa sạch WiFi đã lưu mỗi khi khởi động.
   * Chỉ nên mở comment (bỏ //) khi bạn muốn test quá trình cấu hình lại từ đầu.
   */
  // wm.resetSettings();

  // Thiết lập thời gian chờ (timeout) cho Portal 
  // Nếu sau 3 phút không có ai cấu hình, ESP sẽ tự thoát Portal để chạy code tiếp theo hoặc restart
  wm.setConfigPortalTimeout(180); 

  // Cố gắng kết nối với WiFi đã lưu, nếu không thấy sẽ mở Access Point tên "ESP_Config_AP"
  // Mật khẩu AP là "12345678"
  if (!wm.autoConnect("ESP_Config_AP", "12345678")) {
    Serial.println("❌ Khong ket noi duoc va qua thoi gian cho (Timeout).");
    delay(3000);
    ESP.restart(); // Khởi động lại để thử lại
  }

  // Nếu chạy đến đây nghĩa là đã kết nối thành công
  Serial.println("\n✅ DA KET NOI WIFI THANH CONG!");
  Serial.print("Dia chi IP cua ESP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Code chính của bạn chạy ở đây
}