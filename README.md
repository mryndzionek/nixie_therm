Nixie 1-wire thermometer
========================

![nixie_therm](images/therm.png?raw=true "Nixie thermometer")

Introduction
------------

Simple 1-wire thermometer with two-digit nixie display 

How to build
------------

1. Configure the project:

		cd nixie_therm
		mkdir build
		cd build
		cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-avr-gcc.make -DCMAKE_BUILD_TYPE=MinSizeRel ..

Feel free to edit the file `toolchain-avr-gcc.make` and set the programmer type and other options.

2. Build the project:

		make

3. Upload the hex file to the MCU:

		make upload_nixie

Hardware setup
--------------

Attiny2313 on a perfboard. Internal RC oscilator - 8Mhz.
Fuses set to: 
		`-U lfuse:w:0xe4:m -U hfuse:w:0x9f:m -U efuse:w:0xff:m`.

ISP header at the back of the perfboard for easy reprogramming.
The DS1820 sensor connected using 3-wire tape for external temperature measurements.
AM2302 (DHT22) sensor on the main perfboard - internal temperature and humidity measurements.
Nixies on a separate board together with PCF8574 GPIO expander and two russian K155ID1 BCD decoders.
It comprises a module that communicates with the MCU only via I2C bus so it is possible to add up to 8 modules (16 nixie lamps !!!).
The HV PSU is based on [this](http://threeneurons.files.wordpress.com/2011/08/mc34063_mk15cm1.gif?w=500&h=384) schematic.
It uses the MC34063 switching regulator with external MOSFET and an active pull-down.
The nixies can be dimmed using MPSA92+MPSA42 anode drivers.

| Pin  | Peripheral|
| ------------- | -------------|
| PD3  | DS1820 1-wire data pin |
| PD6  | blue LED - negative external temperatures |
| PB0  | button - manual measurement trigger |
| PB1  | red LED - sensor readout error indicator |
| PB2  | nixie PWM dimming pin |
| PB4  | AM3203 data pin |
| PB5  | I2C SDA |
| PB7  | I2C SCL |


Contact
-------
If you have questions, contact Mariusz Ryndzionek at:

<mryndzionek@gmail.com>
