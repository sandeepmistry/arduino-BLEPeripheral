#ifndef _BLE_HID_DEVICE_H_
#define _BLE_HID_DEVICE_H_

#include "BLELocalAttribute.h"
#include "BLECharacteristic.h"

class BLEHIDDevice
{
  friend class BLEHID;

  public:
    BLEHIDDevice(const unsigned char* descriptor, unsigned char descriptorLength, unsigned char reportIdOffset);

    unsigned char getDescriptorLength();
    unsigned char getDescriptorValueAtOffset(unsigned char offset);

  protected:
    static unsigned char numDevices();

    void sendData(BLECharacteristic& characteristic, unsigned char data[], unsigned char dataLength);

    virtual void setReportId(unsigned char reportId);
    virtual unsigned char numAttributes() = 0;
    virtual BLELocalAttribute** attributes() = 0;

  private:
    static unsigned char _numDevices;

    unsigned char _reportId;
    const unsigned char* _descriptor;
    unsigned char _descriptorLength;
    unsigned char _reportIdOffset;
};

#endif
