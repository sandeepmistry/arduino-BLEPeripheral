#ifndef _BLE_SERVICE_H_
#define _BLE_SERVICE_H_

#include "BLELocalAttribute.h"

class BLEService : public BLELocalAttribute
{
  public:
    BLEService(const char* uuid);
};

#endif
