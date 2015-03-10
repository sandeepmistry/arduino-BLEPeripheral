#include "BLEProgmemConstantCharacteristic.h"

BLEProgmemConstantCharacteristic::BLEProgmemConstantCharacteristic(const char* uuid, const unsigned char value[], unsigned char length) :
  BLEConstantCharacteristic(uuid, value, length)
{
}

BLEProgmemConstantCharacteristic::BLEProgmemConstantCharacteristic(const char* uuid, const char* value) :
#if defined(NRF51) || defined(__RFduino__)
  BLEConstantCharacteristic(uuid, value)
#else
  BLEConstantCharacteristic(uuid, (const unsigned char *)value, strlen_PF((uint_farptr_t)value))
#endif
{
}

BLEProgmemConstantCharacteristic::~BLEProgmemConstantCharacteristic() {
}

unsigned char BLEProgmemConstantCharacteristic::operator[] (int offset) const {
  return pgm_read_byte_near(&this->_value[offset]);
}
