#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// --- Configurações da Rede Wi-Fi (AP) ---
const char* ssid = "MAC-01";     // Nome da rede Wi-Fi do robô
const char* password = "a123456A"; // Senha da rede Wi-Fi (min. 8 caracteres)

ESP8266WebServer server(80); // Cria um servidor web na porta 80

// --- Definição dos Pinos dos Motores ---
// Motor Esquerdo (conectado na Saída A do Motor Shield)
const int pwmMotorLeft = D1;  // Pino PWM para controle de velocidade do Motor Esquerdo (GPIO5)
const int dirMotorLeft = D3;  // Pino Digital para controle de direção do Motor Esquerdo (GPIO0)

// Motor Direito (conectado na Saída B do Motor Shield)
const int pwmMotorRight = D2; // Pino PWM para controle de velocidade do Motor Direito (GPIO4)
const int dirMotorRight = D4; // Pino Digital para controle de direção do Motor Direito (GPIO2)

// --- Definição de Velocidades dos Motores ---
// Os valores para analogWrite no ESP8266 variam de 0 a 1023.
const int MOTOR_STOP = 0;       // Motores parados
// const int MOTOR_SLOW = 175;     // Não será usado diretamente com joystick
// const int MOTOR_NORMAL = 500;   // Não será usado diretamente com joystick
// const int MOTOR_FAST = 800;     // Não será usado diretamente com joystick
const int MAX_PWM_VALUE = 1023; // Valor máximo para analogWrite no ESP8266

// --- Variáveis de Controle de Timeout ---
unsigned long lastCommandTime = 0;
const long COMMAND_TIMEOUT_MS = 250; // Tempo em milissegundos para parar se não houver comando

// --- Funções de Controle de Motor ---

/**
 * @brief Controla a velocidade e direção de um único motor.
 * Esta função é adaptada para Shields que usam 1 pino PWM e 1 pino Digital para direção.
 * @param pwmPin Pino PWM do motor (para velocidade via analogWrite).
 * @param dirPin Pino de direção do motor (para sentido via digitalWrite HIGH/LOW).
 * @param speed Velocidade desejada (de -MAX_PWM_VALUE a MAX_PWM_VALUE).
 *              Positivo para um sentido de rotação ("para frente"), negativo para o sentido oposto ("para trás").
 */
void setMotorSpeed(int pwmPin, int dirPin, int speed) {
  // Garante que a velocidade esteja dentro dos limites PWM do ESP8266
  speed = constrain(speed, -MAX_PWM_VALUE, MAX_PWM_VALUE);

  if (speed >= 0) { // Motor no sentido "para frente"
    digitalWrite(dirPin, HIGH);  // Define a direção
    analogWrite(pwmPin, speed);  // Aplica a velocidade
  } else { // Motor no sentido "para trás" (velocidade negativa)
    digitalWrite(dirPin, LOW);   // Define a direção oposta
    analogWrite(pwmPin, -speed); // Aplica o valor absoluto da velocidade
  }
}

/**
 * @brief Para ambos os motores, definindo a velocidade PWM para 0.
 */
void stopMotors() {
  analogWrite(pwmMotorLeft, MOTOR_STOP);
  analogWrite(pwmMotorRight, MOTOR_STOP);
  // Serial.println("Ação: Motores Parados."); // Descomentar para depuração
}

// --- Funções de Manipulação de Requisições HTTP ---

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no'>
<title>SumoBot - Suporte Informática</title>
<style>
body {
    margin: 0;
    overflow: hidden;
    background-color: #282c34; /* Dark background */
    color: white;
    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
    display: flex;
    flex-direction: column;
    justify-content: space-between;
    align-items: center;
    height: 100vh;
    box-sizing: border-box;
}
#status {
    width: 100%;
    padding: 10px;
    text-align: center;
    font-size: 1.2em;
    color: #61dafb; /* Light blue for status */
}
#joystick-container {
    width: 70vmin; /* Responsive size based on viewport min dimension */
    height: 70vmin;
    max-width: 400px; /* Max size for larger screens */
    max-height: 400px;
    border-radius: 50%;
    background-color: rgba(255, 255, 255, 0.1);
    border: 2px solid rgba(255, 255, 255, 0.3);
    display: flex;
    justify-content: center;
    align-items: center;
    margin-bottom: 5vh; /* Space from bottom */
    position: relative; /* For the absolute positioning of the joystick handle */
}
#joystick {
    position: absolute; /* Allows handle to move within container */
    width: 40%; /* Joystick handle size relative to container */
    height: 40%;
    border-radius: 50%;
    background-color: #61dafb; /* Light blue handle */
    box-shadow: 0 0 15px rgba(0, 0, 0, 0.7);
    cursor: grab;
    touch-action: none; /* Prevents default touch actions like scrolling */
}
#info {
    font-size: 0.9em;
    color: #aaa;
    margin-top: 30px;
    text-align: center;
}
</style>
</head>
<body>
<div id="status">X: 0, Y: 0</div>
<div id="joystick-container">
    <div id="joystick"></div>
</div>
<div id="info">
  <h1>Mac 04</h1>
  <h6>Suporte Informática</h6>
  <br>
  Mantenha o toque/clique para controlar. Solte para parar.
</div>

<script>
const joystickContainer = document.getElementById('joystick-container');
const joystick = document.getElementById('joystick');
const statusDiv = document.getElementById('status');

let isDragging = false;
let joyX = 0; // -100 to 100
let joyY = 0; // -100 to 100 (inverted for screen coords)

let containerRect = joystickContainer.getBoundingClientRect(); // Initial rect
let centerX = containerRect.width / 2;
let centerY = containerRect.height / 2;
let radius = containerRect.width / 2; // Radius of the container

// Function to update dimensions if window resizes (especially important for mobile rotation)
function updateDimensions() {
    containerRect = joystickContainer.getBoundingClientRect();
    centerX = containerRect.width / 2;
    centerY = containerRect.height / 2;
    radius = containerRect.width / 2;
    // Reset joystick position when dimensions change
    joystick.style.left = `${centerX - joystick.offsetWidth / 2}px`;
    joystick.style.top = `${centerY - joystick.offsetHeight / 2}px`;
}
window.addEventListener('resize', updateDimensions);
// Initial call to set dimensions
updateDimensions();


function updateJoystickPosition(e) {
    if (!isDragging) return;

    let clientX, clientY;
    if (e.touches) { // For touch events
        clientX = e.touches[0].clientX;
        clientY = e.touches[0].clientY;
    } else { // For mouse events
        clientX = e.clientX;
        clientY = e.clientY;
    }

    // Get position relative to the center of the container
    const x = clientX - containerRect.left - centerX;
    const y = clientY - containerRect.top - centerY;

    // Calculate distance from center
    const distance = Math.min(radius, Math.sqrt(x * x + y * y));

    // Calculate angle
    const angle = Math.atan2(y, x);

    // Limit joystick position to within the container's circle
    const limitedX = Math.cos(angle) * distance;
    const limitedY = Math.sin(angle) * distance;

    // Position the joystick handle
    joystick.style.left = `${limitedX + centerX - joystick.offsetWidth / 2}px`;
    joystick.style.top = `${limitedY + centerY - joystick.offsetHeight / 2}px`;

    // Map to -100 to 100 range for sending to ESP
    // X: left to right (-100 to 100)
    // Y: up to down (-100 to 100), screen Y is inverted for robot's forward/backward
    joyX = Math.round((limitedX / radius) * 100);
    joyY = Math.round((limitedY / radius) * 100 * -1);

    statusDiv.textContent = `X: ${joyX}, Y: ${joyY}`;
}

function stopJoystick() {
    isDragging = false;
    // Reset joystick to center
    joystick.style.left = `${centerX - joystick.offsetWidth / 2}px`;
    joystick.style.top = `${centerY - joystick.offsetHeight / 2}px`;
    joyX = 0;
    joyY = 0;
    statusDiv.textContent = `X: 0, Y: 0`;
    sendJoystickData(); // Send zeroed values immediately to stop the robot
}

// Event Listeners for Mouse
joystickContainer.addEventListener('mousedown', (e) => {
    isDragging = true;
    updateJoystickPosition(e);
});
joystickContainer.addEventListener('mousemove', (e) => {
    if (isDragging) {
        e.preventDefault(); // Prevent text selection etc.
        updateJoystickPosition(e);
    }
});
joystickContainer.addEventListener('mouseup', stopJoystick);
joystickContainer.addEventListener('mouseleave', () => {
    if (isDragging) { // Stop if mouse leaves container while dragging
        stopJoystick();
    }
});

// Event Listeners for Touch
joystickContainer.addEventListener('touchstart', (e) => {
    isDragging = true;
    e.preventDefault(); // Prevent scrolling and default browser touch actions
    updateJoystickPosition(e);
}, { passive: false }); // Use passive: false to allow preventDefault
joystickContainer.addEventListener('touchmove', (e) => {
    if (isDragging) {
        e.preventDefault(); // Prevent scrolling
        updateJoystickPosition(e);
    }
}, { passive: false });
joystickContainer.addEventListener('touchend', stopJoystick);
joystickContainer.addEventListener('touchcancel', stopJoystick); // In case touch is interrupted

// Send data to ESP periodically
let lastSentX = null; // Use null to ensure first send happens
let lastSentY = null;
const SEND_INTERVAL = 100; // Send updates every 100 milliseconds

setInterval(() => {
    // Only send if values have changed or if it's the first time/stop
    if (joyX !== lastSentX || joyY !== lastSentY) {
        sendJoystickData();
        lastSentX = joyX;
        lastSentY = joyY;
    }
}, SEND_INTERVAL);

function sendJoystickData() {
    // Use GET request for simplicity, passing x and y as query parameters
    const url = `/joystick_control?x=${joyX}&y=${joyY}`;
    fetch(url)
        .then(response => {
            if (!response.ok) {
                console.error('Network response was not ok:', response.statusText);
            }
        })
        .catch(error => {
            console.error('Error sending joystick data:', error);
        });
}

</script>
</body>
</html>
)rawliteral";
  server.send(200, "text/html", html);
}

void handleJoystickControl() {
  // Pega os valores X e Y enviados do navegador
  int joystickX = server.arg("x").toInt(); // -100 to 100 (horizontal)
  int joystickY = server.arg("y").toInt(); // -100 to 100 (vertical, positivo para frente)

  // Zona morta: se o joystick estiver muito próximo do centro, pare os motores
  const int DEAD_ZONE = 10; // Ajuste este valor conforme a sensibilidade desejada

  if (abs(joystickX) < DEAD_ZONE && abs(joystickY) < DEAD_ZONE) {
    stopMotors();
    Serial.println("Joystick: STOP (dead zone)");
  } else {
    // Lógica de Tração Diferencial
    // joystickY controla a velocidade linear (para frente/trás)
    // joystickX controla a velocidade angular (virar)

    // Calcula as potências brutas para cada motor
    // Nota: joystickY é "frente/trás", joystickX é "esquerda/direita"
    // Motor Esquerdo: Y + X (se X > 0, vira à direita, L diminui; se X < 0, vira à esquerda, L aumenta)
    // Motor Direito: Y - X (se X > 0, vira à direita, R aumenta; se X < 0, vira à esquerda, R diminui)
    float leftMotorPower = (float)joystickY + (float)joystickX;
    float rightMotorPower = (float)joystickY - (float)joystickX;

    // Normaliza as potências para a escala -100 a 100 (base da entrada do joystick)
    // Isso evita que a soma ultrapasse o limite de controle (e.g., 100 + 100 = 200)
    float maxAbsRaw = fmax(fabs(leftMotorPower), fabs(rightMotorPower));
    if (maxAbsRaw > 100.0) { // Se o valor absoluto máximo exceder 100 (o raio do joystick)
        leftMotorPower /= (maxAbsRaw / 100.0); // Reduz proporcionalmente
        rightMotorPower /= (maxAbsRaw / 100.0);
    }

    // Escala as potências normalizadas para a faixa de PWM do motor (-MAX_PWM_VALUE a MAX_PWM_VALUE)
    int leftSpeed = map(leftMotorPower, -100, 100, -MAX_PWM_VALUE, MAX_PWM_VALUE);
    int rightSpeed = map(rightMotorPower, -100, 100, -MAX_PWM_VALUE, MAX_PWM_VALUE);

    // Aplica as velocidades aos motores
    setMotorSpeed(pwmMotorLeft, dirMotorLeft, leftSpeed);
    setMotorSpeed(pwmMotorRight, dirMotorRight, rightSpeed);

    Serial.print("Joystick: X="); Serial.print(joystickX);
    Serial.print(", Y="); Serial.print(joystickY);
    Serial.print(" -> L="); Serial.print(leftSpeed);
    Serial.print(", R="); Serial.println(rightSpeed);

    lastCommandTime = millis(); // Atualiza o tempo do último comando recebido
  }
  server.send(200, "text/plain", "OK"); // Responde ao navegador
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not found");
}

// --- Setup ---
void setup() {
  Serial.begin(115200);
  Serial.println("\nConfigurando Ponto de Acesso (AP) para Joystick...");

  // Configura os pinos de controle dos motores como SAÍDA
  pinMode(pwmMotorLeft, OUTPUT);
  pinMode(dirMotorLeft, OUTPUT);
  pinMode(pwmMotorRight, OUTPUT);
  pinMode(dirMotorRight, OUTPUT);
  stopMotors(); // Garante que os motores estão parados ao iniciar o robô

  WiFi.softAP(ssid, password); // Inicia o NodeMCU como AP
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("Ponto de Acesso (AP) iniciado! SSID: ");
  Serial.println(ssid);
  Serial.print("IP do Robô: ");
  Serial.println(myIP);

  // Define os manipuladores de requisição para o servidor web
  server.on("/", handleRoot); // Servir a página HTML do joystick
  server.on("/joystick_control", handleJoystickControl); // Receber dados do joystick
  server.onNotFound(handleNotFound); // Para URLs não reconhecidas

  server.begin(); // Inicia o servidor web
  Serial.println("Servidor HTTP iniciado.");

  lastCommandTime = millis(); // Inicializa o timer de timeout
}

// --- Loop Principal ---
void loop() {
  server.handleClient(); // Processa as requisições HTTP recebidas (incluindo as do joystick)

  // Lógica de Timeout para Parar o Robô:
  // Se nenhum comando de joystick for recebido dentro do COMMAND_TIMEOUT_MS, pare os motores.
  if (millis() - lastCommandTime > COMMAND_TIMEOUT_MS) {
    stopMotors();
  }

  // Pequeno atraso para estabilidade e para o ESP8266 realizar tarefas internas.
  delay(1); // Pode ser 1ms ou mais, dependendo da necessidade de resposta e carga.
}