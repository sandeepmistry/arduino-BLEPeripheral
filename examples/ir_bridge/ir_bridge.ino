// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <BLEPeripheral.h>

// https://github.com/shirriff/Arduino-IRremote
#include <IRremote.h>

#define IR_SEND_PIN 3
#define IR_RECV_PIN 4

#define LED_PIN     5

struct IRValue {
  char type;
  char bits;
  unsigned int address;
  unsigned long value;
};

// create IR send and recv instance, see pinouts above
IRsend                           irSend                      = IRsend(/*IR_SEND_PIN*/);
IRrecv                           irRecv                      = IRrecv(IR_RECV_PIN);
IRValue                          irValue;

//custom boards may override default pin definitions with BLEPeripheral(PIN_REQ, PIN_RDY, PIN_RST)
BLEPeripheral                    blePeripheral                            = BLEPeripheral();

// create service and characteristics
BLEService                       irService                   = BLEService("00004952-0000-bbbb-0123-456789abcdef");
BLEFixedLengthCharacteristic     irOutputCharacteristic      = BLEFixedLengthCharacteristic("00004953-0000-bbbb-0123-456789abcdef", BLEWrite, sizeof(irValue));
BLEFixedLengthCharacteristic     irInputCharacteristic       = BLEFixedLengthCharacteristic("00004954-0000-bbbb-0123-456789abcdef", BLENotify, sizeof(irValue));


void setup() {
  Serial.begin(115200);

  // set advertised local name and service UUID
  blePeripheral.setLocalName("IR");
  blePeripheral.setAdvertisedServiceUuid(irService.uuid());

  // add service and characteristics
  blePeripheral.addAttribute(irService);
  blePeripheral.addAttribute(irOutputCharacteristic);
  blePeripheral.addAttribute(irInputCharacteristic);

   // assign event handlers for connected, disconnected to peripheral
  blePeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  blePeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

  // begin initialization
  blePeripheral.begin();

  Serial.println(F("BLE IR Peripheral"));

  // enable the IR receiver
  irRecv.enableIRIn();

  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  // poll peripheral
  blePeripheral.poll();

  // poll the ouput characteristic
  pollIrOutput();

  // poll the IR receiver
  pollIrInput();
}

void blePeripheralConnectHandler(BLECentral& central) {
  // central connected event handler
  digitalWrite(LED_PIN, HIGH);
}

void blePeripheralDisconnectHandler(BLECentral& central) {
  // central disconnected event handler
  digitalWrite(LED_PIN, LOW);
}

void pollIrOutput() {
    // check if central has written to the output value
  if (irOutputCharacteristic.written()) {
    // copy the value written
    memcpy(&irValue, irOutputCharacteristic.value(), sizeof(irValue));

    // extract the data
    char          irOutputType    = irValue.type;
    char          irOutputBits    = irValue.bits;
    unsigned int  irOutputAddress = irValue.address;
    unsigned long irOutputValue   = irValue.value;

    int sendCount;

    // calculate how many times to send the value, depends on the type
    if (irOutputType == SONY || irOutputType == RC5 || irOutputType == RC6) {
      sendCount = 3;
    } else {
      sendCount = 1;
    }

    for (int i = 0; i < sendCount; i++) {
      switch (irOutputType) {
        case NEC:
          irSend.sendNEC(irOutputValue, irOutputBits);
          break;

        case SONY:
          irSend.sendSony(irOutputValue, irOutputBits);
          break;

        case RC5:
          irSend.sendRC5(irOutputValue, irOutputBits);
          break;

        case RC6:
          irSend.sendRC6(irOutputValue, irOutputBits);
          break;

        case DISH:
          irSend.sendDISH(irOutputValue, irOutputBits);
          break;

        case SHARP:
          irSend.sendSharpRaw(irOutputValue, irOutputBits);
          break;

        case PANASONIC:
          irSend.sendPanasonic(irOutputAddress, irOutputValue);
          break;

        case JVC:
          irSend.sendJVC(irOutputValue, irOutputBits, 0);
          break;

        case SAMSUNG:
          irSend.sendSAMSUNG(irOutputValue, irOutputBits);
          break;

        case SANYO:
        case MITSUBISHI:
        case LG:
        default:
          // not implemented
          break;
      }

      delay(40);
    }

    // re-enable the IR receiver
    irRecv.enableIRIn();
  }
}

void pollIrInput() {
  decode_results irDecodeResults;

  // check if IR recv has a result that can be decoded
  if (irRecv.decode(&irDecodeResults)) {

    // must have non-zero number of bits and known decode type
    if (irDecodeResults.bits && irDecodeResults.decode_type != UNKNOWN) {

      // extract the decoded value
      irValue.type = irDecodeResults.decode_type;
      irValue.address = (irValue.type == PANASONIC) ? irDecodeResults.panasonicAddress : 0;
      irValue.bits = irDecodeResults.bits;
      irValue.value = irDecodeResults.value;

      // update the IR characteristic with the result
      irInputCharacteristic.setValue((unsigned char *)&irValue, sizeof(irValue));
    }

    // resume IR receiving
    irRecv.resume();
  }
}
