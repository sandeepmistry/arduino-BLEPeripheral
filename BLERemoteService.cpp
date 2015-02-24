#include "BLERemoteService.h"

BLERemoteService::BLERemoteService(const char* uuid) :
  BLERemoteAttribute(uuid, BLETypeService)
{
}
