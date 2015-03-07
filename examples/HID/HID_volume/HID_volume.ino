// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <BLEHIDPeripheral.h>
#include <BLEMultimedia.h>

// define pins (varies per shield/board)
#define BLE_REQ   9
#define BLE_RDY   8
#define BLE_RST   4

#define BUTTON_PIN 5

#define ENC_A_PIN 6
#define ENC_B_PIN 7

BLEHIDPeripheral bleHIDPeripheral = BLEHIDPeripheral(BLE_REQ, BLE_RDY, BLE_RST);
BLEMultimedia bleMultimedia;

int buttonState;

void setup() {
  Serial.begin(9600);
#if defined (__AVR_ATmega32U4__)
  while(!Serial);
#endif

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  buttonState = digitalRead(BUTTON_PIN);
  
  pinMode(ENC_A_PIN, INPUT_PULLUP);
  pinMode(ENC_B_PIN, INPUT_PULLUP);

  if (buttonState == LOW) {
    Serial.println(F("BLE HID Peripheral clearing bond data"));
    
    // clears bond data
    bleHIDPeripheral.clearBondStoreData();
  }
  
  bleHIDPeripheral.setLocalName("HID Volume");

  bleHIDPeripheral.addHID(bleMultimedia);

  bleHIDPeripheral.begin();

  Serial.println(F("BLE HID Volume Knob"));
}

void loop() {
  BLECentral central = bleHIDPeripheral.central();

  if (central) {
    // central connected to peripheral
    Serial.print(F("Connected to central: "));
    Serial.println(central.address());

    while (bleHIDPeripheral.connected()) {
      
      // check the button
      int tempButtonState = digitalRead(BUTTON_PIN);
      if (tempButtonState != buttonState) {
        buttonState = tempButtonState;
        
        if (buttonState == LOW) {
          Serial.println(F("Mute"));
          bleMultimedia.write(MMKEY_MUTE);
        }
      }
      
      // check the encoder
      int encoderState = readEncoder();
  
      if (encoderState == -1) {
        Serial.println(F("Volume down"));
        bleMultimedia.write(MMKEY_VOL_DOWN);
      } else if (encoderState == 1) {
        Serial.println(F("Volume up"));
        bleMultimedia.write(MMKEY_VOL_UP);
      }
    }

    // central disconnected
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}

//
// Based off of: http://www.circuitsathome.com/mcu/programming/reading-rotary-encoder-on-arduino
//

int8_t encStates[] = { 0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0 };
uint8_t oldAB = 0;

int8_t readEncoder()
{
  oldAB <<= 1;
  oldAB |= digitalRead(ENC_A_PIN);
  
  oldAB <<= 1;
  oldAB |= digitalRead(ENC_B_PIN);
  
  return ( encStates[( oldAB & 0x0f )]);
}

