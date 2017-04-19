// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "BLEUuid.h"

#include "BLEDeviceLimits.h"
#include "BLEUtil.h"

#include "BLEPeripheral.h"

//#define BLE_PERIPHERAL_DEBUG

#define DEFAULT_DEVICE_NAME "Arduino"
#define DEFAULT_APPEARANCE  0x0000

BLEPeripheral::BLEPeripheral(unsigned char req, unsigned char rdy, unsigned char rst) :
#if defined(NRF51) || defined(NRF52) || defined(__RFduino__)
  _nRF51822(),
#else
  _nRF8001(req, rdy, rst),
#endif

  _localAttributes(NULL),
  _numLocalAttributes(0),
  _remoteAttributes(NULL),
  _numRemoteAttributes(0),

  _genericAccessService("1800"),
  _deviceNameCharacteristic("2a00", BLERead, 19),
  _appearanceCharacteristic("2a01", BLERead, 2),
  _genericAttributeService("1801"),
  _servicesChangedCharacteristic("2a05", BLEIndicate, 4),

  _remoteGenericAttributeService("1801"),
  _remoteServicesChangedCharacteristic("2a05", BLEIndicate),

  _central(this)
{
#if defined(NRF51) || defined(NRF52) || defined(__RFduino__)
  this->_device = &this->_nRF51822;
#else
  this->_device = &this->_nRF8001;
#endif

  memset(this->_eventHandlers, 0x00, sizeof(this->_eventHandlers));
  memset(this->_advData, 0x00, 32);
  memset(this->_scanData, 0x00, 32);
  
  this->_advData[0] = 2;
  this->_advData[1] = 0x01;
  this->_advData[2] = 0x06;
  
  this->_advDataLength = 3;
  
  this->setDeviceName(DEFAULT_DEVICE_NAME);
  this->setAppearance(DEFAULT_APPEARANCE);

  this->_device->setEventListener(this);
}

BLEPeripheral::~BLEPeripheral() {
  this->end();

  if (this->_remoteAttributes) {
    free(this->_remoteAttributes);
  }

  if (this->_localAttributes) {
    free(this->_localAttributes);
  }
}

void BLEPeripheral::begin() {
  if (this->_localAttributes == NULL) {
    this->initLocalAttributes();
  }

  for (int i = 0; i < this->_numLocalAttributes; i++) {
    BLELocalAttribute* localAttribute = this->_localAttributes[i];
    if (localAttribute->type() == BLETypeCharacteristic) {
      BLECharacteristic* characteristic = (BLECharacteristic*)localAttribute;

      characteristic->setValueChangeListener(*this);
    }
  }

  for (int i = 0; i < this->_numRemoteAttributes; i++) {
    BLERemoteAttribute* remoteAttribute = this->_remoteAttributes[i];
    if (remoteAttribute->type() == BLETypeCharacteristic) {
      BLERemoteCharacteristic* remoteCharacteristic = (BLERemoteCharacteristic*)remoteAttribute;

      remoteCharacteristic->setValueChangeListener(*this);
    }
  }

  if (this->_numRemoteAttributes) {
    this->addRemoteAttribute(this->_remoteGenericAttributeService);
    this->addRemoteAttribute(this->_remoteServicesChangedCharacteristic);
  }

  this->_device->begin((unsigned char)this->_advDataLength, (unsigned char *)this->_advData,
                        (unsigned char)this->_scanDataLength, (unsigned char *)this->_scanData,
                        this->_localAttributes, this->_numLocalAttributes,
                        this->_remoteAttributes, this->_numRemoteAttributes);

  this->_device->requestAddress();
}

void BLEPeripheral::poll() {
  this->_device->poll();
}

void BLEPeripheral::end() {
  this->_device->end();
}

void BLEPeripheral::setAdvertisedServiceUuid(const char* advertisedServiceUuid) {
  BLEUuid advServiceUuid = BLEUuid(advertisedServiceUuid);
  unsigned char uuidLength = advServiceUuid.length();
  uint8_t type = uuidLength > 2 ? 0x06 : 0x02;
	
  addFieldInPck(type, uuidLength, (unsigned char *)advServiceUuid.data());
}

void BLEPeripheral::setServiceSolicitationUuid(const char* serviceSolicitationUuid) {
  BLEUuid sSolUuid = BLEUuid(serviceSolicitationUuid);

  uint8_t len = sSolUuid.length();
  uint8_t type = (len > 2) ? 0x15 : 0x14;
 
  addFieldInPck(type, len, (unsigned char *)sSolUuid.data());
}

void BLEPeripheral::setManufacturerData(const unsigned char manufacturerData[], unsigned char manufacturerDataLength) {
  addFieldInPck(0xff, manufacturerDataLength, (unsigned char *) manufacturerData);
}

void BLEPeripheral::setLocalName(const char* localName) {
  uint8_t len = strlen(localName);
  uint8_t type = 0x09;
  if(len > BLE_SCAN_DATA_MAX_VALUE_LENGTH){
    len = BLE_SCAN_DATA_MAX_VALUE_LENGTH;
    type = 0x08;
  }
  addFieldInPck(type, len, (unsigned char *)localName);
}

void BLEPeripheral::setConnectable(bool connectable) {
  this->_device->setConnectable(connectable);
}

bool  BLEPeripheral::setTxPower(int txPower) {
  addFieldInPck(0x0A, 1, (unsigned char *)&txPower);
  return this->_device->setTxPower(txPower);
}

void BLEPeripheral::setBondStore(BLEBondStore& bondStore) {
  this->_device->setBondStore(bondStore);
}

void BLEPeripheral::setDeviceName(const char* deviceName) {
  this->_deviceNameCharacteristic.setValue(deviceName);
}

void BLEPeripheral::setAppearance(unsigned short appearance) {
  addFieldInPck(0x19, 2, (unsigned char *)&appearance);
  this->_appearanceCharacteristic.setValue((unsigned char *)&appearance, sizeof(appearance));
}

void BLEPeripheral::addAttribute(BLELocalAttribute& attribute) {
  this->addLocalAttribute(attribute);
}

void BLEPeripheral::addLocalAttribute(BLELocalAttribute& localAttribute) {
  if (this->_localAttributes == NULL) {
    this->initLocalAttributes();
  }

  this->_localAttributes[this->_numLocalAttributes] = &localAttribute;
  this->_numLocalAttributes++;
}

void BLEPeripheral::addRemoteAttribute(BLERemoteAttribute& remoteAttribute) {
  if (this->_remoteAttributes == NULL) {
    this->_remoteAttributes = (BLERemoteAttribute**)malloc(BLERemoteAttribute::numAttributes() * sizeof(BLERemoteAttribute*));
  }

  this->_remoteAttributes[this->_numRemoteAttributes] = &remoteAttribute;
  this->_numRemoteAttributes++;
}

void BLEPeripheral::setAdvertisingInterval(unsigned short advertisingInterval) {
  this->_device->setAdvertisingInterval(advertisingInterval);
}

void BLEPeripheral::setConnectionInterval(unsigned short minimumConnectionInterval, unsigned short maximumConnectionInterval) {
  this->_device->setConnectionInterval(minimumConnectionInterval, maximumConnectionInterval);
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

bool BLEPeripheral::canReadRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  return this->_device->canReadRemoteCharacteristic(characteristic);
}

bool BLEPeripheral::readRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  return this->_device->readRemoteCharacteristic(characteristic);
}

bool BLEPeripheral::canWriteRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  return this->_device->canWriteRemoteCharacteristic(characteristic);
}

bool BLEPeripheral::writeRemoteCharacteristic(BLERemoteCharacteristic& characteristic, const unsigned char value[], unsigned char length) {
  return this->_device->writeRemoteCharacteristic(characteristic, value, length);
}

bool BLEPeripheral::canSubscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  return this->_device->canSubscribeRemoteCharacteristic(characteristic);
}

bool BLEPeripheral::subscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  return this->_device->subscribeRemoteCharacteristic(characteristic);
}

bool BLEPeripheral::canUnsubscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  return this->_device->canUnsubscribeRemoteCharacteristic(characteristic);
}

bool BLEPeripheral::unsubcribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  return this->_device->unsubcribeRemoteCharacteristic(characteristic);
}

void BLEPeripheral::BLEDeviceConnected(BLEDevice& /*device*/, const unsigned char* address) {
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

void BLEPeripheral::BLEDeviceDisconnected(BLEDevice& /*device*/) {
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

void BLEPeripheral::BLEDeviceBonded(BLEDevice& /*device*/) {
#ifdef BLE_PERIPHERAL_DEBUG
  Serial.print(F("Peripheral bonded: "));
  Serial.println(this->_central.address());
#endif

  BLEPeripheralEventHandler eventHandler = this->_eventHandlers[BLEBonded];
  if (eventHandler) {
    eventHandler(this->_central);
  }
}

void BLEPeripheral::BLEDeviceRemoteServicesDiscovered(BLEDevice& /*device*/) {
#ifdef BLE_PERIPHERAL_DEBUG
  Serial.print(F("Peripheral discovered central remote services: "));
  Serial.println(this->_central.address());
#endif

  BLEPeripheralEventHandler eventHandler = this->_eventHandlers[BLERemoteServicesDiscovered];
  if (eventHandler) {
    eventHandler(this->_central);
  }
}

void BLEPeripheral::BLEDeviceCharacteristicValueChanged(BLEDevice& /*device*/, BLECharacteristic& characteristic, const unsigned char* value, unsigned char valueLength) {
  characteristic.setValue(this->_central, value, valueLength);
}

void BLEPeripheral::BLEDeviceCharacteristicSubscribedChanged(BLEDevice& /*device*/, BLECharacteristic& characteristic, bool subscribed) {
  characteristic.setSubscribed(this->_central, subscribed);
}

void BLEPeripheral::BLEDeviceRemoteCharacteristicValueChanged(BLEDevice& /*device*/, BLERemoteCharacteristic& remoteCharacteristic, const unsigned char* value, unsigned char valueLength) {
  remoteCharacteristic.setValue(this->_central, value, valueLength);
}

void BLEPeripheral::BLEDeviceAddressReceived(BLEDevice& /*device*/, const unsigned char* /*address*/) {
#ifdef BLE_PERIPHERAL_DEBUG
  char addressStr[18];

  BLEUtil::addressToString(address, addressStr);

  Serial.print(F("Peripheral address: "));
  Serial.println(addressStr);
#endif
}

void BLEPeripheral::BLEDeviceTemperatureReceived(BLEDevice& /*device*/, float /*temperature*/) {
}

void BLEPeripheral::BLEDeviceBatteryLevelReceived(BLEDevice& /*device*/, float /*batteryLevel*/) {
}

void BLEPeripheral::initLocalAttributes() {
  this->_localAttributes = (BLELocalAttribute**)malloc(BLELocalAttribute::numAttributes() * sizeof(BLELocalAttribute*));

  this->_localAttributes[0] = &this->_genericAccessService;
  this->_localAttributes[1] = &this->_deviceNameCharacteristic;
  this->_localAttributes[2] = &this->_appearanceCharacteristic;

  this->_localAttributes[3] = &this->_genericAttributeService;
  this->_localAttributes[4] = &this->_servicesChangedCharacteristic;

  this->_numLocalAttributes = 5;
}

void BLEPeripheral::addFieldInPck(uint8_t type, uint8_t len, unsigned char* data){
	uint8_t *packet;
	uint8_t *pckLen;
	bool inPck = false;
	uint8_t pos = 0;

	//check if there is room enough in one of the two packets
	if(this->_advDataLength < BLE_ADVERTISEMENT_DATA_MAX_VALUE_LENGTH + 2 && ((BLE_ADVERTISEMENT_DATA_MAX_VALUE_LENGTH + 2) - this->_advDataLength) >= len){
		packet = (uint8_t *)_advData;
		pckLen = &_advDataLength;
	}
	else if(this->_scanDataLength < BLE_SCAN_DATA_MAX_VALUE_LENGTH && (BLE_SCAN_DATA_MAX_VALUE_LENGTH - this->_scanDataLength) >= len){
		packet = (uint8_t *)_scanData;
		pckLen = &_scanDataLength;
	}
	// if the room is not enough return
	else
		return;
	
	//check if the field is already in the packet
	for(pos = 0; pos < BLE_GAP_ADV_MAX_SIZE && packet[pos] != 0; pos += packet[pos] + 1)
		if(packet[pos+1] == type){
			inPck = true;
			break;
		}
	if(inPck){
		//in case of fixed length types, do a simple update of the existing values
		if(type == 0x0A || type == 0x19)
			memcpy(&packet[pos + 2], data, len);
		//otherwise append the value at the end of the list
		else{
			//move fields if necessary to make room for the extension
			for(int i = *pckLen + len - 1; i >= (pos + packet[pos] + 1 + len); i--){
				packet[i] = packet[i - len];
			}
			//copy the new data into the field and update the length byte
			memcpy(&packet[pos + packet[pos] + 1], data, len);
			packet[pos] += len;
			*pckLen += len;
		}
	}
	else{
		//add the packet
		packet[*pckLen] = len + 1;
		packet[*pckLen + 1] = type;
		
		memcpy(&packet[*pckLen + 2], data, len);
		
		*pckLen += len + 2;
	}
}