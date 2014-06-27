#include <SPI.h>
#include <BLEPeripheral.h>

#define BLE_REQ 10
#define BLE_RDY 2
#define BLE_RST 9

BLEPeripheral blePeripheral = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);

BLEService ledService = BLEService("19b10001e8f2537e4f6cd104768a1214");
BLECharacteristic switchCharacteristic = BLECharacteristic("19b10001e8f2537e4f6cd104768a1214", BLE_PROPERTY_READ | BLE_PROPERTY_WRITE, 1);

void setup() {                
  Serial.begin(115200);
  
  pinMode(3, OUTPUT);
  
  blePeripheral.setLocalName("LED");
  blePeripheral.setAdvertisedServiceUuid("19b10001e8f2537e4f6cd104768a1214");

  blePeripheral.addAttribute(ledService);
  blePeripheral.addAttribute(switchCharacteristic);

  char initialSwitchValue[1] = {0x00};
  
  switchCharacteristic.setValue(initialSwitchValue, 1);

  blePeripheral.begin();
  
  Serial.println(F("BLE LED Peripheral"));
}

void loop() {
  blePeripheral.poll();

  if (blePeripheral.isConnected()) {
    if (switchCharacteristic.valueUpdated()) {
      if (switchCharacteristic.value()[0]) {
        Serial.println(F("LED on"));
        digitalWrite(3, HIGH);
      } else {
        Serial.println(F("LED off"));
        digitalWrite(3, LOW);
      }
    }
  }
}
