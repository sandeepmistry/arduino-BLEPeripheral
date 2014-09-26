// TimerOne library: https://code.google.com/p/arduino-timerone/
#include <TimerOne.h> 
#include <SPI.h>
#include <BLEPeripheral.h>

// define pins (varies per shield/board)
#define BLE_REQ   10
#define BLE_RDY   2
#define BLE_RST   9

BLEPeripheral blePeripheral = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);

BLEService tempService = BLEService("CCC0");

BLEIntCharacteristic tempCharacteristic = BLEIntCharacteristic("CCC1", BLERead | BLENotify);
BLEDescriptor tempDescriptor = BLEDescriptor("2901", "Celsius");

volatile bool readTemperature = false;

void setup() {
  Serial.begin(115200);
#if defined (__AVR_ATmega32U4__)
  //Wait until the serial port is available (useful only for the Leonardo)
  //As the Leonardo board is not reseted every time you open the Serial Monitor
  while(!Serial) {}
  delay(5000);  //5 seconds delay for enabling to see the start up comments on the serial board
#endif

  blePeripheral.setLocalName("Temperature");
  blePeripheral.setAdvertisedServiceUuid(tempService.uuid());
  blePeripheral.addAttribute(tempService);
  blePeripheral.addAttribute(tempCharacteristic);
  blePeripheral.addAttribute(tempDescriptor);

  blePeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  blePeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

  blePeripheral.begin();
  
  Timer1.initialize(1 * 1000000); // in milliseconds
  Timer1.attachInterrupt(timerHandler);
}

void loop() {
  blePeripheral.poll();
  
  if (readTemperature) {
    setTempCharacteristicValue();
    readTemperature = false;
  }
}

void timerHandler() {
  readTemperature = true;
}

void setTempCharacteristicValue() {
  int temp = readTempC();
  tempCharacteristic.setValue(temp);
  Serial.println(temp);
}

int readTempC() {
  // Stubbing out for demo with random value generator
  // Replace with actual sensor reading code
  return random(100);
}

void blePeripheralConnectHandler(BLECentral& central) {
  Serial.print(F("Connected event, central: "));
  Serial.println(central.address());
}

void blePeripheralDisconnectHandler(BLECentral& central) {
  Serial.print(F("Disconnected event, central: "));
  Serial.println(central.address());
}
