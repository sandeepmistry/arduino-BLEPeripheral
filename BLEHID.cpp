#include "BLEHID.h"

static const unsigned char hidInformationCharacteriticValue[]   = { 0x11, 0x01, 0x00, 0x03 };

// From: https://github.com/adafruit/Adafruit-Trinket-USB/blob/master/TrinketHidCombo/TrinketHidComboC.h
//       permission to use under MIT license by @ladyada (https://github.com/adafruit/Adafruit-Trinket-USB/issues/10)
#define REPID_MOUSE         1
#define REPID_KEYBOARD      2
#define REPID_MMKEY         3
#define REPID_SYSCTRLKEY    4

static const unsigned char hidReportDescriptorValue[] = {
// From: https://github.com/adafruit/Adafruit-Trinket-USB/blob/master/TrinketHidCombo/TrinketHidComboC.c
//       permission to use under MIT license by @ladyada (https://github.com/adafruit/Adafruit-Trinket-USB/issues/10)

  0x05, 0x01,           // USAGE_PAGE (Generic Desktop)
  0x09, 0x02,           // USAGE (Mouse)
  0xa1, 0x01,           // COLLECTION (Application)
  0x09, 0x01,           //   USAGE (Pointer)
  0xA1, 0x00,           //   COLLECTION (Physical)
  0x85, REPID_MOUSE,    //     REPORT_ID
  0x05, 0x09,           //     USAGE_PAGE (Button)
  0x19, 0x01,           //     USAGE_MINIMUM
  0x29, 0x03,           //     USAGE_MAXIMUM
  0x15, 0x00,           //     LOGICAL_MINIMUM (0)
  0x25, 0x01,           //     LOGICAL_MAXIMUM (1)
  0x95, 0x03,           //     REPORT_COUNT (3)
  0x75, 0x01,           //     REPORT_SIZE (1)
  0x81, 0x02,           //     INPUT (Data,Var,Abs)
  0x95, 0x01,           //     REPORT_COUNT (1)
  0x75, 0x05,           //     REPORT_SIZE (5)
  0x81, 0x03,           //     INPUT (Const,Var,Abs)
  0x05, 0x01,           //     USAGE_PAGE (Generic Desktop)
  0x09, 0x30,           //     USAGE (X)
  0x09, 0x31,           //     USAGE (Y)
  0x15, 0x81,           //     LOGICAL_MINIMUM (-127)
  0x25, 0x7F,           //     LOGICAL_MAXIMUM (127)
  0x75, 0x08,           //     REPORT_SIZE (8)
  0x95, 0x02,           //     REPORT_COUNT (2)
  0x81, 0x06,           //     INPUT (Data,Var,Rel)
  0xC0,                 //   END_COLLECTION
  0xC0,                 // END COLLECTION

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
#ifdef USE_LED_REPORT
  0x95, 0x05,           //   REPORT_COUNT (5)
  0x75, 0x01,           //   REPORT_SIZE (1)
  0x05, 0x08,           //   USAGE_PAGE (LEDs)
  0x19, 0x01,           //   USAGE_MINIMUM (Num Lock)
  0x29, 0x05,           //   USAGE_MAXIMUM (Kana)
  0x91, 0x02,           //   OUTPUT (Data,Var,Abs) ; LED report
  0x95, 0x01,           //   REPORT_COUNT (1)
  0x75, 0x03,           //   REPORT_SIZE (3)
  0x91, 0x03,           //   OUTPUT (Cnst,Var,Abs) ; LED report padding
#endif
  0x95, 0x05,           //   REPORT_COUNT (5)
  0x75, 0x08,           //   REPORT_SIZE (8)
  0x15, 0x00,           //   LOGICAL_MINIMUM (0)
  0x26, 0xA4, 0x00,     //   LOGICAL_MAXIMUM (164)
  0x05, 0x07,           //   USAGE_PAGE (Keyboard)(Key Codes)
  0x19, 0x00,           //   USAGE_MINIMUM (Reserved (no event indicated))(0)
  0x2A, 0xA4, 0x00,     //   USAGE_MAXIMUM (Keyboard Application)(164)
  0x81, 0x00,           //   INPUT (Data,Ary,Abs)
  0xC0,                 // END_COLLECTION

  // this second multimedia key report is what handles the multimedia keys
  0x05, 0x0C,           // USAGE_PAGE (Consumer Devices)
  0x09, 0x01,           // USAGE (Consumer Control)
  0xA1, 0x01,           // COLLECTION (Application)
  0x85, REPID_MMKEY,    //   REPORT_ID
  0x19, 0x00,           //   USAGE_MINIMUM (Unassigned)
  0x2A, 0x3C, 0x02,     //   USAGE_MAXIMUM
  0x15, 0x00,           //   LOGICAL_MINIMUM (0)
  0x26, 0x3C, 0x02,     //   LOGICAL_MAXIMUM
  0x95, 0x01,           //   REPORT_COUNT (1)
  0x75, 0x10,           //   REPORT_SIZE (16)
  0x81, 0x00,           //   INPUT (Data,Ary,Abs)
  0xC0,                 // END_COLLECTION

  // system controls, like power, needs a 3rd different report and report descriptor
  0x05, 0x01,             // USAGE_PAGE (Generic Desktop)
  0x09, 0x80,             // USAGE (System Control)
  0xA1, 0x01,             // COLLECTION (Application)
  0x85, REPID_SYSCTRLKEY, //   REPORT_ID
  0x95, 0x01,             //   REPORT_COUNT (1)
  0x75, 0x02,             //   REPORT_SIZE (2)
  0x15, 0x01,             //   LOGICAL_MINIMUM (1)
  0x25, 0x03,             //   LOGICAL_MAXIMUM (3)
  0x09, 0x82,             //   USAGE (System Sleep)
  0x09, 0x81,             //   USAGE (System Power)
  0x09, 0x83,             //   USAGE (System Wakeup)
  0x81, 0x60,             //   INPUT
  0x75, 0x06,             //   REPORT_SIZE (6)
  0x81, 0x03,             //   INPUT (Cnst,Var,Abs)
  0xC0,                   // END_COLLECTION
};

static const unsigned char hidMouseReportReferenceDescriptorValue[] = { REPID_MOUSE, 0x01 };
static const unsigned char hidKeyboardReportReferenceDescriptorValue[] = { REPID_KEYBOARD, 0x01 };
#ifdef USE_LED_REPORT
static const unsigned char hidLedReportReferenceDescriptorValue[] = { REPID_KEYBOARD, 0x02 };
#endif
static const unsigned char hidMMKeyReportReferenceDescriptorValue[] = { REPID_MMKEY, 0x01 };
static const unsigned char hidSysCtrlKeyReportReferenceDescriptorValue[] = { REPID_SYSCTRLKEY, 0x01 };


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

  _hidMouseReportCharacteristic("2a4d", BLERead | BLENotify, 3),
  _hidMouseReportReferenceDescriptor("2908", hidMouseReportReferenceDescriptorValue, sizeof(hidMouseReportReferenceDescriptorValue)),
  _hidKeyboardReportCharacteristic("2a4d", BLERead | BLENotify, 7),
  _hidKeyboardReportReferenceDescriptor("2908", hidKeyboardReportReferenceDescriptorValue, sizeof(hidKeyboardReportReferenceDescriptorValue)),
#ifdef USE_LED_REPORT
  _hidLedReportCharacteristic("2a4d", BLERead | BLEWrite | BLEWriteWithoutResponse, 1),
  _hidLedReportReferenceDescriptor("2908", hidLedReportReferenceDescriptorValue, sizeof(hidLedReportReferenceDescriptorValue)),
#endif
  _hidMMKeyReportCharacteristic("2a4d", BLERead | BLENotify, 2),
  _hidMMKeyReportReferenceDescriptor("2908", hidMMKeyReportReferenceDescriptorValue, sizeof(hidMMKeyReportReferenceDescriptorValue)),
  _hidSysCtrlKeyReportCharacteristic("2a4d", BLERead | BLENotify, 1),
  _hidSysCtrlKeyReportReferenceDescriptor("2908", hidSysCtrlKeyReportReferenceDescriptorValue, sizeof(hidSysCtrlKeyReportReferenceDescriptorValue))
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

  this->_blePeripheral.addAttribute(this->_hidMouseReportCharacteristic);
  this->_blePeripheral.addAttribute(this->_hidMouseReportReferenceDescriptor);
  this->_blePeripheral.addAttribute(this->_hidKeyboardReportCharacteristic);
  this->_blePeripheral.addAttribute(this->_hidKeyboardReportReferenceDescriptor);
#ifdef USE_LED_REPORT
  this->_blePeripheral.addAttribute(this->_hidLedReportCharacteristic);
  this->_blePeripheral.addAttribute(this->_hidLedReportReferenceDescriptor);
#endif
  this->_blePeripheral.addAttribute(this->_hidMMKeyReportCharacteristic);
  this->_blePeripheral.addAttribute(this->_hidMMKeyReportReferenceDescriptor);
  this->_blePeripheral.addAttribute(this->_hidSysCtrlKeyReportCharacteristic);
  this->_blePeripheral.addAttribute(this->_hidSysCtrlKeyReportReferenceDescriptor);

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

void BLEHID::mouseMove(signed char x, signed char y, uint8_t buttonMask) {
  uint8_t mouseMove[3]= { 0x00, 0x00, 0x00 };

  // wait until we can notify
  while(!this->_hidMouseReportCharacteristic.canNotify()) {
    this->_blePeripheral.poll();
  }

  // send hid key code
  mouseMove[0] = buttonMask;
  mouseMove[1] = x;
  mouseMove[2] = y;

  this->_hidMouseReportCharacteristic.setValue(mouseMove, sizeof(mouseMove));
}

void BLEHID::pressKey(uint8_t key) {
  uint8_t keyPress[7]= { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

  // wait until we can notify
  while(!this->_hidKeyboardReportCharacteristic.canNotify()) {
    this->_blePeripheral.poll();
  }

  // send hid key code
  keyPress[2] = key;
  this->_hidKeyboardReportCharacteristic.setValue(keyPress, sizeof(keyPress));

  // wait until we can notify
  while(!this->_hidKeyboardReportCharacteristic.canNotify()) {
    this->_blePeripheral.poll();
  }

  // send cleared hid code
  keyPress[2] = 0x00;
  this->_hidKeyboardReportCharacteristic.setValue(keyPress, sizeof(keyPress));
}

void BLEHID::pressMultimediaKey(uint8_t key) {
  uint8_t multimediaKeyPress[2]= { 0x00, 0x00 };

  // wait until we can notify
  while(!this->_hidMMKeyReportCharacteristic.canNotify()) {
    this->_blePeripheral.poll();
  }

  // send hid key code
  multimediaKeyPress[0] = key;
  this->_hidMMKeyReportCharacteristic.setValue(multimediaKeyPress, sizeof(multimediaKeyPress));

  // wait until we can notify
  while(!this->_hidMMKeyReportCharacteristic.canNotify()) {
    this->_blePeripheral.poll();
  }

  // send cleared hid code
  multimediaKeyPress[0] = 0x00;
  this->_hidMMKeyReportCharacteristic.setValue(multimediaKeyPress, sizeof(multimediaKeyPress));
}

void BLEHID::pressSystemCtrlKey(uint8_t key) {
  uint8_t sysCtrlKeyPress[1]= { 0x00 };

  // wait until we can notify
  while(!this->_hidSysCtrlKeyReportCharacteristic.canNotify()) {
    this->_blePeripheral.poll();
  }

  // send hid key code
  sysCtrlKeyPress[0] = key;
  this->_hidSysCtrlKeyReportCharacteristic.setValue(sysCtrlKeyPress, sizeof(sysCtrlKeyPress));

  // wait until we can notify
  while(!this->_hidSysCtrlKeyReportCharacteristic.canNotify()) {
    this->_blePeripheral.poll();
  }

  // send cleared hid code
  sysCtrlKeyPress[0] = 0x00;
  this->_hidSysCtrlKeyReportCharacteristic.setValue(sysCtrlKeyPress, sizeof(sysCtrlKeyPress));
}
