#include "tasksHandler.h"


void MainThread(void *arg) {
    printf("Start MainTask thread.\n");
    while (1) {
        printf("\n\nRunning the task");

        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
