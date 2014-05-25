#ifndef __DS1820__H__
#define __DS1820__H__
#include <avr/io.h>
#include <util/delay.h>
#include "OneWire.h"

#define DS18B20_PORTIN ONEWIRE_PORTIN
#define DS18B20_PORT ONEWIRE_PORT
#define DS18B20_IO_CTL ONEWIRE_IO_CTL
#define DS18B20_PIN ONEWIRE_PIN

#define SPAD_TEMP_LSB   0
#define SPAD_TEMP_MSB   1
#define SPAD_ALARM_HIGH 2
#define SPAD_ALARM_LOW  3
#define SPAD_CONFIG     4
#define SPAD_RESERVED1  5
#define SPAD_RESERVED2  6
#define SPAD_RESERVED3  7
#define SPAD_CRC        8
#define SPAD_MAX_BYTES  9

#define CMD_1820_CONV_TEMP 0x44
#define CMD_1820_WRITE_SPAD 0x4E
#define CMD_1820_READ_SPAD 0xBE
#define CMD_1820_COPY_SPAD 0x48
#define CMD_1820_RECALL_E 0xB8
#define CMD_1820_READ_POWER_SUPPLY 0xB4

#define DS1820_FAMILY_CODE 0x10

uint8_t ds1820_check_spad_crc(uint8_t* spad);
uint8_t ds1820_read_spad(uint8_t* spad);
uint8_t ds1820_write_spad(uint8_t* spad);

#if ONEWIRE_USE_MACROS == 1
#define ds1820_read_temperature(temperature, error)\
{\
    temperature = 0;\
    if(onewire_select_device_and_issue_command(CMD_18B20_CONV_TEMP, DS1820_FAMILY_CODE) == EXIT_FAILURE)\
    {\
        error = EXIT_FAILURE;\
    }\
\
    while(1)\
    {\
        if(onewire_readbyte() != 0)\
        {\
            break;\
        }\
    }\
\
    uint8_t spad[SPAD_MAX_BYTES];\
    if(ds18b20_read_spad(&(spad[0])) == EXIT_FAILURE)\
    {\
        error = EXIT_FAILURE;\
    }\
\
    temperature = spad[SPAD_TEMP_MSB];\
    temperature <<= 8;\
    temperature |= spad[SPAD_TEMP_LSB];\
}

#define ds1820_set_resolution(resolution, store_to_eeprom, error)\
{\
    uint8_t spad[SPAD_MAX_BYTES];\
    if(ds18b20_read_spad(&(spad[0])) == EXIT_FAILURE)\
    {\
        error = EXIT_FAILURE;\
    }\
\
    uint8_t config = spad[SPAD_CONFIG];\
    if(((config >> 5) & 3) != resolution)\
    {\
        config &= ~(0x3 << 5);\
        config |= (resolution << 5);\
        spad[SPAD_CONFIG] = config;\
        if(ds18b20_write_spad(&(spad[0])) == EXIT_FAILURE)\
        {\
            error = EXIT_FAILURE;\
        }\
\
        if(store_to_eeprom != 0)\
        {\
            if(onewire_select_device_and_issue_command(CMD_18B20_COPY_SPAD, DS1820_FAMILY_CODE) == EXIT_FAILURE)\
            {\
                error = EXIT_FAILURE;\
            }\
        }\
    }\
}
#else
uint8_t ds1820_read_temperature(int16_t* temperature);
#endif // ONEWIRE_USE_MACROS

#endif // __DS18B20__H__
