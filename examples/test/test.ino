
//#define SHOW_FREE_MEMORY

#ifdef SHOW_FREE_MEMORY
#include <MemoryFree.h>
#endif

#include <SPI.h>
#include <BLEPeripheral.h>

#define BLE_REQ   10
#define BLE_RDY   2
#define BLE_RST   9

BLEPeripheral                    blePeripheral       = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);

BLEService                       testService         = BLEService("fff0");
BLEUnsignedShortCharacteristic   testCharacteristic  = BLEUnsignedShortCharacteristic("fff1", BLERead | BLEWrite | BLEWriteWithoutResponse | BLENotify /*| BLEIndicate*/);
BLEDescriptor                    testDescriptor      = BLEDescriptor("2901", "counter");

unsigned long long               lastSent            = 0;

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

  blePeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  blePeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

  testCharacteristic.setEventHandler(BLEWritten, characteristicWritten);
  testCharacteristic.setEventHandler(BLESubscribed, characteristicSubscribed);
  testCharacteristic.setEventHandler(BLEUnsubscribed, characteristicUnsubscribed);

  testCharacteristic.setValue(0);

  blePeripheral.begin();

  Serial.println(F("BLE Peripheral"));

#ifdef SHOW_FREE_MEMORY
  Serial.print(F("Free memory = "));
  Serial.println(freeMemory());
#endif
}

void loop() {
  BLECentral central = blePeripheral.central();

  if (central) {
    Serial.print(F("Connected to central: "));
    Serial.println(central.address());

    testCharacteristic.setValue(0);

    while (central.connected()) {
      if (testCharacteristic.written()) {
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

    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}

void blePeripheralConnectHandler(BLECentral& central) {
  Serial.print(F("Connected event, central: "));
  Serial.println(central.address());
}

void blePeripheralDisconnectHandler(BLECentral& central) {
  Serial.print(F("Disconnected event, central: "));
  Serial.println(central.address());
}

void characteristicWritten(BLECentral& central, BLECharacteristic& characteristic) {
  Serial.print(F("Characteristic event, writen: "));
  Serial.println(testCharacteristic.value(), DEC);
}

void characteristicSubscribed(BLECentral& central, BLECharacteristic& characteristic) {
  Serial.println(F("Characteristic event, subscribed"));
}

void characteristicUnsubscribed(BLECentral& central, BLECharacteristic& characteristic) {
  Serial.println(F("Characteristic event, unsubscribed"));
}
