#include "BLEHIDReport.h"

#include "BLEMouse.h"

static const unsigned char reportReferenceDescriptorValue[] = { REPID_MOUSE, 0x01 };

BLEMouse::BLEMouse() :
  _reportCharacteristic("2a4d", BLERead | BLENotify, 4),
  _reportReferenceDescriptor("2908", reportReferenceDescriptorValue, sizeof(reportReferenceDescriptorValue)),
  _button(0)
{
}

void BLEMouse::click(uint8_t b) {
  this->press(b);
  this->release(b);
}

void BLEMouse::move(signed char x, signed char y, signed char wheel) {
  unsigned char mouseMove[4]= { 0x00, 0x00, 0x00, 0x00 };

  // send key code
  mouseMove[0] = this->_button;
  mouseMove[1] = x;
  mouseMove[2] = y;
  mouseMove[3] = wheel;

  this->sendData(this->_reportCharacteristic, mouseMove, sizeof(mouseMove));
}

void BLEMouse::press(uint8_t b) {
  this->_button |= b;

  this->move(0, 0, 0);
}

void BLEMouse::release(uint8_t b) {
  this->_button &= ~b;

  this->move(0, 0, 0);
}

bool BLEMouse::isPressed(uint8_t b) {
 return ((this->_button & b) != 0);
}


unsigned char BLEMouse::numAttributes() {
  return 2;
}

BLELocalAttribute** BLEMouse::attributes() {
  static BLELocalAttribute* attributes[] = {
    &this->_reportCharacteristic,
    &this->_reportReferenceDescriptor
  };

  return attributes;
}
