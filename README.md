# ESP8266-DHT22 Weather Station

A pure C implementation of a weather station using an ESP-01 and a DHT22 sensor. Special thanks to [Fonger](https://github.com/Fonger/ESP8266-RTOS-DHT) for the DHT22 protocol implementation.

## Prerequisites

 - ESP-01 module (or any later versions of an ESP8266 module)
 - [DHT22 sensor module](https://www.sparkfun.com/datasheets/Sensors/Temperature/DHT22.pdf)
 - [ESP8266 RTOS SDK](https://github.com/espressif/ESP8266_RTOS_SDK)

## Building the project

 - Rename `Kconfig.projbuild.example` to `Kconfig.projbuild`.
 - Update configuration settings for your WiFi AP and the MQTT broker in the `Kconfig.projbuild`.
 - Run `make flash` to build and upload the program to ESP8266.
