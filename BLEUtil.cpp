#include "Arduino.h"

#include "BLEUtil.h"

void BLEUtil::addressToString(const unsigned char *in, char* out) {
  String address = "";

  for (int i = 5; i >= 0; i--) {
    if (in[i] < 0x10) {
      address += "0";
    }

    address += String(in[i], 16);

    if (i > 0) {
      address += ":";
    }
  }

  address.toCharArray(out, 18);
}

void BLEUtil::printBuffer(const unsigned char* buffer, unsigned char length) {
  for (int i = 0; i < length; i++) {
    if ((buffer[i] & 0xf0) == 00) {
      Serial.print("0");
    }

    Serial.print(buffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

