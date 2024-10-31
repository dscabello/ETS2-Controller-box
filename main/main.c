#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_err.h"

#include "tasksHandler.h"
#include "ets2ctrl.h"
#include "wifiHandler.h"
#include "httpHandler.h"
#include "spiHandler.h"

void app_main(void)
{
    //ConnectToWifi();

    vTaskDelay(pdMS_TO_TICKS(1000));
    spiHandler();
    vTaskDelay(pdMS_TO_TICKS(10000));
    ESP_LOGI(TAG, "End");
    return;

    // Create tasks
    xTaskCreatePinnedToCore(MainThread, "Main_Thread", 4096, NULL, 2, NULL, tskNO_AFFINITY);

    xTaskCreate(&HttpMainTask, "Http_Main_Task", 8192, NULL, 5, NULL);
}