#include "BLEService.h"

BLEService::BLEService(const char* uuid)
  : BLEAttribute(uuid, BLE_TYPE_SERVICE)
{
}
