// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "BLEHIDPeripheral.h"

static const PROGMEM unsigned char hidInformationCharacteriticValue[]   = { 0x11, 0x01, 0x00, 0x03 };

BLEHIDPeripheral* BLEHIDPeripheral::_instance = NULL;

BLEHIDPeripheral::BLEHIDPeripheral(unsigned char req, unsigned char rdy, unsigned char rst) :
  BLEPeripheral(req, rdy, rst),
  _bleBondStore(),

  _hidService("1812"),
  _hidReportMapCharacteristic(),
  _hidInformationCharacteristic("2a4a", hidInformationCharacteriticValue, sizeof(hidInformationCharacteriticValue)),
  _hidControlPointCharacteristic("2a4c", BLEWriteWithoutResponse),

  _reportIdOffset(0),

  _hids(NULL),
  _numHids(0)
{
  _instance = this;
}

BLEHIDPeripheral::~BLEHIDPeripheral() {
  if (this->_hids) {
    free(this->_hids);
  }
}

BLEHIDPeripheral* BLEHIDPeripheral::instance() {
  return _instance;
}

void BLEHIDPeripheral::begin() {
  this->setBondStore(this->_bleBondStore);

  this->setAdvertisedServiceUuid(this->_hidService.uuid());

  this->addAttribute(this->_hidService);
  this->addAttribute(this->_hidInformationCharacteristic);
  this->addAttribute(this->_hidControlPointCharacteristic);
  this->addAttribute(this->_hidReportMapCharacteristic);

  for (int i = 0; i < this->_numHids; i++) {
    BLEHID *hid = this->_hids[i];

    unsigned char numAttributes = hid->numAttributes();
    BLELocalAttribute** attributes = hid->attributes();

    for (int j = 0; j < numAttributes; j++) {
      this->addAttribute(*attributes[j]);
    }
  }

  this->_hidReportMapCharacteristic.setHids(this->_hids, this->_numHids);

  // begin initialization
  BLEPeripheral::begin();
}

void BLEHIDPeripheral::clearBondStoreData() {
  this->_bleBondStore.clearData();
}

void BLEHIDPeripheral::setReportIdOffset(unsigned char reportIdOffset) {
  this->_reportIdOffset = reportIdOffset;
}

void BLEHIDPeripheral::poll() {
  BLEPeripheral::poll();
}

void BLEHIDPeripheral::addHID(BLEHID& hid) {
  if (this->_hids == NULL) {
    this->_hids = (BLEHID**)malloc(sizeof(BLEHID*) * BLEHID::numHids());
  }

  hid.setReportId(this->_numHids + this->_reportIdOffset);

  this->_hids[this->_numHids] = &hid;
  this->_numHids++;
}
