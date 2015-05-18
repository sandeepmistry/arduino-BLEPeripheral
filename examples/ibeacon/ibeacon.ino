#include <BLEPeripheral.h>
#include <iBeacon.h>

#if !defined(NRF51) && !defined(__RFduino__)
#error "This example only works with nRF51 boards"
#endif

static BLEPeripheral blePeripheral(0, 0, 0);

void setup() {
  char* uuid = "a196c876-de8c-4c47-ab5a-d7afd5ae7127";
  uint16_t major = 0;
  uint16_t minor = 0;
  int8_t measuredPower = -55;
  
  iBeacon::setData(blePeripheral, uuid, major, minor, measuredPower);

  blePeripheral.begin();
}

void loop() {
  blePeripheral.poll();
}
