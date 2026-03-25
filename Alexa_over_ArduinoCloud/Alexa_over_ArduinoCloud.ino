#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

#define OUTPUT_PIN 0

const char DEVICE_LOGIN_NAME[]  = "8fd033ab-46e5-4f88-954e-0cb46c721438";
const char SSID[]               = "TC HOUSE"; 
const char PASS[]               = "thuycuong";
const char DEVICE_KEY[]  = "?gojcX1qVfh#VApDAxj3YCJMA";    

void onSwitchChange();

CloudSwitch Switch;

void initProperties(){

  ArduinoCloud.setBoardId(DEVICE_LOGIN_NAME);
  ArduinoCloud.setSecretDeviceKey(DEVICE_KEY);
  ArduinoCloud.addProperty(Switch, READWRITE, ON_CHANGE, onSwitchChange);

}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);
void setup() {

  Serial.begin(115200);

  delay(1500); 

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  
  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
 */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  pinMode(OUTPUT_PIN, OUTPUT);
}

void loop() {
  ArduinoCloud.update();
}

void onSwitchChange()  {
  if (Switch) {
    digitalWrite(OUTPUT_PIN, HIGH);
  } else {
    digitalWrite(OUTPUT_PIN, LOW);
  }
}


