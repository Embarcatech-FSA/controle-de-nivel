#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h" // Bibliotea para Wi-Fi no Raspberry Pi Pico W
#include "lwip/tcp.h" // Biblioteca para TCP/IP

#define LED_RED 13

#include "credenciais.h" // Arquivo com SSID e Senha do Wi-Fi

const char HTML_BODY[] =
    "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Controle de nível de abastecimento</title>"
    "<style>"
    "body { font-family: sans-serif; text-align: center; padding: 10px; margin: 0; background: #f9f9f9; }"
    ".botao { font-size: 20px; padding: 10px 30px; margin: 10px; border: none; border-radius: 8px; }"
    ".on { background: #4CAF50; color: white; }"
    ".off { background: #f44336; color: white; }"
    "@media (max-width: 600px) { .botao { width: 80%; font-size: 18px; } }"
    "</style>"

    "<script>"
    "function sendCommand(cmd) { fetch('/led/' + cmd); }"
    "function atualizar() {"
    "  fetch('/estado').then(res => res.json()).then(data => {"
    "    document.getElementById('estado').innerText = data.led ? 'Ligado' : 'Desligado';"
    "  });"
    "}"
    "setInterval(atualizar, 1000);"
    "</script></head><body>"

    "<h1>Controle do LED</h1>"

    "<p>Estado do LED: <span id='estado'>--</span></p>"


    "<button class='botao on' onclick=\"sendCommand('on')\">Ligar</button>"
    "<button class='botao off' onclick=\"sendCommand('off')\">Desligar</button>"

    "<hr style='margin-top: 20px;'>"
    "<p style='font-size: 15px; color: #336699; font-style: italic; max-width: 90%; margin: 10px auto;'>"
    "Utilização da BitDogLab para exemplificar a comunicação via rede Wi-Fi utilizando o protocolo HTML com JavaScript"
    "</p>"

    "</body></html>";

struct http_state
{
    char response[4096];
    size_t len;
    size_t sent;
};

static err_t http_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    struct http_state *hs = (struct http_state *)arg;
    hs->sent += len;
    if (hs->sent >= hs->len)
    {
        tcp_close(tpcb);
        free(hs);
    }
    return ERR_OK;
}

static err_t http_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    if (!p)
    {
        tcp_close(tpcb);
        return ERR_OK;
    }

    char *req = (char *)p->payload;
    struct http_state *hs = malloc(sizeof(struct http_state));
    if (!hs)
    {
        pbuf_free(p);
        tcp_close(tpcb);
        return ERR_MEM;
    }
    hs->sent = 0;

    if (strstr(req, "GET /led/on"))
    {
        gpio_put(LED_RED, 1);
        const char *txt = "Ligado";
        hs->len = snprintf(hs->response, sizeof(hs->response),
                           "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/plain\r\n"
                           "Content-Length: %d\r\n"
                           "Connection: close\r\n"
                           "\r\n"
                           "%s",
                           (int)strlen(txt), txt);
    }
    else if (strstr(req, "GET /led/off"))
    {
        gpio_put(LED_RED, 0);
        const char *txt = "Desligado";
        hs->len = snprintf(hs->response, sizeof(hs->response),
                           "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/plain\r\n"
                           "Content-Length: %d\r\n"
                           "Connection: close\r\n"
                           "\r\n"
                           "%s",
                           (int)strlen(txt), txt);
    }    
    else if (strstr(req, "GET /estado"))
    {
        char json_payload[96];
        int json_len = snprintf(json_payload, sizeof(json_payload),
                                "{\"led\":%d}\r\n",
                                gpio_get(LED_RED));

        hs->len = snprintf(hs->response, sizeof(hs->response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        json_len, json_payload);
    }
    else
    {
        hs->len = snprintf(hs->response, sizeof(hs->response),
                           "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/html\r\n"
                           "Content-Length: %d\r\n"
                           "Connection: close\r\n"
                           "\r\n"
                           "%s",
                           (int)strlen(HTML_BODY), HTML_BODY);
    }

    tcp_arg(tpcb, hs);
    tcp_sent(tpcb, http_sent);

    tcp_write(tpcb, hs->response, hs->len, TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);

    pbuf_free(p);
    return ERR_OK;
}

static err_t connection_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    tcp_recv(newpcb, http_recv);
    return ERR_OK;
}

static void start_http_server(void)
{
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb)
    {
        printf("Erro ao criar PCB TCP\n");
        return;
    }
    if (tcp_bind(pcb, IP_ADDR_ANY, 80) != ERR_OK)
    {
        printf("Erro ao ligar o servidor na porta 80\n");
        return;
    }
    pcb = tcp_listen(pcb);
    tcp_accept(pcb, connection_callback);
    printf("Servidor HTTP rodando na porta 80...\n");
}


int main()
{
    stdio_init_all();
    sleep_ms(1000); // Aguarda a inicialização do console

    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_put(LED_RED, 0);

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

        sleep_ms(300);
    }
    cyw43_arch_deinit();
}
