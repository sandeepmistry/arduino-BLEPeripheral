// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <BLEPeripheral.h>
#include <BLEHID.h>

// define pins (varies per shield/board)
#define BLE_REQ   6
#define BLE_RDY   7
#define BLE_RST   4

BLEHID bleHID = BLEHID(BLE_REQ, BLE_RDY, BLE_RST);

void setup() {
  Serial.begin(9600);
#if defined (__AVR_ATmega32U4__)
  while(!Serial);
#endif

  bleHID.begin();

  Serial.println(F("BLE HID"));
}

void loop() {
  BLECentral central = bleHID.central();

  if (central) {
    // central connected to peripheral
    Serial.print(F("Connected to central: "));
    Serial.println(central.address());

    while (bleHID.connected()) {
      if (Serial.available() > 0) {
        Serial.read();
        
        bleHID.write(0x1E); // 'a'
      }
    }

    // central disconnected
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}

