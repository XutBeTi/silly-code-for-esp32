#include <Wire.h>
#include <LiquidCrystal_I2C.h> 
#define addr 0x14

LiquidCrystal_I2C lcd(0x27, 16, 2); 

int data=0;

void xuli(int howMany) {
  data=(int)Wire1.read();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(data);
}

void setup() {
  Wire.begin();
  Wire1.begin(addr,18,19,100000);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(data);
  Wire1.onReceive (xuli);
}

void loop() {
}
