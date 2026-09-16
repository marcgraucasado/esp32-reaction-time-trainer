[🇬🇧 English](README.md) | 🇪🇸 Español

# Entrenador de Tiempo de Reacción — ESP32-S3

> **Autor:** Marc Grau Casado  
> **Asignatura:** Procesadores Digitales — 2025/2026  
> **Grado:** Ingeniería de Sistemas Audiovisuales — Universitat Politècnica de Catalunya (UPC)

## Descripción del proyecto

Este proyecto consiste en un **sistema interactivo de entrenamiento del tiempo de reacción basado en el ESP32-S3**.

El sistema genera objetivos visuales aleatorios y mide el tiempo que tarda el usuario en reaccionar mediante sensores infrarrojos. Los tiempos de reacción se procesan en tiempo real y se utilizan para calcular una puntuación en función del nivel de dificultad seleccionado.

Una **pantalla OLED** proporciona la interfaz de usuario y muestra los resultados de la partida, mientras que el ESP32-S3 crea su propio **punto de acceso Wi-Fi** y aloja un servidor web local desde el que se pueden consultar las mejores puntuaciones.

El proyecto integra **programación de sistemas embebidos, electrónica, integración de sensores, temporización en tiempo real, almacenamiento persistente y comunicación Wi-Fi** en un único sistema hardware/software.

---

## Características principales

- Tres niveles de dificultad: **Fácil, Medio y Difícil**
- Cuatro objetivos independientes LED / sensor IR
- Generación aleatoria de objetivos
- Medición del tiempo de reacción en milisegundos
- Cálculo automático de la puntuación
- Detección de errores y penalización de puntuación
- Interfaz de usuario mediante pantalla OLED
- Ranking persistente de mejores puntuaciones mediante **LittleFS**
- Punto de acceso Wi-Fi local
- Servidor HTTP embebido mediante **ESPAsyncWebServer**
- Sistema de reinicio y selección de dificultad después de cada sesión

---

## Tecnologías

### Hardware

- ESP32-S3
- Cuatro objetivos LED
- Cuatro sensores IR
- Pantalla OLED 128×64
- Buzzer
- Tres pulsadores para seleccionar la dificultad
- Prototipado electrónico y montaje personalizado

### Software

- C++
- Arduino framework
- PlatformIO
- ESPAsyncWebServer
- LittleFS
- Adafruit GFX
- Adafruit SSD1306
- HTML / CSS
- ESP32 en modo Wi-Fi Access Point

---

## Arquitectura del sistema

```text
                         ┌─────────────────────┐
                         │      ESP32-S3       │
                         │                     │
                         │  Lógica de juego    │
                         │  Temporización      │
                         │  Cálculo puntuación │
                         │  Wi-Fi / Servidor   │
                         │  LittleFS           │
                         └───┬─────┬─────┬─────┘
                             │     │     │
                 ┌───────────┘     │     └────────────┐
                 ▼                 ▼                  ▼
        ┌────────────────┐  ┌─────────────┐   ┌──────────────┐
        │ Objetivos LED  │  │ OLED 128×64 │   │ Cliente Wi-Fi│
        │ + Sensores IR  │  │             │   │ Navegador Web│
        └────────────────┘  └─────────────┘   └──────────────┘
                 ▲
                 │
        ┌────────────────┐
        │ Respuesta del  │
        │    usuario     │
        └────────────────┘

             Pulsadores ──────► Selección de dificultad
             Buzzer      ──────► Realimentación sonora
```

El **ESP32-S3** actúa como controlador central del sistema. Gestiona el estado del juego, la selección de objetivos, las entradas de los sensores, la medición del tiempo de reacción, el cálculo de puntuación, la pantalla OLED, el ranking persistente y el servidor web local.

---

## Funcionamiento

### 1. Selección de dificultad

El usuario selecciona uno de los tres niveles de dificultad mediante pulsadores dedicados:

- **Fácil**
- **Medio**
- **Difícil**

Cada nivel utiliza diferentes umbrales de tiempo de reacción para calcular la puntuación.

### 2. Cuenta atrás

Antes de comenzar la sesión se muestra una cuenta atrás en la pantalla OLED, acompañada de una señal sonora mediante el buzzer.

### 3. Generación aleatoria de objetivos

El ESP32-S3 selecciona aleatoriamente uno de los cuatro objetivos disponibles y activa su LED correspondiente.

### 4. Detección de la reacción

El sistema monitoriza continuamente los cuatro sensores IR.

Cuando el usuario activa el sensor correspondiente al LED iluminado, el ESP32-S3 calcula:

```text
Tiempo de reacción = Instante de activación del sensor - Instante de activación del LED
```

El tiempo de reacción medido se almacena durante la sesión actual.

Si el usuario activa un sensor incorrecto, se registra como un error.

### 5. Cálculo de la puntuación

Al finalizar la sesión, el sistema calcula el tiempo medio de reacción.

La puntuación base se determina en función de la dificultad seleccionada y del tiempo medio de reacción obtenido.

Por cada tres respuestas incorrectas se resta un punto de la puntuación final.

### 6. Resultados

La puntuación final se muestra en la pantalla OLED.

A continuación, el usuario puede seleccionar otra dificultad e iniciar una nueva sesión.

### 7. Ranking persistente

Las tres mejores puntuaciones de cada nivel de dificultad se almacenan mediante **LittleFS**, permitiendo conservar el ranking incluso después de reiniciar el dispositivo.

---

## Umbrales de dificultad

Los umbrales de puntuación implementados en el firmware son:

| Dificultad | Puntuación 10 | Puntuación 8 | Puntuación 6 | Resto |
|---|---:|---:|---:|---:|
| **Fácil** | < 500 ms | < 600 ms | < 700 ms | 4 |
| **Medio** | < 300 ms | < 400 ms | < 500 ms | 4 |
| **Difícil** | < 200 ms | < 300 ms | < 400 ms | 4 |

La puntuación final puede reducirse en función del número de respuestas incorrectas.

---

## Interfaz web local

El ESP32-S3 funciona en **modo Wi-Fi Access Point**, por lo que el sistema no necesita un router externo ni conexión a Internet.

Un dispositivo cercano puede conectarse directamente a la red creada por el ESP32-S3 y acceder al servidor web embebido.

La interfaz web muestra las tres mejores puntuaciones de:

- Modo Fácil
- Modo Medio
- Modo Difícil

El servidor HTTP está implementado utilizando **ESPAsyncWebServer**, mientras que los datos del ranking se almacenan de forma persistente mediante **LittleFS**.

---

## Estructura del repositorio

```text
├── Codi/
│   ├── include/
│   ├── lib/
│   ├── src/
│   │   └── main.cpp
│   ├── test/
│   └── platformio.ini
├── Memòria.pdf
├── README.md
└── README_ES.md
```

El firmware se encuentra en `Codi/src/main.cpp`, mientras que `platformio.ini` contiene la configuración del entorno ESP32-S3 y las dependencias del proyecto.

---

## Instalación

### Clonar el repositorio

```bash
git clone https://github.com/marcgraucasado/esp32-reaction-time-trainer.git
cd esp32-reaction-time-trainer/Codi
```

### Compilar

El proyecto utiliza **PlatformIO**:

```bash
pio run
```

### Subir al ESP32-S3

```bash
pio run -t upload --upload-port /dev/ttyUSB0
```

Sustituye `/dev/ttyUSB0` por el puerto serie asignado al ESP32-S3 en tu sistema.

---

## Dependencias principales

El proyecto utiliza las siguientes librerías:

```text
Adafruit GFX Library
Adafruit SSD1306
Adafruit BusIO
ESPAsyncWebServer
```

Estas dependencias están configuradas mediante `platformio.ini` y son gestionadas automáticamente por PlatformIO.

---

## Competencias desarrolladas

Este proyecto ha permitido desarrollar experiencia práctica en:

- Desarrollo en C++ para sistemas embebidos
- Programación del ESP32-S3
- Integración hardware/software
- Gestión de entradas y salidas digitales
- Integración de sensores IR
- Temporización y detección de eventos en tiempo real
- Control de pantallas OLED
- Redes Wi-Fi en sistemas embebidos
- Desarrollo de servidores HTTP
- Almacenamiento persistente mediante LittleFS
- Prototipado electrónico
- Pruebas y depuración de sistemas

---

## Contexto académico

Este proyecto se desarrolló para la asignatura **Procesadores Digitales** del grado en **Ingeniería de Sistemas Audiovisuales** de la **Universitat Politècnica de Catalunya (UPC)**.

El objetivo era diseñar e implementar un sistema embebido completo que combinara interacción física con el usuario, procesamiento en tiempo real, almacenamiento local de datos y comunicación de red.

---

## Autor

**Marc Grau Casado**  
Ingeniería de Sistemas Audiovisuales  
Universitat Politècnica de Catalunya — UPC
