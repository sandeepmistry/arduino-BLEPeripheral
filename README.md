# Arduino BLEPeripheral

An [Arduino](http://arduino.cc) library for creating custom BLE peripherals.

## Compatible Hardware

### [Nordic Semiconductor nRF8001](http://www.nordicsemi.com/eng/Products/Bluetooth-R-low-energy/nRF8001)

 * [Adafruit](http://www.adafruit.com)
   * [Bluefruit LE - nRF8001 Breakout](http://www.adafruit.com/products/1697)
 * [RedBearLab](http://redbearlab.com)
   * [BLE Shield](http://redbearlab.com/bleshield/)
   * [Blend Micro](http://redbearlab.com/blendmicro/)
   * [Blend](http://redbearlab.com/blend/)

**Note:** Does not require use of [nRFgo Studio](http://www.nordicsemi.com/chi/node_176/2.4GHz-RF/nRFgo-Studio)!

#### Pinouts

| Shield/Board | REQ Pin | RDY Pin | RST Pin |
| ------------ | ------- | ------- | ------- |
| Bluefruit LE | 10 | 2 | 9 |
| BLE Shield 1.x | 9 | 8 | UNUSED |
| BLE Shield 2.x | 9 | 8 | UNUSED or 4/7 via jumper|
| Blend | 9 | 8 | UNUSED or 5/6 via jumper |
| Blend Micro | 6 | 7 | UNUSED or 4 |

## Usage

### Download Library
```
cd ~/Documents/Arduino/libraries/
git clone https://github.com/sandeepmistry/arduino-BLEPeripheral BLEPeripheral
```

### Starter sketch
Load [starter.ino](examples/starter/starter.ino)

## API
See [API.md](API.md).

## Examples
See [examples](examples) folder.

## Useful Links
 * [@lizardo](https://github.com/lizardo)'s [nRF8001 Experiments](https://github.com/lizardo/nrf8001)
   * used as a starting point to reverse engineer the proprietary setup message format for the chips
 * [@NordicSemiconductor](https://github.com/NordicSemiconductor)'s [ble-sdk-arduino](https://github.com/NordicSemiconductor/ble-sdk-arduino)
   * Original Arduino SDK for nRF8001
 * [@guanix](https://github.com/guanix)'s [arduino-nrf8001](https://github.com/guanix/arduino-nrf8001)
   * nRF8001 support for Arduino




