#include <SPI.h>
#include <BLEPeripheral.h>

#define BLE_REQ 10
#define BLE_RDY 2
#define BLE_RST 9

BLEPeripheral     blePeripheral        = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);

BLEService        ledService           = BLEService("19b10010e8f2537e4f6cd104768a1214");
BLECharacteristic switchCharacteristic = BLECharacteristic("19b10011e8f2537e4f6cd104768a1214", BLEPropertyRead | BLEPropertyWrite, 1);
BLECharacteristic buttonCharacteristic = BLECharacteristic("19b10012e8f2537e4f6cd104768a1214", BLEPropertyRead | BLEPropertyNotify, 1);

unsigned char switchValue[1];
unsigned char buttonValue[1];

void setup() {                
  Serial.begin(115200);

  pinMode(3, OUTPUT);
  pinMode(4, INPUT);

  blePeripheral.setLocalName("LED Switch");
  blePeripheral.setAdvertisedServiceUuid("19b10010e8f2537e4f6cd104768a1214");

  blePeripheral.addAttribute(ledService);
  blePeripheral.addAttribute(switchCharacteristic);
  blePeripheral.addAttribute(buttonCharacteristic);

  switchValue[0] = 0;
  buttonValue[0] = 0;

  switchCharacteristic.setValue(switchValue, 1);
  buttonCharacteristic.setValue(buttonValue, 1);

  blePeripheral.begin();

  Serial.println(F("BLE LED Switch Peripheral"));
}

void loop() {
  blePeripheral.poll();

  buttonValue[0] = digitalRead(4);

  bool buttonChanged = (buttonCharacteristic.value()[0] != buttonValue[0]);

  if (buttonChanged) {
    switchValue[0] = buttonValue[0];

    switchCharacteristic.setValue(switchValue, 1);
    buttonCharacteristic.setValue(buttonValue, 1);
  }

  if (switchCharacteristic.hasNewValue() || buttonChanged) {
    if (switchCharacteristic.value()[0]) {
      Serial.println(F("LED on"));
      digitalWrite(3, HIGH);
    } else {
      Serial.println(F("LED off"));
      digitalWrite(3, LOW);
    }
  }
}
