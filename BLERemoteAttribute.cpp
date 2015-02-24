#include "BLERemoteAttribute.h"

unsigned char BLERemoteAttribute::_numAttributes = 0;

BLERemoteAttribute::BLERemoteAttribute(const char* uuid, enum BLEAttributeType type) :
  BLEAttribute(uuid, type)
{
  _numAttributes++;
}

unsigned char BLERemoteAttribute::numAttributes() {
  return _numAttributes;
}
