#include "tasks.h"
#include "globals.h"
#include "ws2812.h"
#include "math.h"

void draw_water_level(float level_percent) {
    if (level_percent < 0) level_percent = 0;
    if (level_percent > 100) level_percent = 100;

    int full_rows = (int)(level_percent / 20.0f);
    float remainder = fmodf(level_percent, 20.0f);

    for (int row = 0; row < 5; row++) {
        int reversed_row = 4 - row;             // Topo é 0, base é 4
        float r = 0.0f, g = 0.0f, b = 0.0f;

        if (row < full_rows) {
            b = 0.045f;
        } else if (row == full_rows && remainder > 0.0f) {
            if (remainder > 15.0f)      b = 0.035f;
            else if (remainder > 10.0f) b = 0.025f;
            else if (remainder > 5.0f)  b = 0.015f;
            else if (remainder < 5.0f && remainder >= 1.0f ) b = 0.005f;
            else b = 0.0f;
        } else {
            b = 0.0f;
        }

        // Desenha da linha 0 (topo) até 4 (base)
        bool print_last = (row == 4);  // Só manda imprimir na última iteração
        ws2812_draw_row(r, g, b, reversed_row, false, print_last);
    }
}

void vTaskMatrixTankLevel(void *params) {
    PIO pio = pio0;
    int sm = 0;

    uint offset = pio_add_program(pio, &ws2812_program);
  
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW); //Inicializa a matriz de leds

    symbol('*');        // Apaga a matriz

    while (true)
    {
        draw_water_level(percentual_level_value);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}