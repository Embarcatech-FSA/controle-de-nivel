#ifndef TASKS_H
#define TASKS_H

#include "stdio.h"
#include "hardware/pwm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

extern SemaphoreHandle_t xDisplayMut;
extern SemaphoreHandle_t xStopSem;

void vTaskWiFi(void *params);
void vTaskWaterLevelSensor(void *params);
void vTaskRelay(void *params);
void vTaskSystemControl(void *params);
void vTaskMatrixTankLevel(void *params);

#endif