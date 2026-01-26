// --- Định nghĩa chân LCD ---
#define D4 26
#define D5 25
#define D6 33
#define D7 32

#define RS 12
#define E  27


void pulseEnable() {// xung enabe thỏa mãn yêu cầu thời gian
  digitalWrite(E, LOW);
  delay(1);
  digitalWrite(E, HIGH);
  delay(1);
  digitalWrite(E, LOW);
  delay(1);
}

void send4Bits(byte data) { // gửi 4 bit data ( nibble )
  digitalWrite(D4, (data >> 0) & 0x01);
  digitalWrite(D5, (data >> 1) & 0x01);
  digitalWrite(D6, (data >> 2) & 0x01);
  digitalWrite(D7, (data >> 3) & 0x01);
  pulseEnable();
}


void control (uint8_t data,int data_RS) {
  digitalWrite(RS, data_RS);
  send4Bits(data >> 4); //gửi 4 bit cao 
  send4Bits(data & 0x0F); // gửi 4 bit thấp 
}


void lcdInit() {// khởi tạo lcd
  pinMode(RS, OUTPUT);
  pinMode(E, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);

  delay(20);

  //reset chip điều khiển
  send4Bits(0x3);// do chỉ nối 4 dây nên dữ liệu reset là 0x3 hay 0b11
  delay(5);
  send4Bits(0x3);
  delayMicroseconds(150);
  send4Bits(0x3);
  delayMicroseconds(150);

  // đặt về chế độ 4 bit
  send4Bits(0x02); 

  control(0x28,0); // 4-bit, 2 dòng, font 5x8
  control(0x0C,0); // bật hiển thị, tắt con trỏ
  control(0x06,0); // trỏ dịch phải màn đứng yên 
  control(0x01,0); // xóa màn hình đưa con trỏ về 0
  delay(2);
}
//lệnh set vị trí trỏ
void lcdSetCursor(byte col, byte row) {
  byte addr[] = {0x00, 0x40};
  control(0x80 | (addr[row] + col),0);
}
//dữ liệu chữ cái
void lcdPrint(const char *str) {
  while (*str) control(*str++,1);
}

uint8_t heart[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000
};

void setup() {
  int custom_char_index  = 2;
  lcdInit();
  control (0x40|(custom_char_index*8),0);
  for (int i=0 ;i<8;i++) {
    control (heart[i],1);
  }


  lcdSetCursor(4, 0);
  lcdPrint("XIN CHAO");
  lcdSetCursor(1, 1);
  lcdPrint("ELE CLUB PTIT");
  control(custom_char_index,1);
}

// --- Loop ---
void loop() {}
