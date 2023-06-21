#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_event.h>
#include <esp_netif.h>
#include <esp_log.h>

#include "dht.h"
#include "net.h"

static const char *tag = "app";
static const char *topic = "weather-data";

void app_main()
{
	char data[5];
	uint16_t rh, tc;
	esp_mqtt_client_handle_t client;

	esp_netif_init();
	esp_event_loop_create_default();

	dht_init();
	wifi_connect();
	client = mqtt_connect();

	for (;;) {
		if (dht_get_data((uint8_t *)data)) {
			rh = dht_decode_data(data[0], data[1]);
			tc = dht_decode_data(data[2], data[3]);
			esp_mqtt_client_publish(client, topic, data, 4, 0, 0);
			ESP_LOGI(tag, "temperature: %dC, humidity: %d%%", tc, rh);
		}
		vTaskDelay(2000 / portTICK_PERIOD_MS);
	}
}
