#include "EddystoneBeacon.h"

#define MAX_SERVICE_DATA_SIZE 20

#define FLAGS_UID 0x00
#define FLAGS_URL 0x10
#define FLAGS_TLM 0x20
#define float2fix(a) ((int)((a)*256.0)) //Convert float to fix. a is a float
static const char* EDDYSTONE_URL_BEACON_PREFIX_SUBSTITUTIONS[] = {
  "http://www.",
  "https://www.", 
  "http://",
  "https://",
  "urn:uuid:"
};

static const char* EDDYSTONE_URL_BEACON_SUFFIX_SUBSTITUTIONS[] = {
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

EddystoneBeacon::EddystoneBeacon(unsigned char req, unsigned char rdy, unsigned char rst) :
  BLEPeripheral(req, rdy, rst),
  _bleService("feaa"),
  _bleCharacteristic("feab", BLERead | BLEBroadcast, MAX_SERVICE_DATA_SIZE)
{
  this->setAdvertisedServiceUuid(this->_bleService.uuid());

  this->setConnectable(false);

  this->addAttribute(this->_bleService);
  this->addAttribute(this->_bleCharacteristic);
}

void EddystoneBeacon::begin(char power, const BLEUuid& uid) {
  unsigned char serviceData[MAX_SERVICE_DATA_SIZE];

  this->_power = power;

  memset(serviceData, 0x00, sizeof(serviceData));
  serviceData[0] = FLAGS_UID;
  serviceData[1] = this->_power;

  const unsigned char* uidData = uid.data();
  for (int i = 0; i < 16; i++) {
    serviceData[i + 2] = uidData[15 - i];
  }

  serviceData[18] = 0x00; // Reserved for future use, must be: 0x00
  serviceData[19] = 0x00; // Reserved for future use, must be: 0x00

  this->_bleCharacteristic.setValue(serviceData, sizeof(serviceData));

  BLEPeripheral::begin();

  this->_bleCharacteristic.broadcast();
}

void EddystoneBeacon::begin(char power, const char* uri) {
  this->_power = power;
  this->setURI(uri);

  BLEPeripheral::begin();

  this->_bleCharacteristic.broadcast();
}
/***Following is the code for broadcasting TLM ****/
/***Implementation in the examples folder***/
uint8_t buffer[14]= {
  0x20,
  0x00,
  0x00,
  0x00,
  0x00,
  0x80,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00
}; //global
unsigned char len = 14; //global
void EddystoneBeacon::tlm_begin(){
  this->_bleCharacteristic.setValue(buffer, len);

  BLEPeripheral::begin();

  this->_bleCharacteristic.broadcast();

}

void EddystoneBeacon::setURI(const char* uri) {
  unsigned char serviceData[MAX_SERVICE_DATA_SIZE];

  serviceData[0] = FLAGS_URL;
  serviceData[1] = this->_power;
  unsigned char compressedURIlength = this->compressURI(uri, (char *)&serviceData[2], sizeof(serviceData) - 2);

  this->_bleCharacteristic.setValue(serviceData, 2 + compressedURIlength);
}

unsigned char EddystoneBeacon::compressURI(const char* uri, char *compressedUri, unsigned char compressedUriSize) {
  String uriString = uri;

  // replace prefixes
  for (unsigned int i = 0; i < (sizeof(EDDYSTONE_URL_BEACON_PREFIX_SUBSTITUTIONS) / sizeof(char *)); i++) {
    String replacement = " ";
    replacement[0] = (char)(i | 0x80); // set high bit, String.replace does not like '\0' replacement

    uriString.replace(EDDYSTONE_URL_BEACON_PREFIX_SUBSTITUTIONS[i], replacement);
  }

  // replace suffixes
  for (unsigned int i = 0; i < (sizeof(EDDYSTONE_URL_BEACON_SUFFIX_SUBSTITUTIONS) / sizeof(char *)); i++) {
    String replacement = " ";
    replacement[0] = (char)(i | 0x80); // set high bit, String.replace does not like '\0' replacement

    uriString.replace(EDDYSTONE_URL_BEACON_SUFFIX_SUBSTITUTIONS[i], replacement);
  }

  unsigned char i = 0;
  for (i = 0; i < uriString.length() && i < compressedUriSize; i++) {
    compressedUri[i] = (uriString[i] & 0x7f); // assign byte after clearing hight bit
  }

  return i;
}

void EddystoneBeacon::loop() {
  this->poll();
}
/* loop() for TLM below:**/
float temperature;
int B=3975;                  //B value of the thermistor
float resistance;
unsigned long pdu_count = 0;
void EddystoneBeacon::tempLoop(int analogPin) {
  this->poll();
  pdu_count++;
  /** temperature **/
  pinMode(analogPin,INPUT);
  int a = analogRead(analogPin);
  resistance=(float)(1023-a)*10000/a; //get the resistance of the sensor;
  temperature=1/(log(resistance/10000)/B+1/298.15)-273.15;//convert to temperature via datasheet&nbsp;;
  buffer_1[4] = ((float2fix(temperature))&0x0000FF00)>>8; //LSB
  buffer_1[5] = (float2fix(temperature))&0x000000FF;  //MSB
  //temperature
  
  unsigned long val = millis()/100;
  ulong2adv(buffer,10, val);  //UP-TIME SINCE POWER ON OR REBOOT
  ulong2adv(buffer,6, pdu_count);  //PDU COUNT

  _bleCharacteristic.setValue(buffer, len);
}
void EddystoneBeacon::ulong2adv(uint8_t* adv, int off, unsigned long val) {
  off+=3;
  for (int i = 0; i < 4; ++i) {
    uint8_t bval = val & 0xff;
    adv[off] = bval;
    val >>= 8;
    off--;
  }
}
