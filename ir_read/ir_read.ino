#include <IRrecv.h>
#include <IRutils.h>

#define IR_PIN 34  // chân OUT của cảm biến IR nối vào GPIO15

IRrecv irrecv(IR_PIN);
decode_results results;

void setup() {
  Serial.begin(115200);
  irrecv.enableIRIn();  // khởi động nhận tín hiệu
  Serial.println("Đang chờ tín hiệu IR...");
}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.print("Nhận được mã: ");
    Serial.println(results.value, HEX);  // In mã dạng HEX

    // In thêm thông tin chi tiết (tuỳ chọn)
    // serialPrintUint64(results.value, HEX);

    irrecv.resume();  // Chuẩn bị cho lần nhận tiếp theo
  }
}
