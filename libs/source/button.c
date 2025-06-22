#include "button.h"
#include "globals.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

ConfigState configState = CONFIG_OFF;
SemaphoreHandle_t xDisplayMutex;
TaskHandle_t xButtonTaskHandle;
QueueHandle_t xButtonQueue;

// Rotina de interrupção para botões
void button_isr(uint gpio, uint32_t events) {
    static int count = 0;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if(gpio == SW_BNT || gpio == BTNA || gpio == BTNB) {
        xQueueSendFromISR(xButtonQueue, &gpio, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

// Processamento de botões
void process_button_press(uint gpio) {
    static TickType_t lastPressTime = 0;
    const TickType_t debounceInterval = pdMS_TO_TICKS(150);
    
    if((xTaskGetTickCount() - lastPressTime) > debounceInterval) {
        lastPressTime = xTaskGetTickCount();
        
        if(gpio == SW_BNT) {
            switch(configState) {
                case CONFIG_OFF: configState = CONFIG_MIN; break;
                case CONFIG_MIN: configState = CONFIG_MAX; break;
                case CONFIG_MAX:
                    configState = CONFIG_OFF; 
                    printf("Min=%.1f, Max=%.1f\n", 
                           PUMP_OFF_LEVEL, PUMP_ON_LEVEL);
                    break;
            }
        }
       else if(configState != CONFIG_OFF) {
            float *target = (configState == CONFIG_MIN) ? &PUMP_ON_LEVEL : &PUMP_OFF_LEVEL;
            float step = 0.0f;

            if(gpio == BTNA)
                step = 0.1f;
            else if(gpio == BTNB)
                step = -0.1f;
            else
                return; 

              float new_val = *target + step;

            if(configState == CONFIG_MIN) {
                if(new_val >= 0)
                    *target = new_val;
            } else {
                if(new_val <= 100 && PUMP_ON_LEVEL < new_val)
                    *target = new_val;
            }
        }

    }
}
