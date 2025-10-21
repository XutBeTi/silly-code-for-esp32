// src/main.cpp  (hoặc main.ino nếu dùng Arduino IDE)
#include <Arduino.h>
#include <IRremote.h>   // thư viện Arduino-IRremote (Armin). Cài từ Library Manager nếu chưa có.

#define RECV_PIN 34     // chân DATA của mắt IR (ESP32) - input-only pin

void setup() {
  Serial.begin(112500);            // theo yêu cầu của bạn
  while (!Serial) { delay(10); }   // chờ Serial (không bắt buộc trên ESP32, an toàn khi dùng USB)
  Serial.println("IR receiver starting...");

  // Khởi động IrReceiver trên chân RECV_PIN
  // Một số phiên bản thư viện hỗ trợ IrReceiver.begin(pin, ENABLE_LED_FEEDBACK)
  // nhưng begin(pin) là đủ cho đa số.
  IrReceiver.begin(RECV_PIN);
  IrReceiver.enableIRIn(); // gọi thêm nếu dùng phiên bản yêu cầu
  Serial.print("Listening on pin ");
  Serial.println(RECV_PIN);
}

void loop() {
  // Kiểm tra xem có dữ liệu IR đã decode chưa
  if (IrReceiver.decode()) {
    // IrReceiver.decodedIRData chứa dữ liệu đã giải mã
    // decodedRawData là dạng thô/64-bit chứa mã (tuỳ giao thức)
    unsigned long long raw = IrReceiver.decodedIRData.decodedRawData;
    uint8_t bits = IrReceiver.decodedIRData.numberOfBits; // số bit hữu ích (nếu thư viện cung cấp)

    // In ra HEX an toàn cho số 64-bit
    char buf[32];
    snprintf(buf, sizeof(buf), "HEX: 0x%llX  (bits: %u)", raw, bits);
    Serial.println(buf);

    // Bạn có thể in cả thông tin rút gọn do thư viện hỗ trợ:
    // IrReceiver.printIRResultShort(&Serial);
    // hoặc in chi tiết:
    // IrReceiver.printIRResultRawFormatted(&Serial, true);

    IrReceiver.resume(); // sẵn sàng nhận khung tiếp theo
  }

  // Nếu không cần vòng lặp gắt, thêm một delay nhỏ để nhẹ CPU
  delay(5);
}
