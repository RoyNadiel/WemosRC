# Wemos RC Pro 🚗💨

Un sistema de control remoto premium para vehículos RC basado en **Wemos D1 Mini (ESP8266)**. Este proyecto permite controlar motores DC a través de una interfaz web moderna y receptiva, con medidas de seguridad integradas.

## ✨ Características

- **Diseño Premium**: Interfaz web con estética futurista (Glassmorphism), fuentes modernas (Orbitron & Inter) y animaciones suaves.
- **Control en Tiempo Real**: Botones táctiles de alta respuesta para avance, retroceso y parada.
- **Control de Velocidad**: Deslizador PWM ajustable para una aceleración precisa.
- **Watchdog de Seguridad**: El sistema detiene automáticamente el motor si se pierde la conexión con el controlador por más de 2.5 segundos.
- **Modo Punto de Acceso (AP)**: El Wemos crea su propia red WiFi para una conexión directa y estable.

## 🛠️ Hardware Requerido

- **Microcontrolador**: Wemos D1 Mini (o cualquier ESP8266).
- **Driver de Motor**: L298N o similar (Puente H).
- **Alimentación**: Batería 2S (7.4V) con BMS/cargador recomendado.
- **Chasis RC**: Cualquier chasis estándar con motor DC.

## 📌 Configuración de Pines

| Pin Wemos | Función      | Descripción          |
| --------- | ------------ | -------------------- |
| **D6**    | Motor IN1    | Dirección 1          |
| **D7**    | Motor IN2    | Dirección 2          |
| **D8**    | Enable (PWM) | Control de Velocidad |

## 🚀 Instalación y Uso

1. Carga el código `main.ino` a tu Wemos D1 Mini usando el IDE de Arduino.
2. Asegúrate de tener instalada la placa ESP8266 en el gestor de tarjetas.
3. Una vez encendido, busca la red WiFi:
   - **SSID**: `Wemos_RC_Pro`
   - **Password**: `WemosPro`
4. Conéctate a la red y abre en tu navegador la dirección: `192.168.4.1`
5. ¡Disfruta del control!

---

_Desarrollado con UDONE Engine &bull; Motor Control v1_
