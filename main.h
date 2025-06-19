#ifndef MAIN_H
#define MAIN_H


// --- Includes Comuns do Sistema ---
#include <stdio.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"


// Pinos de Hardware
#define PINO_POTENCIOMETRO        28
#define PINO_RELE                 17
#define BUZZER_PIN_1              10

// Calibração do Sensor ADC
#define VALOR_MINIMO_ADC          500
#define VALOR_MAXIMO_ADC          3500

// Variáveis de estado do sistema
extern float nivel_percentual_atual;
extern volatile bool estadoBomba;
extern bool alerta_nivel_critico_ativo;

// Variáveis de configuração (limites que podem ser alterados pela web)
extern float NIVEL_LIGAR_BOMBA;
extern float NIVEL_DESLIGAR_BOMBA;

// --- tempos do buzzer ---
#define BUZZER_ALERT_FREQ      1500 // F#6 (mais agudo e rápido)
#define BUZZER_ALERT_ON_MS     150

#endif // MAIN_H