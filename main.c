#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include "server.h"   
#include "buzzer.h"
#include "globals.h"
#include "tasks.h"

//Os Dados de wifi são definidos em wifi_task.h

SemaphoreHandle_t xDisplayMut;

volatile uint64_t show_limits_display = 0;
float percentual_level_value = 0.0f;
volatile bool water_pump_state = false;
volatile uint8_t tank_state = 1;
bool critical_level_alert = false;
float PUMP_ON_LEVEL = 20.0f;
float PUMP_OFF_LEVEL = 80.0f;
bool SHUTDOWN = false;
bool wifi_connected = false;
char ip_address_str[16] = "0.0.0.0";

int main()
{
    stdio_init_all();
    sleep_ms(2000); // Aguarda a inicialização do console

    printf("Inicializando sistema...\n");

    xDisplayMut = xSemaphoreCreateMutex();
    if (xDisplayMut == NULL) {
        panic("Falha ao criar o mutex do display");
    }

    //printf("ADC: %4d | Nivel: %6.2f%% | Bomba: %s\n", ValorPuroADC, nivel_percentual_atual, estadoBomba ? "LIGADA" : "DESLIGADA");

    xTaskCreate(vTaskWiFi, "Wi-Fi Task", configMINIMAL_STACK_SIZE + 128, NULL, 1, NULL);                        // Controla a conexão Wi-Fi
    xTaskCreate(vTaskDisplay, "Display Task", configMINIMAL_STACK_SIZE + 512, NULL, 1, NULL);                   // Controla o display ssd1306
    xTaskCreate(vTaskWaterLevelSensor, "Level Resistor Task", configMINIMAL_STACK_SIZE + 128, NULL, 1, NULL);   // Mede nível do tanque
    xTaskCreate(vTaskRelay, "Relay Task", configMINIMAL_STACK_SIZE + 128, NULL, 1, NULL);                       // Ativa ou desativa o relé
    xTaskCreate(vTaskSystemControl, "System Control Task", configMINIMAL_STACK_SIZE + 128, NULL, 1, NULL);      // Define o comportamento do sistema
    xTaskCreate(vTaskMatrixTankLevel, "Matrix Task", configMINIMAL_STACK_SIZE + 128, NULL, 1, NULL);            // Mostra o nivel do tanque na matriz

    vTaskStartScheduler();
    panic_unsupported();

    return 0;
}