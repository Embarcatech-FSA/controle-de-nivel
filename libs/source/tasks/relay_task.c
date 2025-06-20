#include "tasks.h"
#include "globals.h"

void vTaskRelay(void *params)
{
    gpio_init(RELAY_PIN);               // Inicializa o pino do relé
    gpio_set_dir(RELAY_PIN, GPIO_OUT);  // Configura o pino como saída
    gpio_put(RELAY_PIN, false);         // Garante que a bomba comece desligada

    bool critical_level_alert;

    while (true)
    {
        // --- ATUAÇÃO NO RELÉ ---
        gpio_put(RELAY_PIN, water_pump_state);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
}