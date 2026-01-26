// --- Định nghĩa Chân cắm ---
 // Shift Clock (SRCLK/SHCP)

// --- Cấu hình Cube và Biến điều khiển ---
int layer_map[4] = {l1, l2, l3, l4};
volatile int ledMode = 1; // 0=Tắt, 1-20=Hiệu ứng
const int NUM_EFFECTS = 20;
int speedFactor = 2; // 1 = rất nhanh, 3 = trung bình, 5 = chậm


bool auto_mode = true; // Trạng thái ban đầu: Bật Auto
unsigned long last_mode_change = 0;
const long MODE_CHANGE_INTERVAL = 10000; // 10 giây cho mỗi hiệu ứng trong chế độ Auto


// --- KHAI BÁO HÀM (Prototypes) ---
void send_data(int data);
void display_frame(int data_0, int data_1, int data_2, int data_3, int display_time );
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

// ===============================
//   PHẦN CẢI TIẾN HIỂN THỊ MƯỢT
// ===============================

// Giữ frame hiện tại và frame kế
uint16_t currentFrame[4] = {0};
uint16_t nextFrame[4] = {0};

// Quét từng tầng liên tục (refresh cube)
void refreshCube() {
    static int layer = 0;
    // Tắt tầng cũ
    digitalWrite(layer_map[layer], LOW);
    layer = (layer + 1) % 4;
    // Hiển thị tầng mới
    send_data(~currentFrame[layer]);
    digitalWrite(layer_map[layer], HIGH);
}

// Gửi dữ liệu ra 74HC595 (không đổi)
void send_data(int data) {
    digitalWrite(st, LOW);
    for (int i = 15; i >= 0; i--) {
        digitalWrite(sh, LOW);
        digitalWrite(ds, (data >> i) & 1);
        digitalWrite(sh, HIGH);
    }
    digitalWrite(st, HIGH);
}

// Chuyển frame mượt (không tắt hẳn)
void updateFrameSmoothly() {
    for (int i = 0; i < 4; i++) {
        currentFrame[i] = nextFrame[i];
    }
}

// Hiển thị frame (mới, không chớp, không delay blocking)
void display_frame(int d0, int d1, int d2, int d3, int display_time) {
    nextFrame[0] = d0;
    nextFrame[1] = d1;
    nextFrame[2] = d2;
    nextFrame[3] = d3;

    unsigned long start = millis();
    while (millis() - start < display_time) {
        refreshCube();                   // quét tầng liên tục
        delayMicroseconds(800);          // thời gian quét mỗi tầng
    }
    updateFrameSmoothly();               // cập nhật frame khi xong
}

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
    } 
    else {
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

// =========================================================
//              HIỆU ỨNG LED CUBE (TỐI ƯU NHỊP)
// =========================================================

void effect_0_all_off() { 
    display_frame(0, 0, 0, 0, 50 * speedFactor);
}

// --- Hiệu ứng quét tường ---
void effect_1_wall_sweep() { 
    static int x = 0; static bool up = true;
    display_frame((x == 0 ? 0xFFFF : 0), (x == 1 ? 0xFFFF : 0), (x == 2 ? 0xFFFF : 0), (x == 3 ? 0xFFFF : 0), 100 * speedFactor);
    if (up) x++; else x--;
    if (x == 3) up = false; else if (x == 0) up = true;
    delay(80 * speedFactor);
}

// --- Mưa rơi ngẫu nhiên ---
void effect_2_rain_drop() { 
    int layers[4];
    for (int i = 0; i < 4; i++) layers[i] = random(0, 0xFFFF);
    display_frame(layers[0], layers[1], layers[2], layers[3], 80 * speedFactor);
    delay(100 * speedFactor);
}

// --- Rắn bò ---
void effect_3_snake_fill() { 
    static int bit = 0; static int layer = 0;
    int data = (1 << (bit + 1)) - 1;
    display_frame((layer == 0 ? data : 0), (layer == 1 ? data : 0), (layer == 2 ? data : 0), (layer == 3 ? data : 0), 100 * speedFactor);
    bit = (bit + 1) % 16;
    if (bit == 0) layer = (layer + 1) % 4;
}

// --- Quét chéo ---
void effect_4_diagonal_sweep() { 
    static int i = 0;
    int mask = 1 << i;
    display_frame(mask, mask, mask, mask, 100 * speedFactor);
    i = (i + 1) % 16;
    delay(100 * speedFactor);
}

// --- Từ ngoài vào trong ---
void effect_5_out_to_in() { 
    static int step = 0;
    int masks[] = {0xF00F, 0x3C3C, 0x1818, 0x0000};
    int mask = masks[step];
    display_frame(mask, mask, mask, mask, 120 * speedFactor);
    step = (step + 1) % 4;
    delay(80 * speedFactor);
}

// --- Từ trong ra ngoài ---
void effect_6_in_to_out() { 
    static int step = 3;
    int masks[] = {0xF00F, 0x3C3C, 0x1818, 0x0000};
    int mask = masks[step];
    display_frame(mask, mask, mask, mask, 120 * speedFactor);
    step = (step - 1 + 4) % 4;
    delay(80 * speedFactor);
}

// --- Xoắn ốc hướng lên ---
void effect_7_spiral_up() { 
    static int l = 0;
    display_frame((l==0?0xFFFF:0), (l==1?0xFFFF:0), (l==2?0xFFFF:0), (l==3?0xFFFF:0), 100 * speedFactor);
    l = (l + 1) % 4;
    delay(80 * speedFactor);
}

// --- Nhấp nháy cột ---
void effect_8_column_pulse() { 
    static int col = 0;
    int mask = (1 << col);
    display_frame(mask, mask, mask, mask, 100 * speedFactor);
    delay(100 * speedFactor);
    display_frame(0, 0, 0, 0, 100 * speedFactor);
    delay(100 * speedFactor);
    col = (col + 1) % 16;
}

// --- Nháy ngẫu nhiên ---
void effect_9_random_blink() { 
    int r = random(0, 0xFFFF);
    display_frame(r, r, r, r, 80 * speedFactor);
    delay(120 * speedFactor);
}

// --- Chồng mặt phẳng ---
void effect_10_plane_stack() { 
    static int l = 0;
    int data_l0 = (l >= 0) ? 0xFFFF : 0;
    int data_l1 = (l >= 1) ? 0xFFFF : 0;
    int data_l2 = (l >= 2) ? 0xFFFF : 0;
    int data_l3 = (l >= 3) ? 0xFFFF : 0;
    display_frame(data_l0, data_l1, data_l2, data_l3, 150 * speedFactor);
    l = (l + 1) % 5;
    delay(100 * speedFactor);
}

// --- Khung xoay ---
void effect_11_wireframe_spin() { 
    int mask_edge = 0xFFFF ^ 0x3C3C; 
    display_frame(mask_edge, mask_edge, mask_edge, mask_edge, 100 * speedFactor);
    delay(150 * speedFactor);
}

// --- Bảng caro nhấp nháy ---
void effect_12_checkerboard_flash() { 
    int checker_1 = 0xAAAA;
    int checker_2 = 0x5555;
    display_frame(checker_1, checker_2, checker_1, checker_2, 100 * speedFactor);
    delay(80 * speedFactor);
    display_frame(checker_2, checker_1, checker_2, checker_1, 100 * speedFactor);
    delay(80 * speedFactor);
}

// --- Hộp quay ---
void effect_13_box_spin() { 
    static int l = 0;
    int box_mask = 0xF00F; 
    display_frame((l == 0 ? box_mask : 0), (l == 1 ? box_mask : 0), (l == 2 ? box_mask : 0), (l == 3 ? box_mask : 0), 150 * speedFactor);
    l = (l + 1) % 4;
    delay(100 * speedFactor);
}

// --- Mặt phẳng dâng lên ---
void effect_14_level_rise() { 
    static int l = 0;
    int data_l0 = (l >= 0) ? 0xFFFF : 0;
    int data_l1 = (l >= 1) ? 0xFFFF : 0;
    int data_l2 = (l >= 2) ? 0xFFFF : 0;
    int data_l3 = (l >= 3) ? 0xFFFF : 0;
    display_frame(data_l0, data_l1, data_l2, data_l3, 120 * speedFactor);
    l = (l + 1) % 5;
    delay(100 * speedFactor);
}

// --- Lên rồi xuống ---
void effect_15_z_up_then_down() { 
    static int x = 0; static bool up = true;
    display_frame((x == 0 ? 0xFFFF : 0), (x == 1 ? 0xFFFF : 0), (x == 2 ? 0xFFFF : 0), (x == 3 ? 0xFFFF : 0), 100 * speedFactor);
    if (up) x++; else x--;
    if (x == 3) up = false; else if (x == 0) up = true;
    delay(80 * speedFactor);
}

// --- Quét theo trục X xen kẽ ---
void effect_16_x_sweep_alternate() { 
    static int y = 0;
    int mask = get_x_column_mask(y);
    display_frame(mask, 0, mask, 0, 100 * speedFactor);
    display_frame(0, mask, 0, mask, 100 * speedFactor);
    y = (y + 1) % 4;
    delay(80 * speedFactor);
}

// --- Quét theo trục Y xen kẽ ---
void effect_17_y_sweep_alternate() { 
    static int z = 0;
    int mask = get_y_row_mask(z);
    display_frame(mask, 0, 0, mask, 100 * speedFactor);
    display_frame(0, mask, mask, 0, 100 * speedFactor);
    z = (z + 1) % 4;
    delay(80 * speedFactor);
}

// --- Cột ngẫu nhiên ---
void effect_18_random_column() { 
    int col = random(0, 16);
    int mask = (1 << col);
    display_frame(mask, mask, mask, mask, 100 * speedFactor);
    delay(100 * speedFactor);
    display_frame(0, 0, 0, 0, 100 * speedFactor);
    delay(100 * speedFactor);
}

// --- Sóng ---
void effect_19_wave_pattern() { 
    int full = 0xFFFF;
    display_frame(full, 0, full, 0, 150 * speedFactor);
    display_frame(0, full, 0, full, 150 * speedFactor);
    delay(100 * speedFactor);
}

// --- Toàn khối sáng rồi tắt ---
void effect_20_full_on_off() { 
    int full = 0xFFFF;
    display_frame(full, full, full, full, 200 * speedFactor);
    delay(200 * speedFactor);
    display_frame(0, 0, 0, 0, 200 * speedFactor);
    delay(200 * speedFactor);
}
