#ifndef _BLE_ATTRIBUTE_H_
#define _BLE_ATTRIBUTE_H_

#define BLETypeService                          0x2800
#define BLETypeCharacteristic                   0x2803
#define BLETypeDescriptor                       0x2900

#define BLE_ATTRIBUTE_MAX_VALUE_LENGTH            19

class BLEAttribute
{
  public:
    BLEAttribute(const char* uuid, unsigned short type);

    const char* uuid();

    unsigned short type();

    static unsigned char numAttributes();

  private:
    static unsigned char _numAttributes;

    const char*          _uuid;
    unsigned short       _type;
};

#endif
