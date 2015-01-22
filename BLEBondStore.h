#ifndef _BLE_BOND_STORE_H_
#define _BLE_BOND_STORE_H_

class BLEBondStore
{
  public:
    BLEBondStore(int offset = 0);
    ~BLEBondStore();

    bool hasData();
    void clearData();
    void storeData(const unsigned char* data, unsigned char length);
    void restoreData(unsigned char* data, unsigned char length);

  private:
#ifdef __AVR__
    int             _offset;
#elif defined(NRF51) || defined(__RFduino__)
    uint32_t*       _flashPageStartAddress;
#else
    unsigned char   _dataLength;
    unsigned char*  _data;
#endif
};

#endif
