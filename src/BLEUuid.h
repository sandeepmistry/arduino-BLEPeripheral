// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef _BLE_UUID_H_
#define _BLE_UUID_H_

#define MAX_UUID_LENGTH 16

class BLEUuid
{
  public:
    BLEUuid(const char * str);

    const char* str() const;
    const unsigned char* data() const;
    unsigned char length() const;

  private:
    const char*    _str;
    unsigned char _data[MAX_UUID_LENGTH];
    unsigned char _length;
};

#endif
