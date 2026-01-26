#include <Wire.h>

int dem=0;
volatile bool flag=false; //volatile dùng cho các biến có được xử lí bằng ngắt để báo các hàm khác có sự thay đổi biến này từ hàm khác

unsigned long last_time = 0;
void IRAM_ATTR cong() {
  unsigned long now = millis();
  if (now - last_time > 200) {
    flag = true;
    last_time = now;
  }
}

void setup() {
  Wire.begin();
  Wire.setClock(100000);
  pinMode(4, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(4), cong, RISING);   
}

void loop() {
  if (flag){
    flag=false;
    Wire.beginTransmission(0x14);
    Wire.write(++dem);
    Wire.endTransmission();
  }
}
