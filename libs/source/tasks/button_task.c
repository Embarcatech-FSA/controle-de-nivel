#include "globals.h"
#include "button.h"

void vTaskButton(void *pvParameters) {
    // Inicialização da fila
    xButtonQueue = xQueueCreate(10, sizeof(uint8_t));
    
    // Configuração de GPIOs

    gpio_init(BTNA);
    gpio_set_dir(BTNA, GPIO_IN);
    gpio_pull_up(BTNA);
    
    gpio_init(BTNB);
    gpio_set_dir(BTNB, GPIO_IN);
    gpio_pull_up(BTNB);
    
    // Configuração de interrupções
    gpio_set_irq_enabled_with_callback(BTNA, GPIO_IRQ_EDGE_FALL, true, button_isr);
    gpio_set_irq_enabled(BTNB, GPIO_IRQ_EDGE_FALL, true);

    uint8_t gpio_num;
    while(1) {

        // Aguarda evento da fila
        if(xQueueReceive(xButtonQueue, &gpio_num, pdMS_TO_TICKS(20)) == pdPASS) {
            process_button_press(gpio_num);
        }

            if(configState != CONFIG_OFF) {
            process_joystick_movement();
            vTaskDelay(pdMS_TO_TICKS(20));
        }
    }
}