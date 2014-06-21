#include "BLEAttribute.h"

BLEAttribute::BLEAttribute(const char* uuid, unsigned short type)
{
  this->_uuid = uuid;
  this->_type = type;

  this->_handle = 0x0000;
}

const char* BLEAttribute::uuid() {
  return this->_uuid;
}

unsigned short BLEAttribute::type() {
  return this->_type;
}

unsigned short BLEAttribute::handle() {
  return this->_handle;
}

void BLEAttribute::setHandle(unsigned short handle) {
  this->_handle = handle;
}
