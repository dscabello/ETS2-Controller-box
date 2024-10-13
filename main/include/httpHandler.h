#ifndef HTTPHANDLER_H
#define HTTPHANDLER_H

#include <stdio.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_http_client.h"
#include "esp_event.h"

#include "ets2ctrl.h"
#include "wifiHandler.h"

extern bool RequestData;

extern void HttpMainTask(void *pvParameters);

#endif // HTTPHANDLER_H