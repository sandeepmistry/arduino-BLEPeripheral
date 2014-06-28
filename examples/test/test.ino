#include <SPI.h>

//#define SHOW_FREE_MEMORY

#ifdef SHOW_FREE_MEMORY
#include <MemoryFree.h>
#endif

#include <BLEPeripheral.h>

#define BLE_REQ 10
#define BLE_RDY 2
#define BLE_RST 9

BLEPeripheral     blePeripheral        = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);

BLEService        test1Service         = BLEService("fff0");
BLECharacteristic test1Characteristic1 = BLECharacteristic("fff1", BLE_PROPERTY_READ | BLE_PROPERTY_WRITE | BLE_PROPERTY_WRITE_WITHOUT_RESPONSE | BLE_PROPERTY_NOTIFY | BLE_PROPERTY_INDICATE, 2);
BLEDescriptor     test1Descriptor      = BLEDescriptor("2901", 7);

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
  blePeripheral.addAttribute(test1Characteristic1);
  blePeripheral.addAttribute(test1Descriptor);

  test1Characteristic1.setValue("yo", 2);
  test1Descriptor.setValue("counter", 7);

  blePeripheral.begin();
  
  Serial.println(F("BLE Peripheral"));

#ifdef SHOW_FREE_MEMORY
  Serial.print(F("Free memory = "));
  Serial.println(freeMemory());
#endif
}

void loop() {
  blePeripheral.poll();

  static unsigned short s = 0;
  static unsigned long last_sent = 0;

  if (blePeripheral.isConnected()) {
    if (test1Characteristic1.valueUpdated()) {
      Serial.println(F("counter written, reset"));

      last_sent = 0;
      s = 0;
    }

    if ((millis() - 1000) > last_sent) {
      last_sent = millis();

      s++;

      Serial.print(F("counter = "));
      Serial.println(s, DEC);

      test1Characteristic1.setValue((char *)&s, 2);
    }
  }
}
