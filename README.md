# ESP_STK500

This is a port of the STK500 protocol for the ESP8266. The goal is to allow to program an Arduino from an ESP8266. The binary file could come from any place (SPIFFS, Web) and then the ESP takes care and communicates with the bootloader of the arduino to upload the file

The wiring is simple. One wire is connected to the arduino RST pin, and then two others from some free GPIO ports on the ESP to the RX and TX pins on the arduino.

A smillar implementation for java can be found here
https://github.com/Cloudino/Cloudino-engine/blob/master/src/main/java/io/cloudino/utils/HexSender.java
This is where i looked up the commands for the STK500 protocol (As well as the actual bootloader source of course)
