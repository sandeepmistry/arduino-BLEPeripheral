#ifndef _BLE_HID_H_
#define _BLE_HID_H_

#include "BLELocalAttribute.h"
#include "BLECharacteristic.h"

class BLEHID
{
  friend class BLEHIDPeripheral;

  public:
    BLEHID(const unsigned char* descriptor, unsigned char descriptorLength, unsigned char reportIdOffset);

    unsigned char getDescriptorLength();
    unsigned char getDescriptorValueAtOffset(unsigned char offset);

  protected:
    static unsigned char numHids();

    void sendData(BLECharacteristic& characteristic, unsigned char data[], unsigned char dataLength);

    virtual void setReportId(unsigned char reportId);
    virtual unsigned char numAttributes() { }
    virtual BLELocalAttribute** attributes() { }

  private:
    static unsigned char _numHids;

    unsigned char _reportId;
    const unsigned char* _descriptor;
    unsigned char _descriptorLength;
    unsigned char _reportIdOffset;
};

#endif
