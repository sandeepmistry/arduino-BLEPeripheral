// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <BLEPeripheral.h>

// define pins (varies per shield/board)
#define BLE_REQ   10
#define BLE_RDY   2
#define BLE_RST   9

// create peripheral instance, see pinouts above
BLEPeripheral blePeripheral = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);

// create service
BLEService physicalWebService = BLEService("fed8");

// create characteristic
BLECCharacteristic physicalWebCharacterisitc = BLECharCharacteristic("fed9", BLERead | BLEBroadcast, 15);

void setup() {
  Serial.begin(9600);
#if defined (__AVR_ATmega32U4__)
  //Wait until the serial port is available (useful only for the Leonardo)
  //As the Leonardo board is not reseted every time you open the Serial Monitor
  while(!Serial) {}
  delay(5000);  //5 seconds delay for enabling to see the start up comments on the serial board
#endif

  blePeripheral.setLocalName("physical-web");
  blePeripheral.setAdvertisedServiceUuid(service.uuid());

  // add attributes to peripheral
  blePeripheral.addAttribute(physicalWebService);
  blePeripheral.addAttribute(characteristic);

  // set value
  unsigned char physicalWebData[15]; // only 15 bytes (instead of 18), because flags (3 bytes) are in advertisement data
  unsigned char i = 0;
  
  physicalWebData[i++] = 0x00; // flags
  physicalWebData[i++] = 0x20; // power
  physicalWebData[i++] = 0x00; // http://www.
  
  physicalWebData[i++] = 'A';
  physicalWebData[i++] = 'B';
  physicalWebData[i++] = 'C';
  
  physicalWebData[i++] = 0x07; // .com
  
  physicalWebCharacterisitc.setValue(physicalWebData, i);

  // begin initialization
  blePeripheral.begin();
  
  // broadcast the characteristic
  physicalWebCharacterisitc.broadcast();
}

void loop() {
  BLECentral central = blePeripheral.central();

  if (central) {
    // central connected to peripheral
    Serial.print(F("Connected to central: "));
    Serial.println(central.address());

    while (central.connected()) {
      central.disconnect(); // disconnect central ASAP
    }

    // central disconnected
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}
