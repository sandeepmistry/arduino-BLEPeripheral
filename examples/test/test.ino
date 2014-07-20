
//#define SHOW_FREE_MEMORY

#ifdef SHOW_FREE_MEMORY
#include <MemoryFree.h>
#endif

#include <SPI.h>
#include <BLEPeripheral.h>

//#define REDBEARLAB_SHIELD

#if defined(BLEND_MICRO)
  #define BLE_REQ   6
  #define BLE_RDY   7
  #define BLE_RST   UNUSED
#elif defined(BLEND) || defined(REDBEARLAB_SHIELD)
  #define BLE_REQ   9
  #define BLE_RDY   8
  #define BLE_RST   UNUSED
#else // Adafruit
  #define BLE_REQ   10
  #define BLE_RDY   2
  #define BLE_RST   9
#endif

BLEPeripheral                      blePeripheral       = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);

BLEService                         testService         = BLEService("fff0");
BLECharacteristicT<unsigned short> testCharacteristic  = BLECharacteristicT<unsigned short>("fff1", BLEPropertyRead | BLEPropertyWrite | BLEPropertyWriteWithoutResponse | BLEPropertyNotify /*| BLEPropertyIndicate*/);
BLEDescriptor                      testDescriptor      = BLEDescriptor("2901", "counter");

unsigned long long                 lastSent            = 0;

void setup() {                
  Serial.begin(115200);
#if defined (__AVR_ATmega32U4__)
  //Wait until the serial port is available (useful only for the Leonardo)
  //As the Leonardo board is not reseted every time you open the Serial Monitor
  while(!Serial) {}
  delay(5000);  //5 seconds delay for enabling to see the start up comments on the serial board
#endif

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
