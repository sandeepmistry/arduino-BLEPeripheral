#include <SPI.h>
#include <BLEPeripheral.h>

#define BLE_REQ 10
#define BLE_RDY 2
#define BLE_RST 9

#define LED_PIN 3

BLEPeripheral            blePeripheral        = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);

BLEService               ledService           = BLEService("19b10000e8f2537e4f6cd104768a1214");
BLECharacteristicT<char> switchCharacteristic = BLECharacteristicT<char>("19b10001e8f2537e4f6cd104768a1214", BLEPropertyRead | BLEPropertyWrite);

void setup() {                
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);

  blePeripheral.setLocalName("LED");
  blePeripheral.setAdvertisedServiceUuid(ledService.uuid());

  blePeripheral.addAttribute(ledService);
  blePeripheral.addAttribute(switchCharacteristic);

  blePeripheral.setConnectHandler(blePeripheralConnectHandler);
  blePeripheral.setDisconnectHandler(blePeripheralDisconnectHandler);
  switchCharacteristic.setNewValueHandler(switchCharacteristicHasNewValue);

  blePeripheral.begin();

  Serial.println(F("BLE LED Peripheral"));
}

void loop() {
  blePeripheral.poll();
}

void blePeripheralConnectHandler(const char* address) {
  Serial.print(F("Connected to central "));
  Serial.println(address);
}

void blePeripheralDisconnectHandler() {
  Serial.println(F("Disconnected from central "));
}

void switchCharacteristicHasNewValue() {
  if (switchCharacteristic.value()) {
    Serial.println(F("LED on"));
    digitalWrite(LED_PIN, HIGH);
  } else {
    Serial.println(F("LED off"));
    digitalWrite(LED_PIN, LOW);
  }
}
