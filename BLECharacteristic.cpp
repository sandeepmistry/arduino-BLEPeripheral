#include "BLEPeripheral.h"

#include "BLECharacteristic.h"

extern struct aci_state_t aci_state;

BLECharacteristic::BLECharacteristic(const char* uuid, char properties, unsigned int valueSize)
  : BLEAttribute(uuid, BLE_TYPE_CHARACTERISTIC)
{
  this->_properties = properties;
  this->_valueSize = min(valueSize, BLE_CHARACTERISTIC_MAX_VALUE_LENGTH);
  this->_valueLength = 0;

  this->_valueHandle = 0;
  this->_configHandle = 0;

  this->_pipeStart = 0;
  this->_txPipe = 0;
  this->_txAckPipe = 0;
  this->_rxPipe = 0;
  this->_rxAckPipe = 0;
  this->_setPipe = 0;

  this->_valueUpdated = false;
  this->_isNotifySubscribed = false;
  this->_isIndicateSubscribed = false;
}

char BLECharacteristic::properties() {
  return this->_properties;
}

unsigned int BLECharacteristic::valueSize() {
  return this->_valueSize;
}

char* BLECharacteristic::value() {
  return this->_value;
}

unsigned int BLECharacteristic::valueLength() {
  return this->_valueLength;
}

void BLECharacteristic::setValue(char value[], unsigned int length) {
  this->_valueLength = min(length, this->_valueSize);

  memcpy(this->_value, value, this->_valueLength);

  if (this->_setPipe) {
    BLEPeripheral::instance()->setLocalData(this->_setPipe, value, length);
  }

  if (this->_txPipe && this->isNotifySubscribed()) {
    BLEPeripheral::instance()->sendData(this->_txPipe, value, length);
  }

  if (this->_txAckPipe && this->isIndicateSubscribed()) {
    BLEPeripheral::instance()->sendData(this->_txAckPipe, value, length);
  }
}

unsigned short BLECharacteristic::valueHandle() {
  return this->_valueHandle;
}

void BLECharacteristic::setValueHandle(unsigned short valueHandle) {
  this->_valueHandle = valueHandle;
}

unsigned short BLECharacteristic::configHandle() {
  return this->_configHandle;
}

void BLECharacteristic::setConfigHandle(unsigned short configHandle) {
  this->_configHandle = configHandle;
}

char BLECharacteristic::pipeStart() {
  return this->_pipeStart;
}

void BLECharacteristic::setPipeStart(char pipeStart) {
  this->_pipeStart = pipeStart;
}

char BLECharacteristic::txPipe() {
  return this->_txPipe;
}

void BLECharacteristic::setTxPipe(char txPipe) {
  this->_txPipe = txPipe;
}

char BLECharacteristic::txAckPipe() {
  return this->_txAckPipe;
}

void BLECharacteristic::setTxAckPipe(char txAckPipe) {
  this->_txAckPipe = txAckPipe;
}

char BLECharacteristic::setPipe() {
  return this->_setPipe;
}

void BLECharacteristic::setSetPipe(char setPipe) {
  this->_setPipe = setPipe;
}

char BLECharacteristic::rxPipe() {
  return this->_rxPipe;
}

void BLECharacteristic::setRxPipe(char rxPipe) {
  this->_rxPipe = rxPipe;
}

char BLECharacteristic::rxAckPipe() {
  return this->_rxAckPipe;
}

void BLECharacteristic::setRxAckPipe(char rxAckPipe) {
  this->_rxAckPipe = rxAckPipe;
}

bool BLECharacteristic::valueUpdated() {
  bool valueUpdated = this->_valueUpdated;

  if (valueUpdated) {
    this->_valueUpdated = false;
  }

  return valueUpdated;
}

void BLECharacteristic::setValueUpdated(bool valueUpdated) {
  this->_valueUpdated = valueUpdated;
}

bool BLECharacteristic::isNotifySubscribed() {
  return this->_isNotifySubscribed;
}

void BLECharacteristic::setIsNotifySubscribed(bool isNotifySubscribed) {
  this->_isNotifySubscribed = isNotifySubscribed;
}

bool BLECharacteristic::isIndicateSubscribed() {
  return this->_isIndicateSubscribed;
}

void BLECharacteristic::setIsIndicateSubscribed(bool isIndicateSubscribed) {
  this->_isIndicateSubscribed = isIndicateSubscribed;
}
