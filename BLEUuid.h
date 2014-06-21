#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#ifndef _BLE_UUID_H_
#define _BLE_UUID_H_

#define MAX_UUID_LENGTH 16

class BLEUuid
{

  public:
    BLEUuid(const char * str);
  
    const char* str();
    char* data();
    unsigned int length();

  private:
    const char* _str;
    char _data[MAX_UUID_LENGTH];
    unsigned int _length;
};

#endif
