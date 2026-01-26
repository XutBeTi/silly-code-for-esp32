#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WiFi.h>
#include <LiquidCrystal_I2C.h> 
#include <Wire.h> 
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define button 0

LiquidCrystal_I2C lcd(0x27, 20, 04); 
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7 * 3600);

const char* ssid = "HiimTus";
const char* password = "12345678bangchu";

const char* OPEN_WEATHER_MAP_API_KEY = "28e21cc0ecfc6cf52c06b0a4e174cc16";
const char* CITY = "Hanoi";
const char* COUNTRY = "VN";

String weatherDescription = "Loading...";
String more_description = "";
float temperature = 0.0;
float feel_like = 0.0;
float humidity = 0.0;
int pressure = 0;
float wind_speed = 0;
float wind_deg = 0;

unsigned long lastWeatherUpdate = 0;
const long WEATHER_UPDATE_INTERVAL = 15 * 60 * 1000;

int index_of_field = 0;
unsigned long last_press = 0;
bool updated = true;

void getWeatherData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    char url[256];
    sprintf(url,
            "http://api.openweathermap.org/data/2.5/weather?q=%s,%s&units=metric&APPID=%s",
            CITY, COUNTRY, OPEN_WEATHER_MAP_API_KEY);

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      DynamicJsonDocument doc(2048);
      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.f_str());
        weatherDescription = "JSON Error!";
      } else {
        temperature = doc["main"]["temp"].as<float>();
        feel_like = doc["main"]["feels_like"].as<float>();
        humidity = doc["main"]["humidity"].as<float>();
        pressure = doc["main"]["pressure"].as<int>();
        wind_speed = doc["wind"]["speed"].as<float>();
        wind_deg = doc["wind"]["deg"].as<float>();
        weatherDescription = doc["weather"][0]["main"].as<String>();
        more_description = doc["weather"][0]["description"].as<String>();
        


        Serial.print("\t ");
        Serial.print(weatherDescription);
        Serial.print(", ");
        Serial.print(temperature);
        Serial.print("°C");
        Serial.print(", ");
        Serial.print(humidity);
        Serial.print("%");
        Serial.print(", ");
        Serial.print(pressure);
        Serial.print("hPa");
        Serial.print(", ");
        Serial.print(wind_speed);
        Serial.print("m/s");
        Serial.print(", ");
        Serial.print(wind_deg);
        Serial.println("°");
     
      }
    } else {
      Serial.printf("[HTTP] GET failed, error: %s\n", http.errorToString(httpCode).c_str());
      weatherDescription = "API Error!";
    }
    http.end();
  } else {
    weatherDescription = "Connecting...";
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(button, INPUT_PULLUP);

  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.print("Connecting WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    for (int i = 0; i < (millis() / 500) % 16; i++) {
      lcd.print(".");
    }
  }

  Serial.println("\nWiFi Connected");
  timeClient.begin();
  timeClient.setUpdateInterval(60000);

  getWeatherData();

  lcd.clear();
  lcd.print("NTP & Weather OK!");
  delay(2000);
  lcd.clear();
}

void loop() {
  unsigned long startTime = millis();

  // --- cập nhật thời gian + thời tiết ---
  timeClient.update();

  if (millis() - lastWeatherUpdate >= WEATHER_UPDATE_INTERVAL) {
    Serial.println(timeClient.getFormattedTime());
    getWeatherData();
    lastWeatherUpdate = millis();
    updated = true;
  }
  
  // --- xử lý nút bấm ---
  if (digitalRead(button) == LOW) {
    if (millis() - last_press > 400) {
      if (WiFi.status() == WL_CONNECTED) {  // chỉ cho bấm khi có WiFi
        index_of_field++;
       
        if (index_of_field > 3) index_of_field = 0;
        lcd.clear();
      } else {
        index_of_field = 0;  // nếu mất WiFi thì chỉ hiển thị case 0
      }
      updated=true;
      last_press = millis();
    }
    while (digitalRead(button) == LOW) delay(10);
  }

  // --- Nếu mất WiFi -> luôn quay về case 0 ---
  if (WiFi.status() != WL_CONNECTED && index_of_field != 0) {
    index_of_field = 0;
    updated=true;
    lcd.clear();
  }

  // --- tự động quay về case 0 sau 5s không bấm ---
  if (millis() - last_press > 5000 && index_of_field != 0) {
    index_of_field = 0;
    updated=true;
    lcd.clear();
  }

  if (!index_of_field){
     
    lcd.setCursor(3, 0);
    lcd.print("Time: ");
    lcd.print(timeClient.getFormattedTime());
  }

  // --- hiển thị ---
  if (updated){

    switch (index_of_field) {
      case 0: {
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);

        if (WiFi.status() == WL_CONNECTED) {
          lcd.print(weatherDescription);
          lcd.setCursor(12, 1);
          lcd.print(temperature, 1);
          lcd.write(0xDF);
          lcd.print("C");
        } else {
          String msg = "Connecting...";
          int startCol = (16 - msg.length()) / 2;
          lcd.setCursor(startCol, 1);
          lcd.print(msg);
        }
        break;
      }

      case 1:
        lcd.setCursor(0, 0);
        lcd.print("  Description   ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor((16 - more_description.length()) / 2, 1);
        lcd.print(more_description);
        break;

      case 2:
        lcd.setCursor(0, 0);
        lcd.print(" Press   Humid  ");
        lcd.setCursor(1, 1);
        lcd.print(pressure);
        lcd.print("hPa");
        lcd.setCursor(9, 1);
        lcd.print(humidity, 1);
        lcd.print("%");
        break;

      case 3:
        lcd.setCursor(0, 0);
        lcd.print(" W-speed  W-dir ");
        lcd.setCursor(1, 1);
        lcd.print(wind_speed, 1);
        lcd.print("m/s");
        lcd.setCursor(10, 1);
        lcd.print(wind_deg, 0);
        lcd.write(0xDF);
        break;
    }
  }
  updated = false ;

  unsigned long runTime = millis() - startTime;
  long remainingDelay = 1000 - runTime;
  if (remainingDelay > 0) delay(remainingDelay);
}
