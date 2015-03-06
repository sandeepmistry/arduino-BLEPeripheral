#include "BLEKeyboard.h"

static const PROGMEM unsigned char descriptorValue[] = {
  // From: https://github.com/adafruit/Adafruit-Trinket-USB/blob/master/TrinketHidCombo/TrinketHidComboC.c
  //       permission to use under MIT license by @ladyada (https://github.com/adafruit/Adafruit-Trinket-USB/issues/10)

  0x05, 0x01,           // USAGE_PAGE (Generic Desktop)
  0x09, 0x06,           // USAGE (Keyboard)
  0xA1, 0x01,           // COLLECTION (Application)
  0x85, 0x00,           // REPORT_ID
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
};

BLEKeyboard::BLEKeyboard() :
  BLEHIDDevice(descriptorValue, sizeof(descriptorValue), 7),
  _reportCharacteristic("2a4d", BLERead | BLENotify, 8),
  _reportReferenceDescriptor(BLEHIDDescriptorTypeInput)
{
}

size_t BLEKeyboard::write(uint8_t k) {
  return 0;
}

size_t BLEKeyboard::press(uint8_t k) {
  unsigned char keyPress[8]= { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

  // send key code
  keyPress[2] = k;

  for (int i = 0; i < 2; i++) {
    this->sendData(this->_reportCharacteristic, keyPress, sizeof(keyPress));

    // send cleared code
    keyPress[2] = 0x00;
  }

  return 1;
}

size_t BLEKeyboard::release(uint8_t k) {
  return 0;
}

void BLEKeyboard::releaseAll(void) {

}

void BLEKeyboard::setReportId(unsigned char reportId) {
  BLEHIDDevice::setReportId(reportId);

  this->_reportReferenceDescriptor.setReportId(reportId);
}

unsigned char BLEKeyboard::numAttributes() {
  return 2;
}

BLELocalAttribute** BLEKeyboard::attributes() {
  static BLELocalAttribute* attributes[] = {
    &this->_reportCharacteristic,
    &this->_reportReferenceDescriptor
  };

  return attributes;
}
