#ifndef SPIHANDLER_H
#define SPIHANDLER_H

#include "driver/gpio.h"
#include "driver/i2c_master.h"

#include "ets2ctrl.h"
#include "httpHandler.h"
#include "spiHandler.h"
#include "wifiHandler.h"


extern bool WriteAllPWM(uint8_t *values, size_t len);
extern bool WritePWMNum(uint8_t pwm_num, uint8_t value);
extern bool DisablePWMOutput();
extern bool EnablePWMOutput();

extern void spiHandler();

#endif // SPIHANDLER_H