#include "tasks.h"
#include "globals.h"
#include "ssd1306.h"
#include "wifi.h"
#include "pico/time.h"
#include "button.h"
/*
    Aluno: Luis Felipe Pereira de Carvalho
*/

/**
 * @brief Task principal para gerenciar e atualizar o display OLED SSD1306.
 *
 * Esta task do FreeRTOS é responsável por toda a interação com o display.

 * A tela possui dois modos de exibição principais:
 * 1.  Tela de Status Normal: Mostra o nível do reservatório, o estado da bomba e o
 * status da conexão/IP do sistema.
 * 2.  Tela de Alerta Temporário: Exibe uma mensagem por 3 segundos quando os limites
 * da bomba são atualizados via servidor web.
 *
 * @note O acesso aos dados globais compartilhados (como nível, estado da bomba, etc.)
 * é protegido por um mutex (xDisplayMutex) para garantir a segurança da thread
 * (thread-safety) e evitar condições de corrida.
 *
 * @note A task depende das seguintes variáveis globais para exibir informações:
 * - `percentual_level_value`: O nível atual do tanque em porcentagem.
 * - `water_pump_state`: O estado atual da bomba (ligada/desligada).
 * - `tank_state`: O estado geral do tanque (normal, alerta, erro).
 * - `wifi_connected`: O status da conexão Wi-Fi.
 * - `ip_address_str`: O endereço IP do dispositivo.
 * - `show_limits_display`: Controla a exibição do alerta temporário.
 * - `PUMP_ON_LEVEL`, `PUMP_OFF_LEVEL`: Para exibir no alerta.

 * @see xDisplayMutex
 * @see show_limits_display
 * @see http_recv (em server.c, para o gatilho do alerta)
 */

void vTaskDisplay(void *params) {
    ssd1306_t ssd;

    // --- Inicialização do I2C ---
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init(&ssd, DISPLAY_WIDTH, DISPLAY_HEIGHT, false, DISPLAY_ADDRESS, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_draw_string(&ssd, "Iniciando...", 18, 28);
    ssd1306_send_data(&ssd);
    
    init_buttons();

    vTaskDelay(pdMS_TO_TICKS(2000));

    char line1_str[22];
    char line2_str[22];

    while (true) {
        if (xSemaphoreTake(xDisplayMut, portMAX_DELAY) == pdTRUE) {
            
            ssd1306_fill(&ssd, false); 

            if (time_us_64() < show_limits_display || configState != CONFIG_OFF) {
                // TELA DE ALERTA DE LIMITES ATUALIZADOS
                ssd1306_draw_string(&ssd, "Novos limites", 2, 8);
                ssd1306_draw_string(&ssd, "para bomba", 2, 17);
                ssd1306_line(&ssd, 0, 32, 127, 32, true);

                snprintf(line1_str, sizeof(line1_str), "Minimo: %.0f%%", PUMP_ON_LEVEL);
                snprintf(line2_str, sizeof(line2_str), "Maximo: %.0f%%", PUMP_OFF_LEVEL);
                ssd1306_draw_string(&ssd, line1_str, 4, 36);
                ssd1306_draw_string(&ssd, line2_str, 4, 50);
            } else {
                // TELA NORMAL DE STATUS
                ssd1306_draw_string(&ssd, "Sistema Nivel", 2, 0);
                ssd1306_line(&ssd, 0, 10, DISPLAY_WIDTH - 1, 10, true);
                ssd1306_line(&ssd, 64, 11, 64, DISPLAY_HEIGHT - 17, true);

                snprintf(line1_str, sizeof(line1_str), "%.0f%%", percentual_level_value);
                ssd1306_draw_string(&ssd, "Nivel:", 10, 14);
                ssd1306_draw_string(&ssd, line1_str, 10, 28);
                
                snprintf(line1_str, sizeof(line1_str), "%s", water_pump_state ? "LIGADA" : "DESLIG");
                ssd1306_draw_string(&ssd, "Bomba:", 74, 14);
                ssd1306_draw_string(&ssd, line1_str, 74, 28);
                
                ssd1306_line(&ssd, 0, 48, DISPLAY_WIDTH - 1, 48, true);
                
                if (!wifi_connected) {
                    snprintf(line1_str, sizeof(line1_str), "Erro: Sem Wi-Fi");
                } else if (tank_state == 0) {
                    snprintf(line1_str, sizeof(line1_str), "NIVEL BAIXO!!!");
                } else if (tank_state == 4) {
                    snprintf(line1_str, sizeof(line1_str), "TRANSBORDANDO!!!");
                } else {
                    snprintf(line1_str, sizeof(line1_str), "%s", ip_address_str);
                }
                ssd1306_draw_string(&ssd, line1_str, 2, 52);
            }
        }

            ssd1306_send_data(&ssd);
            xSemaphoreGive(xDisplayMut);
        }
        vTaskDelay(pdMS_TO_TICKS(250));
}
