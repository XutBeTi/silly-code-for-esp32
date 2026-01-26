// Thư viện cần thiết. Dùng <ESP8266WiFi.h> và <ESP8266WebServer.h> nếu dùng ESP8266.
#include <WiFi.h>
#include <WebServer.h>

// --- Cấu hình Wi-Fi Access Point ---
const char* ssid = "LED_Cube_AP"; 
const char* password = "ledcube_password"; 

WebServer server(80);

// --- Định nghĩa Chân cắm ---
#define l1 13
#define l2 12
#define l3 11
#define l4 10

#define ds 15 // Data Serial (MOSI/Data)
#define st 16 // Latch/Store (RCLK/STCP)
#define sh 17 // Shift Clock (SRCLK/SHCP)

// --- Cấu hình Cube và Biến điều khiển ---
int layer_map[4] = {l1, l2, l3, l4};
volatile int ledMode = 1; // 0=Tắt, 1-20=Hiệu ứng
const int NUM_EFFECTS = 20;

bool auto_mode = true; // Trạng thái ban đầu: Bật Auto
unsigned long last_mode_change = 0;
const long MODE_CHANGE_INTERVAL = 10000; // 10 giây cho mỗi hiệu ứng trong chế độ Auto

// --- KHAI BÁO HÀM (Prototypes) ---
void send_data(int data);
void display_frame(int data_0, int data_1, int data_2, int data_3, int delay_ms);
void handleRoot();
void handleSet();
void handleStatus();
int get_x_column_mask(int y);
int get_y_row_mask(int z);

// Khai báo 21 hàm hiệu ứng (0 là Tắt)
void effect_0_all_off();
void effect_1_wall_sweep();
void effect_2_rain_drop();
void effect_3_snake_fill();
void effect_4_diagonal_sweep();
void effect_5_out_to_in();
void effect_6_in_to_out();
void effect_7_spiral_up();
void effect_8_column_pulse();
void effect_9_random_blink();
void effect_10_plane_stack();
void effect_11_wireframe_spin();
void effect_12_checkerboard_flash();
void effect_13_box_spin();
void effect_14_level_rise();
void effect_15_z_up_then_down();
void effect_16_x_sweep_alternate();
void effect_17_y_sweep_alternate();
void effect_18_random_column();
void effect_19_wave_pattern();
void effect_20_full_on_off();

// Mảng con trỏ hàm: Danh sách 21 hàm (index 0 đến 20)
void (*effect_list[NUM_EFFECTS + 1])() = {
    effect_0_all_off, effect_1_wall_sweep, effect_2_rain_drop, effect_3_snake_fill, effect_4_diagonal_sweep,
    effect_5_out_to_in, effect_6_in_to_out, effect_7_spiral_up, effect_8_column_pulse,
    effect_9_random_blink, effect_10_plane_stack, effect_11_wireframe_spin, effect_12_checkerboard_flash,
    effect_13_box_spin, effect_14_level_rise, effect_15_z_up_then_down, effect_16_x_sweep_alternate,
    effect_17_y_sweep_alternate, effect_18_random_column, effect_19_wave_pattern, effect_20_full_on_off
};

// =========================================================
//                  HÀM CƠ BẢN VÀ LED CONTROL
// =========================================================

void send_data(int data) {
    digitalWrite(st, LOW);
    for (int i = 15; i >= 0; i--) { 
        digitalWrite(sh, LOW);
        digitalWrite(ds, (data >> i) & 1);
        digitalWrite(sh, HIGH);
    }
    digitalWrite(st, HIGH);
}

void display_frame(int data_0, int data_1, int data_2, int data_3, int delay_ms = 1) {
    int data_array[4] = {data_0, data_1, data_2, data_3};
    for (int i = 0; i < 4; i++) {
        send_data(~data_array[i]); 
        digitalWrite(layer_map[i], HIGH);
        delay(delay_ms);
        digitalWrite(layer_map[i], LOW);
    }
    server.handleClient(); 
}

// =========================================================
//                        HÀM WEBSERVER
// =========================================================

// HÀM handleRoot ĐÃ ĐƯỢC SỬA LỖI VÀ TÁCH CÁC CHUỖI JS/HTML
void handleRoot() {
    String html = "";
    
    // Thêm phần đầu HTML/CSS
    html += F("<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1>");
    html += F("<title>LED Portal</title><style>body { font-family: Arial, sans-serif; text-align: center; margin-top: 20px; background-color: #f4f4f4; }");
    html += F("h2 { color: #333; margin-bottom: 20px; }.container { display: flex; justify-content: center; gap: 10px; flex-wrap: wrap; max-width: 90%; margin: auto; }");
    html += F(".mode-card { padding: 10px; width: 100px; border-radius: 8px; box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1); cursor: pointer; transition: background-color 0.2s, transform 0.1s; border: 2px solid #ddd; font-weight: bold; color: #333; display: flex; flex-direction: column; justify-content: center; align-items: center; min-height: 50px; font-size: 0.9em; }");
    html += F(".mode-card:hover { transform: scale(1.03); }.active { background-color: #007bff; color: white; border-color: #007bff; box-shadow: 0 4px 8px rgba(0, 123, 255, 0.3); }");
    html += F(".auto-mode { background-color: #ffc107; color: #333; border-color: #ffc107; }.on-off { background-color: #dc3545; color: white; border-color: #dc3545; margin-bottom: 20px;}");
    html += F(".auto-btn { background-color: #28a745; color: white; border-color: #28a745; margin-top: 20px;}");
    html += F("</style></head><body><h2>⚡️ Điều Khiển LED Cube</h2>");
    
    // Thêm các thẻ điều khiển chính
    html += F("<div class=\"container\">");
    html += F("<div class=\"mode-card on-off\" data-mode=\"0\"><h3>TẮT LED</h3></div>");
    
    String autoStatusStr = auto_mode ? "BẬT" : "TẮT";
    html += "<div class=\"mode-card auto-btn\" data-mode=\"21\"><h3>AUTO MODE</h3><small id=\"auto_status\">Trạng thái: " + autoStatusStr + "</small></div>";
    html += F("</div><hr style=\"width: 80%; margin: 20px auto;\"><div class=\"container\" id=\"effects-container\">");
    
    // TẠO THẺ HIỆU ỨNG ĐỘNG BẰNG C++
    const char* effectNames[] = {
        "TẮT (0)", "Quét Tường (1)", "Mưa Rơi (2)", "Rắn Lấp Đầy (3)", "Quét Chéo (4)", 
        "Ngoài Vào (5)", "Trong Ra (6)", "Xoắn Ốc (7)", "Cột Nhấp Nháy (8)", "Ngẫu Nhiên (9)",
        "Tầng Xếp (10)", "Khung Quay (11)", "Bảng Cờ (12)", "Hộp Quay (13)", "Mực Nước (14)",
        "Z Lên Xuống (15)", "Quét X Alt (16)", "Quét Y Alt (17)", "Cột Ngẫu Nhiên (18)", "Sóng (19)",
        "Bật Tắt Full (20)"
    };

    for (int i = 1; i <= 20; i++) {
        html += "<div class=\"mode-card\" data-mode=\"" + String(i) + "\"><h3>" + String(effectNames[i]) + "</h3></div>";
    }

    html += F("</div>"); // Kết thúc container effects

    // Thêm phần JavaScript
    html += F("<script>");
    html += "const currentMode = parseInt('" + String(ledMode) + "');\n";
    html += "const autoMode = ('" + String(auto_mode ? "1" : "0") + "' === '1');\n";
    html += F("const cards = document.querySelectorAll('.mode-card');\n");
    html += F("const autoStatusElement = document.getElementById('auto_status');\n");

    // Hàm setMode
    html += F("function setMode(mode) {");
    html += F("fetch('/set?mode=' + mode)");
    html += F(".then(response => {");
    html += F("fetch('/status').then(res => res.json()).then(data => updateUI(data.mode, data.auto));"); 
    html += F("}).catch(error => { console.error('Error setting mode:', error); });");
    html += F("}");

    // Hàm updateUI
    html += F("function updateUI(newMode, newAutoStatus) {");
    html += F("cards.forEach(card => {");
    html += F("const mode = parseInt(card.getAttribute('data-mode'));");
    html += F("card.classList.remove('active');");
    html += F("if (mode === newMode && newMode != 21) { card.classList.add('active'); }");
    html += F("});");
    html += F("const autoCard = document.querySelector('.mode-card[data-mode=\"21\"]');");
    html += F("autoCard.classList.remove('active');");
    html += F("if (newAutoStatus) { autoCard.classList.add('active'); autoStatusElement.textContent = 'Trạng thái: BẬT'; }");
    html += F("else { autoStatusElement.textContent = 'Trạng thái: TẮT'; }");
    html += F("document.querySelectorAll('.mode-card:not([data-mode=\"0\"]):not([data-mode=\"21\"])').forEach(card => {");
    html += F("card.style.pointerEvents = newAutoStatus ? 'none' : 'auto';");
    html += F("card.style.opacity = newAutoStatus ? '0.5' : '1.0';");
    html += F("});");
    html += F("}");
    
    // Xử lý sự kiện click
    html += F("cards.forEach(card => {");
    html += F("card.addEventListener('click', function() {");
    html += F("const newMode = parseInt(this.getAttribute('data-mode'));");
    html += F("setMode(newMode);");
    html += F("});");
    html += F("});");

    html += F("updateUI(currentMode, autoMode);"); // Thiết lập màu sắc ban đầu
    html += F("</script></body></html>");
    
    server.send(200, "text/html", html);
}

void handleSet() {
    if (server.hasArg("mode")) {
        int requestedMode = server.arg("mode").toInt();
        if (requestedMode == 21) {
            auto_mode = !auto_mode;
            if (auto_mode) {
                ledMode = 1; 
                last_mode_change = millis();
            }
        } else if (requestedMode >= 0 && requestedMode <= NUM_EFFECTS) {
            auto_mode = false; 
            ledMode = requestedMode;
        }
    }
    server.send(200, "text/plain", "OK");
}

void handleStatus() {
    String json = "{\"mode\":" + String(ledMode) + ", \"auto\":" + (auto_mode ? "true" : "false") + "}";
    server.send(200, "application/json", json);
}

// =========================================================
//                   SETUP VÀ VÒNG LẶP CHÍNH
// =========================================================

void setup() {
    Serial.begin(115200);
    
    // 1. Khởi tạo chân LED
    pinMode(ds, OUTPUT);
    pinMode(st, OUTPUT);
    pinMode(sh, OUTPUT);
    for (int i : layer_map) {
        pinMode(i, OUTPUT);
        digitalWrite(i, LOW);
    }
    randomSeed(analogRead(A0)); 

    // 2. Khởi tạo Wi-Fi Access Point (AP)
    WiFi.softAP(ssid, password);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("Access Point IP: ");
    Serial.println(IP);

    // 3. Khởi tạo WebServer
    server.on("/", handleRoot);
    server.on("/set", handleSet);
    server.on("/status", handleStatus);
    server.begin();

    last_mode_change = millis(); // Thiết lập thời gian bắt đầu
}

void loop() {
    // 1. Logic Tự động đổi Mode
    if (auto_mode && (millis() - last_mode_change >= MODE_CHANGE_INTERVAL)) {
        ledMode++;
        if (ledMode > NUM_EFFECTS) {
            ledMode = 1; 
        }
        last_mode_change = millis();
    }

    // 2. Chạy Hiệu ứng hiện tại
    if (ledMode >= 0 && ledMode <= NUM_EFFECTS) {
        effect_list[ledMode]();
    } else {
        effect_0_all_off();
        ledMode = 0;
    }
}


// =========================================================
//                    ĐỊNH NGHĨA HÀM HIỆU ỨNG (ĐÃ SỬA LỖI NAME CONFLICT)
// =========================================================

// Hàm Utility
int get_x_column_mask(int y) {
    int mask = 0;
    for (int j = 0; j < 4; j++) mask |= (1 << (y + 4 * j));
    return mask;
}
int get_y_row_mask(int z) {
    int mask = 0;
    for (int j = 0; j < 4; j++) mask |= (1 << (4 * z + j));
    return mask;
}

void effect_0_all_off() { display_frame(0, 0, 0, 0, 10); }

void effect_1_wall_sweep() { 
    static int x = 0; static bool up = true;
    display_frame((x == 0 ? 0xFFFF : 0), (x == 1 ? 0xFFFF : 0), (x == 2 ? 0xFFFF : 0), (x == 3 ? 0xFFFF : 0), 30);
    if (up) x++; else x--;
    if (x == 3) up = false; else if (x == 0) up = true;
}

void effect_2_rain_drop() { 
    int layers[4] = {0};
    for (int i = 0; i < 4; i++) layers[i] = random(0, 0xFFFF);
    display_frame(layers[0], layers[1], layers[2], layers[3], 5);
}

void effect_3_snake_fill() { 
    static int bit = 0; static int layer = 0;
    int data = (1 << (bit + 1)) - 1;
    display_frame((layer == 0 ? data : 0), (layer == 1 ? data : 0), (layer == 2 ? data : 0), (layer == 3 ? data : 0), 5);
    bit = (bit + 1) % 16;
    if (bit == 0) layer = (layer + 1) % 4;
}

void effect_4_diagonal_sweep() { 
    static int i = 0;
    int mask = 1 << i;
    display_frame(mask, mask, mask, mask, 5);
    i = (i + 1) % 16;
}

void effect_5_out_to_in() { 
    static int step = 0;
    int masks[] = {0xF00F, 0x3C3C, 0x1818, 0x0000};
    int mask = masks[step];
    display_frame(mask, mask, mask, mask, 50);
    step = (step + 1) % 4;
}

void effect_6_in_to_out() { 
    static int step = 3;
    int masks[] = {0xF00F, 0x3C3C, 0x1818, 0x0000};
    int mask = masks[step];
    display_frame(mask, mask, mask, mask, 50);
    step = (step - 1 + 4) % 4;
}

void effect_7_spiral_up() { 
    static int l = 0;
    display_frame((l==0?0xFFFF:0), (l==1?0xFFFF:0), (l==2?0xFFFF:0), (l==3?0xFFFF:0), 30);
    l = (l + 1) % 4;
}

void effect_8_column_pulse() { 
    static int col = 0;
    int mask = (1 << col);
    for(int i=0; i<3; i++) display_frame(mask, mask, mask, mask, 5);
    for(int i=0; i<3; i++) display_frame(0, 0, 0, 0, 5);
    col = (col + 1) % 16;
}

void effect_9_random_blink() { 
    int r = random(0, 0xFFFF);
    display_frame(r, r, r, r, 5);
}

// SỬA LỖI: Đổi tên biến cục bộ l0, l1, l2, l3 thành data_l0, data_l1, ...
void effect_10_plane_stack() { 
    static int l = 0;
    int data_l0 = (l >= 0) ? 0xFFFF : 0;
    int data_l1 = (l >= 1) ? 0xFFFF : 0;
    int data_l2 = (l >= 2) ? 0xFFFF : 0;
    int data_l3 = (l >= 3) ? 0xFFFF : 0;
    display_frame(data_l0, data_l1, data_l2, data_l3, 20);
    l = (l + 1) % 5;
}

void effect_11_wireframe_spin() { 
    int mask_edge = 0xFFFF ^ 0x3C3C; 
    display_frame(mask_edge, mask_edge, mask_edge, mask_edge, 10);
    delay(10);
}

void effect_12_checkerboard_flash() { 
    int checker_1 = 0xAAAA;
    int checker_2 = 0x5555;
    for(int i=0; i<2; i++) display_frame(checker_1, checker_2, checker_1, checker_2, 5);
    for(int i=0; i<2; i++) display_frame(checker_2, checker_1, checker_2, checker_1, 5);
}

void effect_13_box_spin() { 
    static int l = 0;
    int box_mask = 0xF00F; 
    display_frame((l == 0 ? box_mask : 0), (l == 1 ? box_mask : 0), (l == 2 ? box_mask : 0), (l == 3 ? box_mask : 0), 20);
    l = (l + 1) % 4;
}

// SỬA LỖI: Đổi tên biến cục bộ l0, l1, l2, l3 thành data_l0, data_l1, ...
void effect_14_level_rise() { 
    static int l = 0;
    int data_l0 = (l >= 0) ? 0xFFFF : 0;
    int data_l1 = (l >= 1) ? 0xFFFF : 0;
    int data_l2 = (l >= 2) ? 0xFFFF : 0;
    int data_l3 = (l >= 3) ? 0xFFFF : 0;
    display_frame(data_l0, data_l1, data_l2, data_l3, 30);
    l = (l + 1) % 5;
}

void effect_15_z_up_then_down() { 
    static int x = 0; static bool up = true;
    display_frame((x == 0 ? 0xFFFF : 0), (x == 1 ? 0xFFFF : 0), (x == 2 ? 0xFFFF : 0), (x == 3 ? 0xFFFF : 0), 30);
    if (up) x++; else x--;
    if (x == 3) up = false; else if (x == 0) up = true;
}

void effect_16_x_sweep_alternate() { 
    static int y = 0;
    int mask = get_x_column_mask(y);
    display_frame(mask, 0, mask, 0, 10);
    display_frame(0, mask, 0, mask, 10);
    y = (y + 1) % 4;
}

void effect_17_y_sweep_alternate() { 
    static int z = 0;
    int mask = get_y_row_mask(z);
    display_frame(mask, 0, 0, mask, 10);
    display_frame(0, mask, mask, 0, 10);
    z = (z + 1) % 4;    
}

void effect_18_random_column() { 
    int col = random(0, 16);
    int mask = (1 << col);
    display_frame(mask, mask, mask, mask, 5);
    display_frame(0, 0, 0, 0, 5);
}

void effect_19_wave_pattern() { 
    int full = 0xFFFF;
    display_frame(full, 0, full, 0, 20);
    display_frame(0, full, 0, full, 20);
}

void effect_20_full_on_off() { 
    int full = 0xFFFF;
    display_frame(full, full, full, full, 100);
    display_frame(0, 0, 0, 0, 100);
}