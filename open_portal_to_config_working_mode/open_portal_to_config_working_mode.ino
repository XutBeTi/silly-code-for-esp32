#include <WiFi.h>
#include <WebServer.h>

#define LED_PIN 2

WebServer server(80);

int mode = 0;  // 0: tắt, 1: nháy chậm, 2: nháy nhanh

void handleRoot() {
  String html = "<html><head><meta charset='UTF-8'><title>ESP Portal</title></head><body>";
  html += "<h2>🟢 ESP Control Portal</h2>";
  html += "<p>Chế độ hiện tại: <b>" + String(mode) + "</b></p>";
  html += "<a href='/set?mode=0'><button>Tắt LED</button></a> ";
  html += "<a href='/set?mode=1'><button>Nháy chậm</button></a> ";
  html += "<a href='/set?mode=2'><button>Nháy nhanh</button></a>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleSetMode() {
  if (server.hasArg("mode")) {
    mode = server.arg("mode").toInt();
    Serial.println("Chon che do: " + String(mode));
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  // Tạo Access Point riêng
  WiFi.softAP("ESP_Control", "12345678");
  Serial.println("AP da mo: ESP_Control (pass: 12345678)");
  Serial.println("Truy cap: http://192.168.4.1");

  // Cấu hình web server
  server.on("/", handleRoot);
  server.on("/set", handleSetMode);
  server.begin();
  Serial.println("WebServer da bat dau");
}

void loop() {
  server.handleClient();

  // Xử lý LED theo chế độ
  switch (mode) {
    case 0: digitalWrite(LED_PIN, LOW); break;
    case 1: digitalWrite(LED_PIN, HIGH); delay(500); digitalWrite(LED_PIN, LOW); delay(500); break;
    case 2: digitalWrite(LED_PIN, HIGH); delay(150); digitalWrite(LED_PIN, LOW); delay(150); break;
  }
}
