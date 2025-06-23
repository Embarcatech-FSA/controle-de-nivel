#include "tasks.h"
#include "globals.h"
#include "buzzer.h"

void jingle() {
    for (int i = 0; i < 2; i++) {
        play_buzzer(RE);
        vTaskDelay(pdMS_TO_TICKS(300));
        
        stop_buzzer();
        vTaskDelay(pdMS_TO_TICKS(200));
    }

    stop_buzzer();
    vTaskDelay(pdMS_TO_TICKS(600));
}

void vTaskSystemControl(void *params) {
    bool critical_level_alert;

    while (true)
    {
        // --- LÓGICA DE CONTROLE DA BOMBA (HISTERESE) ---
        if ((percentual_level_value <= PUMP_ON_LEVEL) && !SHUTDOWN) {
            water_pump_state = true;
        } else if (percentual_level_value >= PUMP_OFF_LEVEL || SHUTDOWN) {
            water_pump_state = false;
        }
        // ESTADO DO TANQUE
        if(percentual_level_value < PUMP_ON_LEVEL-PUMP_ON_LEVEL*0.1) {
            tank_state = 0;
        }
        else if(PUMP_ON_LEVEL-PUMP_ON_LEVEL*0.1 < percentual_level_value && percentual_level_value< PUMP_ON_LEVEL) {
            tank_state = 1;
        }
        else if(PUMP_ON_LEVEL <  percentual_level_value && percentual_level_value < PUMP_OFF_LEVEL) {
            tank_state = 2;
        }
        else if(PUMP_OFF_LEVEL < percentual_level_value && percentual_level_value < PUMP_OFF_LEVEL+PUMP_OFF_LEVEL*0.1) {
            tank_state = 3;
        }
        else if(percentual_level_value > PUMP_OFF_LEVEL+PUMP_OFF_LEVEL*0.1) {
            tank_state = 4;
        }

        // Função para emitir alerta com o buzzer se o nível estiver crítico
        if ((tank_state == 0 || tank_state == 4) && !critical_level_alert) {
            printf((tank_state==0)?"!!! ALERTA: NÍVEL DE ÁGUA ESTÁ ABAIXO DE 10%% DO MÍNIMO PERMITIDO !!!\n":"!!! ALERTA: NÍVEL DE ÁGUA ESTÁ ACIMA DE 10%% DO MÁXIMO PERMITIDO !!!\n");
            jingle(); // Toca o som de alerta
            critical_level_alert = true; // Ativa a flag para não tocar de novo
        } else if(!(tank_state == 0 || tank_state == 4) && critical_level_alert) {
            critical_level_alert = false;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
}