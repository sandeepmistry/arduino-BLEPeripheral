#include <SPI.h>

//#define SHOW_FREE_MEMORY

#ifdef SHOW_FREE_MEMORY
#include <MemoryFree.h>
#endif

#include <BLEPeripheral.h>

#define BLE_REQ 10
#define BLE_RDY 2
#define BLE_RST 9

BLEPeripheral blePeripheral = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);

BLEService test1Service = BLEService("fff0");
BLECharacteristic test1Characteristic = BLECharacteristic("fff1", BLE_CHARACTERISTIC_PROPERTY_READ, 2);
BLEService test2Service = BLEService("FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFF0");
BLECharacteristic test2Characteristic = BLECharacteristic("FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFF1", BLE_CHARACTERISTIC_PROPERTY_READ, 5);

void setup() {                
  Serial.begin(57600);
  
  blePeripheral.setLocalName("test");
#if 1
  blePeripheral.setAdvertisedServiceUuid("FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFF0");
#else
  const char manufacturerData[4] = {0x12, 0x34, 0x56, 0x78};
  blePeripheral.setManufacturerData(manufacturerData, sizeof(manufacturerData));
#endif

  blePeripheral.setDeviceName("Test");
  blePeripheral.setAppearance(0x0080);

  blePeripheral.addAttribute(test1Service);
  blePeripheral.addAttribute(test1Characteristic);
  blePeripheral.addAttribute(test2Service);
  blePeripheral.addAttribute(test2Characteristic);

  test1Characteristic.setValue("hi", 2);
  test2Characteristic.setValue("there", 5);

  blePeripheral.begin();
  
  Serial.println(F("BLE Peripheral"));

#ifdef SHOW_FREE_MEMORY
  Serial.print(F("Free memory = "));
  Serial.println(freeMemory());
#endif
}

void loop() { 
  blePeripheral.poll();
}
