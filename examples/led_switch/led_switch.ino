#include <SPI.h>
#include <BLEPeripheral.h>

//#define REDBEARLAB_SHIELD

#if defined(BLEND_MICRO)
  #define BLE_REQ   6
  #define BLE_RDY   7
  #define BLE_RST   UNUSED
#elif defined(BLEND) || defined(REDBEARLAB_SHIELD)
  #define BLE_REQ   9
  #define BLE_RDY   8
  #define BLE_RST   UNUSED
#else // Adafruit
  #define BLE_REQ   10
  #define BLE_RDY   2
  #define BLE_RST   9
#endif

#define LED_PIN    3
#define BUTTON_PIN 4

BLEPeripheral            blePeripheral        = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);

BLEService               ledService           = BLEService("19b10010e8f2537e4f6cd104768a1214");
BLECharacteristicT<char> switchCharacteristic = BLECharacteristicT<char>("19b10011e8f2537e4f6cd104768a1214", BLEPropertyRead | BLEPropertyWrite);
BLECharacteristicT<char> buttonCharacteristic = BLECharacteristicT<char>("19b10012e8f2537e4f6cd104768a1214", BLEPropertyRead | BLEPropertyNotify);

void setup() {                
  Serial.begin(115200);
#if defined (__AVR_ATmega32U4__)
  //Wait until the serial port is available (useful only for the Leonardo)
  //As the Leonardo board is not reseted every time you open the Serial Monitor
  while(!Serial) {}
  delay(5000);  //5 seconds delay for enabling to see the start up comments on the serial board
#endif

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);

  blePeripheral.setLocalName("LED Switch");
  blePeripheral.setAdvertisedServiceUuid(ledService.uuid());

  blePeripheral.addAttribute(ledService);
  blePeripheral.addAttribute(switchCharacteristic);
  blePeripheral.addAttribute(buttonCharacteristic);

  blePeripheral.begin();

  Serial.println(F("BLE LED Switch Peripheral"));
}

void loop() {
  blePeripheral.poll();

  char buttonValue = digitalRead(BUTTON_PIN);

  bool buttonChanged = (buttonCharacteristic.value() != buttonValue);

  if (buttonChanged) {
    switchCharacteristic.setValue(buttonValue);
    buttonCharacteristic.setValue(buttonValue);
  }

  if (switchCharacteristic.hasNewValue() || buttonChanged) {
    if (switchCharacteristic.value()) {
      Serial.println(F("LED on"));
      digitalWrite(LED_PIN, HIGH);
    } else {
      Serial.println(F("LED off"));
      digitalWrite(LED_PIN, LOW);
    }
  }
}
