// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "BLESystemControl.h"

static const PROGMEM unsigned char descriptorValue[] = {
  // From: https://github.com/adafruit/Adafruit-Trinket-USB/blob/master/TrinketHidCombo/TrinketHidComboC.c
  //       permission to use under MIT license by @ladyada (https://github.com/adafruit/Adafruit-Trinket-USB/issues/10)

  // system controls, like power, needs a 3rd different report and report descriptor
  0x05, 0x01,             // USAGE_PAGE (Generic Desktop)
  0x09, 0x80,             // USAGE (System Control)
  0xA1, 0x01,             // COLLECTION (Application)
  0x85, 0x00,             //   REPORT_ID
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
  0xC0                    // END_COLLECTION
};

BLESystemControl::BLESystemControl() :
  BLEHID(descriptorValue, sizeof(descriptorValue), 7),
  _reportCharacteristic("2a4d", BLERead | BLENotify, 4),
  _reportReferenceDescriptor(BLEHIDDescriptorTypeInput)
{
}

size_t BLESystemControl::write(uint8_t k) {
  uint8_t sysCtrlKeyPress[1]= { 0x00 };

  // send key code
  sysCtrlKeyPress[0] = k;

  for (int i = 0; i < 2; i++) {
    this->sendData(this->_reportCharacteristic, sysCtrlKeyPress, sizeof(sysCtrlKeyPress));

    // send cleared code
    sysCtrlKeyPress[0] = 0x00;
  }

  return 1;
}

void BLESystemControl::setReportId(unsigned char reportId) {
  BLEHID::setReportId(reportId);

  this->_reportReferenceDescriptor.setReportId(reportId);
}

unsigned char BLESystemControl::numAttributes() {
  return 2;
}

BLELocalAttribute** BLESystemControl::attributes() {
  static BLELocalAttribute* attributes[2];

  attributes[0] = &this->_reportCharacteristic;
  attributes[1] = &this->_reportReferenceDescriptor;

  return attributes;
}
