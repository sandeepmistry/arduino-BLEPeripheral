/*

Copyright (c) 2012-2014 RedBearLab

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

/*
 *    Chat
 *
 *    Simple chat sketch, work with the BLEController iOS/Android App.
 *    Type something from the Arduino serial monitor to send
 *    to the BLEController App or vice verse.
 *    Characteristics received from App will print on Serial Monitor.
 */

// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <BLEPeripheral.h>

// define pins (varies per shield/board)
#define BLE_REQ   6
#define BLE_RDY   7
#define BLE_RST   4

/*----- BLE Utility -------------------------------------------------------------------------*/
// create peripheral instance, see pinouts above
BLEPeripheral            blePeripheral        = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);

// create service
BLEService               uartService          = BLEService("713d0000503e4c75ba943148f18d941e");

// create characteristic
BLECharacteristic    txCharacteristic = BLECharacteristic("713d0002503e4c75ba943148f18d941e", BLENotify, 20);
BLECharacteristic    rxCharacteristic = BLECharacteristic("713d0003503e4c75ba943148f18d941e", BLEWriteWithoutResponse, 20);
/*--------------------------------------------------------------------------------------------*/

void setup()
{
  Serial.begin(115200);
#if defined (__AVR_ATmega32U4__)
  delay(3000);  //5 seconds delay for enabling to see the start up comments on the serial board
#endif

/*----- BLE Utility ---------------------------------------------*/
  // set advertised local name and service UUID
  blePeripheral.setLocalName("ble-Micro");
  blePeripheral.setAdvertisedServiceUuid(uartService.uuid());

  // add service and characteristic
  blePeripheral.addAttribute(uartService);
  blePeripheral.addAttribute(rxCharacteristic);
  blePeripheral.addAttribute(txCharacteristic);

  // begin initialization
  blePeripheral.begin();
/*---------------------------------------------------------------*/

  Serial.println(F("BLE UART Peripheral"));
}

unsigned char buf[16] = {0};
unsigned char len = 0;

void loop()
{
  BLECentral central = blePeripheral.central();

  if (central)
  {
    // central connected to peripheral
    Serial.print(F("Connected to central: "));
    Serial.println(central.address());

    while (central.connected())
    {
      // central still connected to peripheral
      if (rxCharacteristic.written())
      {
        unsigned char len = rxCharacteristic.valueLength();
        const unsigned char *val = rxCharacteristic.value();
        Serial.print("didCharacteristicWritten, Length: ");
        Serial.println(len, DEC);
        unsigned char i = 0;
        while(i<len)
        {
          Serial.write(val[i++]);
        }
      }

      if ( Serial.available() )
      {
        delay(5);
        unsigned char len = 0;
        len = Serial.available();
        char val[len];
        Serial.readBytes(val, len);
        txCharacteristic.setValue((const unsigned char *)val, len);
      }
    }

    // central disconnected
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}

