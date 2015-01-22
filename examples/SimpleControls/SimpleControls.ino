/*

Copyright (c) 2012-2014 RedBearLab

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

/*
 *    SimpleControls
 *
 *    SimpleControls works with the BLEController iOS/Android App.
 *    The Arduino's pin can acts as DIGITAL_IN, DIGITAL_OUT, PWM, SERVO, ANALOG_IN.
 *    The sketch is to show you how to control the pin as one of the abilities.
 *    Note that not every pin can own all of the abilities.
 *    You can change the following macro to specify which pin to be controlled.
 *      #define DIGITAL_OUT_PIN    2
 *      #define DIGITAL_IN_PIN     A4
 *      #define PWM_PIN            3
 *      #define SERVO_PIN          5
 *      #define ANALOG_IN_PIN      A5
 *    The sketch will report the state of DIGITAL_IN_PIN and ANALOG_IN_PIN to App.
 *    The App can control the state of DIGITAL_OUT_PIN / PWM_PIN / SERVO_PIN
 */

// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <BLEPeripheral.h>
#include <Servo.h>

// define pins (varies per shield/board)
#define BLE_REQ   6
#define BLE_RDY   7
#define BLE_RST   4

#define DIGITAL_OUT_PIN    2
#define DIGITAL_IN_PIN     A4
#define PWM_PIN            3
#define SERVO_PIN          5
#define ANALOG_IN_PIN      A5

Servo myservo;

/*----- BLE Utility -------------------------------------------------------------------------*/
// create peripheral instance, see pinouts above
BLEPeripheral            blePeripheral        = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);

// create service
BLEService               uartService          = BLEService("713d0000503e4c75ba943148f18d941e");

// create characteristic
BLECharacteristic    txCharacteristic = BLECharacteristic("713d0002503e4c75ba943148f18d941e", BLENotify, 20);
BLECharacteristic    rxCharacteristic = BLECharacteristic("713d0003503e4c75ba943148f18d941e", BLEWriteWithoutResponse, 20);
/*--------------------------------------------------------------------------------------------*/

void setup()
{
  Serial.begin(115200);
#if defined (__AVR_ATmega32U4__)
  delay(3000);  //5 seconds delay for enabling to see the start up comments on the serial board
#endif

  pinMode(DIGITAL_OUT_PIN, OUTPUT);
  pinMode(DIGITAL_IN_PIN, INPUT);

  // Default to internally pull high, change it if you need
  digitalWrite(DIGITAL_IN_PIN, HIGH);
  //digitalWrite(DIGITAL_IN_PIN, LOW);

  myservo.attach(SERVO_PIN);

/*----- BLE Utility ---------------------------------------------*/
  // set advertised local name and service UUID
  blePeripheral.setLocalName("ble-Micro");
  blePeripheral.setAdvertisedServiceUuid(uartService.uuid());

  // add service and characteristic
  blePeripheral.addAttribute(uartService);
  blePeripheral.addAttribute(rxCharacteristic);
  blePeripheral.addAttribute(txCharacteristic);

  // begin initialization
  blePeripheral.begin();
/*---------------------------------------------------------------*/

  Serial.println(F("BLE UART Peripheral"));
}

void loop()
{
  static boolean analog_enabled = false;
  static byte old_state = LOW;

  BLECentral central = blePeripheral.central();

  if (central)
  {
    // central connected to peripheral
    Serial.print(F("Connected to central: "));
    Serial.println(central.address());

    while (central.connected())
    {
      // central still connected to peripheral
      if (rxCharacteristic.written())
      {
        unsigned char len = rxCharacteristic.valueLength();
        const unsigned char *val = rxCharacteristic.value();

        Serial.print("didCharacteristicWritten, Length: ");
        Serial.println(len, DEC);

        unsigned char i = 0;
        while(i<len)
        {
          unsigned char data0 = val[i++];
          unsigned char data1 = val[i++];
          unsigned char data2 = val[i++];

          Serial.println(data0, HEX);
          Serial.println(data1, HEX);
          Serial.println(data2, HEX);

          if (data0 == 0x01)  // Command is to control digital out pin
          {
            if (data1 == 0x01)
              digitalWrite(DIGITAL_OUT_PIN, HIGH);
            else
              digitalWrite(DIGITAL_OUT_PIN, LOW);
          }
          else if (data0 == 0xA0) // Command is to enable analog in reading
          {
            if (data1 == 0x01)
              analog_enabled = true;
            else
              analog_enabled = false;
          }
          else if (data0 == 0x02) // Command is to control PWM pin
          {
            analogWrite(PWM_PIN, data1);
          }
          else if (data0 == 0x03)  // Command is to control Servo pin
          {
            myservo.write(data1);
          }
          else if (data0 == 0x04)
          {
            analog_enabled = false;
            myservo.write(0);
            analogWrite(PWM_PIN, 0);
            digitalWrite(DIGITAL_OUT_PIN, LOW);
          }
        }
      }

      if (analog_enabled)  // if analog reading enabled
      {
        Serial.println("didNotifyAnalogIn");
        // Read and send out
        uint16_t value = analogRead(ANALOG_IN_PIN);

        const unsigned char val[3] = {0x0B, value >> 8, value};
        txCharacteristic.setValue(val, 3);
      }

      // If digital in changes, report the state
      if (digitalRead(DIGITAL_IN_PIN) != old_state)
      {
        old_state = digitalRead(DIGITAL_IN_PIN);

        if (digitalRead(DIGITAL_IN_PIN) == HIGH)
        {
            Serial.println("didNotifyDigitalIn: From LOW to HIGH");
            const unsigned char val[3] = {0x0A, 0x01, 0x00};
            txCharacteristic.setValue(val, 3);
        }
        else
        {
            Serial.println("didNotifyDigitalIn: From HIGH to LOW");
            const unsigned char val[3] = {0x0A, 0x00, 0x00};
            txCharacteristic.setValue(val, 3);
        }
      }
    }

    analog_enabled = false;
    digitalWrite(DIGITAL_OUT_PIN, LOW);

    // central disconnected
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}
