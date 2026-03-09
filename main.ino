#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// --- Configuración de Red ---
const char* ssid = "Wemos_RC_Pro";
const char* password = "WemosPro";

// --- Configuración de Pines ---
const int motorPin1 = D6;  // Dirección 1
const int motorPin2 = D7;  // Dirección 2
const int enablePin = D8;  // PWM para Velocidad

ESP8266WebServer server(80);

int currentSpeed = 200; // Velocidad inicial (0-255)
bool isMoving = false;
unsigned long lastWatchdog = 0;
const unsigned long WATCHDOG_TIMEOUT = 2500; // 2.5 segundos de gracia

// HTML, CSS y JS embebidos con diseño Premium
const char INDEX_HTML[] PROGMEM = R"=====("
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>Wemos RC Pro</title>
    <link href="https://fonts.googleapis.com/css2?family=Orbitron:wght@400;700&family=Inter:wght@300;500&display=swap" rel="stylesheet">
    <style>
        :root {
            --bg: #0f172a;
            --glass: rgba(30, 41, 59, 0.7);
            --primary: #22d3ee;
            --secondary: #10b981;
            --danger: #ef4444;
            --text: #f8fafc;
        }

        * {
            box-sizing: border-box;
            -webkit-tap-highlight-color: transparent;
        }

        body {
            font-family: 'Inter', sans-serif;
            background: radial-gradient(circle at top right, #1e293b, #0f172a);
            color: var(--text);
            margin: 0;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            min-height: 100vh;
            overflow: hidden;
        }

        .header {
            margin-bottom: 2rem;
            text-align: center;
        }

        h1 {
            font-family: 'Orbitron', sans-serif;
            font-size: 1.8rem;
            letter-spacing: 2px;
            color: var(--primary);
            text-shadow: 0 0 15px rgba(34, 211, 238, 0.5);
            margin: 0;
        }

        .status {
            font-size: 0.8rem;
            color: var(--secondary);
            margin-top: 0.5rem;
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 5px;
        }

        .status-dot {
            width: 8px;
            height: 8px;
            background: var(--secondary);
            border-radius: 50%;
            box-shadow: 0 0 8px var(--secondary);
            animation: pulse 2s infinite;
        }

        @keyframes pulse {
            0% { opacity: 0.4; }
            50% { opacity: 1; }
            100% { opacity: 0.4; }
        }

        .control-panel {
            background: var(--glass);
            backdrop-filter: blur(12px);
            border: 1px solid rgba(255, 255, 255, 0.1);
            border-radius: 30px;
            padding: 2rem;
            box-shadow: 0 25px 50px -12px rgba(0, 0, 0, 0.5);
            width: 90%;
            max-width: 400px;
            display: flex;
            flex-direction: column;
            gap: 1.5rem;
        }

        .direction-controls {
            display: grid;
            grid-template-columns: 1fr;
            gap: 1rem;
        }

        .btn {
            padding: 1.5rem;
            border: none;
            border-radius: 16px;
            font-family: 'Orbitron', sans-serif;
            font-size: 1.2rem;
            font-weight: bold;
            color: white;
            cursor: pointer;
            transition: all 0.2s cubic-bezier(0.4, 0, 0.2, 1);
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 10px;
            box-shadow: 0 4px 6px -1px rgba(0, 0, 0, 0.1);
            touch-action: none;
            -webkit-user-select: none;
            user-select: none;
        }

        .btn:active {
            transform: scale(0.95);
            filter: brightness(1.2);
        }

        .fwd { background: linear-gradient(135deg, #10b981, #059669); }
        .bwd { background: linear-gradient(135deg, #3b82f6, #2563eb); }
        .stop { background: linear-gradient(135deg, #ef4444, #dc2626); }

        .slider-container {
            margin-top: 1rem;
        }

        .slider-label {
            display: flex;
            justify-content: space-between;
            margin-bottom: 0.8rem;
            font-size: 0.9rem;
            color: #94a3b8;
        }

        input[type=range] {
            -webkit-appearance: none;
            width: 100%;
            background: transparent;
        }

        input[type=range]::-webkit-slider-runnable-track {
            width: 100%;
            height: 8px;
            background: #334155;
            border-radius: 4px;
        }

        input[type=range]::-webkit-slider-thumb {
            -webkit-appearance: none;
            height: 24px;
            width: 24px;
            border-radius: 50%;
            background: var(--primary);
            cursor: pointer;
            margin-top: -8px;
            box-shadow: 0 0 15px var(--primary);
        }

        .footer {
            margin-top: 2rem;
            color: #64748b;
            font-size: 0.7rem;
            text-transform: uppercase;
            letter-spacing: 1px;
        }
    </style>
</head>
<body>
    <div class="header">
        <h1>WEMOS RC PRO</h1>
        <div class="status">
            <div class="status-dot"></div>
            SISTEMA ONLINE
        </div>
    </div>

    <div class="control-panel">
        <div class="direction-controls">
            <button class="btn fwd" 
                onpointerdown="event.preventDefault(); sendCommand('/forward')" 
                onpointerup="sendCommand('/stop')" 
                onpointerleave="sendCommand('/stop')"
                onpointercancel="sendCommand('/stop')">
                AVANZAR ▲
            </button>
            
            <button class="btn stop" onclick="sendCommand('/stop')">
                PARAR 🛑
            </button>
            
            <button class="btn bwd" 
                onpointerdown="event.preventDefault(); sendCommand('/backward')" 
                onpointerup="sendCommand('/stop')" 
                onpointerleave="sendCommand('/stop')"
                onpointercancel="sendCommand('/stop')">
                RETROCEDER ▼
            </button>
        </div>

        <div class="slider-container">
            <div class="slider-label">
                <span>POTENCIA</span>
                <span id="speedVal">78%</span>
            </div>
            <input type="range" min="0" max="255" value="200" oninput="updateSpeed(this.value)">
        </div>
    </div>

    <div class="footer">UDONE Engine &bull; Motor Control v1</div>

    <script>
        let lastSpeedSend = 0;
        let speedTimeout;

        function sendCommand(path) {
            console.log('Sending:', path);
            fetch(path).catch(err => console.error('Fetch Error:', err));
        }

        function updateSpeed(val) {
            const percent = Math.round((val / 255) * 100);
            document.getElementById('speedVal').innerText = percent + '%';
            
            // Throttling: Enviar máximo cada 100ms
            clearTimeout(speedTimeout);
            const now = Date.now();
            if (now - lastSpeedSend > 100) {
                sendCommand('/speed?val=' + val);
                lastSpeedSend = now;
            } else {
                speedTimeout = setTimeout(() => {
                    sendCommand('/speed?val=' + val);
                    lastSpeedSend = Date.now();
                }, 100);
            }
        }

        // Prevent context menu on long press
        window.oncontextmenu = function(event) {
            event.preventDefault();
            event.stopPropagation();
            return false;
        };

        // Watchdog Heartbeat: Enviar señal de vida cada 1 segundo
        setInterval(() => {
            fetch('/ping').catch(() => {});
        }, 1000);
    </script>
</body>
</html>
)=====";


// Helper interno para evitar duplicidad de código y asegurar estado LOW
void stopMotorHelper() {
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  analogWrite(enablePin, 0);
  isMoving = false;
  delay(10); // Breve pausa para proteger el Puente H
}

void handleRoot() {
  lastWatchdog = millis(); // Reset al cargar la web
  server.send(200, "text/html", FPSTR(INDEX_HTML));
}

void handlePing() {
  lastWatchdog = millis();
  server.send(200, "text/plain", "PONG");
}

void moveForward() {
  lastWatchdog = millis();
  stopMotorHelper(); // Seguridad: Parar antes de cambiar dirección
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
  analogWrite(enablePin, currentSpeed);
  isMoving = true;
  server.send(200, "text/plain", "OK Forward");
  Serial.println("Motor: Forward");
}

void moveBackward() {
  lastWatchdog = millis();
  stopMotorHelper(); // Seguridad: Parar antes de cambiar dirección
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  analogWrite(enablePin, currentSpeed);
  isMoving = true;
  server.send(200, "text/plain", "OK Backward");
  Serial.println("Motor: Backward");
}

void stopMotor() {
  lastWatchdog = millis();
  stopMotorHelper();
  server.send(200, "text/plain", "OK Stop");
  Serial.println("Motor: Stopped");
}

void changeSpeed() {
  lastWatchdog = millis();
  if (server.hasArg("val")) {
    currentSpeed = server.arg("val").toInt();
    if (isMoving) analogWrite(enablePin, currentSpeed);
    server.send(200, "text/plain", "OK Speed set");
    Serial.print("Speed updated: ");
    Serial.println(currentSpeed);
  } else {
    server.send(400, "text/plain", "Missing val");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n--- Iniciando Wemos RC Pro ---");
  
  pinMode(enablePin, OUTPUT);
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  
  // Configuración PWM (Frecuencia y Rango)
  analogWriteRange(255);
  analogWriteFreq(5000); 
  
  stopMotorHelper(); // Asegurar que esté apagado al inicio

  // Configurar IP fija (más estable)
  IPAddress local_IP(192, 168, 4, 1);
  IPAddress gateway(192, 168, 4, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(local_IP, gateway, subnet);

  // Configurar Punto de Acceso (Access Point)
  WiFi.mode(WIFI_AP);
  // Parámetros: SSID, Password, canal 1, red visible, máximo 1 cliente
  if (WiFi.softAP(ssid, password, 1, 0, 1)) {
    Serial.print("Punto de Acceso iniciado: ");
    Serial.println(ssid);
  } else {
    Serial.println("Error al iniciar Punto de Acceso");
  }
  
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("Dirección IP: ");
  Serial.println(myIP);

  // Definir rutas del servidor
  server.on("/", handleRoot);
  server.on("/ping", handlePing);
  server.on("/forward", moveForward);
  server.on("/backward", moveBackward);
  server.on("/stop", stopMotor);
  server.on("/speed", changeSpeed);

  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  server.handleClient();

  // Watchdog de seguridad: detiene el motor si se pierde la conexión
  if (isMoving && (millis() - lastWatchdog > WATCHDOG_TIMEOUT)) {
    Serial.println("Watchdog Timeout! La conexión se perdió. Deteniendo motor.");
    stopMotorHelper();
  }
}