#include "BLEUuid.h"

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
  memset(this->_eventHandlers, 0x00, sizeof(this->_eventHandlers));

  this->setDeviceName(DEFAULT_DEVICE_NAME);
  this->setAppearance(DEFAULT_APPEARANCE);

#ifdef NRF51
  this->_nRF51822.setEventListener(this);
#else
  this->_nRF8001.setEventListener(this);
#endif
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

  unsigned char advertisementData[20];
  unsigned char scanData[20];

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
#ifdef NRF51
  this->_nRF51822.begin(advertisementDataType, advertisementDataLength, advertisementData, scanDataType, scanDataLength, scanData, this->_attributes, this->_numAttributes);

  this->_nRF51822.requestAddress();
#else
  this->_nRF8001.begin(advertisementDataType, advertisementDataLength, advertisementData, scanDataType, scanDataLength, scanData, this->_attributes, this->_numAttributes);

  this->_nRF8001.requestAddress();
#endif
}

void BLEPeripheral::poll() {
#ifdef NRF51
  this->_nRF51822.poll();
#else
  this->_nRF8001.poll();
#endif
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
#ifdef NRF51
  this->_nRF51822.disconnect();
#else
  this->_nRF8001.disconnect();
#endif
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
#ifdef NRF51
  return this->_nRF51822.updateCharacteristicValue(characteristic);
#else
  return this->_nRF8001.updateCharacteristicValue(characteristic);
#endif
}

bool BLEPeripheral::canNotifyCharacteristic(BLECharacteristic& characteristic) {
#ifdef NRF51
  return this->_nRF51822.canNotifyCharacteristic(characteristic);
#else
  return this->_nRF8001.canNotifyCharacteristic(characteristic);
#endif
}

bool BLEPeripheral::canIndicateCharacteristic(BLECharacteristic& characteristic) {
#ifdef NRF51
  return this->_nRF51822.canIndicateCharacteristic(characteristic);
#else
  return this->_nRF8001.canIndicateCharacteristic(characteristic);
#endif
}

#ifdef NRF51
void BLEPeripheral::nRF51822Connected(nRF51822& nRF51822, const unsigned char* address) {
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

void BLEPeripheral::nRF51822Disconnected(nRF51822& nRF51822) {
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

void BLEPeripheral::nRF51822CharacteristicValueChanged(nRF51822& nRF51822, BLECharacteristic& characteristic, const unsigned char* value, unsigned char valueLength) {
  characteristic.setValue(this->_central, value, valueLength);
}

void BLEPeripheral::nRF51822CharacteristicSubscribedChanged(nRF51822& nRF51822, BLECharacteristic& characteristic, bool subscribed) {
  characteristic.setSubscribed(this->_central, subscribed);
}

void BLEPeripheral::nRF51822AddressReceived(nRF51822& nRF51822, const unsigned char* address) {
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

void BLEPeripheral::nRF51822TemperatureReceived(nRF51822& nRF51822, float temperature) {
}

void BLEPeripheral::nRF51822BatteryLevelReceived(nRF51822& nRF51822, float batteryLevel) {
}
#else
void BLEPeripheral::nRF8001Connected(nRF8001& nRF8001, const unsigned char* address) {
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

void BLEPeripheral::nRF8001Disconnected(nRF8001& nRF8001) {
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

void BLEPeripheral::nRF8001CharacteristicValueChanged(nRF8001& nRF8001, BLECharacteristic& characteristic, const unsigned char* value, unsigned char valueLength) {
  characteristic.setValue(this->_central, value, valueLength);
}

void BLEPeripheral::nRF8001CharacteristicSubscribedChanged(nRF8001& nRF8001, BLECharacteristic& characteristic, bool subscribed) {
  characteristic.setSubscribed(this->_central, subscribed);
}

void BLEPeripheral::nRF8001AddressReceived(nRF8001& nRF8001, const unsigned char* address) {
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

void BLEPeripheral::nRF8001TemperatureReceived(nRF8001& nRF8001, float temperature) {
}

void BLEPeripheral::nRF8001BatteryLevelReceived(nRF8001& nRF8001, float batteryLevel) {
}
#endif
