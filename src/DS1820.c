
#include "DS1820.h"
#include <util/crc16.h>

uint8_t ds1820_check_spad_crc(uint8_t* spad)
{
    uint16_t crc = 0;
    uint8_t i;
    for(i = 0; i < SPAD_MAX_BYTES; ++i)
    {
        crc = _crc_ibutton_update(crc, spad[i]);
    }

    return (crc == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}


uint8_t ds1820_read_spad(uint8_t* spad)
{
    if(onewire_select_device_and_issue_command(CMD_1820_READ_SPAD, DS1820_FAMILY_CODE) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    uint8_t i;
    for(i = 0; i < SPAD_MAX_BYTES; i++)
    {
        spad[i] = onewire_readbyte();
    }

#ifdef ONEWIRE_CHECK_CRC
    return ds1820_check_spad_crc(spad);
#else
    return EXIT_SUCCESS;
#endif // ONEWIRE_CHECK_CRC
}

uint8_t ds1820_write_spad(uint8_t* spad)
{
    if(onewire_select_device_and_issue_command(CMD_1820_WRITE_SPAD, DS1820_FAMILY_CODE) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    onewire_writebyte(spad[SPAD_ALARM_HIGH]);
    onewire_writebyte(spad[SPAD_ALARM_LOW]);
    onewire_writebyte(spad[SPAD_CONFIG]);

    return EXIT_SUCCESS;
}

#if ONEWIRE_USE_MACROS == 0
uint8_t ds1820_read_temperature(int16_t* temperature)
{
    *temperature = 0;
    if(onewire_select_device_and_issue_command(CMD_1820_CONV_TEMP, DS1820_FAMILY_CODE) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    while(1)
    {
        const uint8_t done = onewire_readbyte();

        // There might be a 1 bit anywhere in the output
        // so just check != 0
        if(done != 0)
        {
            break;
        }
    }

    uint8_t spad[SPAD_MAX_BYTES];
    if(ds1820_read_spad(&(spad[0])) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    *temperature = spad[SPAD_TEMP_MSB];
    *temperature <<= 8;
    *temperature |= spad[SPAD_TEMP_LSB];

    return EXIT_SUCCESS;
}
#endif //ONEWIRE_USE_MACROS
