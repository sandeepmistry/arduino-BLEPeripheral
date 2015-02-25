#include "BLEHID.h"

static const unsigned char hidInformationCharacteriticValue[]   = { 0x11, 0x01, 0x00, 0x03 };

static const unsigned char hidReportDescriptorValue[] = {
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

static const unsigned char hidReportReferenceDescriptorValue1[] = { 0x00, 0x01 };


BLEHID::BLEHID(unsigned char req, unsigned char rdy, unsigned char rst) :
  _blePeripheral(req, rdy, rst),
  _bleBondStore(),

  _hidService("1812"),
#ifdef USE_BOOT_PROTOCOL_MODE
  _hidProtocolModeCharacteristic("2a4e", BLERead | BLEWriteWithoutResponse),
  _bootKeyboardInputReportCharacateristic("2A23", BLERead | BLEWrite | BLEWriteWithoutResponse, 8)
#endif
  _hidInformationCharacteristic("2a4a", hidInformationCharacteriticValue, sizeof(hidInformationCharacteriticValue)),
  _hidControlPointCharacteristic("2a4c", BLEWriteWithoutResponse),
  _hidReportMapCharacteristic("2a4b", hidReportDescriptorValue, sizeof(hidReportDescriptorValue)),

  _hidReportCharacteristic1("2A4D", BLERead | BLENotify, 8),
  _reportReferenceDescriptor1("2908", hidReportReferenceDescriptorValue1, sizeof(hidReportReferenceDescriptorValue1))
{

}

void BLEHID::begin() {
  // clears bond data on every boot
  this->_bleBondStore.clearData();

  this->_blePeripheral.setBondStore(this->_bleBondStore);

  this->_blePeripheral.setDeviceName("Arduino BLE HID");
  // this->_blePeripheral.setAppearance(961);

  this->_blePeripheral.setLocalName("HID");
  this->_blePeripheral.setAdvertisedServiceUuid(this->_hidService.uuid());

  // add attributes (services, characteristics, descriptors) to peripheral
  this->_blePeripheral.addAttribute(this->_hidService);
#ifdef USE_BOOT_PROTOCOL_MODE
  this->_blePeripheral.addAttribute(this->_hidProtocolModeCharacteristic);
#endif
  this->_blePeripheral.addAttribute(this->_hidInformationCharacteristic);
  this->_blePeripheral.addAttribute(this->_hidControlPointCharacteristic);
  this->_blePeripheral.addAttribute(this->_hidReportMapCharacteristic);

  this->_blePeripheral.addAttribute(this->_hidReportCharacteristic1);
  this->_blePeripheral.addAttribute(this->_reportReferenceDescriptor1);

#ifdef USE_BOOT_PROTOCOL_MODE
  this->_blePeripheral.addAttribute(this->_bootKeyboardInputReportCharacateristic);

  this->_hidProtocolModeCharacteristic.setValue(0x01);

  unsigned char bootKeyboardInputReportCharacateristicValue[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  this->_bootKeyboardInputReportCharacateristic.setValue(bootKeyboardInputReportCharacateristicValue, sizeof(bootKeyboardInputReportCharacateristicValue));
#endif

  // begin initialization
  this->_blePeripheral.begin();
}


BLECentral BLEHID::central() {
  return this->_blePeripheral.central();
}

bool BLEHID::connected() {
  return this->_blePeripheral.connected();
}

size_t BLEHID::write(uint8_t k) {
  uint8_t keyPress[8]= { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

  // wait until we can notify
  while(!this->_hidReportCharacteristic1.canNotify()) {
    this->_blePeripheral.poll();
  }

  // send hid key code
  keyPress[2] = k;
  this->_hidReportCharacteristic1.setValue(keyPress, sizeof(keyPress));

  // wait until we can notify
  while(!this->_hidReportCharacteristic1.canNotify()) {
    this->_blePeripheral.poll();
  }

  // send cleared hid code
  keyPress[2] = 0x00;
  this->_hidReportCharacteristic1.setValue(keyPress, sizeof(keyPress));
}
