#ifndef _BLE_SERVICE_H_
#define _BLE_SERVICE_H_

#include "BLEAttribute.h"

class BLEService : public BLEAttribute
{
  public:
    BLEService(const char* uuid);
};

#endif
