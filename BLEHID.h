#ifndef _BLE_HID_H_
#define _BLE_HID_H_

#include "BLEHIDDevice.h"
#include "BLEPeripheral.h"

class BLEHID
{
  friend class BLEHIDDevice;

  public:
    BLEHID(unsigned char req, unsigned char rdy, unsigned char rst);
    ~BLEHID();

    void begin();

    void clearBondStoreData();
    void setLocalName(const char *localName);
    void setDeviceName(const char* deviceName);
    void setAppearance(unsigned short appearance);

    BLECentral central();
    bool connected();
    void poll();

    void addDevice(BLEHIDDevice& device);
    void addAttribute(BLELocalAttribute& attribute);

  protected:
    static BLEHID* instance();

  private:
    static BLEHID*                  _instance;

    BLEPeripheral                   _blePeripheral;
    BLEBondStore                    _bleBondStore;

    BLEService                      _hidService;
    BLEConstantCharacteristic       _hidReportMapCharacteristic;
    BLEConstantCharacteristic       _hidInformationCharacteristic;
    BLEUnsignedCharCharacteristic   _hidControlPointCharacteristic;

    BLEHIDDevice**                  _devices;
    unsigned char                   _numDevices;
};

#endif
