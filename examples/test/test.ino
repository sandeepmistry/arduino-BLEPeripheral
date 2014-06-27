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
BLECharacteristic test1Characteristic1 = BLECharacteristic("fff1", BLE_PROPERTY_READ | BLE_PROPERTY_WRITE, 2);
BLECharacteristic test1Characteristic2 = BLECharacteristic("fff2", BLE_PROPERTY_READ | BLE_PROPERTY_WRITE, 2);
BLEDescriptor test1Descriptor = BLEDescriptor("2901", 4);

BLEService test2Service = BLEService("FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFF0");
BLECharacteristic test2Characteristic = BLECharacteristic("FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFF1", BLE_PROPERTY_READ | BLE_PROPERTY_WRITE, 5);

void setup() {                
  Serial.begin(115200);
  
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
  blePeripheral.addAttribute(test1Characteristic1);
  blePeripheral.addAttribute(test1Descriptor);
  blePeripheral.addAttribute(test1Characteristic2);

  blePeripheral.addAttribute(test2Service);
  blePeripheral.addAttribute(test2Characteristic);

  test1Characteristic1.setValue("yo", 2);
  test1Characteristic2.setValue("hi", 2);
  test1Descriptor.setValue("desc", 4);
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

  if (blePeripheral.isConnected()) {
    if (test1Characteristic1.valueUpdated()) {
       Serial.print(F("value updated = "));
       Serial.print(test1Characteristic1.value()[0]);
       Serial.print(test1Characteristic1.value()[1]);
       Serial.println();

      test1Characteristic1.setValue("oy", 2);
    }
  }
}
