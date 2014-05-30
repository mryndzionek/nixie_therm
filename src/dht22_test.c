
#include "USI_TWI_Master.h"
#include "dht22.h"
#include <avr/io.h>
#include <util/delay.h>

#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/power.h>

#define BLUE_LED PD6
#define PWM_DIGIT_1 PB2
#define RED_LED PB1
#define BUTTON PB0

#define PCF8574_ADDRESS 0x38
#define WDT_MAX 4

volatile unsigned int wdt_count = WDT_MAX;
volatile unsigned int button = 0;

static void setup() {

    set_output(DDRD, BLUE_LED);
    output_low(PORTD, BLUE_LED);
    set_output(DDRB, RED_LED);
    output_low(PORTB, RED_LED);

    set_output(DDRB, PWM_DIGIT_1);
    TCCR0A = _BV(COM0A1) | _BV(WGM00);
    OCR0A  = 0xFF;
    TCCR0B = _BV(CS01) | _BV(CS00);

    set_input(DDRB, BUTTON);

    USI_TWI_Master_Initialise();
    init_DHT22();
}

static void dimm_digit(unsigned int level)
{
    if(level > 0xFF) level = 0xFF;
    OCR0A = level;
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

static void display_value(short int value)
{
    if(value < 0)
        {
            output_high(PORTB, BLUE_LED);
            value = -value;
        }
    else
        output_low(PORTB, BLUE_LED);

    display_number(value);
}

static void display_seq(short int value)
{
    display_value(value);
    dimm_digit(0xFF);
    _delay_ms(1000);
    dimm_digit(0xa0);
    _delay_ms(200);
    dimm_digit(0x10);
    _delay_ms(200);
    dimm_digit(0x00);
}

ISR(WDT_OVERFLOW_vect)
{
    wdt_count--;
}

ISR (PCINT_vect)
{
    button = 1;
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

    setup();

    GIMSK |= _BV (PCIE);
    PCMSK |= _BV (PCINT0);

    sei();
    while (1) {

            DHT22_ERROR_t rc = readData();
            if(rc == DHT_ERROR_NONE)
                {
                    display_seq(getTemperatureCInt()/10);
                    display_seq(getHumidityInt()/10);
                } else {
                        output_high(PORTB, RED_LED);
                        _delay_ms(20);
                        output_low(PORTB, RED_LED);
                }

            if(button)
                button = 0;

            while(wdt_count && (button == 0))
                sleep();

            wdt_count = WDT_MAX;
    }
}
