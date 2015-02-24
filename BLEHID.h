#ifndef _BLE_HID_H_
#define _BLE_HID_H_

#include "BLEPeripheral.h"

class BLEHID
{
  public:
    BLEHID(unsigned char req, unsigned char rdy, unsigned char rst);

    void begin();

    BLECentral central();
    bool connected();

    size_t write(uint8_t k);

  private:
    BLEPeripheral                   _blePeripheral;
    BLEBondStore                    _bleBondStore;
    BLEService                      _hidService;

    BLEConstantCharacteristic       _hidInformationCharacteristic;
    BLEUnsignedCharCharacteristic   _hidControlPointCharacteristic;
    BLEConstantCharacteristic       _hidReportDescriptor;

    BLECharacteristic               _hidReportCharacteristic1;
    BLEDescriptor                   _reportReferenceDescriptor1;
};

#endif
