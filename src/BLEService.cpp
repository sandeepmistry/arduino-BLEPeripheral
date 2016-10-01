#include "BLEService.h"

BLEService::BLEService(const char* uuid) :
  BLELocalAttribute(uuid, BLETypeService)
{
}
