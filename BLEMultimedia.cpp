#include "BLEHIDReport.h"

#include "BLEMultimedia.h"

static const unsigned char reportReferenceDescriptorValue[] = { REPID_MMKEY, 0x01 };

BLEMultimedia::BLEMultimedia() :
  _reportCharacteristic("2a4d", BLERead | BLENotify, 2),
  _reportReferenceDescriptor("2908", reportReferenceDescriptorValue, sizeof(reportReferenceDescriptorValue))
{
}

size_t BLEMultimedia::write(uint8_t k) {
  uint8_t multimediaKeyPress[2]= { 0x00, 0x00 };

  // send key code
  multimediaKeyPress[0] = k;

  for (int i = 0; i < 2; i++) {
    this->sendData(this->_reportCharacteristic, multimediaKeyPress, sizeof(multimediaKeyPress));

    // send cleared code
    multimediaKeyPress[0] = 0x00;
  }

  return 1;
}

unsigned char BLEMultimedia::numAttributes() {
  return 2;
}

BLELocalAttribute** BLEMultimedia::attributes() {
  static BLELocalAttribute* attributes[] = {
    &this->_reportCharacteristic,
    &this->_reportReferenceDescriptor
  };

  return attributes;
}
