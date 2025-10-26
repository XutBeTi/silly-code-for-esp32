#include <esp_now.h> // Thư viện bắt buộc để sử dụng giao thức ESP-NOW
#include <WiFi.h>    // Thư viện WiFi, cần thiết để thiết lập chế độ Wi-Fi cho ESP-NOW

#define dt 33 // Định nghĩa chân GPIO 33 để đọc dữ liệu (analogRead)

// ⚙️ MAC của thiết bị nhận (thay đúng MAC thật của ESP32 receiver)
uint8_t receiverAddress[] = {0x3C, 0xDC, 0x75, 0x6B, 0xC0, 0x90}; // Khai báo địa chỉ MAC của thiết bị nhận

// Dữ liệu gửi
typedef struct struct_message { // Định nghĩa cấu trúc dữ liệu để gửi
 int value;
} struct_message;

struct_message dataToSend; // Khai báo biến cấu trúc để chứa dữ liệu sẽ gửi

// 🛰 Callback xác nhận gửi xong (phiên bản mới)
void OnDataSent(const esp_now_send_info_t *info, esp_now_send_status_t status) { // Hàm callback được gọi khi quá trình gửi kết thúc
  Serial.print("Gửi dữ liệu: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "✅ Thành công" : "❌ Thất bại");// Kiểm tra trạng thái gửi: ESP_NOW_SEND_SUCCESS = thành công
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);    // Thiết lập ESP ở chế độ Station (bắt buộc cho ESP-NOW)
  WiFi.disconnect();      // Ngắt kết nối Wi-Fi hiện tại để giải phóng tài nguyên

  if (esp_now_init() != ESP_OK) { // Khởi tạo giao thức ESP-NOW
    Serial.println("❌ Lỗi khởi tạo ESP-NOW!");
    return;
  }
  
  esp_now_register_send_cb(OnDataSent); // Đăng ký hàm callback để xác nhận trạng thái gửi

  // Thêm thiết bị nhận (Peer)
  esp_now_peer_info_t peerInfo = {}; // Khai báo biến cấu trúc peerInfo để lưu thông tin thiết bị nhận
  memcpy(peerInfo.peer_addr, receiverAddress, 6); // Sao chép địa chỉ MAC vào cấu trúc peerInfo
  peerInfo.channel = 0; // Kênh Wi-Fi (0 là kênh mặc định). Phải khớp với bên nhận
  peerInfo.encrypt = false; // Tắt mã hóa (thiết lập mặc định)

  if (esp_now_add_peer(&peerInfo) != ESP_OK) { // Thêm thiết bị nhận vào danh sách Peer
    Serial.println("❌ Thêm peer thất bại!");
    return;
  }

  pinMode(dt, INPUT); // Thiết lập chân dt (GPIO 33) là đầu vào để đọc ADC
}

void loop() {
  dataToSend.value = analogRead(dt); // Đọc giá trị ADC từ chân GPIO 33 (0-4095 trên ESP32)

  // Gửi dữ liệu ESP-NOW
  esp_now_send(
    receiverAddress, // Địa chỉ MAC của thiết bị nhận
    (uint8_t *)&dataToSend, // Con trỏ đến dữ liệu cần gửi (phải ép kiểu sang uint8_t*)
    sizeof(dataToSend)      // Kích thước của dữ liệu (bytes)
  ); 
  
  // Tạm dừng một chút để không gửi quá nhanh, giúp hệ thống ổn định hơn
  delay(100); 
}