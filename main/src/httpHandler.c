#include "httpHandler.h"
#include "cJSON.h"

#define HTTP_SERVER_URL         CONFIG_HTTP_SERVER_URL
#define BUFFER_SIZE             CONFIG_BUFFER_SIZE

bool RequestData = true;

int JsonBufPOS=0;
char JsonBuf[BUFFER_SIZE];

bool WipersOn = false;

esp_err_t HttpEventHandler(esp_http_client_event_t *evt) {
    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER: %.*s", evt->data_len, (char*)evt->data);
            break;
        case HTTP_EVENT_ON_DATA:
            if (!esp_http_client_is_chunked_response(evt->client)) {
               // ESP_LOGI(TAG, "Received data: %.*s", evt->data_len, (char*)evt->data);
                if ((JsonBufPOS + evt->data_len) < BUFFER_SIZE) {
                    memcpy(JsonBuf + JsonBufPOS, evt->data, evt->data_len);
                    JsonBufPOS = JsonBufPOS + evt->data_len;
                    ESP_LOGI(TAG, "Adding data on JsonBuffer (size = %d) (total = %d)",evt->data_len, JsonBufPOS);
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
                ESP_LOGI(TAG, "Received data: %s", JsonBuf);
            } else {
                cJSON *ItemTruck = cJSON_GetObjectItem(root, "truck");
                cJSON *ItemWipersOn = cJSON_GetObjectItem(ItemTruck, "wipersOn");
                if (cJSON_IsNumber(ItemWipersOn)) {
                    ESP_LOGI(TAG, "Is a number");
                    // WipersOn = ItemWipersOn->valueint;
                    // ESP_LOGI(TAG, "\nDisplay Gear = %d.\n", WipersOn);
                } else if (cJSON_IsBool(ItemWipersOn)) {
                    WipersOn = cJSON_IsTrue(ItemWipersOn);
                    // WipersOn = ItemWipersOn->valueint;
                    ESP_LOGI(TAG, "\nDisplay Gear = %d.\n", WipersOn);
                    ESP_LOGI(TAG, "Is a bool");
                } else {
                    ESP_LOGI(TAG, "ERROR is not a number");
                }
                // int displayedGear = cJSON_GetObjectItem(root, "displayedGear")->valueint;
            }

            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            JsonBufPOS = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DISCONNECTED");
            break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGI(TAG, "HTTP_EVENT_REDIRECT");
            break;
        case HTTP_EVENT_HEADERS_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADERS_SENT");
            break;
        default:
            ESP_LOGI(TAG, "Unhandled event: %d", evt->event_id);
            break;
    }
    return ESP_OK;
}

void HttpRequest() {
    ESP_ERROR_CHECK(nvs_flash_init());
    esp_http_client_config_t config = {
        .url = HTTP_SERVER_URL, // TODO:CHANGE TO CONFIG
        .event_handler = HttpEventHandler,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTPS Status ok");
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}

void HttpMainTask(void *pvParameters) {
    ESP_LOGI(TAG, "HttpMainTask init");
    while(1) {
        if (WifiConnected) {
            if (RequestData) {
                HttpRequest();
                RequestData = false;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}