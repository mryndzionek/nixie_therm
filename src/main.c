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

static void blink(int n)
{
    int i;
    for(i=0; i<n;i++)
        {
            output_high(PORTD, LED);
            _delay_ms(100);
            // now turn off the LED for another 200ms
            output_low(PORTD, LED);
            _delay_ms(200);
        }
}

int main(void) {

    unsigned char data[2];

    setup();

    data[0] = PCF8574_ADDRESS << 1;
    data[1] = 0x11;
    USI_TWI_Start_Read_Write(data, 2);
    int s = USI_TWI_Get_State_Info();

    while (1) {
            blink(s);
            _delay_ms(1000);
    }
}
