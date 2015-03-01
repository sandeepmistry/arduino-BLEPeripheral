#include "BLEHIDReport.h"

#include "BLESystemControl.h"

static const unsigned char reportReferenceDescriptorValue[] = { REPID_SYSCTRLKEY, 0x01 };

BLESystemControl::BLESystemControl() :
  _reportCharacteristic("2a4d", BLERead | BLENotify, 1),
  _reportReferenceDescriptor("2908", reportReferenceDescriptorValue, sizeof(reportReferenceDescriptorValue))
{
}

size_t BLESystemControl::write(uint8_t k) {
  uint8_t sysCtrlKeyPress[1]= { 0x00 };

  // send key code
  sysCtrlKeyPress[0] = k;

  for (int i = 0; i < 2; i++) {
    this->sendData(this->_reportCharacteristic, sysCtrlKeyPress, sizeof(sysCtrlKeyPress));

    // send cleared code
    sysCtrlKeyPress[0] = 0x00;
  }
}

unsigned char BLESystemControl::numAttributes() {
  return 2;
}

BLELocalAttribute** BLESystemControl::attributes() {
  static BLELocalAttribute* attributes[] = {
    &this->_reportCharacteristic,
    &this->_reportReferenceDescriptor
  };

  return attributes;
}
