#define d1 17
#define d2 5
#define d3 18
#define d4 19
#define ds 25
#define st 33
#define sh 32   
#define dl 34   
#define button 0

volatile int dir = 0;  // hướng đếm

void IRAM_ATTR handleButton() {
  dir = !dir;  // đảo hướng
}

int digit[10] = {
  0b11111100, // 0
  0b01100000, // 1
  0b11011010, // 2
  0b11110010, // 3
  0b01100110, // 4
  0b10110110, // 5
  0b10111110, // 6
  0b11100000, // 7
  0b11111110, // 8
  0b11110110  // 9
};

void setup() {
  pinMode(ds, OUTPUT);
  pinMode(st, OUTPUT);
  pinMode(sh, OUTPUT);
  
  pinMode(d1, OUTPUT);
  pinMode(d2, OUTPUT);
  pinMode(d3, OUTPUT);
  pinMode(d4, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  pinMode(dl, INPUT);
  attachInterrupt(digitalPinToInterrupt(button), handleButton, FALLING);
}

void displayDigit(int num, int selPin) {
  digitalWrite(st, LOW);
  shiftOut(ds, sh, LSBFIRST, digit[num]);
  digitalWrite(st, HIGH);
  digitalWrite(selPin, LOW);
  delay(1);
  digitalWrite(selPin, HIGH);
}

void loop() {
  static int value = 0;

  // Tách từng chữ số
  int d4v = value % 10;
  int d3v = (value / 10) % 10;
  int d2v = (value / 100) % 10;
  int d1v = (value / 1000) % 10;

  // Quét hiển thị nhanh
  for (int i = 0; i < map (analogRead(34),0,4095,1,100); i++) {
    displayDigit(d1v, d1);
    displayDigit(d2v, d2);
    displayDigit(d3v, d3);
    displayDigit(d4v, d4);
  }

  // Đếm
  if (dir == 0) value++;
  else value--;
  if (value > 9999) value = 0;
  if (value < 0) value = 9999;
}
