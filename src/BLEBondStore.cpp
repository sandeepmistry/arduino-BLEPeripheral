// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef __AVR__
  #include <avr/eeprom.h>
#elif defined(__RFduino__)
#define FLASH_WAIT_READY { \
  while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {}; \
}
#elif defined(NRF51) || defined(NRF52)
  #include <nrf_soc.h>
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
#elif defined(NRF51) || defined(NRF52)
  int32_t pageNo = (uint32_t)_flashPageStartAddress / NRF_FICR->CODEPAGESIZE;

  while(sd_flash_page_erase(pageNo) == NRF_ERROR_BUSY);
#elif defined(__RFduino__)

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
#endif
}

void BLEBondStore::putData(const unsigned char* data, unsigned int offset, unsigned int length) {
#if defined(__AVR__) || defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
  eeprom_write_byte((unsigned char *)this->_offset, 0x01);

  for (unsigned int i = 0; i < length; i++) {
    eeprom_write_byte((unsigned char *)this->_offset + offset + i + 1, data[i]);
  }
#elif defined(NRF51) || defined(NRF52)  // ignores offset
  this->clearData();

  while (sd_flash_write((uint32_t*)_flashPageStartAddress, (uint32_t*)data, (uint32_t)length/4) == NRF_ERROR_BUSY);
#elif defined(__RFduino__) // ignores offset
  this->clearData();

  // turn on flash write enable
  NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos);

  // wait until ready
  FLASH_WAIT_READY

  uint32_t *out = this->_flashPageStartAddress;
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
