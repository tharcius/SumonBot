# 🤖 SumoBot WiFi Joystick Control (ESP8266 + Motor Shield)

Um projeto de robótica para um SumoBot (ou carrinho RC) controlado remotamente via uma interface web com um joystick virtual, utilizando um NodeMCU ESP8266 e um Motor Shield L293D. Este projeto permite controlar o robô de qualquer dispositivo (smartphone, tablet, PC) que esteja conectado à rede Wi-Fi criada pelo próprio robô.

## 🚀 Visão Geral

Este projeto transforma um NodeMCU ESP8266 em um ponto de acesso Wi-Fi, servindo uma página web com um joystick virtual. Ao interagir com o joystick no navegador, comandos são enviados para o NodeMCU, que por sua vez controla os motores do robô através de um Motor Shield L293D. A lógica de tração diferencial é utilizada para traduzir os movimentos do joystick em velocidades individuais para cada motor, permitindo manobras precisas. Um sistema de timeout garante que o robô pare automaticamente se a conexão for perdida ou nenhum comando for recebido.

## ✨ Funcionalidades

*   **Controle Remoto via WiFi:** Utilize qualquer navegador web em um dispositivo conectado à rede Wi-Fi do robô.
*   **Joystick Virtual:** Interface intuitiva na tela para movimentos fluidos (frente, trás, esquerda, direita, diagonais).
*   **Ponto de Acesso (AP) Autônomo:** O próprio ESP8266 cria sua rede Wi-Fi, eliminando a necessidade de um roteador externo.
*   **Tração Diferencial:** Lógica de controle que permite manobras precisas, ajustando a velocidade de cada motor com base na posição do joystick.
*   **Sistema de Timeout:** O robô para automaticamente após um curto período de inatividade ou perda de conexão, garantindo segurança.
*   **Base Robusta:** Construído sobre a plataforma ESP8266, conhecida por sua capacidade Wi-Fi e facilidade de programação com Arduino IDE.

## 🛠️ Componentes Necessários

*   **1x NodeMCU ESP8266** (modelo Amica, ESP-12E, ou similar)
*   **1x Motor Shield L293D** (ou outro driver de motor compatível com controle de 2 pinos por motor)
*   **2x Motores DC** (com caixa de redução, ideais para robótica)
*   **3x Bateria** para alimentar o NodeMCU e os motores (e.g., BL 18650, ~4.2V)
*   **Fios Jumper**
*   **Plataforma do Robô** (chassi, rodas, etc.)

## �� Montagem Hardware

1.  **Conexão do Motor Shield ao NodeMCU:**
    *   Encaixe o Motor Shield L293D diretamente nos pinos do NodeMCU. Verifique a pinagem para garantir que os pinos de controle (PWM e Direção) estejam corretamente mapeados.
    *   Para este código, os pinos estão configurados da seguinte forma:
        *   **Motor Esquerdo:** `pwmMotorLeft = D1` (GPIO5), `dirMotorLeft = D3` (GPIO0)
        *   **Motor Direito:** `pwmMotorRight = D2` (GPIO4), `dirMotorRight = D4` (GPIO2)
    *   *Atenção:* O pino `D0` (GPIO16) pode ter limitações para alguns usos de `SoftwareSerial`, mas para `digitalWrite` e `analogWrite` via o Motor Shield, funciona normalmente.

2.  **Conexão dos Motores DC:**
    *   Conecte os dois motores DC às saídas de motor do L293D. Geralmente, há saídas marcadas como `OUT1`, `OUT2` (para o Motor A) e `OUT3`, `OUT4` (para o Motor B).
    *   Verifique a polaridade: se um motor girar no sentido inverso, basta inverter os fios dele no Motor Shield.

3.  **Alimentação:**
    *   Conecte a bateria de 7.4V (2S LiPo/Li-ion) aos terminais de alimentação do Motor Shield (geralmente marcados como `VIN` e `GND`). O Motor Shield deve ter um regulador de tensão para alimentar o NodeMCU (geralmente via o pino Vin do NodeMCU, mas verifique a documentação do seu shield).
    *   **IMPORTANTE:** Certifique-se de que o Motor Shield está configurado para fornecer a tensão correta (geralmente 5V ou 3.3V) ao NodeMCU, ou alimente o NodeMCU separadamente via USB se o shield não tiver essa capacidade integrada ou você não tiver certeza.

## 💻 Configuração Software

1.  **Instale o Arduino IDE:** Se você ainda não tem, baixe e instale o [Arduino IDE](https://www.arduino.cc/en/software).
2.  **Adicione Suporte ao ESP8266:**
    *   No Arduino IDE, vá em `Arquivo > Preferências`.
    *   Em "URLs Adicionais para Gerenciadores de Placas", adicione: `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
    *   Vá em `Ferramentas > Placa > Gerenciador de Placas...`. Pesquise por "ESP8266" e instale o pacote "esp8266".
3.  **Selecione a Placa e Porta:**
    *   Vá em `Ferramentas > Placa` e selecione `NodeMCU 1.0 (ESP-12E Module)`.
    *   Vá em `Ferramentas > Porta` e selecione a porta COM correta para o seu NodeMCU.
4.  **Bibliotecas:** Certifique-se de que as bibliotecas `ESP8266WiFi` e `ESP8266WebServer` estão disponíveis. Elas geralmente vêm com a instalação do pacote ESP8266.
5.  **Copie o Código:** Copie o código-fonte fornecido (`.ino`) para o Arduino IDE.
6.  **Personalize as Configurações:**
    *   No início do código, altere `ssid` e `password` para o nome e senha que você deseja para a rede Wi-Fi do seu robô.
    ```cpp
    const char* ssid = "MEU_SUMOBOT_WIFI";     // Nome da rede Wi-Fi do robô
    const char* password = "MINHA_SENHA_FORTE"; // Senha da rede Wi-Fi (mín. 8 caracteres)
    ```
7.  **Faça Upload:** Clique no botão "Upload" (seta para a direita) no Arduino IDE para carregar o código para o seu NodeMCU.

## 🕹️ Como Usar

1.  **Ligue o Robô:** Após o upload, energize o seu SumoBot. O NodeMCU iniciará a rede Wi-Fi.
2.  **Conecte seu Dispositivo:** No seu smartphone, tablet ou computador, procure por redes Wi-Fi e conecte-se à rede com o SSID que você configurou (ex: `MAC-01`). Insira a senha definida.
3.  **Acesse a Interface:** Abra um navegador web (Chrome, Firefox, Safari, Edge, etc.) no seu dispositivo conectado e digite o endereço IP do NodeMCU na barra de endereços. O IP padrão em modo AP é geralmente `192.168.4.1`.
4.  **Controle o Robô:**
    *   A página web exibirá o joystick virtual.
    *   Toque/clique e arraste o círculo menor (o "handle" do joystick) dentro do círculo maior.
    *   Mova para cima para ir para frente, para baixo para ir para trás.
    *   Mova para os lados para virar.
    *   Mova em diagonais para combinar movimentos e viradas.
    *   Quando você soltar o handle do joystick, o robô parará automaticamente após alguns milissegundos (devido ao sistema de timeout).

## �� Lógica de Controle (Tração Diferencial)

O código utiliza um método de **tração diferencial** para controlar os motores. Basicamente:

*   A componente **vertical** (Y) do joystick controla a velocidade de deslocamento do robô (para frente e para trás).
*   A componente **horizontal** (X) do joystick controla a rotação do robô (virar para a esquerda ou direita).

Essas duas componentes são combinadas para calcular as velocidades individuais de cada motor:
*   `velocidade_motor_esquerdo = joystickY + joystickX`
*   `velocidade_motor_direito = joystickY - joystickX`

Essas velocidades são então normalizadas e mapeadas para a faixa de valores PWM (`0-1023`) que os motores podem entender.

## �� Mecanismo de Timeout

Para garantir a segurança e evitar que o robô continue em movimento descontrolado se a conexão Wi-Fi for perdida ou se o usuário simplesmente soltar o joystick, um mecanismo de timeout foi implementado. Se nenhum comando do joystick for recebido por um período de `COMMAND_TIMEOUT_MS` (definido como 250ms), os motores são automaticamente parados.

##  troubleshooting

*   **Robô não conecta à rede Wi-Fi:**
    *   Verifique se o `ssid` e `password` no código correspondem aos que você está tentando usar.
    *   Certifique-se de que a senha tem no mínimo 8 caracteres.
    *   Verifique se o NodeMCU está energizado e o código foi carregado corretamente.
    *   Monitore a Serial do NodeMCU (com 115200 baud) para mensagens de erro durante a inicialização do AP.
*   **Página web não carrega / "This site can't be reached":**
    *   Verifique se seu dispositivo está realmente conectado à rede Wi-Fi do robô.
    *   Confirme o endereço IP do robô (geralmente `192.168.4.1` em modo AP).
    *   Verifique o Monitor Serial do NodeMCU para ver o IP que ele está reportando.
*   **Motores não respondem / respondem de forma errada:**
    *   Verifique as conexões dos motores ao Motor Shield.
    *   Verifique as conexões do Motor Shield ao NodeMCU.
    *   Certifique-se de que a bateria está carregada e fornecendo energia suficiente tanto para o Motor Shield quanto para o NodeMCU.
    *   Inverta os fios de um motor se ele estiver girando no sentido oposto ao esperado.
    *   Aumente o `COMMAND_TIMEOUT_MS` se o robô estiver parando muito rapidamente.
    *   Ajuste o `DEAD_ZONE` para a sensibilidade do joystick.
*   **Joystick funciona, mas movimentos são erráticos/lentos:**
    *   Ajuste as constantes de velocidade (`MOTOR_NORMAL`, `MAX_PWM_VALUE`) no código para corresponder à potência dos seus motores.
    *   Verifique a fonte de alimentação. Motores exigem corrente, e uma fonte fraca pode causar movimentos erráticos.

## 🔧 Personalização

*   **Nome e Senha da Rede Wi-Fi:** Modifique as variáveis `ssid` e `password` no código.
*   **Sensibilidade do Joystick:** Ajuste `DEAD_ZONE` no `handleJoystickControl()` para controlar a "zona morta" central do joystick.
*   **Velocidade Máxima:** A velocidade máxima é determinada por `MAX_PWM_VALUE`. Você pode ajustá-lo, mas o valor `1023` é o máximo para o ESP8266.
*   **Tempo de Timeout:** Ajuste `COMMAND_TIMEOUT_MS` para mudar quanto tempo o robô espera antes de parar.

## 📚 Créditos

*   **Desenvolvido e Adaptado por Tharcius** para um projeto da Suporte Informática

## 📄 Licença

Este projeto é de código aberto e está sob a licença MIT. Sinta-se à vontade para usá-lo, modificá-lo e distribuí-lo conforme suas necessidades.

---
