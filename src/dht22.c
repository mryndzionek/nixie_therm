#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "dht22.h"

#define DHT22_DATA_PIN PB4

static volatile uint32_t millisec = 0;

struct dht22_t {
    unsigned long _lastReadTime;
    short int _lastHumidity;
    short int _lastTemperature;
};

struct dht22_t sensor;

#define DHT22_DATA_BIT_COUNT 41


ISR(TIMER1_COMPA_vect)
{
    ++millisec;
}

void init_DHT22()
{
    TCCR1A &= ~(_BV(WGM11) | _BV(WGM10));
    TCCR1B &= ~_BV(WGM13);
    TCCR1B |=  _BV(WGM12) | _BV(CS12);
    TIMSK  |= _BV(OCIE1A);
    OCR1A =  F_CPU/256/1000;

    sensor._lastHumidity = DHT22_ERROR_VALUE;
    sensor._lastTemperature = DHT22_ERROR_VALUE;
}

DHT22_ERROR_t readData()
{
    uint8_t retryCount;
    uint8_t bitTimes[DHT22_DATA_BIT_COUNT];
    int currentHumidity;
    int currentTemperature;
    uint8_t checkSum, csPart1, csPart2, csPart3, csPart4;
    unsigned long currentTime;
    int i;

    currentHumidity = 0;
    currentTemperature = 0;
    checkSum = 0;
    currentTime = millisec;
    for(i = 0; i < DHT22_DATA_BIT_COUNT; i++)
            bitTimes[i] = 0;

    if(currentTime - sensor._lastReadTime < 2000)
        {
            // Caller needs to wait 2 seconds between each call to readData
            return DHT_ERROR_TOOQUICK;
        }
    sensor._lastReadTime = currentTime;

    set_input(DDRB, DHT22_DATA_PIN);

    retryCount = 0;
    do
        {
            if (retryCount > 125)
                {
                    return DHT_BUS_HUNG;
                }
            retryCount++;
            _delay_us(2);
        } while(bit_is_clear(PINB, DHT22_DATA_PIN));

    set_output(DDRB, DHT22_DATA_PIN);
    output_low(PORTB, DHT22_DATA_PIN);
    _delay_us(900);
    set_input(DDRB, DHT22_DATA_PIN);

    retryCount = 0;
    do
        {
            if (retryCount > 25)
                {
                    return DHT_ERROR_NOT_PRESENT;
                }
            retryCount++;
            _delay_us(2);
        } while(bit_is_clear(PINB, DHT22_DATA_PIN));

    retryCount = 0;
    do
        {
            if (retryCount > 50) //(Spec is 80 us, 50*2 == 100 us)
                {
                    return DHT_ERROR_ACK_TOO_LONG;
                }
            retryCount++;
            _delay_us(2);
        } while(bit_is_set(PINB, DHT22_DATA_PIN));

    for(i = 0; i < DHT22_DATA_BIT_COUNT; i++)
        {
            // Find the start of the sync pulse
            retryCount = 0;
            do
                {
                    if (retryCount > 35) //(Spec is 50 us, 35*2 == 70 us)
                        {
                            return DHT_ERROR_SYNC_TIMEOUT;
                        }
                    retryCount++;
                    _delay_us(2);
                } while(bit_is_clear(PINB, DHT22_DATA_PIN));
            // Measure the width of the data pulse
            retryCount = 0;
            do
                {
                    if (retryCount > 50) //(Spec is 80 us, 50*2 == 100 us)
                        {
                            return DHT_ERROR_DATA_TIMEOUT;
                        }
                    retryCount++;
                    _delay_us(2);
                } while(bit_is_set(PINB, DHT22_DATA_PIN));
            bitTimes[i] = retryCount;
        }

    for(i = 0; i < 16; i++)
        {
            if(bitTimes[i + 1] > 11)
                {
                    currentHumidity |= (1 << (15 - i));
                }
        }
    for(i = 0; i < 16; i++)
        {
            if(bitTimes[i + 17] > 11)
                {
                    currentTemperature |= (1 << (15 - i));
                }
        }
    for(i = 0; i < 8; i++)
        {
            if(bitTimes[i + 33] > 11)
                {
                    checkSum |= (1 << (7 - i));
                }
        }

    sensor._lastHumidity = currentHumidity & 0x7FFF;
    if(currentTemperature & 0x8000)
        {
            // Below zero, non standard way of encoding negative numbers!
            // Convert to native negative format.
            sensor._lastTemperature = -currentTemperature & 0x7FFF;
        }
    else
        {
            sensor._lastTemperature = currentTemperature;
        }

    csPart1 = currentHumidity >> 8;
    csPart2 = currentHumidity & 0xFF;
    csPart3 = currentTemperature >> 8;
    csPart4 = currentTemperature & 0xFF;
    if(checkSum == ((csPart1 + csPart2 + csPart3 + csPart4) & 0xFF))
        {
            return DHT_ERROR_NONE;
        }
    return DHT_ERROR_CHECKSUM;
}

inline short int getHumidityInt()
{
    return sensor._lastHumidity;
}

inline short int getTemperatureCInt()
{
    return sensor._lastTemperature;
}

void clockReset()
{
    sensor._lastReadTime = millisec;
}
