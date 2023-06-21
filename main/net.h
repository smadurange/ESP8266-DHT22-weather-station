#ifndef NET_H
#define NET_H

#include <mqtt_client.h>

void wifi_connect(void);

esp_mqtt_client_handle_t mqtt_connect(void);

#endif
