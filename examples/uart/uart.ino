/*
 * BLESerial - UART
 * Provide UART service compatible with Nordic's *nRF Toolbox*, *nRF UART* and Adafruit's *Bluefruit LE*
 * iOS/Android apps.
 *
 * BLESerial class implements same protocols as Serial port and can be used as it's wireless replacement.
 * Data transfers are routed through BLE UART service with TX and RX characteristics. To make the UART
 * service discoverable all UUIDs are NUS (Nordic UART Service) compatible.
 *
 * Please note that TX and RX characteristics use Notify and WriteWithoutResponse, so there's no guarantee
 * that the data will make it to the other end. Hovewer, under normal circumstances and resonable signal
 * strengths everything works well.
 */

// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <BLEPeripheral.h>
#include <BLESerial.h>

// define pins (varies per shield/board)
#define BLE_REQ   10
#define BLE_RDY   2
#define BLE_RST   9

// create uart instance, see pinouts above
BLESerial BLESerial(BLE_REQ, BLE_RDY, BLE_RST);


void setup() {
  // custom services or characteristics can be added too
  BLESerial.setLocalName("UART");

  Serial.begin(115200);
  BLESerial.begin();
}

void loop() {
  BLESerial.poll();

  forward();
  // loopback();
  // spam();
}


// forward received from Serial to BLESerial and vice versa
void forward() {
  if (BLESerial && Serial) {
    int byte;
    while ((byte = BLESerial.read()) > 0) Serial.write((char)byte);
    while ((byte = Serial.read()) > 0) BLESerial.write((char)byte);
  }
}

// echo all received data back
void loopback() {
  if (BLESerial) {
    int byte;
    while ((byte = BLESerial.read()) > 0) BLESerial.write(byte);
  }
}

// periodically sent time stamps
void spam() {
  if (BLESerial) {
    BLESerial.print(millis());
    BLESerial.println(" tick-tacks!");
    delay(1000);
  }
}