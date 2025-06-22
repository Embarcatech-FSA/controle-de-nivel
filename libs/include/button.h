#ifndef BUTTON_H
#define BUTTON_H

#include "tasks.h"
#include "queue.h"

// Estados de configuração
typedef enum {
    CONFIG_OFF,     // Modo normal
    CONFIG_MIN,     // Configurando valor mínimo
    CONFIG_MAX      // Configurando valor máximo
} ConfigState;

// Variáveis compartilhadas
extern ConfigState configState;
//extern SemaphoreHandle_t xDisplayMutex;
extern TaskHandle_t xButtonTaskHandle;
extern QueueHandle_t xButtonQueue;

// Protótipos de funções
void button_isr(uint gpio, uint32_t events);
void process_button_press(uint gpio);
void vTaskButton(void *pvParameters);

#endif // BUTTON_H
