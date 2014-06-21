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
