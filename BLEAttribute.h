#ifndef _BLE_ATTRIBUTE_H_
#define _BLE_ATTRIBUTE_H_

#define BLE_TYPE_SERVICE                          0x2800
#define BLE_TYPE_CHARACTERISTIC                   0x2803
#define BLE_TYPE_DESCRIPTOR                       0x2900

#define BLE_PROPERTY_READ                         0x02
#define BLE_PROPERTY_WRITE_WITHOUT_RESPONSE       0x04
#define BLE_PROPERTY_WRITE                        0x08
#define BLE_PROPERTY_NOTIFY                       0x10
#define BLE_PROPERTY_INDICATE                     0x20


class BLEAttribute
{
  public:
    BLEAttribute(const char* uuid, unsigned short type);

    const char* uuid();

    unsigned short type();

    unsigned short handle();
    void setHandle(unsigned short handle);

  private:
    const char*    _uuid;
    unsigned short _type;
    unsigned short _handle;
};

#endif
