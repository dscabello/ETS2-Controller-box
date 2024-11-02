#include "ets2ctrl.h"

#define TICK_FOR_LED_ANNIMATION_MS         80
#define TABLE_TIME_RESOLUTION_MS           100
#define NUMBER_OF_LEDS                     16
#define TIME_TABLE_POS                     16
#define LED_VALUE_INC_CORRECTION           100 /* Used in the calculation for the fade */

typedef enum {
    LED_CTRL_UNKNOWN,
    LED_CTRL_START,
    LED_CTRL_STOP,
    LED_CTRL_RESET
} LedCtrlCmd_t;

typedef enum {
    LED_STS_UNKNOWN,
    LED_STS_STARTING,
    LED_STS_RUNNING,
    LED_STS_STOPPING,
    LED_STS_STOP
} LedSts_t;

const char *TAG = "Ets2 Controller Box";

const uint8_t InitAnnimation[5][17] = {
/*    PWM0  PWM1  PWM2  PWM3  PWM4  PWM5  PWM6  PWM7  PWM8  PWM9  PWM10 PWM11 PWM12 PWM13 PWM14 PWM15 Delay(x TABLE_TIME_RESOLUTION_MS ms) */
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x70, 0x00, 0x10, 0x64, 0x00, 0x0A },
    { 0x64, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x70, 0x00, 0x10, 0x64, 0x70, 0x10, 0x00, 0x00, 0x0A },
    { 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x30, 0x64, 0x50, 0x10, 0x00, 0x70, 0x00, 0x10, 0x00, 0x00, 0x14 },
    { 0x00, 0x00, 0x70, 0x00, 0x00, 0x64, 0x00, 0x00, 0x70, 0x00, 0x60, 0x64, 0x70, 0x60, 0x10, 0x00, 0x14 },
    { 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x0A },
};

typedef struct {
    LedSts_t LedSts;
    const uint8_t (*matrix)[17];
    uint8_t PosAtual;
    uint8_t PosFinal;
    int16_t Increment_PWM[NUMBER_OF_LEDS]; /* Used to create a fade between the frames * 100 */
    uint8_t CurrentValue[NUMBER_OF_LEDS];
    uint16_t TimeCounter;
} LedAnnimation_t;

LedAnnimation_t LedAnnimation;

void ledAnnimationCalcInc(const uint8_t (*matrix)[17], const uint8_t pos_A, const uint8_t pos_B) {
    uint8_t led_num;
    uint8_t diff_pwm;
    for (led_num = 0 ; led_num < NUMBER_OF_LEDS; led_num++) {
        if (matrix[pos_A][led_num] >  matrix[pos_B][led_num]) {
            diff_pwm = matrix[pos_A][led_num] - matrix[pos_B][led_num];
            LedAnnimation.Increment_PWM[led_num] = -(uint16_t)((diff_pwm * LED_VALUE_INC_CORRECTION) / ( (matrix[pos_A][TIME_TABLE_POS] * TABLE_TIME_RESOLUTION_MS) / TICK_FOR_LED_ANNIMATION_MS));
        } else if (matrix[pos_A][led_num] <  matrix[pos_B][led_num]) {
            diff_pwm = matrix[pos_B][led_num] - matrix[pos_A][led_num];
            LedAnnimation.Increment_PWM[led_num] = (uint16_t)((diff_pwm * LED_VALUE_INC_CORRECTION) / ( (matrix[pos_A][TIME_TABLE_POS] * TABLE_TIME_RESOLUTION_MS) / TICK_FOR_LED_ANNIMATION_MS));
        } else {
            LedAnnimation.Increment_PWM[led_num] = 0;
        }
        // ESP_LOGI(TAG,"ledRunAnnimationCalcInc: Pos %d -> Pos %d LED %d PosA %d posB %d inc %d time = %d.", pos_A, pos_B, led_num, matrix[pos_A][led_num], matrix[pos_B][led_num], LedAnnimation.Increment_PWM[led_num], matrix[pos_A][TIME_TABLE_POS]);
    }
}

void ledRunAnnimationCmd(const LedCtrlCmd_t cmd, const uint8_t (*matrix)[17], const size_t len) {
    switch (cmd)
    {
        case LED_CTRL_UNKNOWN:
            /* Do notthing */
            break;
        case LED_CTRL_START:
            if ( (LedAnnimation.LedSts != LED_STS_STARTING) && (LedAnnimation.LedSts != LED_STS_RUNNING) ) {
                if (len > 1) {
                    LedAnnimation.matrix = matrix;
                    LedAnnimation.PosFinal = len - 1; /* Since we are using absolut position it needs to substract 1 */
                    LedAnnimation.LedSts = LED_STS_STARTING;
                    ESP_LOGI(TAG,"Cmd Start accept");
                }
            }
            break;
        case LED_CTRL_STOP:
            if (LedAnnimation.LedSts != LED_STS_STOP) {
                LedAnnimation.LedSts = LED_STS_STOP;
            }
            break;
        case LED_CTRL_RESET:
            if ( (LedAnnimation.LedSts != LED_STS_STOP) && (LedAnnimation.LedSts != LED_STS_STOP) ) {
                LedAnnimation.LedSts = LED_STS_STOPPING;
            }
            break;

        default:
            break;
    }
}

void ledRunAnnimation() {
    switch (LedAnnimation.LedSts) {
        case LED_STS_UNKNOWN:
            break;
        case LED_STS_STARTING:
                LedAnnimation.PosAtual = 0;
                uint8_t next_pos = LedAnnimation.PosAtual + 1;
                LedAnnimation.LedSts = LED_STS_RUNNING;
                LedAnnimation.TimeCounter = (LedAnnimation.matrix[LedAnnimation.PosAtual][TIME_TABLE_POS] * TABLE_TIME_RESOLUTION_MS) / TICK_FOR_LED_ANNIMATION_MS; /* Read the time */
                ledAnnimationCalcInc(LedAnnimation.matrix, LedAnnimation.PosAtual, next_pos);
                memcpy(LedAnnimation.CurrentValue, LedAnnimation.matrix[LedAnnimation.PosAtual], NUMBER_OF_LEDS);  // Copies entire array
                if (WriteAllPWM(LedAnnimation.CurrentValue, NUMBER_OF_LEDS) == false) {
                    //LedAnnimation.LedSts = LED_STS_STOP;
                }
                ESP_LOGI(TAG,"Starting animation");
            break;
        case LED_STS_RUNNING:
                if (LedAnnimation.TimeCounter) {
                    LedAnnimation.TimeCounter--;
                    /* Adding next step of fade */
                    uint8_t i;
                    for (i = 0 ; i < NUMBER_OF_LEDS; i++) {
                        LedAnnimation.CurrentValue[i] = (uint8_t)( ((LedAnnimation.CurrentValue[i] * LED_VALUE_INC_CORRECTION) + LedAnnimation.Increment_PWM[i]) / LED_VALUE_INC_CORRECTION);
                    }
                    WriteAllPWM(LedAnnimation.CurrentValue, NUMBER_OF_LEDS);
                    //ESP_LOGI(TAG,"Led 0 = %d, inc = %d.", LedAnnimation.CurrentValue[0], LedAnnimation.Increment_PWM[0]);
                } else {
                    LedAnnimation.PosAtual++;
                    uint8_t next_pos;
                    if (LedAnnimation.PosAtual == LedAnnimation.PosFinal) {
                        ESP_LOGI(TAG,"Starting animation again");
                        next_pos = 0;
                    } else if (LedAnnimation.PosAtual > LedAnnimation.PosFinal) {
                        LedAnnimation.PosAtual = 0;
                        next_pos = 1;
                    } else {
                        next_pos = LedAnnimation.PosAtual + 1;
                    }
                    LedAnnimation.TimeCounter = (LedAnnimation.matrix[LedAnnimation.PosAtual][TIME_TABLE_POS] * TABLE_TIME_RESOLUTION_MS) / TICK_FOR_LED_ANNIMATION_MS; /* Read the time */
                    ledAnnimationCalcInc(LedAnnimation.matrix, LedAnnimation.PosAtual, next_pos);
                    memcpy(LedAnnimation.CurrentValue, LedAnnimation.matrix[LedAnnimation.PosAtual], NUMBER_OF_LEDS);
                    if (WriteAllPWM(LedAnnimation.CurrentValue, NUMBER_OF_LEDS) == false) {
                        //LedAnnimation.LedSts = LED_STS_STOP;
                    }
                    // ESP_LOGI(TAG,"Next frame = %d, timer = %d.", LedAnnimation.PosAtual, LedAnnimation.TimeCounter);
                }
            break;
        case LED_STS_STOPPING:
            break;
        case LED_STS_STOP:
            break;
        default:
            break;
    }
}

void ledHandler(void *arg)
{
    // uint8_t data[15];
    // uint8_t i;
    ledRunAnnimationCmd(LED_CTRL_START, InitAnnimation, 5);
    while (1) {
        // for (i = 0 ; i < 15 ; i++)
        //     data[i] = 0x00;
        // WriteAllPWM(data, NUMBER_OF_LEDS);
        // vTaskDelay(pdMS_TO_TICKS(30));
        // for (i = 0 ; i < 15 ; i++)
        //     data[i] = 0xC1;
        // WriteAllPWM(data, NUMBER_OF_LEDS);
        // vTaskDelay(pdMS_TO_TICKS(30));
        // for (i = 0 ; i < 15 ; i++)
        //     data[i] = 0x30;
        // WriteAllPWM(data, NUMBER_OF_LEDS);
        // vTaskDelay(pdMS_TO_TICKS(30));

       ledRunAnnimation();
       vTaskDelay(pdMS_TO_TICKS(TICK_FOR_LED_ANNIMATION_MS));
    }

}