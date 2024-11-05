#include "httpHandler.h"
#include "cJSON.h"

#define HTTP_SERVER_URL         CONFIG_HTTP_SERVER_URL
#define BUFFER_SIZE             CONFIG_BUFFER_SIZE

#include "esp_heap_trace.h"
#define NUM_RECORDS 100

static heap_trace_record_t trace_record[NUM_RECORDS]; // This buffer must be in internal RAM


/* Vars from Http server */
/* Game */
bool GameInfo_connected;

/* Truck */
int32_t TruckInfo_gear;
bool TruckInfo_cruiseControlOn;
bool TruckInfo_wipersOn;
bool TruckInfo_engineOn;
bool TruckInfo_blinkerLeftActive;
bool TruckInfo_blinkerRightActive;
bool TruckInfo_blinkerLeftOn;
bool TruckInfo_blinkerRightOn;
bool TruckInfo_lightsParkingOn;
bool TruckInfo_lightsBeamLowOn;
bool TruckInfo_lightsBeamHighOn;
bool TruckInfo_lightsAuxFrontOn;
bool TruckInfo_lightsAuxRoofOn;
bool TruckInfo_lightsBeaconOn;
bool TruckInfo_lightsBrakeOn;
bool TruckInfo_lightsDashboardOn;

/* Trailer */
bool TrailerInfo_attached;

/* Job */
uint32_t JobInfo_income;

/* Navegation */
uint32_t NavegationInfo_estimatedDistance;

/* var_map used to link json and vars */
VarMap_t var_map[] = {
    { "game"        , "connected"         , &GameInfo_connected                 , BOOL_TYPE     },
    { "truck"       , "gear"              , &TruckInfo_gear                     , INT32_TYPE    },
    { "truck"       , "cruiseControlOn"   , &TruckInfo_cruiseControlOn          , BOOL_TYPE     },
    { "truck"       , "wipersOn"          , &TruckInfo_wipersOn                 , BOOL_TYPE     },
    { "truck"       , "engineOn"          , &TruckInfo_engineOn                 , BOOL_TYPE     },
    { "truck"       , "blinkerLeftActive" , &TruckInfo_blinkerLeftActive        , BOOL_TYPE     },
    { "truck"       , "blinkerRightActive", &TruckInfo_blinkerRightActive       , BOOL_TYPE     },
    { "truck"       , "blinkerLeftOn"     , &TruckInfo_blinkerLeftOn            , BOOL_TYPE     },
    { "truck"       , "blinkerRightOn"    , &TruckInfo_blinkerRightOn           , BOOL_TYPE     },
    { "truck"       , "lightsParkingOn"   , &TruckInfo_lightsParkingOn          , BOOL_TYPE     },
    { "truck"       , "lightsBeamLowOn"   , &TruckInfo_lightsBeamLowOn          , BOOL_TYPE     },
    { "truck"       , "lightsBeamHighOn"  , &TruckInfo_lightsBeamHighOn         , BOOL_TYPE     },
    { "truck"       , "lightsAuxFrontOn"  , &TruckInfo_lightsAuxFrontOn         , BOOL_TYPE     },
    { "truck"       , "lightsAuxRoofOn"   , &TruckInfo_lightsAuxRoofOn          , BOOL_TYPE     },
    { "truck"       , "lightsBeaconOn"    , &TruckInfo_lightsBeaconOn           , BOOL_TYPE     },
    { "truck"       , "lightsBrakeOn"     , &TruckInfo_lightsBrakeOn            , BOOL_TYPE     },
    { "truck"       , "lightsDashboardOn" , &TruckInfo_lightsDashboardOn        , BOOL_TYPE     },
    { "trailer"     , "attached"          , &TrailerInfo_attached               , BOOL_TYPE     },
    { "job"         , "income"            , &JobInfo_income                     , UINT32_TYPE   },
    { "navegation"  , "estimatedDistance" , &NavegationInfo_estimatedDistance   , UINT32_TYPE   }
};

uint8_t var_map_size = sizeof(var_map) / sizeof(VarMap_t);

bool RequestData = true;
int JsonBufPOS=0;
char JsonBuf[BUFFER_SIZE];
bool WipersOn = false;

SemaphoreHandle_t HTTPSemaphore = NULL;

void ETS2_setVar(cJSON* cJSON_item_ptr, VarMap_t* var_map_ptr) {
    if (var_map_ptr->types == BOOL_TYPE) {
        if (cJSON_IsBool(cJSON_item_ptr)) {
            bool aux_bool = cJSON_IsTrue(cJSON_item_ptr);
            *((bool*)var_map_ptr->varPointer) = aux_bool;
            // ESP_LOGI(TAG,"Decode group = \"%s\", item = \"%s\", BOOL_TYPE = %d.", var_map_ptr->group, var_map_ptr->name, aux_bool);
        } else {
            ESP_LOGE(TAG,"Error: Decoding group = \"%s\", item = \"%s\" as a BOOL_TYPE", var_map_ptr->group, var_map_ptr->name);
        }
    } else if (var_map_ptr->types == INT32_TYPE) {
        if (cJSON_IsNumber(cJSON_item_ptr)) {
            int32_t aux_number = (int32_t)(cJSON_GetNumberValue(cJSON_item_ptr));
            *((int32_t*)var_map_ptr->varPointer) = aux_number;
            // ESP_LOGI(TAG,"Decode group = \"%s\", item = \"%s\", INT32_TYPE = %" PRId32, var_map_ptr->group, var_map_ptr->name, aux_number);
        } else {
            ESP_LOGE(TAG,"Error: Decoding group = \"%s\", item = \"%s\" as a INT32_TYPE", var_map_ptr->group, var_map_ptr->name);
        }
    } else if (var_map_ptr->types == UINT32_TYPE) {
        if (cJSON_IsNumber(cJSON_item_ptr)) {
            uint32_t aux_number = (uint32_t)(cJSON_GetNumberValue(cJSON_item_ptr));
            *((uint32_t*)var_map_ptr->varPointer) = aux_number;
            // ESP_LOGI(TAG,"Decode group = \"%s\", item = \"%s\", INT32_TYPE = %" PRId32, var_map_ptr->group, var_map_ptr->name, aux_number);
        } else {
            ESP_LOGE(TAG,"Error: Decoding group = \"%s\", item = \"%s\" as a UINT32_TYPE", var_map_ptr->group, var_map_ptr->name);
        }
    } else {
        ESP_LOGE(TAG,"Error: no valid type.");
    }
}

void ETS2_interpreter(cJSON* cJSON_ptr) {
    uint32_t index;
    ESP_LOGD(TAG, "ETS2_interpreter: Get data from JSON.");
    if (cJSON_ptr == NULL) {
        ESP_LOGE(TAG, "Error decoding JSON data");
        return;
    } else {
        for (index = 0; index < var_map_size ; index++) {
            cJSON *ItemGroup = cJSON_GetObjectItem(cJSON_ptr, var_map[index].group);
            if (cJSON_IsObject(ItemGroup)) {
                cJSON *Item = cJSON_GetObjectItem(ItemGroup, var_map[index].name);
                if (Item == NULL)
                    ESP_LOGE(TAG,"Not find group = %s, item = %s.", var_map[index].group, var_map[index].name);
                else
                    ETS2_setVar(Item, &var_map[index]);
            }
        }
    }
}

esp_err_t HttpEventHandler(esp_http_client_event_t *evt) {
    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            // ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            // ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_ON_HEADER:
            // ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER: %.*s", evt->data_len, (char*)evt->data);
            break;
        case HTTP_EVENT_ON_DATA:
            if (!esp_http_client_is_chunked_response(evt->client)) {
               // ESP_LOGI(TAG, "Received data: %.*s", evt->data_len, (char*)evt->data);
                if ((JsonBufPOS + evt->data_len) < BUFFER_SIZE) {
                    memcpy(JsonBuf + JsonBufPOS, evt->data, evt->data_len);
                    JsonBufPOS = JsonBufPOS + evt->data_len;
                    // ESP_LOGI(TAG, "Adding data on JsonBuffer (size = %d) (total = %d)",evt->data_len, JsonBufPOS);
                } else {
                    ESP_LOGI(TAG, "ERROR wrong size %d", JsonBufPOS);
                }
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "\n Decode json (size = %d)", JsonBufPOS);
            JsonBuf[JsonBufPOS] = '\0';
            cJSON *root = cJSON_Parse(JsonBuf);
            if (root == NULL) {
                ESP_LOGI(TAG, "Erro ao decodificar JSON");
                // ESP_LOGI(TAG, "Received data: %s", JsonBuf);
            } else {
                ETS2_interpreter(root);
            }
            cJSON_Delete(root);

            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            JsonBufPOS = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DISCONNECTED");
            break;
        case HTTP_EVENT_REDIRECT:
            // ESP_LOGI(TAG, "HTTP_EVENT_REDIRECT");
            break;
        case HTTP_EVENT_HEADERS_SENT:
            // ESP_LOGI(TAG, "HTTP_EVENT_HEADERS_SENT");
            break;
        default:
            // ESP_LOGI(TAG, "Unhandled event: %d", evt->event_id);
            break;
    }
    return ESP_OK;
}

void HttpRequest() {
    ESP_LOGI("MEMORY", "Free heap 1 size: %ld", esp_get_free_heap_size());
    ESP_ERROR_CHECK(nvs_flash_init());
    esp_http_client_config_t config = {
        .url = HTTP_SERVER_URL,
        .event_handler = HttpEventHandler,
    };
    ESP_LOGI(TAG, "HTTPS Init");
    esp_http_client_handle_t client = esp_http_client_init(&config);
    ESP_LOGI(TAG, "HTTPS perform");
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTPS Status ok");
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    ESP_LOGI(TAG, "HTTPS cleanup");
    esp_http_client_cleanup(client);
}

bool HttpRequestData() {
    if (xSemaphoreTake(HTTPSemaphore, (TickType_t) 10) == pdTRUE) {
        if (RequestData == false) {
            RequestData = true;
            xSemaphoreGive(HTTPSemaphore);
            return true;
        }
        xSemaphoreGive(HTTPSemaphore);
    }
    return false;
}

void HttpMainTask(void *pvParameters) {
    ESP_LOGI(TAG, "HttpMainTask init");
    ESP_ERROR_CHECK( heap_trace_init_standalone(trace_record, NUM_RECORDS) );
    while(1) {
        if (WifiConnected()) {
            if (xSemaphoreTake(HTTPSemaphore, (TickType_t) 10) == pdTRUE) {
                if (RequestData) {
                    ESP_ERROR_CHECK( heap_trace_start(HEAP_TRACE_LEAKS) );
                    HttpRequest();
                    RequestData = false;

                    ESP_ERROR_CHECK( heap_trace_stop() );
                    heap_trace_dump();
                }
                xSemaphoreGive(HTTPSemaphore);
            }
            vTaskDelay(pdMS_TO_TICKS(5));
        } else {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}