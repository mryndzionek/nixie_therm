
#include "USI_TWI_Master.h"
#include "DS1820.h"
#include <avr/io.h>
#include <util/delay.h>

#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/power.h>

#define BLUE_LED PD6
#define PWM_DIGIT_1 PB2
#define PWM_DIGIT_2 PB3
#define RED_LED PB1
#define BUTTON PB0

#define PCF8574_ADDRESS 0x38

#define output_low(port,pin) port &= ~(1<<pin)
#define output_high(port,pin) port |= (1<<pin)
#define set_input(portdir,pin) portdir &= ~(1<<pin)
#define set_output(portdir,pin) portdir |= (1<<pin)

static void setup() {

    set_output(DDRD, BLUE_LED);
    output_low(PORTD, BLUE_LED);
    set_output(DDRB, RED_LED);
    output_low(PORTB, RED_LED);

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

    if(digit == 0)
        OCR0A = level;
    else {
            OCR1A = level << 2;
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

    USI_TWI_Start_Transceiver_With_Data(data, 2);

    return  USI_TWI_Get_State_Info();
}

static void display_temp(int16_t temp)
{
    if((temp>>8) == 0xFF)
        {
            temp = ((~temp) & 0xFF) + 1;
            output_high(PORTB, BLUE_LED);
        } else
            output_low(PORTB, BLUE_LED);

    display_number(temp>>1);
}

ISR(WDT_OVERFLOW_vect)
{
    PORTB ^= _BV(RED_LED);
}

void sleep()
{
    WDTCSR |= (1<<WDP3) | (1<<WDP0);
    WDTCSR |= (1<<WDIE);
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();
    sleep_cpu();

    //wake up
    sleep_disable();
    MCUSR = 0;
}

int main(void) {

    int16_t temp;

    setup();

    sei();
    while (1) {

            ds1820_read_temperature(&temp);
            display_temp(temp);
            dimm_digit(0, 0xFF);
            dimm_digit(1, 0xFF);
            _delay_ms(500);
            dimm_digit(0, 0xa0);
            dimm_digit(1, 0xa0);
            _delay_ms(200);
            dimm_digit(0, 0x10);
            dimm_digit(1, 0x10);
            _delay_ms(200);
            dimm_digit(0, 0x00);
            dimm_digit(1, 0x00);

            sleep();
    }
}
