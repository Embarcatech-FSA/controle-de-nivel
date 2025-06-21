# **Controle de nível de reservatório de água com BitDogLab e FreeRTOS**

---

## **Objetivo** 🎯

Criar um sistema embarcado que:

- Acione uma bomba para encher o reservatório quando o nível estiver abaixo do mínimo definido.
- Desligue a bomba quando o nível atingir o máximo definido.
- Emita um alerta sonoro quando o nível estiver abaixo do mínimo.
- Monitore o estado da bomba e do nível de água em tempo real via página web.

---

## **Como funciona?** ⚙️

- Uma boia acoplada a um potenciômetro mede o nível de água no reservatório, gerando um valor analógico que é tratado pelo microcontrolador.
- Os limites mínimo e máximo são definidos no código, mas podem ser ajustados via página web.
- Se o nível estiver abaixo do mínimo, a bomba é acionada e um alerta sonoro é emitido. Caso o nível ultrapasse o máximo, a bomba é desligada.
- O estado da bomba e do nível de água é atualizado em tempo real na página web.
- O sistema é implementado usando FreeRTOS para gerenciar as tarefas de monitoramento e controle.

## Funcionalidades dos periféricos 🛠️

### **Matriz de LEDs 5x5**

- A matriz de LEDs 5x5 é usada para fornecer um feedback visual do atual nível de água. Quando o nível está em um intervalo aceitável a cor dos LEDs é verde, se o nível estiver abaixo do mínimo ou acima do máximo, os LEDs ficam vermelhos, sinalizando a necessidade de atenção.

### **Display OLED**

- O display exibe informações sobre:
  - O nível atual de água
  - O estado da bomba
  - IP para acesso à página web
  - Mensagens de erro caso não seja possível conectar a rede
  - Mensagens se a água transbordar ou se o nível estiver abaixo.
  - Novos limites mínimo e máximo definidos pelo usuário.

### **Buzzer**

- O buzzer emite um alerta sonoro quando o nível de água está abaixo ou acima dos limites definidos.

### **Botão**

- O botão é usado para redefinir os limites.

## Conexão Wi-Fi 📡

- O sistema se conecta a uma rede Wi-Fi para permitir o monitoramento remoto via página web.
- esmo que o sistema não esteja conectado à rede, o display OLED e a matriz continuarão a exibir informações sobre o nível de água e o estado da bomba.
- A página web é acessível através do IP exibido no display OLED.
- A página web permite ajustar os limites mínimo e máximo do nível de água, além de monitorar o estado da bomba e do nível de água em tempo real.

## 🛠️ Como Executar o Projeto

### 1. Configure o ambiente

- SDK do Raspberry Pi Pico devidamente instalado.
- Extensão do VS Code para Pico configurada.
- FreeRTOS instalado.

### 2. Clone o repositório

```
git clone https://github.com/Embarcatech-FSA/controle-de-nivel.git
```

### 3. Configure o caminho do FreeRTOS

- No arquivo `CMakeLists.txt`, localize a linha que define o caminho do FreeRTOS e ajuste conforme necessário. Por exemplo:

  ```cmake
  set(FREERTOS_KERNEL_PATH "/caminho/para/o/seu/FreeRTOS")
  ```

### 4. Insira as crdenciais Wi-Fi

- No arquivo `libs/include/wifi.h`, modifique as seguintes variáveis para incluir suas credenciais Wi-Fi:

  ```c
  #define WIFI_SSID "SeuSSID"
  #define WIFI_PASS "SuaSenha"
  ```

### 5. Compile e execute

- **Clean CMake** → limpeza do cache
- **Configure Project** → detectar arquivos
- **Build Project** → compila e gera `.uf2`
- **Run [USB]** → instala o firmware na placa BitDogLab

## **Equipe** 👥

- _Camila Bastos_
- _Jackson Silva_
- _Lucas Carneiro_
- _Luis Felipe_
- _Mariana Santos_
