#include <SPI.h>

#include <BLEPeripheral.h>

#define BLE_REQ 10
#define BLE_RDY 2
#define BLE_RST 9

BLEPeripheral blePeripheral = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);

void setup() {                
  Serial.begin(57600);
  
  blePeripheral.setLocalName("test");
  blePeripheral.setAdvertisedServiceUuid("B9407F30-F5F8-466E-AFF9-25556B57FE6D");
  
  blePeripheral.begin();
  
  Serial.println(F("BLE Peripheral"));
}

void loop() { 
  blePeripheral.poll();
}
