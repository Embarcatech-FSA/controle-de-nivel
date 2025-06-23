#include "tasks.h"
#include "globals.h"

void vTaskRelay(void *params)
{
    gpio_init(RELAY_PIN);               // Inicializa o pino do relé
    gpio_set_dir(RELAY_PIN, GPIO_OUT);  // Configura o pino como saída
    gpio_put(RELAY_PIN, false);         // Garante que a bomba comece desligada

    gpio_init(LED_GREEN);              // Inicializa o LED verde
    gpio_set_dir(LED_GREEN, GPIO_OUT); // Configura o LED verde como saída
    gpio_init(LED_RED);                // Inicializa o LED vermelho
    gpio_set_dir(LED_RED, GPIO_OUT);   // Configura o LED vermelho como saída
    gpio_init(LED_BLUE);                // Inicializa o LED azul
    gpio_set_dir(LED_BLUE, GPIO_OUT);   // Configura o LED azul como saída

    gpio_put(LED_RED, true);           // LED vermelho inicialmente ligado, indicando que a bomba está desligada

    bool critical_level_alert;

    while (true)
    {
        // --- ATUAÇÃO NO RELÉ ---
        gpio_put(RELAY_PIN, water_pump_state);

        // LED BRANCO: ACESO (Bomba ativada) / DESLIGADO (Bomba desativada)
        gpio_put(LED_GREEN, water_pump_state);
        gpio_put(LED_RED, water_pump_state);
        gpio_put(LED_BLUE, water_pump_state);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
}