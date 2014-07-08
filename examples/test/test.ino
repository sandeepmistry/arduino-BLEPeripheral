
//#define SHOW_FREE_MEMORY

#ifdef SHOW_FREE_MEMORY
#include <MemoryFree.h>
#endif

#include <SPI.h>
#include <BLEPeripheral.h>

#define BLE_REQ 10
#define BLE_RDY 2
#define BLE_RST 9

BLEPeripheral                      blePeripheral       = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);

BLEService                         testService         = BLEService("fff0");
BLECharacteristicT<unsigned short> testCharacteristic  = BLECharacteristicT<unsigned short>("fff1", BLEPropertyRead | BLEPropertyWrite | BLEPropertyWriteWithoutResponse | BLEPropertyNotify /*| BLEPropertyIndicate*/);
BLEDescriptor                      testDescriptor      = BLEDescriptor("2901", "counter");

unsigned long long                 lastSent            = 0;

void setup() {                
  Serial.begin(115200);

  blePeripheral.setLocalName("test");
#if 1
  blePeripheral.setAdvertisedServiceUuid(testService.uuid());
#else
  const char manufacturerData[4] = {0x12, 0x34, 0x56, 0x78};
  blePeripheral.setManufacturerData(manufacturerData, sizeof(manufacturerData));
#endif

  blePeripheral.setDeviceName("Test");
  blePeripheral.setAppearance(0x0080);

  blePeripheral.addAttribute(testService);
  blePeripheral.addAttribute(testCharacteristic);
  blePeripheral.addAttribute(testDescriptor);

  blePeripheral.setConnectHandler(blePeripheralConnectHandler);
  blePeripheral.setDisconnectHandler(blePeripheralDisconnectHandler);
  testCharacteristic.setNewValueHandler(characteristicHasNewValue);

  testCharacteristic.setValue(0);

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
    if (testCharacteristic.hasNewValue()) {
      Serial.println(F("counter written, reset"));

      lastSent = 0;
      testCharacteristic.setValue(0);
    }

    if (millis() > 1000 && (millis() - 1000) > lastSent) {
      lastSent = millis();

      testCharacteristic.setValue(testCharacteristic.value() + 1);

      Serial.print(F("counter = "));
      Serial.println(testCharacteristic.value(), DEC);
    }
  }
}

void blePeripheralConnectHandler(const char* address) {
  Serial.print(F("Connected to central "));
  Serial.println(address);

  testCharacteristic.setValue(0);
}

void blePeripheralDisconnectHandler() {
  Serial.println(F("Disconnected from central "));
}

void characteristicHasNewValue() {
  Serial.print(F("Characteristic has new value"));
  Serial.println(testCharacteristic.value(), DEC);
}
