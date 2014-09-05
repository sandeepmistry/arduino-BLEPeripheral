/*

Copyright (c) 2012-2014 RedBearLab

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

/*
 *    BLEControllerSketch
 *    To use the sketch, the pins for REQN & RDYN can not be provided to user. 
 *    The Boards.h has excluded D8 and D9 to be notified to app since it treats these two pins as REQN & RDYN defaultly.
 *    So you have to set D8(D9) as REQN and set D9(D8) as RDYN in the sketch, except the Blend Micro board. 
 *    As the REQN & RDYN pins is flexible on those boards except Blend Micro.  
 *    Unless you modify the Boards.h to exculde the pins used as REQN & RDYN in your sketch.
 *    
 *    The sketch works with the BLEController iOS/Android App.
 *    It reports every available Arduino's pin mode and pin state to the App in real-time,
 *    Such as D3 is now acts as INPUT and its state is HIGH.
 *    You can also use App to control the pin mode and pin state,
 *    Such as D5 can be changed as PWM mode and its state is 128.
 */

// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <BLEPeripheral.h>
#include <Servo.h>
#include "Boards.h"

// define pins (varies per shield/board)
#define BLE_REQ   6
#define BLE_RDY   7
#define BLE_RST   4

#define PROTOCOL_MAJOR_VERSION   0 //
#define PROTOCOL_MINOR_VERSION   0 //
#define PROTOCOL_BUGFIX_VERSION  2 // bugfix

#define PIN_CAPABILITY_NONE      0x00
#define PIN_CAPABILITY_DIGITAL   0x01
#define PIN_CAPABILITY_ANALOG    0x02
#define PIN_CAPABILITY_PWM       0x04
#define PIN_CAPABILITY_SERVO     0x08
#define PIN_CAPABILITY_I2C       0x10

// pin modes
//#define INPUT                 0x00 // defined in wiring.h
//#define OUTPUT                0x01 // defined in wiring.h
#define ANALOG                  0x02 // analog pin in analogInput mode
#define PWM                     0x03 // digital pin in PWM output mode
#define SERVO                   0x04 // digital pin in Servo output mode

byte pin_mode[TOTAL_PINS];
byte pin_state[TOTAL_PINS];
byte pin_pwm[TOTAL_PINS];
byte pin_servo[TOTAL_PINS];

Servo servos[MAX_SERVOS];

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
  delay(3000);  //3 seconds delay for enabling to see the start up comments on the serial board
  
  /* Default all to digital input */
  for (int pin = 0; pin < TOTAL_PINS; pin++)
  {
    // Set pin to input with internal pull up
    pinMode(pin, INPUT);
    digitalWrite(pin, HIGH);

    // Save pin mode and state
    pin_mode[pin] = INPUT;
    pin_state[pin] = LOW;
  }
  
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
  
  Serial.println("BLE Arduino Slave");
}

/*   !!!!!!  ble_write_string(byte *bytes, uint8_t len)   !!!!!! 
Every other function wants to send data will call this function at the end.
*/
void ble_write_string(byte *bytes, uint8_t len)
{
  while(!txCharacteristic.canNotify())
  {
    blePeripheral.poll();
  }
  txCharacteristic.setValue(bytes, len); 
}

byte reportDigitalInput()
{
  static byte pin = 0;
  byte report = 0;
  
  if (!IS_PIN_DIGITAL(pin))
  {
    pin++;
    if (pin >= TOTAL_PINS)
      pin = 0;
    return 0;
  }
  
  if (pin_mode[pin] == INPUT)
  {
      byte current_state = digitalRead(pin);
            
      if (pin_state[pin] != current_state)
      {
        pin_state[pin] = current_state;
        byte buf[] = {'G', pin, INPUT, current_state};
        ble_write_string(buf, 4);
        
        report = 1;
      }
  }
  
  pin++;
  if (pin >= TOTAL_PINS)
    pin = 0;
    
  return report;
}

void reportPinCapability(byte pin)
{
  byte buf[] = {'P', pin, 0x00};
  byte pin_cap = 0;
                    
  if (IS_PIN_DIGITAL(pin))
    pin_cap |= PIN_CAPABILITY_DIGITAL;
            
  if (IS_PIN_ANALOG(pin))
    pin_cap |= PIN_CAPABILITY_ANALOG;

  if (IS_PIN_PWM(pin))
    pin_cap |= PIN_CAPABILITY_PWM;

  if (IS_PIN_SERVO(pin))
    pin_cap |= PIN_CAPABILITY_SERVO;

  buf[2] = pin_cap;
  ble_write_string(buf, 3);
}

void reportPinServoData(byte pin)
{
//  if (IS_PIN_SERVO(pin))
//    servos[PIN_TO_SERVO(pin)].write(value);
//  pin_servo[pin] = value;
  
  byte value = pin_servo[pin];
  byte mode = pin_mode[pin];
  byte buf[] = {'G', pin, mode, value};         
  ble_write_string(buf, 4);
}

byte reportPinAnalogData()
{
  static byte pin = 0;
  byte report = 0;
  
  if (!IS_PIN_DIGITAL(pin))
  {
    pin++;
    if (pin >= TOTAL_PINS)
      pin = 0;
    return 0;
  }
  
  if (pin_mode[pin] == ANALOG)
  {
    uint16_t value = analogRead(pin);
    byte value_lo = value;
    byte value_hi = value>>8;
    
    byte mode = pin_mode[pin];
    mode = (value_hi << 4) | mode;
    
    byte buf[] = {'G', pin, mode, value_lo};         
    ble_write_string(buf, 4);
  }
  
  pin++;
  if (pin >= TOTAL_PINS)
    pin = 0;
    
  return report;
}

void reportPinDigitalData(byte pin)
{
  byte state = digitalRead(pin);
  byte mode = pin_mode[pin];
  byte buf[] = {'G', pin, mode, state};         
  ble_write_string(buf, 4);
}

void reportPinPWMData(byte pin)
{
  byte value = pin_pwm[pin];
  byte mode = pin_mode[pin];
  byte buf[] = {'G', pin, mode, value};         
  ble_write_string(buf, 4);
}

void sendCustomData(uint8_t *buf, uint8_t len)
{
  uint8_t data[20] = "Z";
  memcpy(&data[1], buf, len);
  ble_write_string(data, len+1);
}

byte queryDone = false;

void loop()
{
  BLECentral central = blePeripheral.central();

  if (central) 
  {
    while (central.connected()) 
    {
      // central still connected to peripheral
      if (rxCharacteristic.written()) 
      {
        unsigned char len = rxCharacteristic.valueLength();
        const unsigned char *val = rxCharacteristic.value();
        
        Serial.println(" ");
        Serial.print("didCharacteristicWritten, Length: "); 
        Serial.println(len, DEC);
        
        unsigned char i = 0;
        while(i<len)
        {
          byte cmd;
          cmd = val[i++];
          Serial.write(cmd);
          
          // Parse data here
          switch (cmd)
          {
            case 'V': // query protocol version
              {
                byte buf[] = {'V', 0x00, 0x00, 0x01};
                ble_write_string(buf, 4);
              }
              break;
            
            case 'C': // query board total pin count
              {
                byte buf[2];
                buf[0] = 'C';
                buf[1] = TOTAL_PINS; 
                ble_write_string(buf, 2);
              }        
              break;
            
            case 'M': // query pin mode
              {  
                byte pin = val[i++];
                byte buf[] = {'M', pin, pin_mode[pin]}; // report pin mode
                ble_write_string(buf, 3);
              }  
              break;
            
            case 'S': // set pin mode
              {
                byte pin = val[i++];
                byte mode = val[i++];
                
                if (IS_PIN_SERVO(pin) && mode != SERVO && servos[PIN_TO_SERVO(pin)].attached())
                  servos[PIN_TO_SERVO(pin)].detach();
        
                /* ToDo: check the mode is in its capability or not */
                /* assume always ok */
                if (mode != pin_mode[pin])
                {              
                  pinMode(pin, mode);
                  pin_mode[pin] = mode;
                
                  if (mode == OUTPUT)
                  {
                    digitalWrite(pin, LOW);
                    pin_state[pin] = LOW;
                  }
                  else if (mode == INPUT)
                  {
                    digitalWrite(pin, HIGH);
                    pin_state[pin] = HIGH;
                  }
                  else if (mode == ANALOG)
                  {
                    if (IS_PIN_ANALOG(pin)) {
                      if (IS_PIN_DIGITAL(pin)) {
                        pinMode(PIN_TO_DIGITAL(pin), LOW);
                      }
                    }
                  }
                  else if (mode == PWM)
                  {
                    if (IS_PIN_PWM(pin))
                    {
                      pinMode(PIN_TO_PWM(pin), OUTPUT);
                      analogWrite(PIN_TO_PWM(pin), 0);
                      pin_pwm[pin] = 0;
                      pin_mode[pin] = PWM;
                    }
                  }
                  else if (mode == SERVO)
                  {
                    if (IS_PIN_SERVO(pin))
                    {
                      pin_servo[pin] = 0;
                      pin_mode[pin] = SERVO;
                      if (!servos[PIN_TO_SERVO(pin)].attached())
                        servos[PIN_TO_SERVO(pin)].attach(PIN_TO_DIGITAL(pin));
                    }
                  }
                }
                  
                //if (mode == ANALOG)
                //  reportPinAnalogData(pin);
                if ( (mode == INPUT) || (mode == OUTPUT) )
                  reportPinDigitalData(pin);
                else if (mode == PWM)
                  reportPinPWMData(pin);
                else if (mode == SERVO)
                  reportPinServoData(pin);
              }
              break;
      
            case 'G': // query pin data
              {
                byte pin = val[i++];
                reportPinDigitalData(pin);
              }
              break;
              
            case 'T': // set pin digital state
              {
                byte pin = val[i++];
                byte state = val[i++];
                
                digitalWrite(pin, state);
                reportPinDigitalData(pin);
              }
              break;
            
            case 'N': // set PWM
              {
                byte pin = val[i++];
                byte value = val[i++];
                
                analogWrite(PIN_TO_PWM(pin), value);
                pin_pwm[pin] = value;
                reportPinPWMData(pin);
              }
              break;
            
            case 'O': // set Servo
              {
                byte pin = val[i++];
                byte value = val[i++];
      
                if (IS_PIN_SERVO(pin))
                  servos[PIN_TO_SERVO(pin)].write(value);
                pin_servo[pin] = value;
                reportPinServoData(pin);
              }
              break;
            
            case 'A': // query all pin status
              {
                for (int pin = 0; pin < TOTAL_PINS; pin++)
                {
                  reportPinCapability(pin);
                  if ( (pin_mode[pin] == INPUT) || (pin_mode[pin] == OUTPUT) )
                    reportPinDigitalData(pin);
                  else if (pin_mode[pin] == PWM)
                    reportPinPWMData(pin);
                  else if (pin_mode[pin] == SERVO)
                    reportPinServoData(pin);  
                }            
                queryDone = true; 
                uint8_t str[] = "ABC";
                sendCustomData(str, 3);
              }
              break;
                
            case 'P': // query pin capability
              {
                byte pin = val[i++];
                reportPinCapability(pin);
              }
              break;
              
            case 'Z':
              {
                byte len = val[i++];
                byte buf[len];
                for (int j=0;j<len;j++)
                {
                  buf[j] = val[i++];
                }
                Serial.println("->");
                Serial.print("Received: ");
                Serial.print(len);
                Serial.println(" byte(s)");
                Serial.print(" Hex: ");
                for (int i=0;i<len;i++)
                  Serial.print(buf[i], HEX);
                Serial.println();
              }
          }
        } 
      } 
      // process text data
      if (Serial.available())
      {
        byte d = 'Z';
        txCharacteristic.setValue(&d, 1);
    
        delay(5);
        while(Serial.available())
        {
          d = Serial.read();
          txCharacteristic.setValue(&d, 1);
        }    
      }     
      if (queryDone) // only report data after the query state
      { 
        reportDigitalInput();        
        reportPinAnalogData();
      }     
    }
    
    queryDone = false; // reset query state
    
    // central disconnected
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}

