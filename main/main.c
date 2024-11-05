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
    ConnectToWifi();
    /* Create  Semaphors*/
    LedSemaphore = xSemaphoreCreateBinary();
    if (LedSemaphore != NULL)
        xSemaphoreGive(LedSemaphore);
    HTTPSemaphore = xSemaphoreCreateBinary();
    if (HTTPSemaphore != NULL)
        xSemaphoreGive(HTTPSemaphore);

    WifiSemaphore = xSemaphoreCreateBinary();
    if (WifiSemaphore != NULL)
        xSemaphoreGive(WifiSemaphore);

    // Create tasks
    vTaskDelay(pdMS_TO_TICKS(2000));
    spiHandler();
    xTaskCreate(&ledHandler, "Led Handler", 4096, NULL, 3, NULL);
    xTaskCreatePinnedToCore(MainThread, "Main_Thread", 4096, NULL, 2, NULL, tskNO_AFFINITY);
    xTaskCreate(&HttpMainTask, "Http_Main_Task", 8192, NULL, 5, NULL);
    ESP_LOGI(TAG, "End");

}