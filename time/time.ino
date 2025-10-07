#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WiFi.h>
#include <LiquidCrystal_I2C.h> // Thư viện LCD

// Khai báo đối tượng LCD (Địa chỉ I2C, số cột, số dòng)
LiquidCrystal_I2C lcd(0x27, 16, 2); 

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7*3600); // múi giờ VN +7


const char* ssid = "HiimTus";
const char* password = "12345678bangchu";

void setup(){
    Serial.begin(115200);

    Wire.begin(21,22);

    // 1. Khởi tạo LCD ngay từ đầu
    lcd.init();
    lcd.backlight();
    lcd.print("Connecting WiFi...");

    Serial.print("Connecting WiFi");

    // 2. Kết nối WiFi
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        // Hiển thị trạng thái chấm trên LCD
        lcd.setCursor(0, 1);
        lcd.print("                "); // Xóa dòng
        lcd.setCursor(0, 1);
        for (int i = 0; i < (millis() / 500) % 16; i++) {
          lcd.print(".");
        }
    }

    Serial.println();
    Serial.println("WiFi Connected");
    
    // 3. Khởi tạo NTP SAU KHI WiFi đã kết nối ổn định
    timeClient.begin();
    
    // Thông báo thành công trên LCD
    lcd.clear();
    lcd.print("WiFi Connected!");
    lcd.setCursor(0, 1);
    lcd.print("NTP Running...");
    
    delay(2000);
    lcd.clear();
}

void loop() {
    timeClient.update();
    
    String formattedTime = timeClient.getFormattedTime(); // Lấy giờ (HH:MM:SS)
    
    // In ra Serial
    Serial.println(formattedTime);
    
    // In ra LCD
    // Dòng 1: Tiêu đề
    lcd.setCursor(0, 0);
    lcd.print("Current Time:");
    
    // Dòng 2: Thời gian thực
    lcd.setCursor(0, 1);
    lcd.print(formattedTime);
    
    delay(1000); // Cập nhật mỗi giây
}