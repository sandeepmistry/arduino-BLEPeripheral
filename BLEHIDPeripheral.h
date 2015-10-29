#ifndef _BLE_HID_PERIPHERAL_H_
#define _BLE_HID_PERIPHERAL_H_

#include "BLEHID.h"
#include "BLEHIDReportMapCharacteristic.h"
#include "BLEPeripheral.h"

class BLEHIDPeripheral : public BLEPeripheral
{
  friend class BLEHID;

  public:
    BLEHIDPeripheral(BLE_Tx_Power_Level tx_power_level, unsigned char req, unsigned char rdy, unsigned char rst);
    ~BLEHIDPeripheral();

    void begin();

    void clearBondStoreData();
    void setReportIdOffset(unsigned char reportIdOffset);

    void poll();

    void addHID(BLEHID& hid);

  protected:
    static BLEHIDPeripheral* instance();

  private:
    static BLEHIDPeripheral*            _instance;

    BLEBondStore                        _bleBondStore;

    BLEService                          _hidService;
    BLEHIDReportMapCharacteristic       _hidReportMapCharacteristic;
    BLEProgmemConstantCharacteristic    _hidInformationCharacteristic;
    BLEUnsignedCharCharacteristic       _hidControlPointCharacteristic;

    unsigned char                       _reportIdOffset;

    BLEHID**                            _hids;
    unsigned char                       _numHids;
};

#endif
