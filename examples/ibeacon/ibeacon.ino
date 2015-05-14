#include <BLEPeripheral.h>
#include <BLEUuid.h>

#ifndef NRF51
#error "This example only works with nRF51 boards"
#endif

static BLEPeripheral blePeripheral(0, 0, 0);

void setup() {
  char* uuidString = "a196c876-de8c-4c47-ab5a-d7afd5ae7127";
  uint16_t major = 0;
  uint16_t minor = 0;
  int8_t measuredPower = -55;
  
  blePeripheral.setIBeaconData(uuidString, major, minor, measuredPower);

  blePeripheral.begin();
}

void loop() {
  blePeripheral.poll();
}
