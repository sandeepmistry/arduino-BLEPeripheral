#include "BLEHIDPeripheral.h"

static const PROGMEM unsigned char hidInformationCharacteriticValue[]   = { 0x11, 0x01, 0x00, 0x03 };

BLEHIDPeripheral* BLEHIDPeripheral::_instance = NULL;

BLEHIDPeripheral::BLEHIDPeripheral(unsigned char req, unsigned char rdy, unsigned char rst) :
  _blePeripheral(req, rdy, rst),
  _bleBondStore(),

  _hidService("1812"),
  _hidInformationCharacteristic("2a4a", hidInformationCharacteriticValue, sizeof(hidInformationCharacteriticValue)),
  _hidControlPointCharacteristic("2a4c", BLEWriteWithoutResponse),
  _hidReportMapCharacteristic(),

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
  this->_blePeripheral.setBondStore(this->_bleBondStore);

  this->_blePeripheral.setAdvertisedServiceUuid(this->_hidService.uuid());

  this->_blePeripheral.addAttribute(this->_hidService);
  this->_blePeripheral.addAttribute(this->_hidInformationCharacteristic);
  this->_blePeripheral.addAttribute(this->_hidControlPointCharacteristic);
  this->_blePeripheral.addAttribute(this->_hidReportMapCharacteristic);

  for (int i = 0; i < this->_numHids; i++) {
    BLEHID *hid = this->_hids[i];

    unsigned char numAttributes = hid->numAttributes();
    BLELocalAttribute** attributes = hid->attributes();

    for (int j = 0; j < numAttributes; j++) {
      this->_blePeripheral.addAttribute(*attributes[j]);
    }
  }

  this->_hidReportMapCharacteristic.setHids(this->_hids, this->_numHids);

  // begin initialization
  this->_blePeripheral.begin();
}

void BLEHIDPeripheral::clearBondStoreData() {
  this->_bleBondStore.clearData();
}

void BLEHIDPeripheral::setLocalName(const char *localName) {
  this->_blePeripheral.setLocalName(localName);
}

void BLEHIDPeripheral::setDeviceName(const char* deviceName) {
  this->_blePeripheral.setDeviceName(deviceName);
}

void BLEHIDPeripheral::setAppearance(unsigned short appearance) {
  this->_blePeripheral.setAppearance(appearance);
}

BLECentral BLEHIDPeripheral::central() {
  return this->_blePeripheral.central();
}

bool BLEHIDPeripheral::connected() {
  return this->_blePeripheral.connected();
}

void BLEHIDPeripheral::poll() {
  this->_blePeripheral.poll();
}

void BLEHIDPeripheral::addHID(BLEHID& hid) {
  if (this->_hids == NULL) {
    this->_hids = (BLEHID**)malloc(sizeof(BLEHID*) * BLEHID::numHids());
  }

  hid.setReportId(this->_numHids);

  this->_hids[this->_numHids] = &hid;
  this->_numHids++;
}

void BLEHIDPeripheral::addAttribute(BLELocalAttribute& attribute) {
  this->_blePeripheral.addAttribute(attribute);
}
