// Import libraries (EddystoneBeacon depends on SPI)
#include <SPI.h>
#include <EddystoneBeacon.h>

// define pins (varies per shield/board)
//
//   Adafruit Bluefruit LE   10, 2, 9
//   Blend                    9, 8, UNUSED
//   Blend Micro              6, 7, 4
//   RBL BLE Shield           9, 8, UNUSED

#define EDDYSTONE_BEACON_REQ   9
#define EDDYSTONE_BEACON_RDY   8
#define EDDYSTONE_BEACON_RST   7

int analogPin = 0;
int resetPin = 7;
int button = 2;
float temp = 0.0;

unsigned char buffer_1[14] = {
0x20,
0x00,
0x00,
0x00,
0x00,
0x80,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00
};

unsigned char buffer_2[14] = {
0x20,
0x00,
0x00,
0x00,
0x80,
0x80,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00
};

unsigned char *ptr1 = buffer_1;
unsigned char *ptr2 = buffer_2;
unsigned char len = 14;//length of the Eddystone-TLM Frame

EddystoneBeacon eddystoneBeacon = EddystoneBeacon(EDDYSTONE_BEACON_REQ, EDDYSTONE_BEACON_RDY, EDDYSTONE_BEACON_RST);

void setup() {
  pinMode(analogPin,INPUT);
  pinMode(resetPin,OUTPUT);
  pinMode(button,INPUT);
  Serial.begin(57600);

  delay(1000);

  eddystoneBeacon.begin(ptr1,len);

}

void loop() {
  Serial.println("main loop");
  eddystoneBeacon.loop();
  if(digitalRead(button) == HIGH){
    digitalWrite(resetPin,HIGH);
    digitalWrite(resetPin,LOW);
    digitalWrite(resetPin,HIGH);
    temp = analogRead(analogPin);
    temp = temp * 0.48828125;
    /*conversion from float to hex
    
    TO DO
    
    */
    buffer_2[5] = 0x35;//Replace this with the hex achieved from above
    
    EddystoneBeacon eddystoneBeacon = EddystoneBeacon(EDDYSTONE_BEACON_REQ, EDDYSTONE_BEACON_RDY, EDDYSTONE_BEACON_RST);
    Serial.begin(57600);

    delay(1000);

    eddystoneBeacon.begin(ptr2,len);
    
  }
}
