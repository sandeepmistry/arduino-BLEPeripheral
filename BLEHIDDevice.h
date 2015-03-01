#ifndef _BLE_HID_DEVICE_H_
#define _BLE_HID_DEVICE_H_

#include "BLELocalAttribute.h"
#include "BLECharacteristic.h"

class BLEHIDDevice
{
  friend class BLEHID;

  public:
    BLEHIDDevice();

  protected:
    static unsigned char numDevices();

    void sendData(BLECharacteristic& characteristic, unsigned char data[], unsigned char dataLength);

    virtual unsigned char numAttributes() = 0;
    virtual BLELocalAttribute** attributes() = 0;

  private:
    static unsigned char _numDevices;
};

#endif
