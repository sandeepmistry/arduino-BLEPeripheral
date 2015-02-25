#include "BLEHID.h"

static const unsigned char hidInformationCharacteriticValue[]   = { 0x11, 0x01, 0x00, 0x03 };

// From: https://github.com/adafruit/Adafruit-Trinket-USB/blob/master/TrinketHidCombo/TrinketHidComboC.h
//       permission to use under MIT license by @ladyada (https://github.com/adafruit/Adafruit-Trinket-USB/issues/10)
#define REPID_MOUSE         1
#define REPID_KEYBOARD      0 //2
#define REPID_MMKEY         3
#define REPID_SYSCTRLKEY    4
#define REPSIZE_MOUSE       4
#define REPSIZE_KEYBOARD    8
#define REPSIZE_MMKEY       3
#define REPSIZE_SYSCTRLKEY  2

static const unsigned char hidReportDescriptorValue[] = {
// From: https://github.com/adafruit/Adafruit-Trinket-USB/blob/master/TrinketHidCombo/TrinketHidComboC.c
//       permission to use under MIT license by @ladyada (https://github.com/adafruit/Adafruit-Trinket-USB/issues/10)

  // 0x05, 0x01,           // USAGE_PAGE (Generic Desktop)
  // 0x09, 0x02,           // USAGE (Mouse)
  // 0xa1, 0x01,           // COLLECTION (Application)
  // 0x09, 0x01,           //   USAGE (Pointer)
  // 0xA1, 0x00,           //   COLLECTION (Physical)
  // 0x85, REPID_MOUSE,    //     REPORT_ID
  // 0x05, 0x09,           //     USAGE_PAGE (Button)
  // 0x19, 0x01,           //     USAGE_MINIMUM
  // 0x29, 0x03,           //     USAGE_MAXIMUM
  // 0x15, 0x00,           //     LOGICAL_MINIMUM (0)
  // 0x25, 0x01,           //     LOGICAL_MAXIMUM (1)
  // 0x95, 0x03,           //     REPORT_COUNT (3)
  // 0x75, 0x01,           //     REPORT_SIZE (1)
  // 0x81, 0x02,           //     INPUT (Data,Var,Abs)
  // 0x95, 0x01,           //     REPORT_COUNT (1)
  // 0x75, 0x05,           //     REPORT_SIZE (5)
  // 0x81, 0x03,           //     INPUT (Const,Var,Abs)
  // 0x05, 0x01,           //     USAGE_PAGE (Generic Desktop)
  // 0x09, 0x30,           //     USAGE (X)
  // 0x09, 0x31,           //     USAGE (Y)
  // 0x15, 0x81,           //     LOGICAL_MINIMUM (-127)
  // 0x25, 0x7F,           //     LOGICAL_MAXIMUM (127)
  // 0x75, 0x08,           //     REPORT_SIZE (8)
  // 0x95, 0x02,           //     REPORT_COUNT (2)
  // 0x81, 0x06,           //     INPUT (Data,Var,Rel)
  // 0xC0,                 //   END_COLLECTION
  // 0xC0,                 // END COLLECTION

  0x05, 0x01,           // USAGE_PAGE (Generic Desktop)
  0x09, 0x06,           // USAGE (Keyboard)
  0xA1, 0x01,           // COLLECTION (Application)
  0x85, REPID_KEYBOARD, // REPORT_ID
  0x75, 0x01,           //   REPORT_SIZE (1)
  0x95, 0x08,           //   REPORT_COUNT (8)
  0x05, 0x07,           //   USAGE_PAGE (Keyboard)(Key Codes)
  0x19, 0xE0,           //   USAGE_MINIMUM (Keyboard LeftControl)(224)
  0x29, 0xE7,           //   USAGE_MAXIMUM (Keyboard Right GUI)(231)
  0x15, 0x00,           //   LOGICAL_MINIMUM (0)
  0x25, 0x01,           //   LOGICAL_MAXIMUM (1)
  0x81, 0x02,           //   INPUT (Data,Var,Abs) ; Modifier byte
  0x95, 0x01,           //   REPORT_COUNT (1)
  0x75, 0x08,           //   REPORT_SIZE (8)
  0x81, 0x03,           //   INPUT (Cnst,Var,Abs) ; Reserved byte
  // 0x95, 0x05,           //   REPORT_COUNT (5)
  // 0x75, 0x01,           //   REPORT_SIZE (1)
  // 0x05, 0x08,           //   USAGE_PAGE (LEDs)
  // 0x19, 0x01,           //   USAGE_MINIMUM (Num Lock)
  // 0x29, 0x05,           //   USAGE_MAXIMUM (Kana)
  // 0x91, 0x02,           //   OUTPUT (Data,Var,Abs) ; LED report
  // 0x95, 0x01,           //   REPORT_COUNT (1)
  // 0x75, 0x03,           //   REPORT_SIZE (3)
  // 0x91, 0x03,           //   OUTPUT (Cnst,Var,Abs) ; LED report padding
  0x95, 0x05,           //   REPORT_COUNT (5)
  0x75, 0x08,           //   REPORT_SIZE (8)
  0x15, 0x00,           //   LOGICAL_MINIMUM (0)
  0x26, 0xA4, 0x00,     //   LOGICAL_MAXIMUM (164)
  0x05, 0x07,           //   USAGE_PAGE (Keyboard)(Key Codes)
  0x19, 0x00,           //   USAGE_MINIMUM (Reserved (no event indicated))(0)
  0x2A, 0xA4, 0x00,     //   USAGE_MAXIMUM (Keyboard Application)(164)
  0x81, 0x00,           //   INPUT (Data,Ary,Abs)
  0xC0,                 // END_COLLECTION

  // // this second multimedia key report is what handles the multimedia keys
  // 0x05, 0x0C,           // USAGE_PAGE (Consumer Devices)
  // 0x09, 0x01,           // USAGE (Consumer Control)
  // 0xA1, 0x01,           // COLLECTION (Application)
  // 0x85, REPID_MMKEY,    //   REPORT_ID
  // 0x19, 0x00,           //   USAGE_MINIMUM (Unassigned)
  // 0x2A, 0x3C, 0x02,     //   USAGE_MAXIMUM
  // 0x15, 0x00,           //   LOGICAL_MINIMUM (0)
  // 0x26, 0x3C, 0x02,     //   LOGICAL_MAXIMUM
  // 0x95, 0x01,           //   REPORT_COUNT (1)
  // 0x75, 0x10,           //   REPORT_SIZE (16)
  // 0x81, 0x00,           //   INPUT (Data,Ary,Abs)
  // 0xC0,                 // END_COLLECTION

  // // system controls, like power, needs a 3rd different report and report descriptor
  // 0x05, 0x01,             // USAGE_PAGE (Generic Desktop)
  // 0x09, 0x80,             // USAGE (System Control)
  // 0xA1, 0x01,             // COLLECTION (Application)
  // 0x85, REPID_SYSCTRLKEY, //   REPORT_ID
  // 0x95, 0x01,             //   REPORT_COUNT (1)
  // 0x75, 0x02,             //   REPORT_SIZE (2)
  // 0x15, 0x01,             //   LOGICAL_MINIMUM (1)
  // 0x25, 0x03,             //   LOGICAL_MAXIMUM (3)
  // 0x09, 0x82,             //   USAGE (System Sleep)
  // 0x09, 0x81,             //   USAGE (System Power)
  // 0x09, 0x83,             //   USAGE (System Wakeup)
  // 0x81, 0x60,             //   INPUT
  // 0x75, 0x06,             //   REPORT_SIZE (6)
  // 0x81, 0x03,             //   INPUT (Cnst,Var,Abs)
  // 0xC0,                   // END_COLLECTION
};

static const unsigned char hidKeyboardreportReferenceDescriptorValue[] = { 0x00, 0x01 };


BLEHID::BLEHID(unsigned char req, unsigned char rdy, unsigned char rst) :
  _blePeripheral(req, rdy, rst),
  _bleBondStore(),

  _hidService("1812"),
#ifdef USE_BOOT_PROTOCOL_MODE
  _hidProtocolModeCharacteristic("2a4e", BLERead | BLEWriteWithoutResponse),
  _hidBootKeyboardInputReportCharacateristic("2A23", BLERead | BLEWrite | BLEWriteWithoutResponse, 8),
#endif
  _hidInformationCharacteristic("2a4a", hidInformationCharacteriticValue, sizeof(hidInformationCharacteriticValue)),
  _hidControlPointCharacteristic("2a4c", BLEWriteWithoutResponse),
  _hidReportMapCharacteristic("2a4b", hidReportDescriptorValue, sizeof(hidReportDescriptorValue)),

  _hidKeyboardReportCharacteristic("2A4D", BLERead | BLENotify, 8),
  _hidKeyboardreportReferenceDescriptor("2908", hidKeyboardreportReferenceDescriptorValue, sizeof(hidKeyboardreportReferenceDescriptorValue))
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

  this->_blePeripheral.addAttribute(this->_hidKeyboardReportCharacteristic);
  this->_blePeripheral.addAttribute(this->_hidKeyboardreportReferenceDescriptor);

#ifdef USE_BOOT_PROTOCOL_MODE
  this->_blePeripheral.addAttribute(this->_hidBootKeyboardInputReportCharacateristic);

  this->_hidProtocolModeCharacteristic.setValue(0x01);

  unsigned char hidBootKeyboardInputReportCharacateristicValue[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  this->_hidBootKeyboardInputReportCharacateristic.setValue(hidBootKeyboardInputReportCharacateristicValue, sizeof(hidBootKeyboardInputReportCharacateristicValue));
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
  while(!this->_hidKeyboardReportCharacteristic.canNotify()) {
    this->_blePeripheral.poll();
  }

  // send hid key code
  keyPress[2] = k;
  this->_hidKeyboardReportCharacteristic.setValue(keyPress, sizeof(keyPress));

  // wait until we can notify
  while(!this->_hidKeyboardReportCharacteristic.canNotify()) {
    this->_blePeripheral.poll();
  }

  // send cleared hid code
  keyPress[2] = 0x00;
  this->_hidKeyboardReportCharacteristic.setValue(keyPress, sizeof(keyPress));
}
