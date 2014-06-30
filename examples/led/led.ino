#include <SPI.h>
#include <BLEPeripheral.h>

#define BLE_REQ 10
#define BLE_RDY 2
#define BLE_RST 9

BLEPeripheral            blePeripheral        = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);

BLEService               ledService           = BLEService("19b10000e8f2537e4f6cd104768a1214");
BLECharacteristicT<char> switchCharacteristic = BLECharacteristicT<char>("19b10001e8f2537e4f6cd104768a1214", BLEPropertyRead | BLEPropertyWrite);

void setup() {                
  Serial.begin(115200);

  pinMode(3, OUTPUT);

  blePeripheral.setLocalName("LED");
  blePeripheral.setAdvertisedServiceUuid("19b10000e8f2537e4f6cd104768a1214");

  blePeripheral.addAttribute(ledService);
  blePeripheral.addAttribute(switchCharacteristic);

  switchCharacteristic.setValue(0);

  blePeripheral.begin();

  Serial.println(F("BLE LED Peripheral"));
}

void loop() {
  blePeripheral.poll();

  if (switchCharacteristic.hasNewValue()) {
    if (switchCharacteristic.value()) {
      Serial.println(F("LED on"));
      digitalWrite(3, HIGH);
    } else {
      Serial.println(F("LED off"));
      digitalWrite(3, LOW);
    }
  }
}
