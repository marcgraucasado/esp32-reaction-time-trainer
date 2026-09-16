🇬🇧 English | [🇪🇸 Español](README_ES.md)

# ESP32-S3 Reaction Time Trainer

> **Author:** Marc Grau Casado  
> **Course:** Digital Processors — 2025/2026  
> **Degree:** Audiovisual Systems Engineering — Universitat Politècnica de Catalunya (UPC)

## Project Overview

This project is an interactive **reaction time training system built around the ESP32-S3**.

The system generates random visual targets and measures how quickly the user reacts through infrared sensors. Reaction times are processed in real time and used to calculate a score according to the selected difficulty level.

An **OLED display** provides the user interface and game results, while the ESP32-S3 creates its own **Wi-Fi Access Point** and hosts a local web server where the best scores can be viewed.

The project integrates **embedded programming, electronics, sensor interfacing, real-time timing, persistent storage and Wi-Fi communication** in a single hardware/software system.

---

## Key Features

- Three difficulty levels: **Easy, Medium and Hard**
- Four independent LED / IR sensor targets
- Random target generation
- Reaction-time measurement in milliseconds
- Automatic score calculation
- Error detection and score penalties
- OLED-based user interface
- Persistent top-score ranking using **LittleFS**
- Local Wi-Fi Access Point
- Embedded HTTP server using **ESPAsyncWebServer**
- Restart and difficulty-selection logic after each session

---

## Technologies

### Hardware

- ESP32-S3
- Four LED targets
- Four IR sensors
- 128×64 OLED display
- Buzzer
- Three difficulty-selection push buttons
- Electronic prototyping and custom assembly

### Software

- C++
- Arduino framework
- PlatformIO
- ESPAsyncWebServer
- LittleFS
- Adafruit GFX
- Adafruit SSD1306
- HTML / CSS
- ESP32 Wi-Fi Access Point mode

---

## System Architecture

```text
                         ┌─────────────────────┐
                         │      ESP32-S3       │
                         │                     │
                         │  Game logic         │
                         │  Timing             │
                         │  Score calculation  │
                         │  Wi-Fi / Web server │
                         │  LittleFS storage   │
                         └───┬─────┬─────┬─────┘
                             │     │     │
                 ┌───────────┘     │     └────────────┐
                 ▼                 ▼                  ▼
        ┌────────────────┐  ┌─────────────┐   ┌──────────────┐
        │ LED Targets +  │  │ OLED 128×64 │   │ Wi-Fi Client│
        │   IR Sensors   │  │   Display   │   │ Web Browser │
        └────────────────┘  └─────────────┘   └──────────────┘
                 ▲
                 │
        ┌────────────────┐
        │ User Response  │
        └────────────────┘

              Push Buttons ──────► Difficulty Selection
              Buzzer       ──────► Audio Feedback
```

The **ESP32-S3** is the central controller. It manages the game state, target selection, sensor inputs, reaction-time measurement, scoring, OLED interface, persistent ranking and local web server.

---

## How It Works

### 1. Difficulty Selection

The user selects one of three difficulty levels using dedicated push buttons:

- **Easy**
- **Medium**
- **Hard**

Each difficulty level uses different reaction-time thresholds for score calculation.

### 2. Countdown

A countdown is displayed on the OLED before the session starts, accompanied by audio feedback from the buzzer.

### 3. Random Target Generation

The ESP32-S3 randomly selects one of the four available targets and activates its corresponding LED.

### 4. Reaction Detection

The system continuously monitors the four IR sensors.

When the user activates the sensor corresponding to the illuminated target, the ESP32-S3 calculates:

```text
Reaction time = Sensor activation time - LED activation time
```

The measured reaction time is stored for the current session.

Activating an incorrect sensor is registered as an error.

### 5. Score Calculation

At the end of the timed session, the system calculates the average reaction time.

A base score is assigned according to the selected difficulty and the measured average reaction time.

For every three incorrect responses, one point is deducted from the final score.

### 6. Results

The final score is displayed on the OLED.

The user can then select another difficulty level and start a new session.

### 7. Persistent Ranking

The best three scores for each difficulty level are stored in **LittleFS**, allowing the ranking to remain available after a restart.

---

## Difficulty Thresholds

The scoring thresholds implemented in the firmware are:

| Difficulty | Score 10 | Score 8 | Score 6 | Otherwise |
|---|---:|---:|---:|---:|
| **Easy** | < 500 ms | < 600 ms | < 700 ms | 4 |
| **Medium** | < 300 ms | < 400 ms | < 500 ms | 4 |
| **Hard** | < 200 ms | < 300 ms | < 400 ms | 4 |

The final score may be reduced according to the number of incorrect responses.

---

## Local Web Interface

The ESP32-S3 operates in **Wi-Fi Access Point mode**, so the system does not require an external router or Internet connection.

A nearby device can connect directly to the ESP32-S3 network and access the embedded web server.

The web interface displays the top three scores for:

- Easy mode
- Medium mode
- Hard mode

The HTTP server is implemented using **ESPAsyncWebServer**, while ranking data is stored persistently using **LittleFS**.

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

The firmware is located in `Codi/src/main.cpp`, while `platformio.ini` contains the ESP32-S3 environment and project dependencies.

---

## Installation

### Clone the repository

```bash
git clone https://github.com/marcgraucasado/esp32-reaction-time-trainer.git
cd esp32-reaction-time-trainer/Codi
```

### Build

The project uses **PlatformIO**:

```bash
pio run
```

### Upload to the ESP32-S3

```bash
pio run -t upload --upload-port /dev/ttyUSB0
```

Replace `/dev/ttyUSB0` with the serial port assigned to the ESP32-S3 on your system.

---

## Main Dependencies

The project uses the following libraries:

```text
Adafruit GFX Library
Adafruit SSD1306
Adafruit BusIO
ESPAsyncWebServer
```

They are configured through `platformio.ini` and are automatically handled by PlatformIO.

---

## Skills Demonstrated

This project provided practical experience in:

- Embedded C++ development
- ESP32-S3 programming
- Hardware/software integration
- Digital input and output management
- IR sensor integration
- Real-time timing and event detection
- OLED display control
- Embedded Wi-Fi networking
- HTTP server development
- Persistent storage with LittleFS
- Electronic prototyping
- System testing and debugging

---

## Academic Context

This project was developed for the **Digital Processors** course as part of the **Audiovisual Systems Engineering** degree at the **Universitat Politècnica de Catalunya (UPC)**.

The objective was to design and implement a complete embedded system combining physical user interaction, real-time processing, local data storage and network communication.

---

## Author

**Marc Grau Casado**  
Audiovisual Systems Engineering  
Universitat Politècnica de Catalunya — UPC
