#if defined(NRF51) || defined(__RFduino__)

#include <BLEUuid.h>

#include "iBeacon.h"

void iBeacon::setData(BLEPeripheral& peripheral, const char* uuidString, uint16_t major, uint16_t minor, int8_t measuredPower) {
  unsigned char manufacturerData[MAX_UUID_LENGTH + 9]; // 4 bytes of header and 5 bytes of trailer.
  BLEUuid uuid(uuidString);
  int i = 0;

  // 0x004c = Apple, see https://www.bluetooth.org/en-us/specification/assigned-numbers/company-identifiers
  manufacturerData[i++] = 0x4c; // Apple Company Identifier LE (16 bit)
  manufacturerData[i++] = 0x00;
  
  // See "Beacon type" in "Building Applications with IBeacon".
  manufacturerData[i++] = 0x02;
  manufacturerData[i++] = uuid.length() + 5;

  for (int j = (uuid.length() - 1); j >= 0; j--) {
    manufacturerData[i++] = uuid.data()[j];
  }

  manufacturerData[i++] = major >> 8;
  manufacturerData[i++] = major;
  manufacturerData[i++] = minor >> 8;
  manufacturerData[i++] = minor;
  manufacturerData[i++] = measuredPower;

  peripheral.setManufacturerData(manufacturerData, i);
}

#endif
