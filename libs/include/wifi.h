#ifndef CYW43_H
#define CYW43_H

#include "pico/cyw43_arch.h"     // Biblioteca para arquitetura Wi-Fi da Pico com CYW43  

#include "lwip/pbuf.h"           // Lightweight IP stack - manipulação de buffers de pacotes de rede
#include "lwip/tcp.h"            // Lightweight IP stack - fornece funções e estruturas para trabalhar com o protocolo TCP
#include "lwip/netif.h"          // Lightweight IP stack - fornece funções e estruturas para trabalhar com interfaces de rede (netif)

// Credenciais WIFI
#define WIFI_SSID ""
#define WIFI_PASS ""

#define LED_PIN CYW43_WL_GPIO_LED_PIN   // GPIO do CI CYW43

#endif