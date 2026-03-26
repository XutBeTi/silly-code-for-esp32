#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include <WiFiManager.h>          // Thêm thư viện WiFiManager

#define OUTPUT_PIN 0

// Các thông tin định danh Device giữ nguyên
const char DEVICE_LOGIN_NAME[]  = "8fd033ab-46e5-4f88-954e-0cb46c721438";
const char DEVICE_KEY[]         = "?gojcX1qVfh#VApDAxj3YCJMA";    

void onSwitchChange();
CloudSwitch Switch;

void initProperties(){
  ArduinoCloud.setBoardId(DEVICE_LOGIN_NAME);
  ArduinoCloud.setSecretDeviceKey(DEVICE_KEY);
  ArduinoCloud.addProperty(Switch, READWRITE, ON_CHANGE, onSwitchChange);
}

// Khong dien SSID va pass co dinh vao nua 
WiFiConnectionHandler ArduinoIoTPreferredConnection("", ""); 

void setup() {
  Serial.begin(115200);
  delay(1500); 

  pinMode(OUTPUT_PIN, OUTPUT);

  WiFiManager wm;

  Serial.println("Dang khoi chay WiFiManager...");
  
  // Tu dong ket noi, neu khong duoc se phat ra WiFi ten "ESP_Config_AP"
  if (!wm.autoConnect("ESP32_Config_AP")) {
    Serial.println("Ket noi that bai va timeout!");
    ESP.restart();
  }
  
  Serial.println("WiFi da ket noi thanh cong!");

  // Khoi tao thuoc tinh Cloud
  initProperties();

  // Bat dau ket noi Cloud (Luc nay WiFi da co san nho WiFiManager)
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  ArduinoCloud.update();
}

void onSwitchChange() {
  Switch ? digitalWrite(OUTPUT_PIN,LOW) : digitalWrite(OUTPUT_PIN,HIGH);
}