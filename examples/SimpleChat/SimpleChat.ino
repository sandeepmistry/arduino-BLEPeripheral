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
  //Wait until the serial port is available (useful only for the Leonardo)
  //As the Leonardo board is not reseted every time you open the Serial Monitor
  while(!Serial) {}
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

