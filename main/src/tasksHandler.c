#include "tasksHandler.h"

void PrintLogOnUpdate(Telemetry_t Telemetry_new)
{
    static Telemetry_t Telemetry_old;
    if ((Telemetry_new.game.connected != Telemetry_old.game.connected)
        // || (Telemetry_new.value_update != Telemetry_old.value_update)
        || (Telemetry_new.truck.lightsParkingOn != Telemetry_old.truck.lightsParkingOn)
        || (Telemetry_new.truck.lightsBeamLowOn != Telemetry_old.truck.lightsBeamLowOn)
        || (Telemetry_new.truck.lightsBeamHighOn != Telemetry_old.truck.lightsBeamHighOn)
        || (Telemetry_new.truck.wipersOn != Telemetry_old.truck.wipersOn)
        || (Telemetry_new.trailer.attached != Telemetry_old.trailer.attached)
        || (Telemetry_new.truck.blinkerLeftOn != Telemetry_old.truck.blinkerLeftOn)
        || (Telemetry_new.truck.blinkerRightOn != Telemetry_old.truck.blinkerRightOn)
        || (Telemetry_new.truck.gear != Telemetry_old.truck.gear) )
    {
        Telemetry_old = Telemetry_new;
        ESP_LOGI(TAG,"Telemetry connected:          %d.", Telemetry_old.game.connected);
        ESP_LOGI(TAG,"Telemetry paused:             %d.", Telemetry_old.game.paused);
        ESP_LOGI(TAG,"Telemetry time:               %s.", Telemetry_old.game.time);
        ESP_LOGI(TAG,"Telemetry gear:               %d.", (int)(Telemetry_old.truck.gear));
        ESP_LOGI(TAG,"Telemetry wipersOn:           %d.", Telemetry_old.truck.wipersOn);
        ESP_LOGI(TAG,"Telemetry lightsParkingOn:    %d.", Telemetry_old.truck.lightsParkingOn);
        ESP_LOGI(TAG,"Telemetry lightsBeamLowOn:    %d.", Telemetry_old.truck.lightsBeamLowOn);
        ESP_LOGI(TAG,"Telemetry lightsBeamHighOn:   %d.", Telemetry_old.truck.lightsBeamHighOn);
        ESP_LOGI(TAG,"Telemetry blinkerLeftOn:      %d.", Telemetry_old.truck.blinkerLeftOn);
        ESP_LOGI(TAG,"Telemetry blinkerRightOn:     %d.", Telemetry_old.truck.blinkerRightOn);
        ESP_LOGI(TAG,"Telemetry attached:           %d.", Telemetry_old.trailer.attached);
        ESP_LOGI(TAG,"---------------------------------");
    }
}

void MainThread(void *arg) {
    Telemetry_t Telemetry_tmp;
    printf("Start MainTask thread.\n");
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(100));
        HttpRequestData();
        Telemetry_tmp = GetTelemetry();
        if (Telemetry_tmp.game.connected) {
            if (Telemetry_tmp.value_update) {
                if (Telemetry_tmp.truck.lightsParkingOn)
                   ledBackLightOn();
                else
                    ledBackLightOFF();
                if (Telemetry_tmp.truck.lightsBeamLowOn)
                    ledCmdTurnOn(CMD_LED_02);
                else
                    ledCmdTurnOFF(CMD_LED_02);
                if (Telemetry_tmp.truck.lightsBeamHighOn)
                    ledCmdTurnOn(CMD_LED_03);
                else
                    ledCmdTurnOFF(CMD_LED_03);
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
            }
        }
        PrintLogOnUpdate(Telemetry_tmp);
    }
}
