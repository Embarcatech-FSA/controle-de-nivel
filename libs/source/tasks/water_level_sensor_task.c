#include "tasks.h"
#include "globals.h"

/**
 * @brief Mapeia um número de uma faixa para outra.
 */
float map(int value, int min_input, int max_input, float min_output, float max_output) {
    if (value <= min_input) return min_output;
    if (value >= max_input) return max_output;
    return min_output + ((float)(value - min_input) / (float)(max_input - min_input)) * (max_output - min_output);
}

void vTaskWaterLevelSensor(void *params)
{   
    while (true)
    { 
        if (xSemaphoreTake(xADCMutex, pdMS_TO_TICKS(portMAX_DELAY)) == pdTRUE) {
            adc_init();
            adc_gpio_init(LEVEL_SENSOR_PIN);
            adc_select_input(2);
            uint16_t adc_value = adc_read();
            xSemaphoreGive(xADCMutex);  // Libera o mutex imediatamente após a leitura
            
            percentual_level_value = map(adc_value, ADC_MIN_VALUE, ADC_MAX_VALUE, 0.0f, 100.0f);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
}