// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <BLEPeripheral.h>
#include <BLEHID.h>
#include <BLEMouse.h>
#include <BLEKeyboard.h>
#include <BLEMultimedia.h>
#include <BLESystemControl.h>

// define pins (varies per shield/board)
#define BLE_REQ   6
#define BLE_RDY   7
#define BLE_RST   4

BLEHID bleHID = BLEHID(BLE_REQ, BLE_RDY, BLE_RST);
BLEMouse bleMouse;
BLEKeyboard bleKeyboard;
BLEMultimedia bleMultimedia;
BLESystemControl bleSystemControl;

void setup() {
  Serial.begin(9600);
#if defined (__AVR_ATmega32U4__)
  while(!Serial);
#endif

  // clears bond data on every boot
  bleHID.clearBondStoreData();
  
   bleHID.setDeviceName("Arduino BLE HID");
//  bleHID.setAppearance(961);

  bleHID.setLocalName("HID");

  bleHID.addDevice(bleMouse);
  bleHID.addDevice(bleKeyboard);
  bleHID.addDevice(bleMultimedia);
  bleHID.addDevice(bleSystemControl);

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
        
        bleMouse.move(100, 100, 0);
        bleKeyboard.write(KEYCODE_A);
        bleMultimedia.write(MMKEY_VOL_UP);
        bleSystemControl.write(SYSCTRLKEY_POWER);
      }
    }

    // central disconnected
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}

