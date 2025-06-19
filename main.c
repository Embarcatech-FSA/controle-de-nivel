#include "main.h"
#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include "conexao.h"   
#include "credenciais.h"    //arquivo com SSID e Senha
#include "buzzer.h"

float nivel_percentual_atual = 0.0f;
volatile bool estadoBomba = false;
bool alerta_nivel_critico_ativo = false;
float NIVEL_LIGAR_BOMBA = 20.0f;
float NIVEL_DESLIGAR_BOMBA = 80.0f;

/**
 * @brief Mapeia um número de uma faixa para outra.
 */
float mapear(int valor, int min_entrada, int max_entrada, float min_saida, float max_saida) {
    if (valor <= min_entrada) return min_saida;
    if (valor >= max_entrada) return max_saida;
    return min_saida + ((float)(valor - min_entrada) / (float)(max_entrada - min_entrada)) * (max_saida - min_saida);
}


int main()
{
    stdio_init_all();
    buzzer_init();
    sleep_ms(2000); // Aguarda a inicialização do console
    printf("Inicializando sistema...\n");

    // --- INICIALIZAÇÃO DO HARDWARE ---
    printf("Inicializando ADC e GPIO...\n");
    adc_init();
    adc_gpio_init(PINO_POTENCIOMETRO);
    adc_select_input(2);

    gpio_init(PINO_RELE);
    gpio_set_dir(PINO_RELE, GPIO_OUT);
    gpio_put(PINO_RELE, false); //bomba desligada

    // --- INICIALIZAÇÃO DO WI-FI ---
    printf("Inicializando Wi-Fi...\n");
    if(cyw43_arch_init()){
        printf("ERRO: Falha ao inicializar o Wi-Fi\n");
        return 1;
    }

    cyw43_arch_enable_sta_mode();
    printf("Conectando a rede: %s\n", WIFI_SSID);
    if(cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000)){
        printf("ERRO: Falha ao conectar ao Wi-Fi\n");
        return 1;
    }
    printf("Conectado com sucesso!\n");
    printf("--> ACESSE A PÁGINA EM: http://%s\n", ip4addr_ntoa(netif_ip4_addr(netif_default)));

    // --- INICIALIZAÇÃO DO SERVIDOR WEB ---
    start_http_server();

    printf("\n--- Sistema online e operacional ---\n\n");

    while (true) {
        // Essencial para o funcionamento do Wi-Fi e do servidor web
        cyw43_arch_poll();

        // --- LEITURA DO SENSOR DE NÍVEL ---
        uint16_t ValorPuroADC = adc_read();
        nivel_percentual_atual = mapear(ValorPuroADC, VALOR_MINIMO_ADC, VALOR_MAXIMO_ADC, 0.0f, 100.0f);

        // --- LÓGICA DE CONTROLE DA BOMBA (HISTERESE) ---
        if (nivel_percentual_atual < NIVEL_LIGAR_BOMBA) {
            estadoBomba = true;
        } else if (nivel_percentual_atual > NIVEL_DESLIGAR_BOMBA) {
            estadoBomba = false;
        }
        
        // --- ATUAÇÃO NO RELÉ ---
        gpio_put(PINO_RELE, estadoBomba);


        /*
         * AQUI VOCÊ PODE ADICIONAR AS OUTRAS FUNÇÕES
         */
        // Função para escrever as informações no display

        // Função para atualizar a matriz de LEDs e representar o nível do reservatório

        // Função para acionar o LED RGB correspondente ao estado da bomba

        // Função para emitir alerta com o buzzer se o nível estiver crítico
        if (nivel_percentual_atual < 5.0 && !alerta_nivel_critico_ativo) {
            // Condição para tocar: nível baixo E o alarme não está ativo
            printf("!!! NÍVEL CRÍTICO DETECTADO !!!\n");
            buzzer_play_tone(BUZZER_ALERT_FREQ, BUZZER_ALERT_ON_MS); // Toca o som de alerta
            alerta_nivel_critico_ativo = true; // Ativa a flag para não tocar de novo
        } else if (nivel_percentual_atual >= 5.0) {
            // Se o nível voltar ao normal, rearma o sistema de alarme
            alerta_nivel_critico_ativo = false;
        }

        // Imprime o status no console serial para depuração
        printf("ADC: %4d | Nivel: %6.2f%% | Bomba: %s\n", ValorPuroADC, nivel_percentual_atual, estadoBomba ? "LIGADA" : "DESLIGADA");


        sleep_ms(500);
    }

    cyw43_arch_deinit();
    return 0;
}