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
 * localName - local name to advertise (up to 20 characters)

### Service UUID
```
void setAdvertisedServiceUuid(const char* advertisedServiceUuid);
```
* advertisedServiceUuid - service UUID to advertise

### Manufacturer Data
```
void setManufacturerData(const unsigned char* manufacturerData, unsigned char manufacturerDataLength);
```
 * manufacturerData - array of bytes
 * manufacturerDataLength - length of array, up to 20 bytes

## Built-in characteristics

### Device name
```
void setDeviceName(const char* deviceName);
```
 * deviceName - device name, up to 19 characters - default value is ```"Arduino"```

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
bool isConnected();
```


### Set connect event callback
```
void setConnectHandler(BLEPeripheralConnectHandler connectHandler);

// callback signature
void connectHandler(const char* address) {
  // address of central

  // ....
}

```

### Set connect event callback
```
void setDisconnectHandler(BLEPeripheralDisconnectHandler disconnectHandler);

// callback signature
void disconnectHandler() {
  // ...
}
```

## Actions

### Disconnect
```
void disconnect();
```
Disconnect connected central.

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
    * ```BLEPropertyRead```
    * ```BLEPropertyWriteWithoutResponse ```
    * ```BLEPropertyWrite ```
    * ```BLEPropertyNotify ```
    * ```BLEPropertyIndicate ```

Choice of:
  * valueSize - size of characteristic in bytes (max 19 bytes)

or

  * value - string value (max 19 bytes)

## Get value
```
const unsigned char* value();
unsigned char valueLength();
```

## Set value

Will automatically notify/indicate central, if characteristic has notify/indicate property and central is subscribed.

```
void setValue(const unsigned char value[], unsigned char length);
```
 * value - value bytes
 * length - value length (upto value size)

```
    void setValue(const char* value);
```
 * value - new value as string

## Value updates
Has the central written a new value since the last call to this method? (only for write or write without response characteristics)
```
bool hasNewValue();
```

Set callback for when central writes value (only for write or write without response characteristics)
```
void setNewValueHandler(BLECharacteristicNewValueHandler newValueHandler);

// callback signature
void newValueHandler() {
  // ...
}
```

## Subscription status
Is the central subscribed (via notify or indicate) to the characteristic? (only for notify/indicate characteristics)
```
bool hasNotifySubscriber();
bool hasIndicateSubscriber();
```

# BLECharacteristicT
Templated subclass of ```BLECharacteristic```

```T``` is type of characteristic: ```char```, ```short```, ```float```, etc.

## Contructor
```
BLECharacteristicT<T>(const char* uuid, unsigned char properties);
```
See ```BLECharacteritic```

## Get value
```
T value();
```

## Set value

```
void setValue(T value);
```

# BLEDescriptor

## Constructor
```
BLEDescriptor(const char* uuid, unsigned char valueSize);

BLEDescriptor(const char* uuid, const char* value);
```
  * uuid - UUID of descriptor

Choice of:
  * valueSize - size of characteristic in bytes (max 19 bytes)

or

  * value - string value (max 19 bytes)

## Get value
```
const unsigned char* value();
unsigned char valueLength();
```

## Set value

Will automatically notify/indicate central, if characteristic has notify/indicate property and central is subscribed.

```
void setValue(const unsigned char value[], unsigned char length);
```
 * value - value bytes
 * length - value length (upto value size)

```
    void setValue(const char* value);
```
 * value - new value as string
