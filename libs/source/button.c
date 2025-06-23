#include "button.h"
#include "globals.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#define JOYSTICK_CENTER 1780
#define DEADZONE 350

ConfigState configState = CONFIG_OFF;
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

uint16_t read_joystick_y() {
    if(xSemaphoreTake(xADCMutex, pdMS_TO_TICKS(portMAX_DELAY)) == pdTRUE) {
        adc_gpio_init(VRY);
        adc_select_input(0);
        uint16_t value = adc_read();
        xSemaphoreGive(xADCMutex);
        return value;
    }
    return 0;
}

// Função para processar movimento do joystick
void process_joystick_movement() {
    uint16_t y_value = read_joystick_y(); 
    float step = 0.0f;

    int16_t deviation = y_value - JOYSTICK_CENTER;

    if(deviation > DEADZONE) {
        step = 1.0f;  
    } 
    
    else if(deviation < -DEADZONE) {
        step = -1.0f;   
    }
    if(step != 0.0f) {
        float *target = (configState == CONFIG_MIN) ? &PUMP_ON_LEVEL : &PUMP_OFF_LEVEL;
        
        float new_val = *target + step;
        
        // Validação de limites (0-100)
        if(new_val < 0) new_val = 0;
        if(new_val > 100) new_val = 100;
        
        // Validação adicional de limites
        if(configState == CONFIG_MIN) {
            if(new_val < PUMP_OFF_LEVEL) *target = new_val;
        } 
        else { // CONFIG_MAX
            if(new_val > PUMP_ON_LEVEL) *target = new_val;
        }
    }
}

// Processamento de botões
void process_button_press(uint gpio) {
    static TickType_t lastPressTime = 0;
    const TickType_t debounceInterval = pdMS_TO_TICKS(500);
    
    if((xTaskGetTickCount() - lastPressTime) > debounceInterval) {
        lastPressTime = xTaskGetTickCount();
        
        // Botão A inicia configuração
        if(gpio == BTNA && configState == CONFIG_OFF) {
            configState = CONFIG_MIN;
            return;
        }
        
        // Botão B confirma e avança estados
        if(gpio == BTNB) {
            switch(configState) {
                case CONFIG_MIN:
                    configState = CONFIG_MAX;
                    break;
                case CONFIG_MAX:
                    configState = CONFIG_OFF;
                    break;
                default: break;
            }
            return;
        }
    }
    
}

