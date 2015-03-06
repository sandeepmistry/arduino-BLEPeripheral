#ifndef _BLE_HID_PERIPHERAL_H_
#define _BLE_HID_PERIPHERAL_H_

#include "BLEHID.h"
#include "BLEHIDReportMapCharacteristic.h"
#include "BLEPeripheral.h"

class BLEHIDPeripheral
{
  friend class BLEHID;

  public:
    BLEHIDPeripheral(unsigned char req, unsigned char rdy, unsigned char rst);
    ~BLEHIDPeripheral();

    void begin();

    void clearBondStoreData();
    void setLocalName(const char *localName);
    void setDeviceName(const char* deviceName);
    void setAppearance(unsigned short appearance);

    BLECentral central();
    bool connected();
    void poll();

    void addHID(BLEHID& hid);
    void addAttribute(BLELocalAttribute& attribute);

  protected:
    static BLEHIDPeripheral* instance();

  private:
    static BLEHIDPeripheral*        _instance;

    BLEPeripheral                   _blePeripheral;
    BLEBondStore                    _bleBondStore;

    BLEService                      _hidService;
    BLEHIDReportMapCharacteristic   _hidReportMapCharacteristic;
    BLEConstantCharacteristic       _hidInformationCharacteristic;
    BLEUnsignedCharCharacteristic   _hidControlPointCharacteristic;

    BLEHID**                        _hids;
    unsigned char                   _numHids;
};

#endif
