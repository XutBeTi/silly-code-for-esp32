#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 5
#define RST_PIN 21
MFRC522 mfrc522(SS_PIN, RST_PIN);

MFRC522::MIFARE_Key key;

// Các Key phổ biến cần thử cho Sector 0
const byte defaultKeys[3][6] = {
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5}
};

// ===========================================
// TÍNH BCC
byte calcBCC(byte *uid) {
    return uid[0] ^ uid[1] ^ uid[2] ^ uid[3];
}

// ===========================================
// THỬ CÁC KEY A CHO BLOCK 0
bool authenticateBlock0() {
    MFRC522::StatusCode status;
    for (int k = 0; k < 3; k++) {
        memcpy(key.keyByte, defaultKeys[k], 6);

        status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                          0, &key, &(mfrc522.uid));

        if (status == MFRC522::STATUS_OK) return true;
    }
    return false;
}

// ===========================================
// KIỂM TRA MAGIC Gen1A (BACKDOOR 0x40 0x43)
bool isGen1A() {
    byte cmd1 = 0x40, cmd2 = 0x43;
    
    // Luôn dừng Crypto1 trước khi gửi lệnh Backdoor
    mfrc522.PCD_StopCrypto1(); 

    // Gửi lệnh Backdoor. Nếu thẻ Gen1A, nó sẽ trả lời cho cả hai
    if (mfrc522.PCD_TransceiveData(&cmd1, 1, nullptr, nullptr) != MFRC522::STATUS_OK) return false;
    if (mfrc522.PCD_TransceiveData(&cmd2, 1, nullptr, nullptr) != MFRC522::STATUS_OK) return false;

    return true; // Cả 2 trả lời → GEN1A
}

// ===========================================
// KIỂM TRA MAGIC Gen2 (Fake write) - ĐÃ SỬA LỖI AN TOÀN
bool isGen2() {
    byte block0[16];
    byte size = sizeof(block0);
    byte backup[16];
    MFRC522::StatusCode st;
    
    // 1. Xác thực và Đọc Block 0
    if (!authenticateBlock0()) return false;
    if (mfrc522.MIFARE_Read(0, block0, &size) != MFRC522::STATUS_OK) return false;
    
    memcpy(backup, block0, 16);

    // 2. Chuẩn bị dữ liệu thử nghiệm (UID[3] và BCC)
    block0[3] ^= 0x5A;
    block0[4] = calcBCC(block0);
    
    // 3. Cố gắng ghi đè
    st = mfrc522.MIFARE_Write(0, block0, 16);

    // 4. RESTORE BLOCK 0 (Dù thành công hay thất bại)
    
    // Ngừng Crypto1 và xác thực lại để đảm bảo ghi hoàn nguyên an toàn
    mfrc522.PCD_StopCrypto1();
    
    // Dùng lệnh authenticateBlock0() một lần nữa. Key đã được lưu trong `key`
    if (authenticateBlock0()) { 
        // Ghi lại block gốc
        mfrc522.MIFARE_Write(0, backup, 16);
    } 
    // Nếu Xác thực lại thất bại, ta không thể RESTORE, nhưng ít nhất đã thử.

    // 5. Kết luận
    if (st == MFRC522::STATUS_OK) {
        // Ghi thử thành công → Magic Card Gen2
        return true;
    }

    return false;
}

// ===========================================
// KIỂM TRA LOẠI THẺ
void checkCard() {
    Serial.println("\n=== CARD DETECTED ===");
    
    // Nếu là Gen1A, ưu tiên kết luận ngay
    if (isGen1A()) {
        Serial.println(">>> MAGIC CARD GEN1A (BACKDOOR 0x40 0x43, UID CHANGABLE)");
        return;
    }

    // Nếu không phải Gen1A, kiểm tra Gen2
    if (isGen2()) {
        Serial.println(">>> MAGIC CARD GEN2 (NO BACKDOOR, BUT UID REWRITABLE)");
        return;
    }

    Serial.println(">>> STANDARD MIFARE CLASSIC (UID READ-ONLY)");
}

// ===========================================
// SETUP
void setup() {
    Serial.begin(115200);
    SPI.begin();
    mfrc522.PCD_Init();
    Serial.println("Ready to scan card...");
}

// ===========================================
// LOOP
void loop() {
    if (!mfrc522.PICC_IsNewCardPresent()) return;
    if (!mfrc522.PICC_ReadCardSerial()) return;

    byte type = mfrc522.PICC_GetType(mfrc522.uid.sak);
    if (type == MFRC522::PICC_TYPE_MIFARE_1K || type == MFRC522::PICC_TYPE_MIFARE_4K)
        checkCard();
    else
        Serial.println("NOT A MIFARE CLASSIC");

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
}