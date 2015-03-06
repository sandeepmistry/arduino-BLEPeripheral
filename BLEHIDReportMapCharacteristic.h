#ifndef _BLE_HID_REPORT_MAP_CHARACTERISTIC_H_
#define _BLE_HID_REPORT_MAP_CHARACTERISTIC_H_

#include "BLEHIDDevice.h"
#include "BLEConstantCharacteristic.h"

class BLEHIDReportMapCharacteristic : public BLEConstantCharacteristic
{
  friend class BLEHIDDevice;

  public:
    BLEHIDReportMapCharacteristic();

    virtual unsigned char valueSize() const;
    virtual unsigned char valueLength() const;
    virtual unsigned char operator[] (int offset) const;

    void setDevices(BLEHIDDevice** devices, unsigned char numDevices);

  private:
    BLEHIDDevice**                  _devices;
    unsigned char                   _numDevices;
};

#endif
