/**
 * Controle do menu exibido no display com o uso de botões A e B. 
 * Os botões são tratados com interrupções e lógica de debounce para evitar múltiplas leituras indesejadas.
 * Botão A alterna entre o modo de exibição normal e o modo de configuração.
 * Botão B reseta as configurações para os valores padrão.
 * Aluna: Mariana Santos
 */
#include "menu_control.h"
#include "globals.h"
#include "pico/stdlib.h"

// Debounce
volatile uint32_t last_button_press = 0;
const uint32_t debounce_time_ms = 200;

// Interrupção para controle do menu
void button_isr(uint gpio, uint32_t events) {
    if (to_ms_since_boot(get_absolute_time()) - last_button_press > debounce_time_ms) {
		if (gpio == BTNA) {
			MENU_MODE = !MENU_MODE; // alterna entre 0 e 1
			last_button_press = to_ms_since_boot(get_absolute_time());
			printf("Modo do menu alterado para %d\n", MENU_MODE);
		} else if (gpio == BTNB) {
			// reseta configurações
            MENU_MODE = 2;
            reset_limits();
            last_button_press = to_ms_since_boot(get_absolute_time());
			printf("Configurações resetadas\n");
		}
    }
}

void init_buttons() {
    gpio_init(BTNA);
    gpio_set_dir(BTNA, GPIO_IN);
    gpio_pull_up(BTNA);
    gpio_set_irq_enabled_with_callback(BTNA, GPIO_IRQ_EDGE_FALL, true, &button_isr);

    gpio_init(BTNB);
    gpio_set_dir(BTNB, GPIO_IN);
    gpio_pull_up(BTNB);
    gpio_set_irq_enabled_with_callback(BTNB, GPIO_IRQ_EDGE_FALL, true, &button_isr);
}

// Função de reinicialização dos limites para valores padrão
void reset_limits() {
    int min_val = 0, max_val = 100;
    PUMP_ON_LEVEL = (float)min_val;
    PUMP_OFF_LEVEL = (float)max_val;
    show_limits_display = time_us_64() + 3000000;
}