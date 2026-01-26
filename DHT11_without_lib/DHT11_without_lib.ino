#define DHT11_PIN 32

byte data[5]; //2 byte nhiệt độ + 2 byte độ ẩm + 1 byte checksum 

void setup(){
  Serial.begin(115200);
  // Chờ cảm biến ổn định sau khi cấp nguồn
  delay(1000);
}

void readDHT11() {
    //Reset mảng data về 0 trước mỗi lần đọc
    for (int i = 0; i < 5; i++) data[i] = 0;
    uint32_t pulseLen;

    // --- 1. GỬI TÍN HIỆU START ---
    pinMode(DHT11_PIN, OUTPUT);
    digitalWrite(DHT11_PIN, LOW);
    delay(20);             // Kéo thấp ít nhất 18ms
    digitalWrite(DHT11_PIN, HIGH);
    delayMicroseconds(30); 
    pinMode(DHT11_PIN, INPUT);

    // --- 2. KIỂM TRA PHẢN HỒI  ---
    pulseLen = micros();
    while(digitalRead(DHT11_PIN) == HIGH) {
        if (micros() - pulseLen > 100) { // Nếu quá 100 micors mà không có tín hiệu kéo xuống của DHT11 thì báo lỗi
            Serial.println("Ngoài thời gian phản hồi!");
            return;
        }
    }

    // Đọc và xác nhận tín hiệu phản hồi từ DHT (LOW 80us rồi HIGH 80us)
    while(digitalRead(DHT11_PIN) == LOW); 
    while(digitalRead(DHT11_PIN) == HIGH);
    // 2 vòng while không lệnh là để chờ tín hiệu phản hồi qua 

    // --- 3. ĐỌC 40 BIT DỮ LIỆU ---
    // Không nên dùng các hàm ngắt để đảm bảo đo thời gian chính xác
    for (int i = 0; i < 40; i++) {
        // xác nhận tín hiệu mức thấp trước khi gửi dữ liệu mỗi bit
        while(digitalRead(DHT11_PIN) == LOW);

        // tính toán thời gian mức HIGH để suy ra bit 1 hay 0
        uint32_t startTime = micros(); 
        while(digitalRead(DHT11_PIN) == HIGH);
        pulseLen = micros() - startTime; 

        data[i / 8] <<= 1; //DHT11 gửi theo MSBFIRST nên mỗi bit mới nhận được thì phải cho vào cuối cùng
        if (pulseLen > 40) { // Ngưỡng 40us là vừa đủ để phân biệt 0 và 1 (28 so với 70)
            data[i / 8] |= 1;
        }
    }

    // --- 4. KIỂM TRA CHECKSUM & IN ---
    if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
        float humidity = data[0] + data[1] * 0.1;
        float temperature = data[2] + data[3] * 0.1;

        //in thông tin lấy 2 chữ số thập phân
        Serial.println("--- Dữ liệu từ DHT11 ---");
        Serial.print("Độ ẩm:    "); Serial.print(humidity, 2); Serial.println(" %");
        Serial.print("Nhiệt độ: "); Serial.print(temperature, 2); Serial.println(" *C");
        Serial.println("------------------------");
    } else {
        Serial.println("Lỗi Checksum!");
    }
}

void loop(){
  readDHT11();
  delay(2000); 
}