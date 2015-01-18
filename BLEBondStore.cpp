#ifdef __AVR__
  #include <avr/eeprom.h>
#else
  #warning "BLEBondStore persistent storage not supported on this platform"
#endif

#include "BLEBondStore.h"

BLEBondStore::BLEBondStore(int offset) :
  _offset(offset)
{
}

bool BLEBondStore::hasData() {
#ifdef __AVR__
  return (eeprom_read_byte((unsigned char *)this->_offset) != 0x00);
#else
  return false;
#endif
}

void BLEBondStore::clearData() {
#ifdef __AVR__
  eeprom_write_byte((unsigned char *)this->_offset, 0x00);
#endif
}

void BLEBondStore::storeData(unsigned char* data, unsigned char length) {
#ifdef __AVR__
  eeprom_write_byte((unsigned char *)this->_offset, length);

  for (unsigned char i = 0; i < length; i++) {
    eeprom_write_byte((unsigned char *)this->_offset + i + 1, data[i]);
  }
#endif
}

void BLEBondStore::restoreData(unsigned char* data, unsigned char length) {
#ifdef __AVR__
  for (unsigned char i = 0; i < length; i++) {
    data[i] = eeprom_read_byte((unsigned char *)this->_offset + i + 1);
  }
#endif
}
