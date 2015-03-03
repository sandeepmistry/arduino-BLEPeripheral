#ifndef _BLE_BOND_STORE_H_
#define _BLE_BOND_STORE_H_

class BLEBondStore
{
  public:
    BLEBondStore(int offset = 0);

    bool hasData();
    void clearData();
    void putData(const unsigned char* data, unsigned char offset, unsigned char length);
    void getData(unsigned char* data, unsigned char offset, unsigned char length);

  private:
#ifdef __AVR__
    int             _offset;
#elif defined(NRF51) || defined(__RFduino__)
    uint32_t*       _flashPageStartAddress;
#endif
};

#endif
