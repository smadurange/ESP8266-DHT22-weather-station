#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/event_groups.h"

#include <esp_event.h>
#include <esp_netif.h>
#include <esp_log.h>

#include "net.h"

#define MQTT_CONNECTED_BIT BIT0

static const char *tag = "mqtt";

static EventGroupHandle_t mqtt_evt_group;

static void mqtt_evt_handler(void *args, esp_event_base_t eb, int32_t evt_id, void *data)
{
	esp_mqtt_event_handle_t evt = (esp_mqtt_event_handle_t) data;
	switch (evt_id) {
		case MQTT_EVENT_CONNECTED:
			ESP_LOGI(tag, "MQTT_EVENT_CONNECTED");
			xEventGroupSetBits(mqtt_evt_group, MQTT_CONNECTED_BIT);
			break;
		case MQTT_EVENT_PUBLISHED:
			ESP_LOGI(tag, "MQTT_EVENT_PUBLISHED, msg_id=%d", evt->msg_id);	
			break;
		default:
			ESP_LOGI(tag, "MQTT event id:%d", evt->event_id);
			break;
	}
}

esp_mqtt_client_handle_t mqtt_connect(void)
{
	esp_mqtt_client_handle_t client = NULL;

	const esp_mqtt_client_config_t cfg = {
		.host = CONFIG_BROKER_HOST,
		.username = CONFIG_BROKER_USERNAME,
		.password = CONFIG_BROKER_PASSWORD,
		.client_id = CONFIG_BROKER_CLIENT_ID,
		.transport = MQTT_TRANSPORT_OVER_SSL,
		.protocol_ver = MQTT_PROTOCOL_V_3_1_1
	};

	if (!(client = esp_mqtt_client_init(&cfg))) {
		ESP_LOGI(tag, "esp_mqtt_client_init() failed");
		return NULL;
	}
	
	ESP_LOGI(tag, "MQTT broker: %s", cfg.host);

	esp_mqtt_client_register_event(client,
		ESP_EVENT_ANY_ID,
		mqtt_evt_handler,
		client);

	mqtt_evt_group = xEventGroupCreate();
	ESP_ERROR_CHECK(esp_mqtt_client_start(client));

	xEventGroupWaitBits(mqtt_evt_group,
		MQTT_CONNECTED_BIT,
		pdFALSE,
		pdFALSE,
		portMAX_DELAY);

	return client;
}
