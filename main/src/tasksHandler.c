#include "tasksHandler.h"


void MainThread(void *arg) {
    printf("Start MainTask thread.\n");
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));
        if (RequestData == false)
        {
            printf("\nRequest Data\n");
            RequestData = true;
        }
    }
}
