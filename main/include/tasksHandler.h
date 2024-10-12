#ifndef TASKHANDLER_H
#define TASKHANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_err.h"


extern void MainThread(void *arg);


#endif // 