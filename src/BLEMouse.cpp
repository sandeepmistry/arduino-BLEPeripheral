// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "BLEMouse.h"

static const PROGMEM unsigned char descriptorValue[] = {
  // From: https://github.com/adafruit/Adafruit-Trinket-USB/blob/master/TrinketHidCombo/TrinketHidComboC.c
  //       permission to use under MIT license by @ladyada (https://github.com/adafruit/Adafruit-Trinket-USB/issues/10)

  0x05, 0x01,           // USAGE_PAGE (Generic Desktop)
  0x09, 0x02,           // USAGE (Mouse)
  0xa1, 0x01,           // COLLECTION (Application)
  0x09, 0x01,           //   USAGE (Pointer)
  0xA1, 0x00,           //   COLLECTION (Physical)
  0x85, 0x00,           //     REPORT_ID
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
  0xC0                  // END COLLECTION
};

BLEMouse::BLEMouse() :
  BLEHID(descriptorValue, sizeof(descriptorValue), 11),
  _reportCharacteristic("2a4d", BLERead | BLENotify, 4),
  _reportReferenceDescriptor(BLEHIDDescriptorTypeInput),
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

void BLEMouse::setReportId(unsigned char reportId) {
  BLEHID::setReportId(reportId);

  this->_reportReferenceDescriptor.setReportId(reportId);
}

unsigned char BLEMouse::numAttributes() {
  return 2;
}

BLELocalAttribute** BLEMouse::attributes() {
  static BLELocalAttribute* attributes[2];

  attributes[0] = &this->_reportCharacteristic;
  attributes[1] = &this->_reportReferenceDescriptor;

  return attributes;
}
