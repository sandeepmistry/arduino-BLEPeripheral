#ifndef _BLE_BOND_STORE_H_
#define _BLE_BOND_STORE_H_

class BLEBondStore
{
  public:
    BLEBondStore(int offset = 0);

    bool hasData();
    void clearData();
    void storeData(unsigned char* data, unsigned char length);
    void restoreData(unsigned char* data, unsigned char length);

  private:
    int _offset;
};

#endif
