#ifndef _BLE_DESCRIPTOR_H_
#define _BLE_DESCRIPTOR_H_

#include "BLEAttribute.h"

class BLEDescriptor : public BLEAttribute
{
  public:
    BLEDescriptor(const char* uuid, unsigned char valueSize);
    BLEDescriptor(const char* uuid, const char* value);

    virtual ~BLEDescriptor();

    unsigned char valueSize();

    const unsigned char* value();
    unsigned char valueLength();
    void setValue(const unsigned char value[], unsigned char length);

    void setValue(const char* value);

  private:
    const char*     _uuid;
    unsigned char   _valueSize;
    unsigned char*  _value;
    unsigned char   _valueLength;
};

#endif
