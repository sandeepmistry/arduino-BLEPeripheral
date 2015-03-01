#include "BLEHIDReport.h"

#include "BLEKeyboard.h"

static const unsigned char reportReferenceDescriptorValue[] = { REPID_KEYBOARD, 0x01 };

BLEKeyboard::BLEKeyboard() :
  _reportCharacteristic("2a4d", BLERead | BLENotify, 7),
  _reportReferenceDescriptor("2908", reportReferenceDescriptorValue, sizeof(reportReferenceDescriptorValue))
{
}

size_t BLEKeyboard::write(uint8_t k) {
  return 0;
}

size_t BLEKeyboard::press(uint8_t k) {
  unsigned char keyPress[7]= { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

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
