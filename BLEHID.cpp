#include "BLEHID.h"

static const PROGMEM unsigned char hidInformationCharacteriticValue[]   = { 0x11, 0x01, 0x00, 0x03 };

BLEHID* BLEHID::_instance = NULL;

BLEHID::BLEHID(unsigned char req, unsigned char rdy, unsigned char rst) :
  _blePeripheral(req, rdy, rst),
  _bleBondStore(),

  _hidService("1812"),
  _hidInformationCharacteristic("2a4a", hidInformationCharacteriticValue, sizeof(hidInformationCharacteriticValue)),
  _hidControlPointCharacteristic("2a4c", BLEWriteWithoutResponse),
  _hidReportMapCharacteristic(),

  _devices(NULL),
  _numDevices(0)
{
  _instance = this;
}

BLEHID::~BLEHID() {
  if (this->_devices) {
    free(this->_devices);
  }
}

BLEHID* BLEHID::instance() {
  return _instance;
}

void BLEHID::begin() {
  this->_blePeripheral.setBondStore(this->_bleBondStore);

  this->_blePeripheral.setAdvertisedServiceUuid(this->_hidService.uuid());

  this->_blePeripheral.addAttribute(this->_hidService);
  this->_blePeripheral.addAttribute(this->_hidInformationCharacteristic);
  this->_blePeripheral.addAttribute(this->_hidControlPointCharacteristic);
  this->_blePeripheral.addAttribute(this->_hidReportMapCharacteristic);

  for (int i = 0; i < this->_numDevices; i++) {
    BLEHIDDevice *device = this->_devices[i];

    unsigned char numAttributes = device->numAttributes();
    BLELocalAttribute** attributes = device->attributes();

    for (int j = 0; j < numAttributes; j++) {
      this->_blePeripheral.addAttribute(*attributes[j]);
    }
  }

  this->_hidReportMapCharacteristic.setDevices(this->_devices, this->_numDevices);

  // begin initialization
  this->_blePeripheral.begin();
}

void BLEHID::clearBondStoreData() {
  this->_bleBondStore.clearData();
}

void BLEHID::setLocalName(const char *localName) {
  this->_blePeripheral.setLocalName(localName);
}

void BLEHID::setDeviceName(const char* deviceName) {
  this->_blePeripheral.setDeviceName(deviceName);
}

void BLEHID::setAppearance(unsigned short appearance) {
  this->_blePeripheral.setAppearance(appearance);
}

BLECentral BLEHID::central() {
  return this->_blePeripheral.central();
}

bool BLEHID::connected() {
  return this->_blePeripheral.connected();
}

void BLEHID::poll() {
  this->_blePeripheral.poll();
}

void BLEHID::addDevice(BLEHIDDevice& device) {
  if (this->_devices == NULL) {
    this->_devices = (BLEHIDDevice**)malloc(sizeof(BLEHIDDevice *) * BLEHIDDevice::numDevices());
  }

  device.setReportId(this->_numDevices);

  this->_devices[this->_numDevices] = &device;
  this->_numDevices++;
}

void BLEHID::addAttribute(BLELocalAttribute& attribute) {
  this->_blePeripheral.addAttribute(attribute);
}
