// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "BLEMultimedia.h"

static const PROGMEM unsigned char descriptorValue[] = {
  // From: https://github.com/adafruit/Adafruit-Trinket-USB/blob/master/TrinketHidCombo/TrinketHidComboC.c
  //       permission to use under MIT license by @ladyada (https://github.com/adafruit/Adafruit-Trinket-USB/issues/10)

  // this second multimedia key report is what handles the multimedia keys
  0x05, 0x0C,           // USAGE_PAGE (Consumer Devices)
  0x09, 0x01,           // USAGE (Consumer Control)
  0xA1, 0x01,           // COLLECTION (Application)
  0x85, 0x00,           //   REPORT_ID
  0x19, 0x00,           //   USAGE_MINIMUM (Unassigned)
  0x2A, 0x3C, 0x02,     //   USAGE_MAXIMUM
  0x15, 0x00,           //   LOGICAL_MINIMUM (0)
  0x26, 0x3C, 0x02,     //   LOGICAL_MAXIMUM
  0x95, 0x01,           //   REPORT_COUNT (1)
  0x75, 0x10,           //   REPORT_SIZE (16)
  0x81, 0x00,           //   INPUT (Data,Ary,Abs)
  0xC0                  // END_COLLECTION
};

BLEMultimedia::BLEMultimedia() :
  BLEHID(descriptorValue, sizeof(descriptorValue), 7),
  _reportCharacteristic("2a4d", BLERead | BLENotify, 2),
  _reportReferenceDescriptor(BLEHIDDescriptorTypeInput)
{
}

size_t BLEMultimedia::write(uint8_t k) {
  uint8_t multimediaKeyPress[2]= { 0x00, 0x00 };

  // send key code
  multimediaKeyPress[0] = k;

  for (int i = 0; i < 2; i++) {
    this->sendData(this->_reportCharacteristic, multimediaKeyPress, sizeof(multimediaKeyPress));

    // send cleared code
    multimediaKeyPress[0] = 0x00;
  }

  return 1;
}

void BLEMultimedia::setReportId(unsigned char reportId) {
  BLEHID::setReportId(reportId);

  this->_reportReferenceDescriptor.setReportId(reportId);
}

unsigned char BLEMultimedia::numAttributes() {
  return 2;
}

BLELocalAttribute** BLEMultimedia::attributes() {
  static BLELocalAttribute* attributes[2];

  attributes[0] = &this->_reportCharacteristic;
  attributes[1] = &this->_reportReferenceDescriptor;

  return attributes;
}
