#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN  21   // CHÂN ĐÚNG TRÊN ESP32
#define SS_PIN    5

MFRC522 mfrc522(SS_PIN, RST_PIN);

// ======================== Loại thẻ ===========================
String getCardType(MFRC522::PICC_Type t) {
  switch (t) {
    case MFRC522::PICC_TYPE_MIFARE_MINI: return "MIFARE Mini";
    case MFRC522::PICC_TYPE_MIFARE_1K:   return "MIFARE Classic 1K";
    case MFRC522::PICC_TYPE_MIFARE_4K:   return "MIFARE Classic 4K";
    case MFRC522::PICC_TYPE_MIFARE_UL:   return "Ultralight / NTAG";
    default: return "Unsupported";
  }
}

// ================== Đọc MIFARE Classic (ỔN ĐỊNH) ===================
void readClassic() {
  MFRC522::MIFARE_Key key;
  for (int i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  byte buffer[18];
  byte size = sizeof(buffer);

  Serial.println("Reading Classic...");

  for (byte block = 0; block < 64; block++) {

    // Authenticate chỉ dùng cho Classic!
    MFRC522::StatusCode status = mfrc522.PCD_Authenticate(
      MFRC522::PICC_CMD_MF_AUTH_KEY_A,
      block,
      &key,
      &(mfrc522.uid)
    );

    if (status != MFRC522::STATUS_OK) {
      Serial.printf("Block %d auth fail\n", block);
      continue;
    }

    status = mfrc522.MIFARE_Read(block, buffer, &size);
    if (status == MFRC522::STATUS_OK) {
      Serial.printf("Block %d: ", block);
      for (byte i = 0; i < 16; i++) {
        Serial.printf("%02X ", buffer[i]);
      }
      Serial.println();
    }

    delay(8); // giúp tránh WDT reset
  }

  mfrc522.PCD_StopCrypto1();
}

// ================== Đọc Ultralight / NTAG ===================
void readUltralight() {
  byte data[4];
  byte len = 4;

  Serial.println("Reading Ultralight / NTAG...");

  for (byte page = 0; page < 48; page++) {
    MFRC522::StatusCode status = mfrc522.MIFARE_Read(page, data, &len);
    if (status == MFRC522::STATUS_OK) {
      Serial.printf("Page %d: %02X %02X %02X %02X\n",
                    page, data[0], data[1], data[2], data[3]);
    }
    delay(4); // tránh reset
  }
}

// ============================ MAIN =============================
void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
  delay(50);

  Serial.println("NFC scanner ready!");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  Serial.println("\n--- NFC TAG DETECTED ---");

  // UID
  Serial.print("UID: ");
  for (int i = 0; i < mfrc522.uid.size; i++) {
    Serial.printf("%02X ", mfrc522.uid.uidByte[i]);
  }
  Serial.println();

  // Type
  MFRC522::PICC_Type type = mfrc522.PICC_GetType(mfrc522.uid.sak);
  String typeName = getCardType(type);
  Serial.print("Type: ");
  Serial.println(typeName);

  // Chọn đọc đúng loại để không crash
  if (type == MFRC522::PICC_TYPE_MIFARE_1K || type == MFRC522::PICC_TYPE_MIFARE_4K)
    readClassic();
  else if (type == MFRC522::PICC_TYPE_MIFARE_UL)
    readUltralight();
  else
    Serial.println("⚠ Loại thẻ này MFRC522 đọc không đủ hoặc không hỗ trợ");

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  delay(300);
}
