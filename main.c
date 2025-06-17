#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h" // Bibliotea para Wi-Fi no Raspberry Pi Pico W
#include "lwip/tcp.h" // Biblioteca para TCP/IP
#include "conexao.h" // Código para conexão HTTP

// Variáveis globais
int nivel_atual = 50;      // Simulado, pode vir de sensor
int bomba_estado = 0;      // 0 = desligada, 1 = ligada
int limite_min = 30;       // Limite mínimo inicial
int limite_max = 90;       // Limite máximo inicial


#include "credenciais.h" // Arquivo com SSID e Senha do Wi-Fi (NA RAIZ DO PROJETO)



int main()
{
    stdio_init_all();
    sleep_ms(1000); // Aguarda a inicialização do console


    if(cyw43_arch_init()){
        printf("Erro ao inicializar o Wi-Fi\n");
        return 1;
    }

    cyw43_arch_enable_sta_mode();
    if(cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS,CYW43_AUTH_WPA2_AES_PSK, 10000)){
        printf("Erro ao conectar ao Wi-Fi\n");
        return 1;
    }

    start_http_server();

    while (true) {
        cyw43_arch_poll();


        /* SIMULAÇÃO DE LEITURA DO NÍVEL DO RESERVATÓRIO */
        nivel_atual += (rand() % 7) - 3; // 
        if (nivel_atual > 100) nivel_atual = 100;
        if (nivel_atual < 0) nivel_atual = 0;

        // Verifica se a bomba deve ser ligada ou desligada    
        if (nivel_atual < limite_min) {
            bomba_estado = 1;
        } else if (nivel_atual > limite_max) {
            bomba_estado = 0;
        }

        sleep_ms(300);
    }
    cyw43_arch_deinit();
}
