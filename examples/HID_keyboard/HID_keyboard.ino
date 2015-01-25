// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <BLEPeripheral.h>

// define pins (varies per shield/board)
#define BLE_REQ   6
#define BLE_RDY   7
#define BLE_RST   4

// create peripheral instance, see pinouts above
BLEPeripheral blePeripheral = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);
BLEBondStore bleBondStore;

const unsigned char hidReportReferenceDescriptorValue1[] = { 0x00, 0x01 };
const unsigned char hidInformationCharacteriticValue[]   = { 0x11, 0x01, 0x00, 0x03 };

// https://github.com/NordicSemiconductor/ble-sdk-arduino/blob/master/libraries/BLE/examples/ble_HID_keyboard_template/USD%20HID%20Report%20Descriptor%20-%20Keyboard.txt
const unsigned char hidReportDescriptorValue[] = {
  0x05, 0x01,                         // Usage Page (Generic Desktop)
  0x09, 0x06,                         // Usage (Keyboard)
  0xA1, 0x01,                         // Collection (Application)
  0x05, 0x07,                         //     Usage Page (Key Codes)
  0x19, 0xe0,                         //     Usage Minimum (224)
  0x29, 0xe7,                         //     Usage Maximum (231)
  0x15, 0x00,                         //     Logical Minimum (0)
  0x25, 0x01,                         //     Logical Maximum (1)
  0x75, 0x01,                         //     Report Size (1)
  0x95, 0x08,                         //     Report Count (8)
  0x81, 0x02,                         //     Input (Data, Variable, Absolute)
  
  0x95, 0x01,                         //     Report Count (1)
  0x75, 0x08,                         //     Report Size (8)
  0x81, 0x01,                         //     Input (Constant) reserved byte(1)
  
  0x95, 0x05,                         //     Report Count (5)
  0x75, 0x01,                         //     Report Size (1)
  0x05, 0x08,                         //     Usage Page (Page# for LEDs)
  0x19, 0x01,                         //     Usage Minimum (1)
  0x29, 0x05,                         //     Usage Maximum (5)
  0x91, 0x02,                         //     Output (Data, Variable, Absolute), Led report
  0x95, 0x01,                         //     Report Count (1)
  0x75, 0x03,                         //     Report Size (3)
  0x91, 0x01,                         //     Output (Data, Variable, Absolute), Led report padding
  
  0x95, 0x06,                         //     Report Count (6)
  0x75, 0x08,                         //     Report Size (8)
  0x15, 0x00,                         //     Logical Minimum (0)
  0x25, 0x65,                         //     Logical Maximum (101)
  0x05, 0x07,                         //     Usage Page (Key codes)
  0x19, 0x00,                         //     Usage Minimum (0)
  0x29, 0x65,                         //     Usage Maximum (101)
  0x81, 0x00,                         //     Input (Data, Array) Key array(6 bytes)
  
  
  0x09, 0x05,                         //     Usage (Vendor Defined)
  0x15, 0x00,                         //     Logical Minimum (0)
  0x26, 0xFF, 0x00,                   //     Logical Maximum (255)
  0x75, 0x08,                         //     Report Count (2)
  0x95, 0x02,                         //     Report Size (8 bit)
  0xB1, 0x02,                         //     Feature (Data, Variable, Absolute)
  
  0xC0                                // End Collection (Application)
};
const unsigned char hidReportReferenceDescriptorValue2[] = {0x00, 0x02};


BLEService                       hidService                              = BLEService("1812");
BLECharacteristic                hidReportCharacteristic                 = BLECharacteristic("2A4D", BLERead | BLENotify, 8);
BLEDescriptor                    reportReferenceDescriptor1              = BLEDescriptor("2908", hidReportReferenceDescriptorValue1, sizeof(hidReportReferenceDescriptorValue1));
BLEConstantCharacteristic        hidInformationCharacteristic            = BLEConstantCharacteristic("2a4a", hidInformationCharacteriticValue, sizeof(hidInformationCharacteriticValue));
BLEUnsignedCharCharacteristic    hidProtocolModeCharacteristic           = BLEUnsignedCharCharacteristic("2a4e", BLERead | BLEWriteWithoutResponse);
BLEUnsignedCharCharacteristic    hidControlPointCharacteristic           = BLEUnsignedCharCharacteristic("2a4c",BLEWriteWithoutResponse);
BLEConstantCharacteristic        hidReportDescriptor                     = BLEConstantCharacteristic("2a4b", hidReportDescriptorValue, sizeof(hidReportDescriptorValue));
BLEUnsignedCharCharacteristic    hidReportOutputCharacteristic           = BLEUnsignedCharCharacteristic("2a4f", BLERead | BLEWrite | BLEWriteWithoutResponse);
BLEDescriptor                    reportReferenceDescriptor2              = BLEDescriptor("2908", hidReportReferenceDescriptorValue2, sizeof(hidReportReferenceDescriptorValue2));
BLECharacteristic                bootKeyboardInputReportCharacateristic  = BLECharacteristic("2A22", BLERead | BLENotify, 8);
BLEUnsignedCharCharacteristic    bootKeyboardOutputReportCharacateristic = BLEUnsignedCharCharacteristic("2A23", BLERead | BLEWrite | BLEWriteWithoutResponse);


void setup() {
  Serial.begin(9600);
#if defined (__AVR_ATmega32U4__)
  delay(5000);  //5 seconds delay for enabling to see the start up comments on the serial board
#endif

  // clears bond data on every boot
  blePeripheral.setBondStore(bleBondStore);
  
  blePeripheral.setDeviceName("Arduino Keyboard");
  blePeripheral.setAppearance(961);

  blePeripheral.setLocalName("HID-keyboard");
  blePeripheral.setAdvertisedServiceUuid(hidService.uuid());

  // add attributes (services, characteristics, descriptors) to peripheral
  blePeripheral.addAttribute(hidService);
  blePeripheral.addAttribute(hidReportCharacteristic);
  blePeripheral.addAttribute(reportReferenceDescriptor1);
  blePeripheral.addAttribute(hidInformationCharacteristic);
  blePeripheral.addAttribute(hidProtocolModeCharacteristic);
  blePeripheral.addAttribute(hidControlPointCharacteristic);
  blePeripheral.addAttribute(hidReportDescriptor);
  blePeripheral.addAttribute(hidReportOutputCharacteristic);
  blePeripheral.addAttribute(reportReferenceDescriptor2);
  blePeripheral.addAttribute(bootKeyboardInputReportCharacateristic);
  blePeripheral.addAttribute(bootKeyboardOutputReportCharacateristic);
  
  // initialize characteristics values
  hidProtocolModeCharacteristic.setValue(0x01);
  
  unsigned char bootKeyboardInputReportCharacateristicValue[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  bootKeyboardInputReportCharacateristic.setValue(bootKeyboardInputReportCharacateristicValue, sizeof(bootKeyboardInputReportCharacateristicValue));

  // begin initialization
  blePeripheral.begin();
  
  Serial.println(F("BLE HID Keyboard Peripheral"));
}

void loop() {
  BLECentral central = blePeripheral.central();

  if (central) {
    // central connected to peripheral
    Serial.print(F("Connected to central: "));
    Serial.println(central.address());

    while (central.connected()) {
      uint8_t keyPress[8]= { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
      
      if (Serial.available() > 0) {
        // read the ascii character
        unsigned char ascii = Serial.read();
        
        Serial.print(F("ascii = "));
        Serial.println(ascii);
        
        // https://github.com/SFE-Chris/UNO-HIDKeyboard-Library/blob/master/HIDKeyboard.h
        const static uint8_t HIDTable[] =  {
      	  0x00, // 0
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A, 0x00, 0x00, // 10
      	  0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 20
      	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00, // 30
      	  0x00, 0x2c, 0x1e, 0x34, 0x20, 0x21, 0x22, 0x24, 0x34, 0x26, // 40
      	  0x27, 0x25, 0x2e, 0x36, 0x2d, 0x37, 0x38, 0x27, 0x1e, 0x1f, // 50
      	  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x33, 0x33, 0x36, // 60
      	  0x2e, 0x37, 0x38, 0x1f, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, // 70
      	  0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, // 80
      	  0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, // 90
      	  0x2f, 0x31, 0x30, 0x23, 0x2d, 0x35, 0x04, 0x05, 0x06, 0x07, // 100
      	  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, // 110
      	  0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, // 120
      	  0x1c, 0x1d, 0x2f, 0x31, 0x30, 0x35, 127 // 127
        };
        
        if (ascii < sizeof(HIDTable)) {
          
          unsigned char hid = HIDTable[ascii];
          
          Serial.print(F("hid = "));
          Serial.println(hid);
          
          // wait until we can notify
          while(!hidReportCharacteristic.canNotify()) {
            blePeripheral.poll();
          }
          
          // send hid key code
          keyPress[2] = hid;
          hidReportCharacteristic.setValue(keyPress, sizeof(keyPress));
          
          // wait until we can notify
          while(!hidReportCharacteristic.canNotify()) {
            blePeripheral.poll();
          }
  
          // send cleared hid code
          keyPress[2] = 0x00;
          hidReportCharacteristic.setValue(keyPress, sizeof(keyPress));
        }
      }
    }

    // central disconnected
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}
