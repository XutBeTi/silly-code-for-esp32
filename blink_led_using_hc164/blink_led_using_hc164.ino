#define DT 16   // Data pin
#define CL 17   // Clock pin
#define DL 34   // Biến trở (analog)

unsigned long prevTime = 0;  // thời điểm lần cuối cùng gửi clock
int delayTime = 200;         // thời gian giữa hai xung clock (ms)
int i = 0;

void setup() {
  pinMode(DT, OUTPUT);
  pinMode(CL, OUTPUT);
  pinMode(DL, INPUT);
}

void loop() {
  // luôn đọc giá trị biến trở
  int val = analogRead(DL);
  delayTime = map(val, 0, 4095, 0, 1000); // tốc độ thay đổi ngay lập tức

  // kiểm tra đã tới thời điểm tạo xung clock chưa
  if (millis() - prevTime >= delayTime) {
    prevTime = millis(); // cập nhật mốc thời gian

    // gửi bit (ví dụ: bit xen kẽ 0–1)
    digitalWrite(CL, LOW);
    digitalWrite(DT, i & 1);
    digitalWrite(CL, HIGH);

    i++; // tăng bit đếm
    if (i >= 8) i = 0;  // lặp lại 8 bit
  }

}
