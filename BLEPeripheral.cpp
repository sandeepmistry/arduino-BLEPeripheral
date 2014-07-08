#include "BLEUuid.h"

#include "BLEPeripheral.h"

//#define BLE_PERIPHERAL_DEBUG

#define DEFAULT_DEVICE_NAME "Arduino"
#define DEFAULT_APPEARANCE  0x0000

BLEPeripheral::BLEPeripheral(unsigned char req, unsigned char rdy, unsigned char rst) :
  _nRF8001(req, rdy, rst),

  _localName(NULL),
  _manufacturerData(NULL),
  _manufacturerDataLength(0),

  _attributes(NULL),
  _numAttributes(0),

  _genericAccessService("1800"),
  _deviceNameCharacteristic("2a00", BLEPropertyRead, 19),
  _appearanceCharacteristic("2a01", BLEPropertyRead, 2),

  _isConnected(false),
  _connectHandler(NULL),
  _disconnectHandler(NULL)
{
  this->setDeviceName(DEFAULT_DEVICE_NAME);
  this->setAppearance(DEFAULT_APPEARANCE);

  this->_nRF8001.setEventListener(this);
}

BLEPeripheral::~BLEPeripheral() {
  if (this->_attributes) {
    free(this->_attributes);
  }
}

void BLEPeripheral::begin() {
  unsigned char advertisementData[20];
  unsigned char scanData[20];

  unsigned char advertisementDataLength = 0;
  unsigned char scanDataLength = 0;

  if (this->_advertisedServiceUuid){
    BLEUuid advertisedServiceUuid = BLEUuid(this->_advertisedServiceUuid);
    unsigned char advertisedServiceUuidLength = advertisedServiceUuid.length();

    advertisementDataLength = 2 + advertisedServiceUuidLength;

    advertisementData[0] = (advertisedServiceUuidLength > 2) ? 0x06 : 0x02;
    advertisementData[1] = advertisedServiceUuidLength;

    memcpy(&advertisementData[2], advertisedServiceUuid.data(), advertisedServiceUuidLength);
  } else if (this->_manufacturerData && this->_manufacturerDataLength > 0) {
    if (this->_manufacturerDataLength > sizeof(advertisementData)) {
      this->_manufacturerDataLength = sizeof(advertisementData);
    }

    advertisementDataLength = 2 + this->_manufacturerDataLength;

    advertisementData[0] = 0xff;
    advertisementData[1] = this->_manufacturerDataLength;
    memcpy(&advertisementData[2], this->_manufacturerData, this->_manufacturerDataLength);
  }

  if (this->_localName){
    unsigned char originalLocalNameLength = strlen(this->_localName);
    unsigned char localNameLength = originalLocalNameLength;

    if (localNameLength > sizeof(scanData)) {
      localNameLength = sizeof(scanData);
    }

    scanDataLength = 2 + localNameLength;

    scanData[0] = (originalLocalNameLength > sizeof(scanData)) ? 0x08 : 0x09;
    scanData[1] = localNameLength;

    memcpy(&scanData[2], this->_localName, localNameLength);
  }

  this->_nRF8001.begin(advertisementData, advertisementDataLength, scanData, scanDataLength, this->_attributes, this->_numAttributes);

  this->_nRF8001.requestAddress();
}

void BLEPeripheral::poll() {
  this->_nRF8001.poll();
}

void BLEPeripheral::setAdvertisedServiceUuid(const char* advertisedServiceUuid) {
  this->_advertisedServiceUuid = advertisedServiceUuid;
}

void BLEPeripheral::setManufacturerData(const unsigned char* manufacturerData, unsigned char manufacturerDataLength) {
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

    this->_numAttributes = 3;
  }

  this->_attributes[this->_numAttributes] = &attribute;
  this->_numAttributes++;
}

void BLEPeripheral::disconnect() {
  this->_nRF8001.disconnect();
}

bool BLEPeripheral::isConnected() {
  return this->_isConnected;
}

void BLEPeripheral::setConnectHandler(BLEPeripheralConnectHandler connectHandler) {
  this->_connectHandler = connectHandler;
}

void BLEPeripheral::setDisconnectHandler(BLEPeripheralDisconnectHandler disconnectHandler) {
  this->_disconnectHandler = disconnectHandler;
}

void BLEPeripheral::nRF8001Connected(nRF8001& nRF8001, const char* address) {
#ifdef BLE_PERIPHERAL_DEBUG
  Serial.print(F("Peripheral connected to central: "));
  Serial.println(address);
#endif
  this->_isConnected = true;

  if (this->_connectHandler) {
    this->_connectHandler(address);
  }
}

void BLEPeripheral::nRF8001Disconnected(nRF8001& nRF8001) {
#ifdef BLE_PERIPHERAL_DEBUG
  Serial.println(F("Peripheral disconnected from central"));
#endif
  this->_isConnected = false;

  if (this->_disconnectHandler) {
    this->_disconnectHandler();
  }
}

void BLEPeripheral::nRF8001AddressReceived(nRF8001& nRF8001, const char* address) {
#ifdef BLE_PERIPHERAL_DEBUG
  Serial.print(F("Peripheral address: "));
  Serial.println(address);
#endif
}

void BLEPeripheral::nRF8001TemperatureReceived(nRF8001& nRF8001, float temperature) {
}

void BLEPeripheral::nRF8001BatteryLevelReceived(nRF8001& nRF8001, float batteryLevel) {
}
