#define BTPIN 19
#define LEDPIN 18
void setup() {
  Serial.begin(9600);
  pinMode(BTPIN,INPUT_PULLDOWN);
  pinMode(LEDPIN,OUTPUT);
}

void loop() {
  Serial.write(digitalRead(BTPIN));
  if (Serial.available()) (digitalWrite(LEDPIN,Serial.read()));
}
