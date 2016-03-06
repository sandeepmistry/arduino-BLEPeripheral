# BLEPeripheral

## Constructor
```c
BLEPeripheral(unsigned char req, unsigned char rdy, unsigned char rst);
```
 * req - REQ pin
 * rdy - RDY pin
 * rst - RST pin, can be ```UNUSED```

## Advertising

### Local name
```c
void setLocalName(const char *localName);
```
 * localName - local name to advertise (up to: 20 characters on nRF8001, 29 characters on nRF51822)

### Service UUID
```c
void setAdvertisedServiceUuid(const char* advertisedServiceUuid);
```
* advertisedServiceUuid - service UUID to advertise

### Manufacturer Data
```c
void setManufacturerData(const unsigned char manufacturerData[], unsigned char manufacturerDataLength);
```
 * manufacturerData - array of bytes
 * manufacturerDataLength - length of array, up to: 20 bytes on nRF8001, 26 bytes on nRF51822

## Set Advertising Interval

```c
void setAdvertisingInterval(unsigned short advertisingInterval);
```

 * set advertising interval in ms, default is 100ms

## Set Connection Interval

Sets the connection interval to use after a connection has been established.

```c
void setConnectionInterval(unsigned short minimumConnectionInterval, unsigned short maximumConnectionInterval); 
```

 * minimumConnectionInterval - minimum connection interval in 1.25 ms increments
 * maximumConnectionInterval - maximum connection interval in 1.25 ms increments

**Note**: Both parameters must be between 0x0006 (7.5 ms) and 0x0c80 (4 s), values outside of this range will be ignored.

## Set TX Power

```c
bool setTxPower(int txPower);
```

 * set TX power in dBm, default value is 0 dBm, must be called after ```begin```. Returns ``true`` on success, ``false`` otherwise.

 **Note**: only certain values can be set with exactly, otherwise the next settable value is used
   * nRF8001: -18, -12, -6, 0
   * nRF51822: -40, -30, -20, -16, -12, -8, -4, 0, 4

## Connectable

```c
void setConnectable(bool connectable);
```

 * make peripheral connectable (**default**) or non-connectable (broadcast only)

## Bond Store

```c
void setBondStore(BLEBondStore& bondStore);
```

 * enable unauthenticated security (pairing), use the bond store to persist bonding data.

## Built-in characteristics

### Device name
```c
void setDeviceName(const char* deviceName);
```
 * deviceName - device name, up to: 20 characters on nRF8001 and nRF51822 - default value is ```"Arduino"```

### Appearance
```c
void setAppearance(unsigned short appearance);
```
 * appearance - appearance, default value is ```0x0000```

## Attributes
```c
void addAttribute(BLEAttribute& attribute);
```
 * attribute - attribute to add, can be ```BLEService```, ```BLECharacteristic```, or ```BLEDescriptor```

## Sketch life cycle
Call from ```setup```.
```c
void begin();
```

Call from ```loop```.
```c
void poll();
```

## Status

### Connection state
Is the peripheral connected to a central?

```c
bool connected();
```

### Central
Central the peripheral is connected to. Bool value evaluates to ```false``` if not connected.

```c
BLECentral central()
```


### Set event handler callbacks
```c
void setEventHandler(BLEPeripheralEvent event, BLEPeripheralEventHandler eventHandler);

// callback signature
void blePeripheralEventHandler(BLECentral& central) {
  // ....
}
```
 * event - ```BLEConnected``` or ```BLEDisconnected```
 * eventHandler - function callback for event

## Actions

### Disconnect
```c
void disconnect();
```
Disconnect connected central.

# BLECentral

## Status

### Valid
Is this a valid central?

```c
BLECentral central;

// ...

if (central) {
  // central is valid
} else {
  // central is invalid
}
```

### Connection state
Is the central connected?

```c
bool connected();
```

### Address
Bluetooth address of central as string.

```c
const char* address();
```

## Actions

### Disconnect
```c
void disconnect();
```
Disconnect central if connected.

### End

```c
void end();
```

Disconnects central if connected, stops advertising, and disables radio.

## Sketch life cycle
Call from ```loop```, while connected.
```c
void poll();
```


# BLEService

## Constructor
```c
BLEService(const char* uuid);
```
 * uuid - UUID of service

# BLECharacteristic

## Contructor
```c
BLECharacteristic(const char* uuid, unsigned char properties, unsigned char valueSize);

BLECharacteristic(const char* uuid, unsigned char properties, const char* value);
```
  * uuid - UUID of characteristic
  * properties - combination of (|'ed):
    * ```BLEBroadcast```
    * ```BLERead```
    * ```BLEWriteWithoutResponse ```
    * ```BLEWrite```
    * ```BLENotify```
    * ```BLEIndicate```

Choice of:
  * valueSize - size of characteristic in bytes (max: 20 characters on nRF8001 and nRF51822)

or

  * value - string value (max: 20 characters on nRF8001 and nRF51822)

## Get value
```c
const unsigned char* value();
unsigned char valueLength();
```

## Set value

Will automatically notify/indicate central, if characteristic has notify/indicate property and central is subscribed and update broadcasted value if broadcasting.

```c
bool setValue(const unsigned char value[], unsigned char length);
```
 * value - value bytes
 * length - value length (upto value size)

Returns true on success (central notified/indicated, if applicable), false on failure (cannot be notified/indicated, if applicable)

```c
bool setValue(const char* value);
```
 * value - new value as string

Returns true on success (central notified/indicated, if applicable), false on failure (cannot be notified/indicated, if applicable)

## Broadcast

Broadcast characteristic value in advertisement data.

```c
bool broadcast();
```

Returns true on success, false on failure

## Writes
Has the central written a new value since the last call to this method? (only for write or write without response characteristics)
```c
bool written();
```

## Subscription status
Is the central subscribed (via notify or indicate) to the characteristic? (only for notify/indicate characteristics)
```c
bool subscribed();
```

## Notify/indicate status
Can the central be notified/indicated of when the value is set. Only applies to characateristics with notify and/or indicate properties when a central is connected and subscribed
```c
bool canNotify();
bool canIndicate();
```

## Set event handler callbacks
```c
void setEventHandler(BLECharacteristicEvent event, BLECharacteristicEventHandler eventHandler);

// callback signature
void bleCharacteristicEventHandler(BLECentral& central, BLECharacteristic& characteristic) {
  // ....
}
```
 * event - ```BLEWritten```, ```BLESubscribed```, or ```BLEUnsubscribed```
 * eventHandler - function callback for event

# Typed BLECharacteristic's

| Data Type | Class |
| --------- | ------|
| ```bool``` | ```BLEBoolCharacteristic``` |
| ```char``` | ```BLECharCharacteristic``` |
| ```unsigned char``` | ```BLEUnsignedCharCharacteristic``` |
| ```short``` | ```BLEShortCharacteristic``` |
| ```unsigned short``` | ```BLEUnsignedShortCharacteristic``` |
| ```int``` | ```BLEIntCharacteristic``` |
| ```unsigned int``` | ```BLEUnsignedIntCharacteristic``` |
| ```long``` | ```BLELongCharacteristic``` |
| ```unsigned long``` | ```BLEUnsignedLongCharacteristic``` |
| ```float``` | ```BLEFloatCharacteristic``` |
| ```double``` | ```BLEDoubleCharacteristic``` |


## Contructor
```c
BLE<Data Type>Characteristic(const char* uuid, unsigned char properties);
```
See ```BLECharacteritic```

## Get value
```c
<Data Type> value();
<Data Type> valueLE(); // little endian
<Data Type> valueBE(); // big endian
```

## Set value

```c
bool setValue(<Data Type> value);
bool setValueLE(<Data Type> value); // little endian
bool setValueBE(<Data Type> value); // big endian
```

Returns true on success (central notified/indicated, if applicable), false on failure (cannot be notified/indicated, if applicable)

# BLEFixedLengthCharacteristic

 * Subclass of ```BLECharacteristic```, value is a fixed length

## Constructor
```c
BLEFixedLengthCharacteristic(const char* uuid, unsigned char properties, unsigned char valueSize);
BLEFixedLengthCharacteristic(const char* uuid, unsigned char properties, const char* value);
```
  * uuid - UUID of characteristic
  * properties - combination of (|'ed):
    * ```BLEBroadcast```
    * ```BLERead```
    * ```BLEWriteWithoutResponse ```
    * ```BLEWrite```
    * ```BLENotify```
    * ```BLEIndicate```

Choice of:
  * valueSize - size of characteristic in bytes (max: 20 characters on nRF8001 and nRF51822)

or

  * value - string value (max: 20 characters on nRF8001 and nRF51822)

# BLEConstantCharacteristic

 * Subclass of ```BLEFixedLengthCharacteristic```, value is constant and a fixed length. Is read only and, ```setValue``` API does nothing.

## Constructor
```c
BLEConstantCharacteristic(const char* uuid, const unsigned char value[], unsigned char length);
BLEConstantCharacteristic(const char* uuid, const char* value);
```
  * uuid - UUID of characteristic

Choice of:
  * value - value of characteristic
  * length - size of characteristic in bytes

or

  * value - string value (max: 20 characters on nRF8001 and nRF51822)

# BLEDescriptor

## Constructor
```c
BLEDescriptor(const char* uuid, const unsigned char value[], unsigned char valueSize);

BLEDescriptor(const char* uuid, const char* value);
```
  * uuid - UUID of descriptor

Choice of:
  * value - value data
  * valueLength - length of value data in bytes

or

  * value - string value

## Get value
```c
const unsigned char* value();
unsigned char valueLength();
```


# BLEBondStore

## Constructor
```c
BLEBondStore(int offset = 0);
```
 * offset - offset in persistent storage (AVR: EEPROM, NRF51: Flash, others RAM)

## Clear Data

```c
void clearData();
```

Clear bond data from store, must be called before ```blePeripheral.begin()```

