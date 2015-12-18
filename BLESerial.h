#ifndef _BLE_SERIAL_H_
#define _BLE_SERIAL_H_

#include <Arduino.h>
#include <BLEPeripheral.h>

class BLESerial : public BLEPeripheral, public Stream
{
  public:
    BLESerial(unsigned char req, unsigned char rdy, unsigned char rst);

    void begin();
    void poll();

    virtual int available(void);
    virtual int peek(void);
    virtual int read(void);
    virtual void flush(void);
    virtual size_t write(uint8_t byte);
    using Print::write;
    virtual operator bool();

  private:
    static BLESerial* _instance;
    unsigned long _flushed;

    size_t _rxHead;
    size_t _rxTail;
    size_t _rxCount() const;
    unsigned char _rxBuffer[256];
    size_t _txCount;
    unsigned char _txBuffer[BLE_ATTRIBUTE_MAX_VALUE_LENGTH];

    BLEService _uartService = BLEService("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
    BLEDescriptor _uartNameDescriptor = BLEDescriptor("2901", "UART");
    BLECharacteristic _rxCharacteristic = BLECharacteristic("6E400002-B5A3-F393-E0A9-E50E24DCCA9E", BLEWriteWithoutResponse, BLE_ATTRIBUTE_MAX_VALUE_LENGTH);
    BLEDescriptor _rxNameDescriptor = BLEDescriptor("2901", "RX - Receive Data (Write)");
    BLECharacteristic _txCharacteristic = BLECharacteristic("6E400003-B5A3-F393-E0A9-E50E24DCCA9E", BLENotify, BLE_ATTRIBUTE_MAX_VALUE_LENGTH);
    BLEDescriptor _txNameDescriptor = BLEDescriptor("2901", "TX - Transfer Data (Notify)");

    void _received(const unsigned char* data, size_t size);
    static void _received(BLECentral& /*central*/, BLECharacteristic& rxCharacteristic);
};

#endif