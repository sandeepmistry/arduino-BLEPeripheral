#include "URIBeacon.h"

#if defined(NRF51) || defined(__RFduino__)
  #define MAX_SERVICE_DATA_SIZE 18
#else
  #define MAX_SERVICE_DATA_SIZE 15 // only 15 bytes (instead of 18), because flags (3 bytes) are in advertisement data
#endif

static const char* URI_BEACON_PREFIX_SUBSTITUTIONS[] = {
  "http://www.",
  "https://www.",
  "http://",
  "https://",
  "urn:uuid:"
};

static const char* URI_BEACON_SUFFIX_SUBSTITUTIONS[] = {
  ".com/",
  ".org/",
  ".edu/",
  ".net/",
  ".info/",
  ".biz/",
  ".gov/",
  ".com",
  ".org",
  ".edu",
  ".net",
  ".info",
  ".biz",
  ".gov"
};

URIBeacon::URIBeacon(unsigned char req, unsigned char rdy, unsigned char rst) :
  BLEPeripheral(req, rdy, rst),
  _bleService("fed8"),
  _bleCharacteristic("fed9", BLERead | BLEBroadcast, MAX_SERVICE_DATA_SIZE)
{
//  this->setLocalName(uri);
//  this->setLocalName("uri-beacon");

  this->setAdvertisedServiceUuid(this->_bleService.uuid());

  this->setConnectable(false);

  this->addAttribute(this->_bleService);
  this->addAttribute(this->_bleCharacteristic);
}

void URIBeacon::begin(unsigned char flags, unsigned char power, const char* uri) {
  this->_flags = flags;
  this->_power = power;
  this->setURI(uri);

  BLEPeripheral::begin();

  this->_bleCharacteristic.broadcast();
}

void URIBeacon::setURI(const char* uri) {
  unsigned char serviceData[MAX_SERVICE_DATA_SIZE];

  serviceData[0] = this->_flags;
  serviceData[1] = this->_power;
  unsigned char compressedURIlength = this->compressURI(uri, (char *)&serviceData[2], sizeof(serviceData) - 2);

  this->_bleCharacteristic.setValue(serviceData, 2 + compressedURIlength);
}

unsigned char URIBeacon::compressURI(const char* uri, char *compressedUri, unsigned char compressedUriSize) {
  String uriString = uri;

  // replace prefixes
  for (unsigned int i = 0; i < (sizeof(URI_BEACON_PREFIX_SUBSTITUTIONS) / sizeof(char *)); i++) {
    String replacement = " ";
    replacement[0] = (char)(i | 0x80); // set high bit, String.replace does not like '\0' replacement

    uriString.replace(URI_BEACON_PREFIX_SUBSTITUTIONS[i], replacement);
  }

  // replace suffixes
  for (unsigned int i = 0; i < (sizeof(URI_BEACON_SUFFIX_SUBSTITUTIONS) / sizeof(char *)); i++) {
    String replacement = " ";
    replacement[0] = (char)(i | 0x80); // set high bit, String.replace does not like '\0' replacement

    uriString.replace(URI_BEACON_SUFFIX_SUBSTITUTIONS[i], replacement);
  }

  unsigned char i = 0;
  for (i = 0; i < uriString.length() && i < compressedUriSize; i++) {
    compressedUri[i] = (uriString[i] & 0x7f); // assign byte after clearing hight bit
  }

  return i;
}

void URIBeacon::loop() {
  this->poll();
}
