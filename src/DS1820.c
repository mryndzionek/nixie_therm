
#include "DS1820.h"

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
