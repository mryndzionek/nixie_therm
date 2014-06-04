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


Contact
-------
If you have questions, contact Mariusz Ryndzionek at:

<mryndzionek@gmail.com>
