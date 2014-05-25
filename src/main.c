#include "USI_TWI_Master.h"
#include <avr/io.h>
#include <util/delay.h>

#define LED PD6
#define PWM_DIGIT_1 PB2
#define PWM_DIGIT_2 PB3
#define BUTTON PB0

#define PCF8574_ADDRESS 0x38

#define output_low(port,pin) port &= ~(1<<pin)
#define output_high(port,pin) port |= (1<<pin)
#define set_input(portdir,pin) portdir &= ~(1<<pin)
#define set_output(portdir,pin) portdir |= (1<<pin)

static const unsigned char DIM_SCALE[] = { 0x05, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11,
        0x12, 0x13, 0x20, 0x25, 0x35, 0x40, 0x45, 0x50,
        0x55, 0x60, 0x65, 0x70, 0x75, 0x80, 0x90, 0xa0,
        0xb0, 0xc0, 0xd0, 0xe0, 0xf0, 0xff, 0xff, 0xff
};

static void setup() {

    set_output(DDRD, LED);
    set_output(DDRB, PWM_DIGIT_1);
    TCCR0A = _BV(COM0A1) | _BV(WGM00);
    OCR0A  = 0xFF;
    TCCR0B = _BV(CS01) | _BV(CS00);

    set_output(DDRB, PWM_DIGIT_2);
    TCCR1A = (1 << COM1A1)| (1 << WGM11) | (1 << WGM10);
    OCR1A = 0x3FF;
    TCCR1B = _BV(CS10) | _BV(CS11);


    set_input(DDRB, BUTTON);

    USI_TWI_Master_Initialise();
}

static void dimm_digit(unsigned int digit, unsigned int level)
{
    if(level > 0xFF) level = 0xFF;

    unsigned char scaled_level = DIM_SCALE[level >> 3];

    if(digit == 0)
        OCR0A = scaled_level;
    else {
        OCR1A = scaled_level << 2;
    }

}

static unsigned int display_number(unsigned int n) {

    unsigned char data[2];
    unsigned int digit_1 = 0;
    unsigned int digit_2 = 0;

    if(n > 99)
        n = 99;

    digit_1 = n % 10;
    digit_2 = n / 10;

    if (digit_2 == 8) digit_2 = 9;
    else if (digit_2 == 9) digit_2 = 8;

    data[0] = PCF8574_ADDRESS << 1;
    data[1] = (digit_1 << 4) | (digit_2);

    USI_TWI_Start_Read_Write(data, 2);

    return  USI_TWI_Get_State_Info();
}

int main(void) {

    int n = 0;
    unsigned int level = 0;

    setup();

    while (1) {
            display_number(n++);

            dimm_digit(0, level);
            dimm_digit(1, level);
            level++;
            if(level > 0xFF)
                level = 0;
            if(n > 99)
                n = 0;
            _delay_ms(100);
    }
}
