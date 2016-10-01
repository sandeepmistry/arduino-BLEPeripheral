#ifndef _BLE_REMOTE_SERVICE_H_
#define _BLE_REMOTE_SERVICE_H_

#include "BLERemoteAttribute.h"

class BLERemoteService : public BLERemoteAttribute
{
  public:
    BLERemoteService(const char* uuid);
};

#endif
