#ifndef _BLE_UTIL_H_
#define _BLE_UTIL_H_


class BLEUtil
{
  public:
    static void addressToString(const unsigned char *in, char* out);

    static void printBuffer(const unsigned char* buffer, unsigned char length);
};

#endif
