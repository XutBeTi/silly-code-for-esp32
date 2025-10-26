#include <esp_now.h>
#include <WiFi.h>

// Định nghĩa các chân LED an toàn cho OUTPUT trên ESP32-S3
// Lưu ý: Đã chuyển từ chân Input-only (35-39) sang các chân Output (1-8)
#define L1 18
#define L2 17
#define L3 16
#define L4 15
#define L5 7
#define L6 6
#define L7 5
#define L8 4

// Mảng chứa các chỉ số chân GPIO của LED
int led_index[8] = {L1, L2, L3, L4, L5, L6, L7, L8};
// Biến lưu trữ giá trị đã được xử lý (0-8)
int led_level = 0; 

// Cấu trúc dữ liệu phải giống hệt với bên Gửi (Sender)
typedef struct struct_message {
  int value; 
} struct_message;

// Khai báo biến để chứa dữ liệu nhận được
struct_message incomingData;

// Hàm Callback được gọi khi nhận dữ liệu qua ESP-NOW
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {

  // 1. Sao chép dữ liệu nhận được vào cấu trúc
  memcpy(&incomingData, data, sizeof(incomingData));
  
  int raw_value = incomingData.value;

  led_level = map(raw_value, 0, 4095, 0, 8);
  
  if (led_level > 8) led_level = 8;
  if (led_level < 0) led_level = 0;

  for (int i = 0; i < 8; i++) {
    
    digitalWrite(led_index[i], (i < led_level) ? HIGH : LOW);
  }

  // 4. In kết quả ra Serial Monitor
  Serial.print("Giá trị thô nhận được: ");
  Serial.print(raw_value);
  Serial.print(" -> Mức LED: ");
  Serial.println(led_level);
}

void setup() {
  Serial.begin(115200);

  // 1. Cấu hình các chân LED là OUTPUT
  for (int i = 0; i < 8; i++) {
    pinMode(led_index[i], OUTPUT);
    digitalWrite(led_index[i], LOW); // Tắt tất cả LED khi khởi động
  }

  // 2. Cấu hình WiFi và ESP-NOW
  WiFi.mode(WIFI_STA);
  /*Đặt chế độ Wi-Fi là Station (STA).Bắt buộc đối với ESP-NOW.*/
  WiFi.disconnect();/*Ngắt kết nối Wi-Fi hiện có để đảm bảo tài nguyên và chế độ hoạt động phù hợp cho ESP-NOW.*/

  if (esp_now_init() != ESP_OK) { /*Khởi tạo ESP-NOW.	Cần thiết . Logic kiểm tra lỗi (!= ESP_OK) là đúng.*/
    Serial.println("❌ Lỗi khởi tạo ESP-NOW! Vui lòng khởi động lại.");
    return;
  }
  
  Serial.println("✅ Khởi tạo ESP-NOW thành công. Đang chờ dữ liệu...");

  // 3. Đăng ký hàm callback nhận dữ liệu
  esp_now_register_recv_cb(OnDataRecv);/*Đăng ký hàm callback khi nhận dữ liệu.	Rất quan trọng, đây là nơi hệ thống biết phải làm gì khi có dữ liệu đến*/
}

void loop() {
}