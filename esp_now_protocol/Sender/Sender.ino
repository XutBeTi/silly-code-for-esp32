#include <esp_now.h> // Thư viện bắt buộc để sử dụng giao thức ESP-NOW
#include <WiFi.h>    // Thư viện WiFi, cần thiết để thiết lập chế độ Wi-Fi cho ESP-NOW

#define dt 4 // Định nghĩa chân GPIO 33 để đọc dữ liệu (analogRead)


uint8_t receiverAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 

// Dữ liệu gửi
typedef struct struct_message { // Định nghĩa cấu trúc dữ liệu để gửi
 int value;
} struct_message;

struct_message dataToSend; 

void OnDataSent(const esp_now_send_info_t *info, esp_now_send_status_t status){
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);    
  WiFi.disconnect();     
  if (esp_now_init() != ESP_OK) { 
    return;
  }
  
  esp_now_register_send_cb(OnDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0; 
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    return;
  }

  pinMode(dt, INPUT_PULLDOWN);
}

void loop() {
  dataToSend.value = digitalRead(dt);

  esp_now_send(
    receiverAddress, 
    (uint8_t *)&dataToSend, 
    sizeof(dataToSend)      
  ); 
  
  delay(100); 
}