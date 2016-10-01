// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef _BLE_CENTRAL_H_
#define _BLE_CENTRAL_H_

class BLEPeripheral;

class BLECentral
{
  friend class BLEPeripheral;

  public:
    operator bool() const;
    bool operator==(const BLECentral& rhs) const;
    bool operator!=(const BLECentral& rhs) const;

    bool connected();
    const char* address() const;
    void poll();

    void disconnect();

  protected:
    BLECentral(BLEPeripheral* peripheral);
    void setAddress(const unsigned char* address);
    void clearAddress();

  private:
    BLEPeripheral* _peripheral;
    unsigned char  _address[6];
};

#endif
