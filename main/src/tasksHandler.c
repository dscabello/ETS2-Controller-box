#include "tasksHandler.h"




void MainThread(void *arg) {
    Telemetry_t Telemetry_tmp;
    printf("Start MainTask thread.\n");
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(100));
        HttpRequestData();
        Telemetry_tmp = GetTelemetry();
        // if (Telemetry_tmp.game.connected) {
            if (Telemetry_tmp.value_update) {
                if (Telemetry_tmp.truck.lightsParkingOn)
                   ledBackLightOn();
                else
                    ledBackLightOFF();
                if (Telemetry_tmp.truck.lightsBeamLowOn) {
                    ledCmdTurnOn(CMD_LED_02);
                    if (Telemetry_tmp.truck.lightsBeamHighOn)
                        ledCmdTurnOn(CMD_LED_03);
                } else {
                    ledCmdTurnOFF(CMD_LED_02 | CMD_LED_03);
                }
                if (Telemetry_tmp.truck.wipersOn)
                    ledCmdTurnOnBlink(CMD_LED_04 | CMD_LED_07);
                else
                    ledCmdTurnOFFBlink(CMD_LED_04 | CMD_LED_07);

                if (Telemetry_tmp.trailer.attached)
                    ledCmdTurnOnBlink(CMD_LED_15);
                else
                    ledCmdTurnOFFBlink(CMD_LED_15);

                if ((Telemetry_tmp.truck.blinkerLeftOn) && (Telemetry_tmp.truck.blinkerRightOn) )
                    ledCmdTurnOnBlink(CMD_LED_08);
                else
                    ledCmdTurnOFFBlink(CMD_LED_08);

                if (Telemetry_tmp.truck.gear < 0) {
                    ledCmdTurnOnBlink(CMD_LED_10);
                    ledCmdTurnOFFBlink(CMD_LED_13);
                } else if (Telemetry_tmp.truck.gear == 0) {
                    ledCmdTurnOnBlink(CMD_LED_13);
                    ledCmdTurnOFFBlink(CMD_LED_10);
                } else {
                    ledCmdTurnOFFBlink(CMD_LED_10 | CMD_LED_13);
                }
        //     } else
        //         printf(">>>> paused <<<.\n");
        // } else {
        //     ledCmdTurnOFFBlink(CMD_LED_ALL);
        //     ledCmdTurnOFF(CMD_LED_ALL);
        }

    }
}
