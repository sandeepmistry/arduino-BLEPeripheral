#ifndef _BLE_REMOTE_ATTRIBUTE_H_
#define _BLE_REMOTE_ATTRIBUTE_H_

#include "BLEAttribute.h"

class BLERemoteAttribute : public BLEAttribute
{
  friend class BLEPeripheral;

  public:
    BLERemoteAttribute(const char* uuid, enum BLEAttributeType type);

  protected:
    static unsigned char numAttributes();

  private:
    static unsigned char _numAttributes;
};

#endif
