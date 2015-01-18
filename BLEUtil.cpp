#include "Arduino.h"

#include "BLEUtil.h"

void BLEUtil::addressToString(const unsigned char *in, char* out) {
  sprintf(out, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
    in[5],
    in[4],
    in[3],
    in[2],
    in[1],
    in[0]);
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

