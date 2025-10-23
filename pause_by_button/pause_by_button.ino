#define DS 25
#define ST 33
#define SH 32
#define BT 22

// Bảng mã hiển thị các số (bit 7 là dấu chấm, Q0 → bit 7 khi dùng LSBFIRST)
const uint8_t digits[10] = {
  0b10000001, // 0
  0b01001111, // 1
  0b10010010, // 2
  0b00000110, // 3
  0b11001100, // 4
  0b00100100, // 5
  0b10100000, // 6
  0b00001111, // 7
  0b10000000, // 8
  0b00000100  // 9
};

bool isPaused = false;
unsigned long lastButtonPress = 0;
const unsigned long debounceDelay = 200; // thời gian chống dội nút (ms)

void setup() {
  pinMode(DS, OUTPUT);
  pinMode(ST, OUTPUT);
  pinMode(SH, OUTPUT);
  pinMode(BT, INPUT_PULLUP);
}

void loop() {
  for (int i = 0; i < 10; i++) {

    // ---- Xử lý nút nhấn ----
    if (digitalRead(BT) == LOW && millis() - lastButtonPress > debounceDelay) {
      isPaused = !isPaused;
      lastButtonPress = millis();
    }

    // ---- Nếu đang pause thì giữ nguyên số hiện tại ----
    while (isPaused) {
      if (digitalRead(BT) == LOW && millis() - lastButtonPress > debounceDelay) {
        isPaused = false;
        lastButtonPress = millis();
      }
      delay(10); // giảm tải CPU
    }

    // ---- Gửi dữ liệu ra 74HC595 ----
    digitalWrite(ST, LOW);
    shiftOut(DS, SH, LSBFIRST, digits[i]);
    digitalWrite(ST, HIGH);

    delay(250);
  }
}
