#include "URIBeacon.h"

#define MAX_SERVICE_DATA_SIZE 15 // only 15 bytes (instead of 18), because flags (3 bytes) are in advertisement data

static const char* URI_BEACON_SUBSTITUTIONS[] = {
  "http://www.",
  "https://www.",
  "http://",
  "https://",
  "tel:",
  "mailto:",
  "geo:",
  ".com",
  ".org",
  ".edu"
};

URIBeacon::URIBeacon(unsigned char req, unsigned char rdy, unsigned char rst) :
  _blePeripheral(req, rdy, rst),
  _bleService("fed8"),
  _bleCharacteristic("fed9", BLERead | BLEBroadcast, MAX_SERVICE_DATA_SIZE)
{
//  this->_blePeripheral.setLocalName(uri);
//  this->_blePeripheral.setLocalName("uri-beacon");

  this->_blePeripheral.setAdvertisedServiceUuid(this->_bleService.uuid());

  this->_blePeripheral.addAttribute(this->_bleService);
  this->_blePeripheral.addAttribute(this->_bleCharacteristic);
}

void URIBeacon::begin(unsigned char flags, unsigned char power, const char* uri) {
  unsigned char serviceData[MAX_SERVICE_DATA_SIZE];

  serviceData[0] = flags;
  serviceData[1] = power;
  unsigned char compressedURIlength = this->compressURI(uri, (char *)&serviceData[2], sizeof(serviceData) - 2);

  this->_bleCharacteristic.setValue(serviceData, 2 + compressedURIlength);

  this->_blePeripheral.begin();

  this->_bleCharacteristic.broadcast();
}

unsigned char URIBeacon::compressURI(const char* uri, char *compressedUri, unsigned char compressedUriSize) {
  String uriString = uri;

  for (int i = 0; i < (sizeof(URI_BEACON_SUBSTITUTIONS) / sizeof(char *)); i++) {
    String replacement = " ";
    replacement[0] = (char)(i | 0x80); // set high bit, String.replace does not like '\0' replacement

    uriString.replace(URI_BEACON_SUBSTITUTIONS[i], replacement);
  }

  unsigned char i = 0;
  for (i = 0; i < uriString.length() && i < compressedUriSize; i++) {
    compressedUri[i] = (uriString[i] & 0x7f); // assign byte after clearing hight bit
  }

  return i;
}

void URIBeacon::loop() {
  BLECentral central = this->_blePeripheral.central();

  if (central) {
    // central connected to peripheral
    Serial.println(central.address());

    while (central.connected()) {
      central.disconnect(); // disconnect central ASAP
    }

    // central disconnected
    Serial.println(central.address());
  }
}
