
#define SHOW_FREE_MEMORY

#ifdef SHOW_FREE_MEMORY
#include <MemoryFree.h>
#endif

#include <SPI.h>
#include <BLEPeripheral.h>

#define BLE_REQ 10
#define BLE_RDY 2
#define BLE_RST 9

BLEPeripheral     blePeripheral                         = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);

BLEService                         test1Service         = BLEService("fff0");
BLECharacteristicT<unsigned short> test1Characteristic  = BLECharacteristicT<unsigned short>("fff1", BLEPropertyRead | BLEPropertyWrite | BLEPropertyWriteWithoutResponse | BLEPropertyNotify | BLEPropertyIndicate);
BLEDescriptor                      test1Descriptor      = BLEDescriptor("2901", "counter");

void setup() {                
  Serial.begin(115200);
  
  blePeripheral.setLocalName("test");
#if 1
  blePeripheral.setAdvertisedServiceUuid("fff0");
#else
  const char manufacturerData[4] = {0x12, 0x34, 0x56, 0x78};
  blePeripheral.setManufacturerData(manufacturerData, sizeof(manufacturerData));
#endif

  blePeripheral.setDeviceName("Test");
  blePeripheral.setAppearance(0x0080);

  blePeripheral.addAttribute(test1Service);
  blePeripheral.addAttribute(test1Characteristic);
  blePeripheral.addAttribute(test1Descriptor);

  test1Characteristic.setValue(0);

  blePeripheral.begin();
  
  Serial.println(F("BLE Peripheral"));

#ifdef SHOW_FREE_MEMORY
  Serial.print(F("Free memory = "));
  Serial.println(freeMemory());
#endif
}

void loop() {
  blePeripheral.poll();

  static unsigned long long lastSent = 0;

  if (test1Characteristic.hasNewValue()) {
    Serial.println(F("counter written, reset"));

    lastSent = 0;
    test1Characteristic.setValue(0);
  }

  if (millis() > 1000 && (millis() - 1000) > lastSent) {
    lastSent = millis();

    test1Characteristic.setValue(test1Characteristic.value() + 1);

    Serial.print(F("counter = "));
    Serial.println(test1Characteristic.value(), DEC);
  }
}
