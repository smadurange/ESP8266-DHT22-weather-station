#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/event_groups.h"

#include <esp_log.h>
#include <esp_wifi.h>

#include "net.h"

#define WIFI_CONNECTED_BIT BIT0

static EventGroupHandle_t wifi_evt_group;
static const char *tag = "wifi";

static void wifi_evt_handler(void *arg, esp_event_base_t eb, int32_t id, void *data)
{
	ip_event_got_ip_t *event = (ip_event_got_ip_t *) data;
	ESP_LOGI(tag, "ip: %s", ip4addr_ntoa(&event->ip_info.ip));
	xEventGroupSetBits(wifi_evt_group, WIFI_CONNECTED_BIT);
}

void wifi_connect(void)
{
    wifi_evt_group = xEventGroupCreate();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_WIFI_SSID,
            .password = CONFIG_WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK
        },
    };
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    esp_wifi_set_mode(WIFI_MODE_STA);

    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_evt_handler, 0);

    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());

    xEventGroupWaitBits(wifi_evt_group,
        WIFI_CONNECTED_BIT,
        pdFALSE,
        pdFALSE,
        portMAX_DELAY);
}
