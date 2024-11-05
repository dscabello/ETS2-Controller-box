#include "tasksHandler.h"


void MainThread(void *arg) {
    printf("Start MainTask thread.\n");
    while (1) {
        HttpRequestData();
        //TODO: Interprete data and use in the leds
        vTaskDelay(pdMS_TO_TICKS(200));
        if (TruckInfo_gear == 0)
            ledCmdTurnOnBlink(CMD_LED_13);
        else
            ledCmdTurnOFFBlink(CMD_LED_13);
    }
}
