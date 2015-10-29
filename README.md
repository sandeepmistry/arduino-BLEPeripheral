# Arduino BLEPeripheral
Linuxdevel fork in order to add some missing functionality.

[![Gitter](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/sandeepmistry/arduino-BLEPeripheral?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)


An [Arduino](http://arduino.cc) library for creating custom BLE peripherals with [Nordic Semiconductor](http://www.nordicsemi.com)'s [nRF8001](http://www.nordicsemi.com/eng/Products/Bluetooth-R-low-energy/nRF8001) or [nR51822](http://www.nordicsemi.com/eng/Products/Bluetooth-R-low-energy/nRF51822).

Enables you to create more customized BLE Peripheral's compared to the basic UART most other Arduino BLE libraries provide.

[nRFgo Studio](http://www.nordicsemi.com/chi/node_176/2.4GHz-RF/nRFgo-Studio) (and Windows) is not required when using the [nRF8001](http://www.nordicsemi.com/eng/Products/Bluetooth-R-low-energy/nRF8001).

## Compatible Hardware

### [Nordic Semiconductor nRF8001](http://www.nordicsemi.com/eng/Products/Bluetooth-R-low-energy/nRF8001)

 * [Adafruit](http://www.adafruit.com)
   * [Bluefruit LE - nRF8001 Breakout](http://www.adafruit.com/products/1697)
 * [RedBearLab](http://redbearlab.com)
   * [BLE Shield](http://redbearlab.com/bleshield/)
   * [Blend Micro](http://redbearlab.com/blendmicro/)
   * [Blend](http://redbearlab.com/blend/)
 * [Femtoduino](http://www.femtoduino.com)
   * [IMUduino BTLE](http://www.femtoduino.com/spex/imuduino-btle)
 * [Olimex](https://www.olimex.com)
   * [MOD-nRF8001](https://www.olimex.com/Products/Modules/RF/MOD-nRF8001/)
   * [OLIMEXINO-NANO-BLE](https://www.olimex.com/Products/Duino/AVR/OLIMEXINO-NANO-BLE/)
 * [Jaycon Systems](http://www.jayconsystems.com)
   * [nRF8001 Bluetooth Breakout Board](http://www.jayconsystems.com/nrf8001-breakout-board.html)

**Note:** Does not require use of [nRFgo Studio](http://www.nordicsemi.com/chi/node_176/2.4GHz-RF/nRFgo-Studio)! However, uses more code space.

### [Nordic Semiconductor nRF51822](http://www.nordicsemi.com/eng/Products/Bluetooth-R-low-energy/nRF51822)

 * [RedBearLab](http://redbearlab.com) with [Arduino Add-on](https://github.com/RedBearLab/nRF51822-Arduino)
   * [nRF51822](http://redbearlab.com/redbearlab-nrf51822)
   * [BLE Nano](http://redbearlab.com/blenano/)
 * [RFduino](http://www.rfduino.com) with [Arduino Add-on](https://github.com/RFduino/RFduino)

#### Pinouts

| Chip | Shield/Board | REQ Pin | RDY Pin | RST Pin |
| ---- | ------------ | ------- | ------- | ------- |
| nRF8001|
| | Bluefruit LE | 10 | 2 | 9 |
| | BLE Shield 1.x | 9 | 8 | UNUSED |
| | BLE Shield 2.x | 9 | 8 | UNUSED or 4/7 via jumper|
| | Blend | 9 | 8 | UNUSED or 4/5 via jumper |
| | Blend Micro | 6 | 7 | UNUSED or 4 |
| | IMUduino BTLE | 10 | 7 | 9 |
| nRF51822 |
| | RedBearLab nRF51822 | UNUSED | UNUSED | UNUSED |
| | BLE Nano | UNUSED | UNUSED | UNUSED |
| | RFduino | UNUSED | UNUSED | UNUSED |

## Compatible IDE's and MCU's

 * [Arduino IDE](http://arduino.cc/en/Main/Software#toc1)
   * AVR
   * SAM3X8E
   * SAMD21G18A
 * [Teensy](https://www.pjrc.com/teensy/) (via [Teensyduino](https://www.pjrc.com/teensy/td_download.html))
   * 2.0
   * 3.0
   * 3.1
   * LC
 * [MPIDE](http://chipkit.net/started/install-chipkit-software/) ([ChipKit](http://chipkit.net))
   * PIC32MX

## Usage

### Download Library

#### Arduino

#### Using the Arduino IDE Library Manager

1. Choose ```Sketch``` -> ```Include Library``` -> ```Manage Libraries...```
2. Type ```BLEPeripheral``` into the search box.
3. Click the row to select the library.
4. Click the ```Install``` button to install the library.

#### Using Git
```sh
cd ~/Documents/Arduino/libraries/
git clone https://github.com/sandeepmistry/arduino-BLEPeripheral BLEPeripheral
```

#### MPIDE
```
cd ~/Documents/mpide/libraries/
git clone https://github.com/sandeepmistry/arduino-BLEPeripheral BLEPeripheral
```

### Starter sketch
Load [starter.ino](examples/starter/starter.ino)

## API
See [API.md](API.md).

## Examples
See [examples](examples) folder.

## License

This libary is [licensed](LICENSE) under the [MIT Licence](http://en.wikipedia.org/wiki/MIT_License).

## Useful Links
 * [@lizardo](https://github.com/lizardo)'s [nRF8001 Experiments](https://github.com/lizardo/nrf8001)
   * used as a starting point to reverse engineer the proprietary setup message format for the chips
 * [@NordicSemiconductor](https://github.com/NordicSemiconductor)'s [ble-sdk-arduino](https://github.com/NordicSemiconductor/ble-sdk-arduino)
   * Original Arduino SDK for nRF8001
 * [@guanix](https://github.com/guanix)'s [arduino-nrf8001](https://github.com/guanix/arduino-nrf8001)
   * nRF8001 support for Arduino


[![Analytics](https://ga-beacon.appspot.com/UA-56089547-1/sandeepmistry/arduino-BLEPeripheral?pixel)](https://github.com/igrigorik/ga-beacon)

