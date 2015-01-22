#ifdef __AVR__
  #include <avr/eeprom.h>
#else
  #warning "BLEBondStore persistent storage not supported on this platform"
#endif

#include "Arduino.h"

#include "BLEBondStore.h"

#if defined(NRF51) || defined(__RFduino__)
#define FLASH_WAIT_READY { \
  while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {}; \
}
#endif

BLEBondStore::BLEBondStore(int offset) :
#ifdef __AVR__
  _offset(offset)
#elif defined(NRF51) || defined(__RFduino__)
  _flashPageStartAddress((uint32_t *)(NRF_FICR->CODEPAGESIZE * (NRF_FICR->CODESIZE - 1 - (uint32_t)offset)))
#else
  _dataLength(0),
  _data(NULL)
#endif
{
}

BLEBondStore::~BLEBondStore() {
#if !defined(__AVR__) && !defined(NRF51) && !defined(__RFduino__)
  this->clearData();
#endif
}

bool BLEBondStore::hasData() {
#ifdef __AVR__
  return (eeprom_read_byte((unsigned char *)this->_offset) != 0x00);
#elif defined(NRF51) || defined(__RFduino__)
  return (*this->_flashPageStartAddress != 0xFFFFFFFF);
#else
  return (this->_dataLength > 0);
#endif
}

void BLEBondStore::clearData() {
#ifdef __AVR__
  eeprom_write_byte((unsigned char *)this->_offset, 0x00);
#elif defined(NRF51) || defined(__RFduino__)
  // turn on flash erase enable
  NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Een << NVMC_CONFIG_WEN_Pos);

  // wait until ready
  FLASH_WAIT_READY

  // erase page
  NRF_NVMC->ERASEPAGE = (uint32_t)this->_flashPageStartAddress;

  // wait until ready
  FLASH_WAIT_READY

  // turn off flash erase enable
  NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);

  // wait until ready
  FLASH_WAIT_READY
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
#elif defined(NRF51) || defined(__RFduino__)
  this->clearData();

  // turn on flash write enable
  NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos);

  // wait until ready
  FLASH_WAIT_READY

  *this->_flashPageStartAddress = length;

  uint32_t *out = (this->_flashPageStartAddress + 1);
  uint32_t *in  = (uint32_t*)data;

  for(unsigned char i = 0; i < length; i += 4) { // assumes length is multiple of 4
    *out = *in;

    out++;
    in++;
  }

  // wait until ready
  FLASH_WAIT_READY

  // turn off flash write enable
  NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);

  // wait until ready
  FLASH_WAIT_READY
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
#elif defined(NRF51) || defined(__RFduino__)
  uint32_t *in = (this->_flashPageStartAddress + 1);
  uint32_t *out  = (uint32_t*)data;

  for(int i = 0; i < length; i += 4) { // assumes length is multiple of 4
    *out = *in;

    out++;
    in++;
  }
#else
  memcpy(data, this->_data, length);
#endif
}
