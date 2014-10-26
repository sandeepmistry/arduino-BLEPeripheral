# Arduino BLEPeripheral

[![Analytics](https://ga-beacon.appspot.com/UA-56089547-1/sandeepmistry/arduino-BLEPeripheral?pixel)](https://github.com/igrigorik/ga-beacon)

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

### [Nordic Semiconductor nRF51822](http://www.nordicsemi.com/eng/Products/Bluetooth-R-low-energy/nRF51822)

 * [RedBearLab](http://redbearlab.com)
   * [nRF51822](http://redbearlab.com/redbearlab-nrf51822)
   * [BLE Nano](http://redbearlab.com/blenano/)

#### Pinouts

| Shield/Board | REQ Pin | RDY Pin | RST Pin |
| ------------ | ------- | ------- | ------- |
| Bluefruit LE | 10 | 2 | 9 |
| BLE Shield 1.x | 9 | 8 | UNUSED |
| BLE Shield 2.x | 9 | 8 | UNUSED or 4/7 via jumper|
| Blend | 9 | 8 | UNUSED or 5/6 via jumper |
| Blend Micro | 6 | 7 | UNUSED or 4 |
| RedBearLab nRF51822 | UNUSED | UNUSED | UNUSED |
| BLE Nano | UNUSED | UNUSED | UNUSED |

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




