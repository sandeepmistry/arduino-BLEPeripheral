#include "BLEAttribute.h"

BLEAttribute::BLEAttribute(const char* uuid, enum BLEAttributeType type) :
  _uuid(uuid),
  _type(type)
{
}

const char* BLEAttribute::uuid() const {
  return this->_uuid;
}

enum BLEAttributeType BLEAttribute::type() const {
  return this->_type;
}
