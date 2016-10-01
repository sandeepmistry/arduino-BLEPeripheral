// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "Arduino.h"

#include "BLEUuid.h"

BLEUuid::BLEUuid(const char * str) :
  _str(str)
{
  char temp[] = {0, 0, 0};

  this->_length = 0;
  for (int i = strlen(str) - 1; i >= 0 && this->_length < MAX_UUID_LENGTH; i -= 2) {
    if (str[i] == '-') {
      i++;
      continue;
    }

    temp[0] = str[i - 1];
    temp[1] = str[i];

    this->_data[this->_length] = strtoul(temp, NULL, 16);

    this->_length++;
  }
}

const char* BLEUuid::str() const {
  return this->_str;
}

const unsigned char* BLEUuid::data() const {
  return this->_data;
}

unsigned char BLEUuid::length() const {
  return this->_length;
}
