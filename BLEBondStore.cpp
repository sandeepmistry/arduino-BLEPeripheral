#ifdef __AVR__
  #include <avr/eeprom.h>
#else
  #warning "BLEBondStore persistent storage not supported on this platform"
#endif

#include "Arduino.h"

#include "BLEBondStore.h"

BLEBondStore::BLEBondStore(int offset) :
#ifndef __AVR__
    _dataLength(0),
    _data(NULL),
#endif
  _offset(offset)
{
}

BLEBondStore::~BLEBondStore() {
#ifndef __AVR__
  this->clearData();
#endif
}

bool BLEBondStore::hasData() {
#ifdef __AVR__
  return (eeprom_read_byte((unsigned char *)this->_offset) != 0x00);
#else
  return (this->_dataLength > 0);
#endif
}

void BLEBondStore::clearData() {
#ifdef __AVR__
  eeprom_write_byte((unsigned char *)this->_offset, 0x00);
#else
  this->_dataLength = 0;
  if (this->_data) {
    free(this->_data);

    this->_data = NULL;
  }
#endif
}

void BLEBondStore::storeData(const unsigned char* data, unsigned char length) {
#ifdef __AVR__
  eeprom_write_byte((unsigned char *)this->_offset, length);

  for (unsigned char i = 0; i < length; i++) {
    eeprom_write_byte((unsigned char *)this->_offset + i + 1, data[i]);
  }
#else
  this->_data = (unsigned char *)malloc(length);

  memcpy(this->_data, data, length);

  this->_dataLength = length;
#endif
}

void BLEBondStore::restoreData(unsigned char* data, unsigned char length) {
#ifdef __AVR__
  for (unsigned char i = 0; i < length; i++) {
    data[i] = eeprom_read_byte((unsigned char *)this->_offset + i + 1);
  }
#else
  memcpy(data, this->_data, length);
#endif
}
