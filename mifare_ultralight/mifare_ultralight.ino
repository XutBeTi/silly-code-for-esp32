#include <SPI.h>
#include <MFRC522.h>

// Chân kết nối RC522
#define SS_PIN 5
#define RST_PIN 21
MFRC522 mfrc522(SS_PIN, RST_PIN);

// URL muốn ghi
String URL = "https://www.youtube.com";

// =================== XÓA THẺ =====================
bool clearTag() {
  byte emptyPage[16] = {0}; // 16 byte = 4 page liên tiếp
  for (byte page = 4; page < 40; page += 4) {
    if (mfrc522.MIFARE_Write(page, emptyPage, 16) != MFRC522::STATUS_OK) {
      Serial.println("Error clearing page " + String(page));
      return false;
    }
  }
  Serial.println("Tag cleared successfully");
  return true;
}

// =================== GHI NDEF URL =====================
bool writeNDEF(String url) {
  int urlLen = url.length() + 1; // 1 byte prefix
  int ndefLen = 4 + urlLen;      // NDEF record header (4 byte) + payload
  int totalLen = 2 + ndefLen + 1; // TLV header + NDEF + terminator

  byte payload[totalLen];
  int idx = 0;

  // 1. TLV Header
  payload[idx++] = 0x03;        // Type = NDEF
  payload[idx++] = ndefLen;     // Length of NDEF record

  // 2. NDEF Record header
  payload[idx++] = 0xD1;        // MB=1, ME=1, SR=1, TNF=1
  payload[idx++] = 0x01;        // Type length
  payload[idx++] = urlLen;      // Payload length
  payload[idx++] = 0x55;        // Type field 'U' = URL

  // 3. Payload
  payload[idx++] = 0x00;        // Prefix indicator (0x00 = No prefix)
  for (int i = 0; i < url.length(); i++)
    payload[idx++] = url[i];

  // 4. TLV Terminator
  payload[idx++] = 0xFE;

  // Ghi 4 byte/lần
  int page = 4;
  for (int i = 0; i < totalLen; i += 4) {
    byte tmp[16] = {0}; // buffer 16 byte
    for (int k = 0; k < 4 && i + k < totalLen; k++)
      tmp[k] = payload[i + k];

    if (mfrc522.MIFARE_Write(page, tmp, 16) != MFRC522::STATUS_OK) {
      Serial.println("Write error at page " + String(page));
      return false;
    }
    page++;
  }

  Serial.println("NDEF URL WRITE OK!");
  return true;
}

// =================== ĐỌC URL =====================
String readNDEF() {
  byte buf[18];
  byte len;
  String url = "";
  int left = 0;
  int headerLen = 0;

  for (int page = 4; page <= 40; page++) {
    len = sizeof(buf);
    if (mfrc522.MIFARE_Read(page, buf, &len) != MFRC522::STATUS_OK)
      continue;

    for (int i = 0; i < 4; i++) {
      if (left == 0 && headerLen == 0) {
        if (buf[i] == 0x03) { headerLen = 1; continue; }
      }
      if (headerLen > 0 && headerLen < 7) {
        if (headerLen == 1) left = buf[i] - 5; // TLV Length - header
        headerLen++;
        continue;
      }
      if (left > 0) {
        if (buf[i] == 0xFE) return url;
        url += (char)buf[i];
        left--;
      }
    }
  }
  return url;
}

// =================== MAIN =====================
void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Ready...");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  Serial.println("\n--- NFC TAG DETECTED ---");

  // Xóa dữ liệu cũ
  if (!clearTag()) {
    Serial.println("Cannot clear tag, aborting.");
    return;
  }

  // Ghi URL
  if (!writeNDEF(URL)) {
    Serial.println("Cannot write URL, aborting.");
    return;
  }

  // Đọc lại URL
  String readURL = readNDEF();
  Serial.println(">>>> URL Read:");
  Serial.println(readURL);

  // Dừng giao tiếp
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  delay(2000);
}
