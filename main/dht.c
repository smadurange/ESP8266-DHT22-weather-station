// Copyright (c) 2015, SuperHouse Automation Pty Ltd
// Copyright (c) 2016, Jonathan Hartsuiker (https://github.com/jsuiker)
// Copyright (c) 2019, Fonger (https://github.com/fonger)

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <driver/gpio.h>
#include <esp8266/gpio_struct.h>
#include <rom/ets_sys.h>
#include <esp_log.h>

#include "dht.h"

#define DHT_PIN GPIO_NUM_2 
#define DHT_DATA_LEN 40

static const char *tag = "dht";

static inline int dht_get_pin_state()
{
	return (GPIO.in >> DHT_PIN) & 0x1;
}

static inline int dht_await_pin_state(int state, int timeout)
{
	int t;
	static const uint16_t delta = 1;

	for (t = 0; t < timeout; t += delta) {
		os_delay_us(delta);
		if (dht_get_pin_state() == state)
			return t;
	}
	return 0;
}

static inline int dht_get_raw_data(uint8_t buf[DHT_DATA_LEN])
{
	uint8_t i, pwl, pwh;

	gpio_set_level(DHT_PIN, 0);
	os_delay_us(1100);
	gpio_set_level(DHT_PIN, 1);

	if (!dht_await_pin_state(0, 40)) {
		ESP_LOGI(tag, "start sequence phase 1 error");
		return 0;
	}
	if (!dht_await_pin_state(1, 80)) {
		ESP_LOGI(tag, "start sequence phase 2 error");
		return 0;
	}
	if (!dht_await_pin_state(0, 80)) {
		ESP_LOGI(tag, "start sequence phase 3 error");
		return 0;
	}

	for (i = 0; i < DHT_DATA_LEN; i++) {
		if (!(pwl = dht_await_pin_state(1, 50))) {
			ESP_LOGI(tag, "low bit timed out");
			return 0;
		}
		if (!(pwh = dht_await_pin_state(0, 70))) {
			ESP_LOGI(tag, "high bit timed out");
			return 0;
		}
		buf[i] = pwh > pwl;
	}
	return 1;
}

static inline int dht_verify_checksum(const uint8_t data[5])
{
	return data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF);
}

int16_t dht_decode_data(uint8_t msb, uint8_t lsb)
{
	int16_t x;

	x = msb & 0x7F; 
	x <<= 8;
	x |= lsb;
	if (msb >> 7)
		x = 0 - x;
	return x;
} 

int dht_get_data(uint8_t result[5])
{
	uint8_t i, rc, buf[DHT_DATA_LEN];
	
	vPortETSIntrLock();
	rc = dht_get_raw_data(buf);
	vPortETSIntrUnlock();
	
	if (rc) {
		for (i = 0; i < DHT_DATA_LEN; i++) {
			result[i / 8] <<= 1;
			result[i / 8] |= buf[i];
		}
		if (dht_verify_checksum(result))
			return 1;
		else
			ESP_LOGI(tag, "checksum failed");
	}
	return 0;
}

void dht_init(void)
{
	gpio_config_t io_conf;

	io_conf.pin_bit_mask = 1UL << DHT_PIN;
	io_conf.mode = GPIO_MODE_OUTPUT_OD;
	gpio_config(&io_conf);
	gpio_set_level(DHT_PIN, 1);

	vTaskDelay(1000 / portTICK_PERIOD_MS);
}
