#include "BLEUuid.h"

#include "BLEDeviceLimits.h"

#include "BLEPeripheral.h"

//#define BLE_PERIPHERAL_DEBUG

#define DEFAULT_DEVICE_NAME "Arduino"
#define DEFAULT_APPEARANCE  0x0000

BLEPeripheral::BLEPeripheral(unsigned char req, unsigned char rdy, unsigned char rst) :
#ifdef NRF51
  _nRF51822(),
#else
  _nRF8001(req, rdy, rst),
#endif

  _localName(NULL),
  _manufacturerData(NULL),
  _manufacturerDataLength(0),

  _attributes(NULL),
  _numAttributes(0),

  _genericAccessService("1800"),
  _deviceNameCharacteristic("2a00", BLERead, 19),
  _appearanceCharacteristic("2a01", BLERead, 2),
  _genericAttributeService("1801"),
  _servicesChangedCharacteristic("2a05", BLEIndicate, 4),

  _central(this)
{
#ifdef NRF51
  this->_device = &this->_nRF51822;
#else
  this->_device = &this->_nRF8001;
#endif

  memset(this->_eventHandlers, 0x00, sizeof(this->_eventHandlers));

  this->setDeviceName(DEFAULT_DEVICE_NAME);
  this->setAppearance(DEFAULT_APPEARANCE);

  this->_device->setEventListener(this);
}

BLEPeripheral::~BLEPeripheral() {
  if (this->_attributes) {
    free(this->_attributes);
  }
}

void BLEPeripheral::begin() {
  unsigned char advertisementDataType = 0;
  unsigned char scanDataType = 0;

  unsigned char advertisementDataLength = 0;
  unsigned char scanDataLength = 0;

  unsigned char advertisementData[BLE_ADVERTISEMENT_DATA_MAX_VALUE_LENGTH];
  unsigned char scanData[BLE_SCAN_DATA_MAX_VALUE_LENGTH];

  if (this->_advertisedServiceUuid){
    BLEUuid advertisedServiceUuid = BLEUuid(this->_advertisedServiceUuid);

    advertisementDataLength = advertisedServiceUuid.length();
    advertisementDataType = (advertisementDataLength > 2) ? 0x06 : 0x02;

    memcpy(advertisementData, advertisedServiceUuid.data(), advertisementDataLength);
  } else if (this->_manufacturerData && this->_manufacturerDataLength > 0) {
    advertisementDataLength = this->_manufacturerDataLength;

    if (advertisementDataLength > sizeof(advertisementData)) {
      advertisementDataLength = sizeof(advertisementData);
    }

    advertisementDataType = 0xff;

    memcpy(advertisementData, this->_manufacturerData, advertisementDataLength);
  }

  if (this->_localName){
    unsigned char localNameLength = strlen(this->_localName);
    scanDataLength = localNameLength;

    if (scanDataLength > sizeof(scanData)) {
      scanDataLength = sizeof(scanData);
    }

    scanDataType = (localNameLength > scanDataLength) ? 0x08 : 0x09;

    memcpy(scanData, this->_localName, scanDataLength);
  }

  for (int i = 0; i < this->_numAttributes; i++) {
    BLEAttribute* attribute = this->_attributes[i];
    if (attribute->type() == BLETypeCharacteristic) {
      BLECharacteristic* characteristic = (BLECharacteristic*)attribute;

      characteristic->setValueChangeListener(*this);
    }
  }

  this->_device->begin(advertisementDataType, advertisementDataLength, advertisementData,
                        scanDataType, scanDataLength, scanData,
                        this->_attributes, this->_numAttributes);

  this->_device->requestAddress();
}

void BLEPeripheral::poll() {
  this->_device->poll();
}

void BLEPeripheral::setAdvertisedServiceUuid(const char* advertisedServiceUuid) {
  this->_advertisedServiceUuid = advertisedServiceUuid;
}

void BLEPeripheral::setManufacturerData(const unsigned char manufacturerData[], unsigned char manufacturerDataLength) {
  this->_manufacturerData = manufacturerData;
  this->_manufacturerDataLength = manufacturerDataLength;
}

void BLEPeripheral::setLocalName(const char* localName) {
  this->_localName = localName;
}

void BLEPeripheral::setConnectable(bool connectable) {
  this->_device->setConnectable(connectable);
}

void BLEPeripheral::setDeviceName(const char* deviceName) {
  this->_deviceNameCharacteristic.setValue(deviceName);
}

void BLEPeripheral::setAppearance(unsigned short appearance) {
  this->_appearanceCharacteristic.setValue((unsigned char *)&appearance, sizeof(appearance));
}

void BLEPeripheral::addAttribute(BLEAttribute& attribute) {
  if (this->_attributes == NULL) {
    this->_attributes = (BLEAttribute**)malloc(BLEAttribute::numAttributes() * sizeof(BLEAttribute*));

    this->_attributes[0] = &this->_genericAccessService;
    this->_attributes[1] = &this->_deviceNameCharacteristic;
    this->_attributes[2] = &this->_appearanceCharacteristic;

    this->_attributes[3] = &this->_genericAttributeService;
    this->_attributes[4] = &this->_servicesChangedCharacteristic;

    this->_numAttributes = 5;
  }

  this->_attributes[this->_numAttributes] = &attribute;
  this->_numAttributes++;
}

void BLEPeripheral::disconnect() {
  this->_device->disconnect();
}

BLECentral BLEPeripheral::central() {
  this->poll();

  return this->_central;
}

bool BLEPeripheral::connected() {
  this->poll();

  return this->_central;
}

void BLEPeripheral::setEventHandler(BLEPeripheralEvent event, BLEPeripheralEventHandler eventHandler) {
  if (event < sizeof(this->_eventHandlers)) {
    this->_eventHandlers[event] = eventHandler;
  }
}

bool BLEPeripheral::characteristicValueChanged(BLECharacteristic& characteristic) {
  return this->_device->updateCharacteristicValue(characteristic);
}

bool BLEPeripheral::broadcastCharacteristic(BLECharacteristic& characteristic) {
  return this->_device->broadcastCharacteristic(characteristic);
}

bool BLEPeripheral::canNotifyCharacteristic(BLECharacteristic& characteristic) {
  return this->_device->canNotifyCharacteristic(characteristic);
}

bool BLEPeripheral::canIndicateCharacteristic(BLECharacteristic& characteristic) {
  return this->_device->canIndicateCharacteristic(characteristic);
}

void BLEPeripheral::BLEDeviceConnected(BLEDevice& device, const unsigned char* address) {
  this->_central.setAddress(address);

#ifdef BLE_PERIPHERAL_DEBUG
  Serial.print(F("Peripheral connected to central: "));
  Serial.println(this->_central.address());
#endif

  BLEPeripheralEventHandler eventHandler = this->_eventHandlers[BLEConnected];
  if (eventHandler) {
    eventHandler(this->_central);
  }
}

void BLEPeripheral::BLEDeviceDisconnected(BLEDevice& device) {
#ifdef BLE_PERIPHERAL_DEBUG
  Serial.print(F("Peripheral disconnected from central: "));
  Serial.println(this->_central.address());
#endif

  BLEPeripheralEventHandler eventHandler = this->_eventHandlers[BLEDisconnected];
  if (eventHandler) {
    eventHandler(this->_central);
  }

  this->_central.clearAddress();
}

void BLEPeripheral::BLEDeviceCharacteristicValueChanged(BLEDevice& device, BLECharacteristic& characteristic, const unsigned char* value, unsigned char valueLength) {
  characteristic.setValue(this->_central, value, valueLength);
}

void BLEPeripheral::BLEDeviceCharacteristicSubscribedChanged(BLEDevice& device, BLECharacteristic& characteristic, bool subscribed) {
  characteristic.setSubscribed(this->_central, subscribed);
}

void BLEPeripheral::BLEDeviceAddressReceived(BLEDevice& device, const unsigned char* address) {
#ifdef BLE_PERIPHERAL_DEBUG
  char addressStr[18];

  sprintf(addressStr, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
    address[5],
    address[4],
    address[3],
    address[2],
    address[1],
    address[0]);

  Serial.print(F("Peripheral address: "));
  Serial.println(addressStr);
#endif
}

void BLEPeripheral::BLEDeviceTemperatureReceived(BLEDevice& device, float temperature) {
}

void BLEPeripheral::BLEDeviceBatteryLevelReceived(BLEDevice& device, float batteryLevel) {
}
