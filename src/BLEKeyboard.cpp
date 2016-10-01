// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "BLEKeyboard.h"

static const PROGMEM unsigned char descriptorValue[] = {
  // From: https://github.com/adafruit/Adafruit-Trinket-USB/blob/master/TrinketHidCombo/TrinketHidComboC.c
  //       permission to use under MIT license by @ladyada (https://github.com/adafruit/Adafruit-Trinket-USB/issues/10)

  0x05, 0x01,           // USAGE_PAGE (Generic Desktop)
  0x09, 0x06,           // USAGE (Keyboard)
  0xA1, 0x01,           // COLLECTION (Application)
  0x85, 0x00,           // REPORT_ID
  0x75, 0x01,           //   REPORT_SIZE (1)
  0x95, 0x08,           //   REPORT_COUNT (8)
  0x05, 0x07,           //   USAGE_PAGE (Keyboard)(Key Codes)
  0x19, 0xE0,           //   USAGE_MINIMUM (Keyboard LeftControl)(224)
  0x29, 0xE7,           //   USAGE_MAXIMUM (Keyboard Right GUI)(231)
  0x15, 0x00,           //   LOGICAL_MINIMUM (0)
  0x25, 0x01,           //   LOGICAL_MAXIMUM (1)
  0x81, 0x02,           //   INPUT (Data,Var,Abs) ; Modifier byte
  0x95, 0x01,           //   REPORT_COUNT (1)
  0x75, 0x08,           //   REPORT_SIZE (8)
  0x81, 0x03,           //   INPUT (Cnst,Var,Abs) ; Reserved byte
#ifdef USE_LED_REPORT
  0x95, 0x05,           //   REPORT_COUNT (5)
  0x75, 0x01,           //   REPORT_SIZE (1)
  0x05, 0x08,           //   USAGE_PAGE (LEDs)
  0x19, 0x01,           //   USAGE_MINIMUM (Num Lock)
  0x29, 0x05,           //   USAGE_MAXIMUM (Kana)
  0x91, 0x02,           //   OUTPUT (Data,Var,Abs) ; LED report
  0x95, 0x01,           //   REPORT_COUNT (1)
  0x75, 0x03,           //   REPORT_SIZE (3)
  0x91, 0x03,           //   OUTPUT (Cnst,Var,Abs) ; LED report padding
#endif
  0x95, 0x05,           //   REPORT_COUNT (5)
  0x75, 0x08,           //   REPORT_SIZE (8)
  0x15, 0x00,           //   LOGICAL_MINIMUM (0)
  0x26, 0xA4, 0x00,     //   LOGICAL_MAXIMUM (164)
  0x05, 0x07,           //   USAGE_PAGE (Keyboard)(Key Codes)
  0x19, 0x00,           //   USAGE_MINIMUM (Reserved (no event indicated))(0)
  0x2A, 0xA4, 0x00,     //   USAGE_MAXIMUM (Keyboard Application)(164)
  0x81, 0x00,           //   INPUT (Data,Ary,Abs)
  0xC0                  // END_COLLECTION
};

BLEKeyboard::BLEKeyboard() :
  BLEHID(descriptorValue, sizeof(descriptorValue), 7),
  _reportCharacteristic("2a4d", BLERead | BLENotify, 8),
  _reportReferenceDescriptor(BLEHIDDescriptorTypeInput)
{
  memset(this->_value, 0, sizeof(this->_value));
}

size_t BLEKeyboard::write(uint8_t k) {
  uint8_t code;
  uint8_t modifier;

  this->keyToCodeAndModifier(k, code, modifier);

  return (this->press(code, modifier) && this->release(code, modifier));
}

size_t BLEKeyboard::press(uint8_t code, uint8_t modifiers) {
  size_t written = 0;

  if (code != 0) {
    for (unsigned int i = 2; i < sizeof(this->_value); i++) {
      if (this->_value[i] == 0) {
        this->_value[0] |= modifiers;
        this->_value[i] = code;

        written = 1;
        break;
      }
    }
  } else if (modifiers) {
    this->_value[0] |= modifiers;
    written = 1;
  }

  if (written) {
    this->sendValue();
  }

  return written;
}

size_t BLEKeyboard::release(uint8_t code, uint8_t modifiers) {
  size_t cleared = 0;

  if (code != 0) {
    for (unsigned int i = 2; i < sizeof(this->_value); i++) {
      if (this->_value[i] == code) {
        this->_value[0] &= ~modifiers;
        this->_value[i] = 0;

        cleared = 1;
        break;
      }
    }
  } else if (modifiers) {
    this->_value[0] &= ~modifiers;
    cleared = 1;
  }

  if (cleared) {
    this->sendValue();
  }

  return cleared;
}

void BLEKeyboard::releaseAll(void) {
  memset(this->_value, 0, sizeof(this->_value));

  this->sendValue();
}

void BLEKeyboard::setReportId(unsigned char reportId) {
  BLEHID::setReportId(reportId);

  this->_reportReferenceDescriptor.setReportId(reportId);
}

unsigned char BLEKeyboard::numAttributes() {
  return 2;
}

BLELocalAttribute** BLEKeyboard::attributes() {
  static BLELocalAttribute* attributes[2];

  attributes[0] = &this->_reportCharacteristic;
  attributes[1] = &this->_reportReferenceDescriptor;

  return attributes;
}

void BLEKeyboard::sendValue() {
  BLEHID::sendData(this->_reportCharacteristic, this->_value, sizeof(this->_value));
}


#define SHIFT 0x80

static const PROGMEM unsigned char asciiMap[] = {
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  KEYCODE_BACKSPACE,          // '\b'
  KEYCODE_TAB,                // '\t'
  KEYCODE_ENTER,              // '\n'
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  KEYCODE_SPACE,              // ' '
  0x1e | SHIFT,               // '!'
  0x34 | SHIFT,               // '"'
  0x20 | SHIFT,               // '#'
  0x21 | SHIFT,               // '$'
  0x22 | SHIFT,               // '%'
  0x24 | SHIFT,               // '&'
  0x34,                       // '''
  0x26 | SHIFT,               // '('
  0x27 | SHIFT,               // ')'
  0x25 | SHIFT,               // '*'
  0x2e | SHIFT,               // '+'
  KEYCODE_COMMA,              // ','
  KEYCODE_MINUS,              // '-'
  KEYCODE_PERIOD,             // '.'
  KEYCODE_SLASH,              // '/'
  KEYCODE_0,                  // '0'
  KEYCODE_1,                  // '1'
  KEYCODE_2,                  // '2'
  KEYCODE_3,                  // '3'
  KEYCODE_4,                  // '4'
  KEYCODE_5,                  // '5'
  KEYCODE_6,                  // '6'
  KEYCODE_7,                  // '7'
  KEYCODE_8,                  // '8'
  KEYCODE_9,                  // '9'
  0x33 | SHIFT,               // ':'
  0x33,                       // ';'
  0x36 | SHIFT,               // '<'
  KEYCODE_EQUAL,              // '='
  0x37 | SHIFT,               // '>'
  0x38 | SHIFT,               // '?'
  0x1f | SHIFT,               // '@'
  KEYCODE_A | SHIFT,          // 'A'
  KEYCODE_B | SHIFT,          // 'B'
  KEYCODE_C | SHIFT,          // 'C'
  KEYCODE_D | SHIFT,          // 'D'
  KEYCODE_E | SHIFT,          // 'E'
  KEYCODE_F | SHIFT,          // 'F'
  KEYCODE_G | SHIFT,          // 'G'
  KEYCODE_H | SHIFT,          // 'H'
  KEYCODE_I | SHIFT,          // 'I'
  KEYCODE_J | SHIFT,          // 'J'
  KEYCODE_K | SHIFT,          // 'K'
  KEYCODE_L | SHIFT,          // 'L'
  KEYCODE_M | SHIFT,          // 'M'
  KEYCODE_N | SHIFT,          // 'N'
  KEYCODE_O | SHIFT,          // 'O'
  KEYCODE_P | SHIFT,          // 'P'
  KEYCODE_Q | SHIFT,          // 'Q'
  KEYCODE_R | SHIFT,          // 'R'
  KEYCODE_S | SHIFT,          // 'S'
  KEYCODE_T | SHIFT,          // 'T'
  KEYCODE_U | SHIFT,          // 'U'
  KEYCODE_V | SHIFT,          // 'V'
  KEYCODE_W | SHIFT,          // 'W'
  KEYCODE_X | SHIFT,          // 'X'
  KEYCODE_Y | SHIFT,          // 'Y'
  KEYCODE_Z | SHIFT,          // 'Z'
  KEYCODE_SQBRAK_LEFT,        // '['
  KEYCODE_BACKSLASH,          // '\'
  KEYCODE_SQBRAK_RIGHT,       // ']'
  0x23 | SHIFT,               // '^'
  0x2d | SHIFT,               // '_'
  0x35,                       // '`'
  KEYCODE_A,                  // 'a'
  KEYCODE_B,                  // 'b'
  KEYCODE_C,                  // 'c'
  KEYCODE_D,                  // 'd'
  KEYCODE_E,                  // 'e'
  KEYCODE_F,                  // 'f'
  KEYCODE_G,                  // 'g'
  KEYCODE_H,                  // 'h'
  KEYCODE_I,                  // 'i'
  KEYCODE_J,                  // 'j'
  KEYCODE_K,                  // 'k'
  KEYCODE_L,                  // 'l'
  KEYCODE_M,                  // 'm'
  KEYCODE_N,                  // 'n'
  KEYCODE_O,                  // 'o'
  KEYCODE_P,                  // 'p'
  KEYCODE_Q,                  // 'q'
  KEYCODE_R,                  // 'r'
  KEYCODE_S,                  // 's'
  KEYCODE_T,                  // 't'
  KEYCODE_U,                  // 'u'
  KEYCODE_V,                  // 'v'
  KEYCODE_W,                  // 'w'
  KEYCODE_X,                  // 'x'
  KEYCODE_Y,                  // 'y'
  KEYCODE_Z,                  // 'z'
  0x2f | SHIFT,               // '{'
  0x31 | SHIFT,               // '|'
  0x30 | SHIFT,               // '}'
  0x35 | SHIFT,               // '~'
  0x0
};

void BLEKeyboard::keyToCodeAndModifier(uint8_t k, uint8_t& code, uint8_t& modifier) {
  code = 0;
  modifier = 0;

  if (k < 128) {
    code = pgm_read_byte(asciiMap + k);

    if (code & 0x80) {
      modifier = KEYCODE_MOD_LEFT_SHIFT;
      code &= 0x7f;
    }
  }
}
