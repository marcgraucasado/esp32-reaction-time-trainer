[🇬🇧 English](README.md) | [🇪🇸 Español](README_ES.md)

# Reaction Time Training System — ESP32-S3

> **Author:** Marc Grau Casado

> **Course:** Digital Processors — 2025/2026

## Project Overview

This project is an interactive **reaction time training system based on the ESP32-S3**, designed to measure and improve user response time through visual, auditory and physical stimuli.

The system combines an **8×8 LED matrix**, buzzers, push buttons and IR sensors to generate different types of stimuli. User responses are processed in real time by the ESP32-S3, while an **OLED display** provides menus, game information and results.

The device also creates its own **Wi-Fi Access Point** and hosts a local web interface where users can check statistics and a persistent ranking of the best scores.

The project combines **embedded systems, electronics, real-time processing, Wi-Fi communication and web development** in a single hardware/software platform.

---

## Key Features

* Three difficulty levels: **Easy, Medium and Hard**.
* Random visual and auditory stimuli.
* Real-time reaction time measurement.
* Automatic score calculation.
* OLED-based menu and results interface.
* Persistent ranking using **LittleFS**.
* Local web interface for statistics and rankings.
* Wi-Fi communication using the ESP32-S3 in **Access Point mode**.
* Support for multiple players.
* Automatic game restart and shutdown logic.
* Penalty system based on incorrect responses.

---

## Technologies

### Hardware

* ESP32-S3
* 8×8 LED matrix
* OLED 128×64 display
* IR sensors
* Push buttons
* Buzzers
* Custom electronic assembly

### Software

* C++
* PlatformIO
* ESPAsyncWebServer
* LittleFS
* HTML / CSS
* Wi-Fi Access Point
* Embedded real-time processing

---

## System Architecture

```text
┌──────────┐       ┌─────────────────┐       ┌──────────────┐
│ 8×8 LED  │       │    ESP32-S3     │       │ OLED Display │
│  Matrix  │◀─────▶│   CPU + Wi-Fi   │──────▶│    128×64    │
└──────────┘       └─────────────────┘       └──────────────┘
      ▲                     ▲
      │                     │
┌──────────┐          ┌──────────────┐
│ Buzzers  │          │ Push Buttons │
└──────────┘          │ / IR Sensors │
                      └──────────────┘
                              │
                              ▼
                     ┌────────────────┐
                     │ Local Web      │
                     │ Interface      │
                     └────────────────┘
```

The **ESP32-S3** acts as the central controller of the system. It manages stimulus generation, user input, reaction time measurement, score calculation, data storage and the local web server.

---

## How It Works

### 1. System Start

The ESP32-S3 initializes the display, sensors, LED matrix, storage system and Wi-Fi interface.

### 2. Game Configuration

The user selects the desired game mode and difficulty level through the OLED interface using the physical controls.

### 3. Countdown

Before the game starts, a **5-second countdown** is displayed.

### 4. Stimulus Generation

The system generates random visual and auditory stimuli using the LED matrix and buzzers.

### 5. Reaction Measurement

The ESP32-S3 measures the time between the stimulus and the user's response through the push buttons or IR sensors.

Incorrect responses are also detected.

For every three mistakes, one point is deducted from the maximum final score.

### 6. Results

At the end of the game, the final score is displayed for approximately **15 seconds**.

The user can then choose to start another game and select a new game mode.

If no new game is selected within the defined period, the system automatically shuts down the game session.

### 7. Ranking

The three best scores and their associated reaction times are stored and displayed through the local web interface.

---

## Main Components

| Component          | Function                                                                |
| ------------------ | ----------------------------------------------------------------------- |
| **ESP32-S3**       | Main processor, Wi-Fi Access Point, game logic, timing and data storage |
| **8×8 LED Matrix** | Visual stimulus generation                                              |
| **Buzzers**        | Auditory stimulus generation                                            |
| **Push Buttons**   | User interaction and menu control                                       |
| **IR Sensors**     | Contactless user response detection                                     |
| **OLED 128×64**    | Menus, game information and results                                     |
| **LittleFS**       | Persistent ranking storage                                              |
| **Web Server**     | Remote access to statistics and rankings                                |

---

## Web Interface

The ESP32-S3 operates as a **Wi-Fi Access Point**, allowing nearby devices to connect directly to the system without requiring an external router or Internet connection.

Once connected, users can access a local web page displaying game statistics and the best recorded scores.

This functionality is implemented using **ESPAsyncWebServer**.

---

## Hardware Cost

| Component             | Quantity | Approx. Price |
| --------------------- | :------: | ------------: |
| ESP32-S3              |     1    |            €6 |
| OLED 128×64 Display   |     1    |            €7 |
| Buzzers               |     4    |            €6 |
| Push Buttons          |    10    |            €9 |
| IR Sensors            |     6    |            €6 |
| Double-sided PCB      |     1    |           €15 |
| Breadboards           |     2    |            €6 |
| Wooden Base           |     1    |           €25 |
| Solder                |     1    |            €8 |
| Soldering Iron        |     1    |            €0 |
| **Approximate Total** |          |       **€88** |

---

## Repository Structure

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

---

## Installation

### Clone the repository

```bash
git clone https://github.com/marcgraucasado/PROYECTO.git
cd PROYECTO
```

### Build the project

The project uses **PlatformIO**.

```bash
pio run
```

### Upload to the ESP32-S3

```bash
pio run -t upload --upload-port /dev/ttyUSB0
```

Replace `/dev/ttyUSB0` with the serial port assigned to your ESP32-S3.

---

## Skills Developed

This project provided practical experience in:

* Embedded systems development
* ESP32-S3 programming
* C++ programming
* Hardware and software integration
* Digital input/output management
* Real-time reaction measurement
* Sensor integration
* Wi-Fi networking
* Local web server development
* Persistent data storage
* Electronic assembly and prototyping
* System testing and debugging

---

## Academic Context

This project was developed as part of the **Digital Processors** course during the **2025/2026 academic year**.

It was designed as a complete embedded system combining hardware, firmware, user interaction and network communication.

---

## Author

**Marc Grau Casado**

Audiovisual Systems Engineering
Universitat Politècnica de Catalunya — UPC
