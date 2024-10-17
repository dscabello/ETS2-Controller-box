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


enum Types_e {
    NOT_DEFINED     = 0,
    BOOL_TYPE       = 1,
    INT32_TYPE      = 2,
    UINT32_TYPE     = 3
};

typedef struct {
    const char *group;
    const char *name;
    void *varPointer;
    enum Types_e types;       // Tipo da variável ('i' para int, 'b' para bool)
} VarMap_t;

extern bool GameInfo_connected;

extern int32_t TruckInfo_gear; // TODO: Verify if we have negative values
extern bool TruckInfo_cruiseControlOn;
extern bool TruckInfo_engineOn;
extern bool TruckInfo_wisperOn;
extern bool TruckInfo_engineOn;
extern bool TruckInfo_blinkerLeftActive;
extern bool TruckInfo_blinkerRightActive;
extern bool TruckInfo_blinkerLeftOn;
extern bool TruckInfo_blinkerRightOn;
extern bool TruckInfo_lightsParkingOn;
extern bool TruckInfo_lightsBeamLowOn;
extern bool TruckInfo_lightsBeamHighOn;
extern bool TruckInfo_lightsAuxFrontOn;
extern bool TruckInfo_lightsAuxRoofOn;
extern bool TruckInfo_lightsBeaconOn;
extern bool TruckInfo_lightsBrakeOn;
extern bool TruckInfo_lightsDashboardOn;

extern bool TrailerInfo_attached;

extern uint32_t JobInfo_income;

extern uint32_t NavegationInfo_estimatedDistance;



extern bool RequestData;

extern void HttpMainTask(void *pvParameters);

#endif // HTTPHANDLER_H