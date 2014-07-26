#ifndef _BLE_UUID_H_
#define _BLE_UUID_H_

#define MAX_UUID_LENGTH 16

class BLEUuid
{
  public:
    BLEUuid(const char * str);

    const char* str();
    const unsigned char* data();
    unsigned char length();

  private:
    const char*    _str;
    unsigned char _data[MAX_UUID_LENGTH];
    unsigned char _length;
};

#endif
