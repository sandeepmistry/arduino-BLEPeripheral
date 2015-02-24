#include "BLELocalAttribute.h"

unsigned char BLELocalAttribute::_numAttributes = 0;

BLELocalAttribute::BLELocalAttribute(const char* uuid, enum BLEAttributeType type) :
  BLEAttribute(uuid, type)
{
  _numAttributes++;
}

unsigned char BLELocalAttribute::numAttributes() {
  return _numAttributes;
}
