#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h" // Bibliotea para Wi-Fi no Raspberry Pi Pico W
#include "lwip/tcp.h" // Biblioteca para TCP/IP



int nivel_atual = 50;      // Simulado, pode vir de sensor
int bomba_ligada = 0;      // 0 = desligada, 1 = ligada
int limite_min = 30;       // Limite mínimo inicial
int limite_max = 90;       // Limite máximo inicial


#include "credenciais.h" // Arquivo com SSID e Senha do Wi-Fi

const char HTML_BODY[] =
"<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Controle de Nível</title>"
"<style>"
"body {"
"  font-family: 'Segoe UI', sans-serif;"
"  text-align: center;"
"  background: linear-gradient(135deg, #e0f7fa, #f1f8e9);"
"  padding: 20px;"
"  color: #2c3e50;"
"}"

"h1 {"
"  color: #2c3e50;"
"  margin-bottom: 10px;"
"}"

"#nivel-container {"
"  width: 300px;"
"  height: 200px;"
"  background: #cfd8dc;"  // Azul acinzentado, simula o tanque
"  border-radius: 20px;"
"  margin: 20px auto;"
"  position: relative;"
"  box-shadow: 0 6px 15px rgba(0, 0, 0, 0.2);"
"  overflow: hidden;"
"  border: 3px solid #90a4ae;"
"}"

"#nivel {"
"  position: absolute;"
"  bottom: 0;"
"  width: 100%;"
"  background: #4CAF50;"  // Inicialmente verde
"  color: white;"
"  text-align: center;"
"  font-size: 18px;"
"  border-radius: 0 0 17px 17px;"
"  transition: height 0.5s ease, background 0.5s ease;"
"}"

"input[type='number'] {"
"  padding: 10px;"
"  margin: 8px;"
"  width: 120px;"
"  border: 1px solid #ccc;"
"  border-radius: 10px;"
"  font-size: 16px;"
"  box-shadow: inset 1px 1px 3px rgba(0,0,0,0.05);"
"  transition: border-color 0.3s;"
"}"

"input[type='number']:focus {"
"  outline: none;"
"  border-color: #007BFF;"
"}"

"button {"
"  padding: 10px 25px;"
"  margin-top: 15px;"
"  font-size: 16px;"
"  border: none;"
"  border-radius: 10px;"
"  background-color: #007BFF;"
"  color: white;"
"  cursor: pointer;"
"  transition: background-color 0.3s ease, transform 0.2s ease;"
"}"

"button:hover {"
"  background-color: #0056b3;"
"  transform: scale(1.03);"
"}"

"@media (max-width: 500px) {"
"  #nivel-container { width: 90%; height: 180px; }"
"  input[type='number'] { width: 80px; }"
"}"
"</style>"

"<script>"
"function atualizar() {"
"  fetch('/estado').then(res => res.json()).then(data => {"
"    const nivel = data.nivel;"
"    const bomba = data.bomba;"
"    const nivelDiv = document.getElementById('nivel');"
"    nivelDiv.style.height = nivel + '%';"
"    nivelDiv.innerText = nivel + '%';"
"    document.getElementById('estado_bomba').innerText = bomba ? 'Ligada' : 'Desligada';"

"    if (nivel < 30) {"
"      nivelDiv.style.background = '#f44336';"  
"    } else if (nivel < 70) {"
"      nivelDiv.style.background = '#ff9800';"  
"    } else {"
"      nivelDiv.style.background = '#4CAF50';"  
"    }"
"  });"
"}"
"function enviarLimites() {"
"  const min = document.getElementById('min').value;"
"  const max = document.getElementById('max').value;"
"  fetch(`/config?min=${min}&max=${max}`);"
"}"
"setInterval(atualizar, 1000);"
"</script></head><body>"

"<h1>Controle de Nível do Reservatório</h1>"

"<div id='nivel-container'>"
"<div id='nivel' style='height: 0%'>0%</div>"
"</div>"

"<p>Estado da bomba: <strong id='estado_bomba'>--</strong></p>"

"<h3>Configurar limites</h3>"
"<div>"
"  <input type='number' id='min' placeholder='Nível mínimo' min='0' max='100'>"
"  <input type='number' id='max' placeholder='Nível máximo' min='0' max='100'><br>"
"  <button onclick='enviarLimites()'>Atualizar Limites</button>"
"<p id='mensagem' style='color: green; font-weight: bold;'></p>"
"</div>"



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

 
    if (strstr(req, "GET /estado"))
    {
        char json_payload[128];
        int json_len = snprintf(json_payload, sizeof(json_payload),
            "{\"nivel\":%d,\"bomba\":%d}\r\n",
            nivel_atual, bomba_ligada);
        
        hs->len = snprintf(hs->response, sizeof(hs->response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n"
            "\r\n"
            "%s",
            json_len, json_payload);
        
    }
    else if (strstr(req, "GET /config?"))
    {
    int min = 0, max = 100;
    sscanf(req, "GET /config?min=%d&max=%d", &min, &max);

    // Simples validação
    if (min >= 0 && max <= 100 && min < max) {
        limite_min = min;
        limite_max = max;
    }

    const char *msg = "Limites atualizados";
    hs->len = snprintf(hs->response, sizeof(hs->response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        (int)strlen(msg), msg);
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
            bomba_ligada = 1;
        } else if (nivel_atual > limite_max) {
            bomba_ligada = 0;
        }

        sleep_ms(300);
    }
    cyw43_arch_deinit();
}
