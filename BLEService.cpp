#include "BLEService.h"

BLEService::BLEService(const char* uuid) :
  BLEAttribute(uuid, BLETypeService)
{
}
