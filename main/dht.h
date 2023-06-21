#ifndef DHT_H
#define DHT_H

void dht_init(void);

int dht_get_data(uint8_t result[5]);

int16_t dht_decode_data(uint8_t msb, uint8_t lsb);

#endif
