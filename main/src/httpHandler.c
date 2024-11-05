#include "httpHandler.h"
#include "cJSON.h"

#define HTTP_SERVER_URL         CONFIG_HTTP_SERVER_URL
#define BUFFER_SIZE             CONFIG_BUFFER_SIZE

Telemetry_t Telemetry;

/* var_map used to link json and vars */
const VarMap_t var_map[] = {
    { "game"        , "connected"         , &Telemetry.game.connected           , BOOL_TYPE     },
    { "game"        , "paused"            , &Telemetry.game.connected           , BOOL_TYPE     },
    { "truck"       , "gear"              , &Telemetry.truck.gear               , INT32_TYPE    },
    { "truck"       , "wipersOn"          , &Telemetry.truck.wipersOn           , BOOL_TYPE     },
    { "truck"       , "lightsParkingOn"   , &Telemetry.truck.lightsParkingOn    , BOOL_TYPE     },
    { "truck"       , "lightsBeamLowOn"   , &Telemetry.truck.lightsBeamLowOn    , BOOL_TYPE     },
    { "truck"       , "lightsBeamHighOn"  , &Telemetry.truck.lightsBeamHighOn  , BOOL_TYPE     },
    { "truck"       , "blinkerLeftOn"     , &Telemetry.truck.blinkerLeftOn      , BOOL_TYPE     },
    { "truck"       , "blinkerRightOn"    , &Telemetry.truck.blinkerRightOn     , BOOL_TYPE     },
    { "trailer"     , "attached"          , &Telemetry.trailer.attached         , BOOL_TYPE     }
};

const uint8_t var_map_size = sizeof(var_map) / sizeof(VarMap_t);

bool RequestData = true;
int JsonBufPOS=0;
char JsonBuf[BUFFER_SIZE];

esp_http_client_handle_t client;

SemaphoreHandle_t TelemetrySemaphore = NULL;
SemaphoreHandle_t HTTPSemaphore = NULL;

void ETS2_setVar(cJSON* cJSON_item_ptr, const VarMap_t* var_map_ptr) {
    if (var_map_ptr->types == BOOL_TYPE) {
        if (cJSON_IsBool(cJSON_item_ptr)) {
            bool aux_bool = cJSON_IsTrue(cJSON_item_ptr);
            if (xSemaphoreTake(TelemetrySemaphore, (TickType_t) 10) == pdTRUE) {
                *((bool*)var_map_ptr->varPointer) = aux_bool;
                xSemaphoreGive(TelemetrySemaphore);
            }
            // ESP_LOGI(TAG,"Decode group = \"%s\", item = \"%s\", BOOL_TYPE = %d.", var_map_ptr->group, var_map_ptr->name, aux_bool);
        } else {
            ESP_LOGE(TAG,"Error: Decoding group = \"%s\", item = \"%s\" as a BOOL_TYPE", var_map_ptr->group, var_map_ptr->name);
        }
    } else if (var_map_ptr->types == INT32_TYPE) {
        if (cJSON_IsNumber(cJSON_item_ptr)) {
            int32_t aux_number = (int32_t)(cJSON_GetNumberValue(cJSON_item_ptr));
            if (xSemaphoreTake(TelemetrySemaphore, (TickType_t) 10) == pdTRUE) {
                *((int32_t*)var_map_ptr->varPointer) = aux_number;
                xSemaphoreGive(TelemetrySemaphore);
            }
            // ESP_LOGI(TAG,"Decode group = \"%s\", item = \"%s\", INT32_TYPE = %" PRId32, var_map_ptr->group, var_map_ptr->name, aux_number);
        } else {
            ESP_LOGE(TAG,"Error: Decoding group = \"%s\", item = \"%s\" as a INT32_TYPE", var_map_ptr->group, var_map_ptr->name);
        }
    } else if (var_map_ptr->types == UINT32_TYPE) {
        if (cJSON_IsNumber(cJSON_item_ptr)) {
            uint32_t aux_number = (uint32_t)(cJSON_GetNumberValue(cJSON_item_ptr));
            if (xSemaphoreTake(TelemetrySemaphore, (TickType_t) 10) == pdTRUE) {
                *((uint32_t*)var_map_ptr->varPointer) = aux_number;
                xSemaphoreGive(TelemetrySemaphore);
            }
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
                    ESP_LOGE(TAG, "ERROR wrong size %d", JsonBufPOS);
                }
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            // ESP_LOGI(TAG, "\n Decode json (size = %d)", JsonBufPOS);
            JsonBuf[JsonBufPOS] = '\0';
            cJSON *root = cJSON_Parse(JsonBuf);
            if (root == NULL) {
                ESP_LOGE(TAG, "Error decoding JSON");
                // ESP_LOGI(TAG, "Received data: %s", JsonBuf);
            } else {
                ETS2_interpreter(root);
            }
            cJSON_Delete(root);
            if (xSemaphoreTake(TelemetrySemaphore, (TickType_t) 100) == pdTRUE) {
                Telemetry.value_update = true;
                xSemaphoreGive(TelemetrySemaphore);
            }
            // ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            JsonBufPOS = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            // ESP_LOGI(TAG, "HTTP_EVENT_ON_DISCONNECTED");
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

void HttpConfigInit() {
    esp_http_client_config_t config = {
        .url = HTTP_SERVER_URL,
        .event_handler = HttpEventHandler,
        .keep_alive_enable = true,
        .keep_alive_interval = 0
    };
    client = esp_http_client_init(&config);
    if (client == NULL) {
        ESP_LOGE(TAG, "Falha ao inicializar o cliente HTTP.");
    } else {
        ESP_LOGI(TAG, "Cliente HTTP inicializado com conexão persistente.");
    }
}

esp_err_t HttpRequest() {
    // ESP_LOGI("MEMORY", "HttpRequest: Free heap size: %ld", esp_get_free_heap_size());
    if (client == NULL) {
        ESP_LOGE(TAG, "Cliente HTTP not initialized.");
        return ESP_FAIL;
    }

    esp_http_client_set_method(client, HTTP_METHOD_GET);

    esp_err_t err = esp_http_client_perform(client);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "HTTP Status = %d", esp_http_client_get_status_code(client));
        return ESP_FAIL;
    }

    return ESP_OK;
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


Telemetry_t GetTelemetry() {
    Telemetry_t Telemetry_tmp;
    Telemetry_tmp.value_update = false;
    if (xSemaphoreTake(TelemetrySemaphore, (TickType_t) 100) == pdTRUE) {
        Telemetry_tmp = Telemetry;
        Telemetry.value_update = false;
        xSemaphoreGive(TelemetrySemaphore);
    }
    return Telemetry_tmp;
}


void HttpMainTask(void *pvParameters) {
    ESP_LOGI(TAG, "HttpMainTask init");
    TelemetrySemaphore = xSemaphoreCreateBinary();
    if (TelemetrySemaphore != NULL)
        xSemaphoreGive(TelemetrySemaphore);
    HttpConfigInit();
    while(1) {
        if (WifiConnected()) {
            if (xSemaphoreTake(HTTPSemaphore, (TickType_t) 10) == pdTRUE) {
                if (RequestData) {
                    HttpRequest();
                    RequestData = false;
                }
                xSemaphoreGive(HTTPSemaphore);
            }
            vTaskDelay(pdMS_TO_TICKS(5));
        } else {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}