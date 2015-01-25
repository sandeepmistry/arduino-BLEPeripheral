#ifndef _BLE_DESCRIPTOR_H_
#define _BLE_DESCRIPTOR_H_

#include "BLEAttribute.h"

class BLEDescriptor : public BLEAttribute
{
  public:
    BLEDescriptor(const char* uuid, const unsigned char value[], unsigned char valueLength);
    BLEDescriptor(const char* uuid, const char* value);

    virtual ~BLEDescriptor();

    const unsigned char* value() const;
    unsigned char valueLength() const;

  private:
    const char*           _uuid;
    const unsigned char*  _value;
    unsigned char         _valueLength;
};

#endif
