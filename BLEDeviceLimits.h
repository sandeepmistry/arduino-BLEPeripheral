#ifndef _BLE_LIMITS_H_
#define _BLE_LIMITS_H_

#include <stdlib.h>

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifdef NRF_51

#define BLE_ADVERTISEMENT_DATA_MAX_VALUE_LENGTH    26
#define BLE_SCAN_DATA_MAX_VALUE_LENGTH             29
#define BLE_ATTRIBUTE_MAX_VALUE_LENGTH             20

#else

#define BLE_ADVERTISEMENT_DATA_MAX_VALUE_LENGTH    20
#define BLE_SCAN_DATA_MAX_VALUE_LENGTH             20
#define BLE_ATTRIBUTE_MAX_VALUE_LENGTH             19

#endif

#endif
