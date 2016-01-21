# Examples

* [led](led)
  * read/write characteristic to turn an LED on/off - polling style
* [led_callback](led_callback)
  * read/write characteristic to turn an LED on/off - callback style
* [led_switch](led_switch)
  * read/write characteristic + notify characteristic to turn an LED on/off and button status
* [physical_web_beacon](physical_web_beacon)
  * broadcast [physical web](http://physical-web.org) beacon
* [test](test)
  * read/write/notify characteristic for counter
* [serial](serial)
  * serial port service compatible with following applications:
    - nRF Toolbox - [iOS](https://itunes.apple.com/us/app/nrf-toolbox/id820906058?mt=8) | [Android](https://play.google.com/store/apps/details?id=no.nordicsemi.android.nrftoolbox)
    - nRF UART - [iOS](https://itunes.apple.com/us/app/nrf-uart/id614594903?mt=8) | [Android](https://play.google.com/store/apps/details?id=com.nordicsemi.nrfUARTv2)
    - Bluefruit LE - [iOS](https://itunes.apple.com/us/app/adafruit-bluefruit-le-connect/id830125974?mt=8) | [Android](https://play.google.com/store/apps/details?id=com.adafruit.bluefruit.le.connect)

# RedBearLab - BLE Controller App

The following examples work with the [RedBearLab](http://redbearlab.com) BLE Controller [iOS](https://itunes.apple.com/us/app/ble-controller/id855062200?mt=8)/[Android](https://play.google.com/store/apps/details?id=com.redbear.redbearbleclient&hl=en) app and were provided by [RedBearLab](http://redbearlab.com).

  * [BLEControllerSketch](RedBearLab/BLEControllerSketch)
    * reports Arduino's pin mode and pin state
  * [SimpleChat](RedBearLab/SimpleChat)
    * Two way chat example from Arduino serial monitor to/from app
  * [SimpleControls](RedBearLab/SimpleControls)
    * Use app to configure and control Arduino pins as: DIGITAL_IN, DIGITAL_OUT, PWM, SERVO, ANALOG_IN
