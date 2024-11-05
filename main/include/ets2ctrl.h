#ifndef ETS2CTRL_H
#define ETS2CTRL_H

#include "tasksHandler.h"
#include "wifiHandler.h"
#include "httpHandler.h"
#include "spiHandler.h"

typedef enum {
    CMD_LED_01 = (1 << 0),
    CMD_LED_02 = (1 << 1),
    CMD_LED_03 = (1 << 2),
    CMD_LED_04 = (1 << 3),
    CMD_LED_05 = (1 << 4),
    CMD_LED_06 = (1 << 5),
    CMD_LED_07 = (1 << 6),
    CMD_LED_08 = (1 << 7),
    CMD_LED_09 = (1 << 8),
    CMD_LED_10 = (1 << 9),
    CMD_LED_11 = (1 << 10),
    CMD_LED_12 = (1 << 11),
    CMD_LED_13 = (1 << 12),
    CMD_LED_14 = (1 << 13),
    CMD_LED_15 = (1 << 14),
    CMD_LED_16 = (1 << 15)
} CmdLedNum_t;

extern const char *TAG;

extern SemaphoreHandle_t LedSemaphore;

void ledCmdOn();
void ledCmdOFF(CmdLedNum_t LedNumber);
void ledCmdTurnOnBlink(CmdLedNum_t LedNumber);
void ledCmdTurnOFFBlink(CmdLedNum_t LedNumber);
void ledCmdTurnOn(CmdLedNum_t LedNumber);
void ledCmdTurnOFF(CmdLedNum_t LedNumber);

extern void ledHandler(void *arg);

#endif // ETS2CTRL_H