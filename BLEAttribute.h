#ifndef _BLE_ATTRIBUTE_H_
#define _BLE_ATTRIBUTE_H_

enum BLEAttributeType {
  BLETypeService        = 0x2800,
  BLETypeCharacteristic = 0x2803,
  BLETypeDescriptor     = 0x2900
};

#define BLE_ATTRIBUTE_MAX_VALUE_LENGTH            19

class BLEAttribute
{
  public:
    BLEAttribute(const char* uuid, enum BLEAttributeType type);

    const char* uuid();

    enum BLEAttributeType type();

    static unsigned char numAttributes();

  private:
    static unsigned char _numAttributes;

    const char*                 _uuid;
    enum BLEAttributeType       _type;
};

#endif
