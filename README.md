# Arduino BLEPeripheral

[![Build Status](https://travis-ci.org/sandeepmistry/arduino-BLEPeripheral.svg?branch=master)](https://travis-ci.org/sandeepmistry/arduino-BLEPeripheral) [![Gitter](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/sandeepmistry/arduino-BLEPeripheral?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)


An [Arduino](http://arduino.cc) library for creating custom BLE peripherals and observers with [Nordic Semiconductor](http://www.nordicsemi.com)'s  [nR51822](http://www.nordicsemi.com/eng/Products/Bluetooth-R-low-energy/nRF51822).

This is heavily based on @sandeepmistry's [arduino-BLEPeripheral](https://github.com/sandeepmistry/arduino-BLEPeripheral) library. This fork adds support for the observer role (i.e. receiving Bluetooth advertisements), but consequently can't support the nRF8001 anymore.

## Compatible Hardware

### [Nordic Semiconductor nRF51822](http://www.nordicsemi.com/eng/Products/Bluetooth-R-low-energy/nRF51822)

 * [RedBearLab](http://redbearlab.com) with [Arduino Add-on](https://github.com/RedBearLab/nRF51822-Arduino)
   * [nRF51822](http://redbearlab.com/redbearlab-nrf51822)
   * [BLE Nano](http://redbearlab.com/blenano/)
 * [RFduino](http://www.rfduino.com) with [Arduino Add-on](https://github.com/RFduino/RFduino)
 
**WARNING**: Adafruit nRF51 boards are **NOT** compatible with this library, this includes the [Bluefruit LE UART Friend](https://www.adafruit.com/products/2479), [Bluefruit LE SPI Friend](https://www.adafruit.com/products/2633), [Bluefruit LE Shield](https://www.adafruit.com/products/2746), [Flora Wearable Bluefruit LE Module](https://www.adafruit.com/products/2487), [Feather 32u4 Bluefruit LE](https://www.adafruit.com/products/2829), and [Feather M0 Bluefruit LE](https://www.adafruit.com/products/2995).

### Nordic Semiconductor nRF51 or nRF52 based boards

 * Various, see [arduino-nRF5 supported boards](https://github.com/sandeepmistry/arduino-nRF5#supported-boards) via [nRF5 Arduino Add-on](https://github.com/sandeepmistry/arduino-nRF5)

## Usage

### Download Library

#### Arduino

#### Using the Arduino IDE Library Manager

1. Choose ```Sketch``` -> ```Include Library``` -> ```Manage Libraries...```
2. Type ```BLEPeripheralObserver``` into the search box.
3. Click the row to select the library.
4. Click the ```Install``` button to install the library.

#### Using Git
```sh
cd ~/Documents/Arduino/libraries/
git clone https://github.com/floe/BLEPeripheralObserver BLEPeripheralObserver
```

#### MPIDE
```
cd ~/Documents/mpide/libraries/
git clone https://github.com/floe/BLEPeripheralObserver BLEPeripheralObserver
```

### [arduino-nRF5x core](https://github.com/sandeepmistry/arduino-nRF5) users

The [arduino-nRF5x core](https://github.com/sandeepmistry/arduino-nRF5) **REQUIRES** a SoftDevice in order to successfully use this library. Please see [Flashing a SoftDevice](https://github.com/sandeepmistry/arduino-nRF5#flashing-a-softdevice).

Further, the error ```fatal error: ble_gatts.h: No such file or directory``` in indicitive of no SoftDevice being selected.

### Starter sketch
Load [starter.ino](examples/starter/starter.ino)

## API
See [API.md](API.md).

## Examples
See [examples](examples) folder.

## License

This libary is [licensed](LICENSE) under the [MIT Licence](http://en.wikipedia.org/wiki/MIT_License).
