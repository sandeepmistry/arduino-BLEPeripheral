#include <SPI.h>
#include <BLEPeripheral.h>

#define BLE_REQ 10
#define BLE_RDY 2
#define BLE_RST 9

BLEPeripheral           blePeripheral        = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);

BLEService              ledService           = BLEService("19b10010e8f2537e4f6cd104768a1214");
BLECharacteristicT<char> switchCharacteristic = BLECharacteristicT<char>("19b10011e8f2537e4f6cd104768a1214", BLEPropertyRead | BLEPropertyWrite);
BLECharacteristicT<char> buttonCharacteristic = BLECharacteristicT<char>("19b10012e8f2537e4f6cd104768a1214", BLEPropertyRead | BLEPropertyNotify);

void setup() {                
  Serial.begin(115200);

  pinMode(3, OUTPUT);
  pinMode(4, INPUT);

  blePeripheral.setLocalName("LED Switch");
  blePeripheral.setAdvertisedServiceUuid("19b10010e8f2537e4f6cd104768a1214");

  blePeripheral.addAttribute(ledService);
  blePeripheral.addAttribute(switchCharacteristic);
  blePeripheral.addAttribute(buttonCharacteristic);

  switchCharacteristic.setValue(0);
  buttonCharacteristic.setValue(0);

  blePeripheral.begin();

  Serial.println(F("BLE LED Switch Peripheral"));
}

void loop() {
  blePeripheral.poll();

  char buttonValue = digitalRead(4);

  bool buttonChanged = (buttonCharacteristic.value() != buttonValue);

  if (buttonChanged) {
    switchCharacteristic.setValue(buttonValue);
    buttonCharacteristic.setValue(buttonValue);
  }

  if (switchCharacteristic.hasNewValue() || buttonChanged) {
    if (switchCharacteristic.value()) {
      Serial.println(F("LED on"));
      digitalWrite(3, HIGH);
    } else {
      Serial.println(F("LED off"));
      digitalWrite(3, LOW);
    }
  }
}
