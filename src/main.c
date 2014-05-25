#include "USI_TWI_Master.h"
#include <avr/io.h>
#include <util/delay.h>

#define LED PD6
#define PWM_DIGIT_1 PB2
#define PWM_DIGIT_2 PB3
#define PCF8574_ADDRESS 0x38

#define output_low(port,pin) port &= ~(1<<pin)
#define output_high(port,pin) port |= (1<<pin)
#define set_input(portdir,pin) portdir &= ~(1<<pin)
#define set_output(portdir,pin) portdir |= (1<<pin)

static void setup() {
    set_output(DDRD, LED);
    set_output(DDRB, PWM_DIGIT_1);
    set_output(DDRB, PWM_DIGIT_2);
    output_high(PORTB, PWM_DIGIT_1);
    output_high(PORTB, PWM_DIGIT_2);

    USI_TWI_Master_Initialise();
}

static void dimm_digit(unsigned int digit, unsigned int level)
{

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

    setup();

    while (1) {
            display_number(n++);
            if(n > 99)
                n = 0;
            _delay_ms(100);
    }
}
