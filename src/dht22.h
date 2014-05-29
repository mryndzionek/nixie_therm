#ifndef _DHT22_H_
#define _DHT22_H_

#define DHT22_ERROR_VALUE -995

typedef enum
{
    DHT_ERROR_NONE = 0,
    DHT_BUS_HUNG,
    DHT_ERROR_NOT_PRESENT,
    DHT_ERROR_ACK_TOO_LONG,
    DHT_ERROR_SYNC_TIMEOUT,
    DHT_ERROR_DATA_TIMEOUT,
    DHT_ERROR_CHECKSUM,
    DHT_ERROR_TOOQUICK
} DHT22_ERROR_t;

void init_DHT22();
DHT22_ERROR_t readData();
short int getHumidityInt();
short int getTemperatureCInt();
void clockReset();

#endif /*_DHT22_H_*/
