# BLEPeripheral

## Constructor
```
BLEPeripheral(unsigned char req, unsigned char rdy, unsigned char rst);
```
 * req - REQ pin
 * rdy - RDY pin
 * rst - RST pin, can be ```UNUSED```

## Advertising

### Local name
```
void setLocalName(const char *localName);
```
 * localName - local name to advertise (up to: 20 characters on nRF8001, 29 characters on nRF51822)

### Service UUID
```
void setAdvertisedServiceUuid(const char* advertisedServiceUuid);
```
* advertisedServiceUuid - service UUID to advertise

### Manufacturer Data
```
void setManufacturerData(const unsigned char manufacturerData[], unsigned char manufacturerDataLength);
```
 * manufacturerData - array of bytes
 * manufacturerDataLength - length of array, up to: 20 bytes on nRF8001, 26 bytes on nRF51822

## Set Advertising Interval

```
void setAdvertisingInterval(unsigned short advertisingInterval);
```

 * set advertising interval in ms, default is 100ms

## Connectable

```
void setConnectable(bool connectable);
```

 * make peripheral connectable (**default**) or non-connectable (broadcast only)

## Built-in characteristics

### Device name
```
void setDeviceName(const char* deviceName);
```
 * deviceName - device name, up to: 20 characters on nRF8001 and nRF51822 - default value is ```"Arduino"```

### Appearance
```
void setAppearance(unsigned short appearance);
```
 * appearance - appearance, default value is ```0x0000```

## Attributes
```
void addAttribute(BLEAttribute& attribute);
```
 * attribute - attribute to add, can be ```BLEService```, ```BLECharacteristic```, or ```BLEDescriptor```

## Sketch life cycle
Call from ```setup```.
```
void begin();
```

Call from ```loop```.
```
void poll();
```

## Status

### Connection state
Is the peripheral connected to a central?

```
bool connected();
```

### Central
Central the peripheral is connected to. Bool value evaluates to ```false``` if not connected.

```
BLECentral central()
```


### Set event handler callbacks
```
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
```
void disconnect();
```
Disconnect connected central.

# BLECentral

## Status

### Valid
Is this a valid central?

```
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

```
bool connected();
```

### Address
Bluetooth address of central as string.

```
const char* address();
```

## Actions

### Disconnect
```
void disconnect();
```
Disconnect central if connected.

## Sketch life cycle
Call from ```loop```, while connected.
```
void poll();
```


# BLEService

## Constructor
```
BLEService(const char* uuid);
```
 * uuid - UUID of service

# BLECharacteristic

## Contructor
```
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
```
const unsigned char* value();
unsigned char valueLength();
```

## Set value

Will automatically notify/indicate central, if characteristic has notify/indicate property and central is subscribed and update broadcasted value if broadcasting.

```
bool setValue(const unsigned char value[], unsigned char length);
```
 * value - value bytes
 * length - value length (upto value size)

Returns true on success (central notified/indicated, if applicable), false on failure (cannot be notified/indicated, if applicable)

```
bool setValue(const char* value);
```
 * value - new value as string

Returns true on success (central notified/indicated, if applicable), false on failure (cannot be notified/indicated, if applicable)

## Broadcast

Broadcast characteristic value in advertisement data.

```
bool broadcast();
```

Returns true on success, false on failure

## Writes
Has the central written a new value since the last call to this method? (only for write or write without response characteristics)
```
bool written();
```

## Subscription status
Is the central subscribed (via notify or indicate) to the characteristic? (only for notify/indicate characteristics)
```
bool subscribed();
```

## Notify/indicate status
Can the central be notified/indicated of when the value is set. Only applies to characateristics with notify and/or indicate properties when a central is connected and subscribed
```
bool canNotify();
bool canIndicate();
```

## Set event handler callbacks
```
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
```
BLE<Data Type>Characteristic(const char* uuid, unsigned char properties);
```
See ```BLECharacteritic```

## Get value
```
<Data Type> value();
<Data Type> valueLE(); // little endian
<Data Type> valueBE(); // big endian
```

## Set value

```
bool setValue(<Data Type> value);
bool setValueLE(<Data Type> value); // little endian
bool setValueBE(<Data Type> value); // big endian
```

Returns true on success (central notified/indicated, if applicable), false on failure (cannot be notified/indicated, if applicable)

# BLEDescriptor

## Constructor
```
BLEDescriptor(const char* uuid, unsigned char valueSize);

BLEDescriptor(const char* uuid, const char* value);
```
  * uuid - UUID of descriptor

Choice of:
  * valueSize - size of characteristic in bytes (max: 20 characters on nRF8001 and nRF51822)

or

  * value - string value (max: 20 characters on nRF8001 and nRF51822)

## Get value
```
const unsigned char* value();
unsigned char valueLength();
```

## Set value

```
void setValue(const unsigned char value[], unsigned char length);
```
 * value - value bytes
 * length - value length (upto value size)

```
void setValue(const char* value);
```
 * value - new value as string
