#ifndef GLOBALS_H
#define GLOBALS_H

// --- Includes Comuns do Sistema ---
#include "hardware/i2c.h"
#include <stdio.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"


#define I2C_PORT i2c1   	// Define que o barramento I2C usado será o "i2c0"
#define I2C_SDA 14      	// Define que o pino GPIO 14 será usado como SDA (linha de dados do I2C)
#define I2C_SCL 15      	// Define que o pino GPIO 15 será usado como SCL (linha de clock do I2C)
#define DISPLAY_ADDRESS 0x3C	    // Define o endereço I2C do dispositivo (0x3C é o endereço padrão de muitos displays OLED SSD1306)
#define DISPLAY_WIDTH       128
#define DISPLAY_HEIGHT      64

#define BTNA 5          	// Botão A
#define BTNB 6          	// Botão B
#define WS2812_PIN 7    	// Matriz de LEDs 5x5
#define LED_GREEN 11        // Led verde
#define LED_RED 13          // Led vermelho

// Pinos de Hardware
#define LEVEL_SENSOR_PIN    28
#define RELAY_PIN           17
#define BUZZER_PIN          10

// Calibração do Sensor ADC
#define ADC_MIN_VALUE   500
#define ADC_MAX_VALUE   3500

// Variáveis de estado do sistema
extern float percentual_level_value;
extern volatile bool water_pump_state;
extern volatile uint8_t tank_state;

// Variáveis de configuração (limites que podem ser alterados pela web)
extern volatile uint64_t show_limits_display;
extern float PUMP_ON_LEVEL;
extern float PUMP_OFF_LEVEL;
extern bool SHUTDOWN;
extern uint8_t MENU_MODE;

// --- tempos do buzzer ---
#define BUZZER_ALERT_FREQ   1500 // F#6 (mais agudo e rápido)
#define BUZZER_ALERT_ON_MS  150

// Frequências das notas musicais (em Hz)
enum Notes {
    DO = 2640, // Dó
    RE = 2970, // Ré
    MI = 3300, // Mi
    FA = 3520, // Fá
    SOL = 3960, // Sol
    LA = 4400, // Lá
    SI = 4950,  // Si
    DO_ALTO = 5280,  // Dó uma oitava acima (C5)
    LA_BAIXO = 880
};

#endif // GLOBALS_H