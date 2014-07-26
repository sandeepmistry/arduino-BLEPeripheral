#include "BLEAttribute.h"

unsigned char BLEAttribute::_numAttributes = 0;

BLEAttribute::BLEAttribute(const char* uuid, enum BLEAttributeType type) :
  _uuid(uuid),
  _type(type)
{
  _numAttributes++;
}

const char* BLEAttribute::uuid() const {
  return this->_uuid;
}

enum BLEAttributeType BLEAttribute::type() const {
  return this->_type;
}

unsigned char BLEAttribute::numAttributes() {
  return _numAttributes;
}
