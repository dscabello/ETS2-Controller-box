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

#define MAX_SIZE_STRING_TYPE 21
#define SIZE_GAME_TIME 21 // Format "0001-01-28T14:35:00Z"

enum Types_e {
    NOT_DEFINED     = 0,
    BOOL_TYPE       = 1,
    INT32_TYPE      = 2,
    UINT32_TYPE     = 3,
    STRING_TYPE     = 4
};

typedef struct {
    const char *group;
    const char *name;
    void *varPointer;
    enum Types_e types;
} VarMap_t;

typedef struct {
    bool connected;
    bool paused;
    char time[SIZE_GAME_TIME];
} Game_t;

typedef struct {
    int32_t gear;
    bool wipersOn;
    bool lightsParkingOn;
    bool lightsBeamLowOn;
    bool lightsBeamHighOn;
    bool blinkerLeftOn;
    bool blinkerRightOn;
} TruckInfo_t;

typedef struct {
    bool attached;
} Trailer_t;

typedef struct {
    bool value_update;
    Game_t game;
    TruckInfo_t truck;
    Trailer_t trailer;
} Telemetry_t;

extern Telemetry_t Telemetry;

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

extern SemaphoreHandle_t HTTPSemaphore;

extern bool HttpRequestData();
extern Telemetry_t GetTelemetry();
extern void HttpMainTask(void *pvParameters);

#endif // HTTPHANDLER_H