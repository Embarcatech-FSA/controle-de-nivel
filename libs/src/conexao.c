#include "conexao.h"
#include <string.h>
#include <stdio.h>


extern float nivel_percentual_atual;
extern volatile bool estadoBomba;
extern float NIVEL_LIGAR_BOMBA;
extern float NIVEL_DESLIGAR_BOMBA;


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
"  background: #cfd8dc;"
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
"  background: #4CAF50;"
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
"</style>"
"<script>"
"function atualizar() {"
"  fetch('/estado').then(res => res.json()).then(data => {"
"    const nivel = data.nivel;"
"    const bomba = data.bomba;"
"    const nivelDiv = document.getElementById('nivel');"
"    nivelDiv.style.height = nivel + '%';"
"    nivelDiv.innerText = nivel.toFixed(1) + '%';"
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
"  if (!min || !max) { alert('Preencha ambos os campos!'); return; }"
// LINHA CORRIGIDA ABAIXO:
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
"  <label for='min'>Ligar em:</label>"
"  <input type='number' id='min' placeholder='ex: 20' min='0' max='100'>"
"  <label for='max'>Desligar em:</label>"
"  <input type='number' id='max' placeholder='ex: 80' min='0' max='100'><br>"
"  <button onclick='enviarLimites()'>Atualizar Limites</button>"
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
            "{\"nivel\":%.1f,\"bomba\":%d}\r\n",
            nivel_percentual_atual, estadoBomba);
        
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
        int min_val = 0, max_val = 100;
        sscanf(req, "GET /config?min=%d&max=%d", &min_val, &max_val);

        if (min_val >= 0 && max_val <= 100 && min_val < max_val) {
            NIVEL_LIGAR_BOMBA = (float)min_val;
            NIVEL_DESLIGAR_BOMBA = (float)max_val;
        }

        const char *msg = "OK";
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

void start_http_server(void)
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