// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#ifdef __AVR__
  #include <avr/eeprom.h>
#elif defined(NRF51) || defined(NRF52) || defined(__RFduino__)
  #include "nrf_soc.h"
#else
  #warning "BLEBondStore persistent storage not supported on this platform"
#endif

#include "Arduino.h"

#include "BLEBondStore.h"

BLEBondStore::BLEBondStore(int offset)
#if defined(__AVR__) || defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
  : _offset(offset)
#elif defined(NRF51) || defined(NRF52) || defined(__RFduino__)
  : _flashPageStartAddress((uint32_t *)(NRF_FICR->CODEPAGESIZE * (NRF_FICR->CODESIZE - 1 - (uint32_t)offset)))
#endif
{
}

bool BLEBondStore::hasData() {
#if defined(__AVR__) || defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
  return (eeprom_read_byte((unsigned char *)this->_offset) == 0x01);
#elif defined(NRF51) || defined(NRF52) || defined(__RFduino__)
  return (*this->_flashPageStartAddress != 0xFFFFFFFF);
#else
  return false;
#endif
}

void BLEBondStore::clearData() {
#if defined(__AVR__) || defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
  eeprom_write_byte((unsigned char *)this->_offset, 0x00);
#elif defined(NRF51) || defined(NRF52) || defined(__RFduino__)

  int32_t pageNo = (uint32_t)_flashPageStartAddress/NRF_FICR->CODEPAGESIZE;
  uint32_t err_code;
  do {
	err_code = sd_flash_page_erase(pageNo);
  } while(err_code == NRF_ERROR_BUSY);
#endif
}

void BLEBondStore::putData(const unsigned char* data, unsigned int offset, unsigned int length) {
#if defined(__AVR__) || defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
  eeprom_write_byte((unsigned char *)this->_offset, 0x01);

  for (unsigned int i = 0; i < length; i++) {
    eeprom_write_byte((unsigned char *)this->_offset + offset + i + 1, data[i]);
  }
#elif defined(NRF51) || defined(NRF52) || defined(__RFduino__) // ignores offset
  this->clearData();

  uint32_t err_code;
  do {
	  err_code = sd_flash_write((uint32_t*)_flashPageStartAddress, (uint32_t*)data, (uint32_t)length/4);
  } while(err_code == NRF_ERROR_BUSY);
#endif
}

void BLEBondStore::getData(unsigned char* data, unsigned int offset, unsigned int length) {
#if defined(__AVR__) || defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
  for (unsigned int i = 0; i < length; i++) {
    data[i] = eeprom_read_byte((unsigned char *)this->_offset + offset + i + 1);
  }
#elif defined(NRF51) || defined(NRF52) || defined(__RFduino__) // ignores offset
  uint32_t *in = this->_flashPageStartAddress;
  uint32_t *out  = (uint32_t*)data;

  offset = offset;

  for(unsigned int i = 0; i < length; i += 4) { // assumes length is multiple of 4
    *out = *in;

    out++;
    in++;
  }
#endif
}
