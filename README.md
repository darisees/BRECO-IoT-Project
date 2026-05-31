# BRECO: Brawijaya Smart Energy Conservation System

**BRECO** stands for **Brawijaya Smart Energy Conservation System**, an IoT-based smart energy monitoring and automation concept designed to reduce electricity waste in university classrooms. This project focuses on monitoring room occupancy, environmental conditions, and electrical load behavior while automatically controlling classroom devices such as lamps and air conditioning units.

This repository is submitted for the **TEKRA 2026 Competition** under the **IoT & Embedded System Concept Challenge** category.

---

## Team Information: Duo Trio HaFaFa

**Institution:** Faculty of Computer Science (FILKOM), Universitas Brawijaya

**Team Members:**

1. Muhammad Haidar Abiyyah Yahya - 255150301111017 - Leader
2. Muhammad Faiz - 255150301111018 - Member 1
3. Fathir Rachman Wicaksono - 255150300111029 - Member 2

---

## Project Background

Classrooms in university environments often use high-power electrical devices, especially central air conditioning systems and lighting installations. In the case study used for this project, FILKOM Universitas Brawijaya is estimated to have around 65 classrooms, each with an average electrical load of approximately:

* Central AC: 3,000 Watts
* Lighting system: 400 Watts
* Total estimated active load per classroom: 3,400 Watts

A major problem occurs when lamps and AC units remain active after the classroom is no longer occupied. Since manual monitoring requires staff to check rooms one by one, energy waste may occur repeatedly, especially after academic activities end.

Based on the project worksheet calculation, the estimated electricity waste can reach approximately **Rp 323,119,680 per semester** for the FILKOM pilot case, using the official PLN P-2/TM electricity tariff assumption of **Rp 1,523/kWh**.

BRECO is proposed as a smart energy conservation system that can monitor classroom conditions in real time and automatically reduce unnecessary electricity usage.

---

## System Overview

BRECO combines embedded system technology, IoT communication, and cloud-based dashboard monitoring. The system is designed to work through the following flow:

1. **Data Acquisition**
   Sensors collect data from the classroom environment:

   * PIR sensor for room occupancy detection
   * DHT22 for temperature and humidity monitoring
   * BH1750 for ambient light intensity monitoring
   * PZEM-004T simulation for electrical parameter monitoring

2. **Edge Processing using ESP32**
   The ESP32 processes sensor readings and determines the room condition. When occupancy is detected, the system enters an active classroom state. When the room becomes empty after a defined safety delay, the system turns off the controlled devices.

3. **Actuator Control**
   The system controls classroom devices through:

   * Relay / SSR simulation for classroom lighting
   * AC soft-shutdown simulation through a dedicated output indicator, representing an IR transmitter in the real implementation

4. **Telemetry Data Formatting**
   Sensor and system status data are structured into a JSON-based telemetry payload.

5. **IoT Communication Concept**
   The telemetry payload is designed to be transmitted using the MQTT publish/subscribe communication model.

6. **Dashboard and Backend Concept**
   In the proposed architecture, Node-RED acts as the middleware gateway, InfluxDB stores historical time-series data, and Blynk IoT provides real-time dashboard visualization.

---

## Current Implementation Status

This repository currently focuses on the **Wokwi-based virtual simulation of the BRECO edge device**.

Implemented in this simulation:

* ESP32 firmware using Arduino C++
* PIR-based occupancy detection
* DHT22 temperature and humidity monitoring
* BH1750 light intensity monitoring
* PZEM-004T electrical monitoring simulation
* Relay / SSR-based lamp control simulation
* AC control indicator simulation
* Room status logic
* JSON telemetry payload structure
* MQTT communication structure
* Blynk IoT integration structure
* Serial Monitor output for system behavior tracking

Backend components such as **Node-RED** and **InfluxDB** are part of the proposed full system architecture and are represented conceptually in this simulation. They can be integrated further in future development.

---

## Main Features

* Real-time classroom occupancy detection
* Automatic lamp control based on room usage
* AC shutdown simulation based on room vacancy
* Environmental monitoring using temperature, humidity, and light sensors
* Electrical load simulation based on classroom device status
* Energy waste detection concept
* Anomaly alert concept when power usage remains high after shutdown
* JSON telemetry output for IoT communication
* MQTT-based data transmission concept
* Blynk dashboard integration concept

---

## Technology Stack

| Layer                         | Technology             |
| ----------------------------- | ---------------------- |
| Microcontroller               | ESP32                  |
| Firmware Programming          | Arduino C++            |
| Simulation Platform           | Wokwi                  |
| Occupancy Sensor              | PIR Motion Sensor      |
| Temperature & Humidity Sensor | DHT22                  |
| Light Sensor                  | BH1750                 |
| Electrical Monitoring         | PZEM-004T Simulation   |
| Actuator                      | Relay / SSR Simulation |
| Data Format                   | JSON                   |
| Communication Protocol        | MQTT                   |
| Dashboard                     | Blynk IoT              |
| Middleware Concept            | Node-RED               |
| Historical Database Concept   | InfluxDB               |

---

## Repository Structure

```text
BRECO-IoT-Project/
│
├── README.md
├── .gitignore
│
├── docs/
│   ├── README.md
│   └── TEKRA2026_Worksheet_BRECO.pdf
│
├── wokwi-simulation/
│   ├── README.md
│   ├── sketch.ino
│   ├── diagram.json
│   ├── libraries.txt
│   │
│   └── chips/
│       ├── README.md
│       ├── bh1750.chip.c
│       ├── bh1750.chip.json
│       ├── pzem-004t.chip.c
│       └── pzem-004t.chip.json
│
├── legacy-original-files/
│   ├── README.md
│   ├── sketch.ino
│   ├── diagram.json
│   ├── libraries.txt
│   ├── bh1750.chip.c
│   ├── bh1750.chip.json
│   ├── pzem-004t.chip.c
│   └── pzem-004t.chip.json
│
└── screenshots/
    ├── README.md
    ├── wokwi-preview.png
    ├── serial-monitor.png
    └── dashboard-preview.png
```

---

## Folder Description

### `docs/`

Contains project documentation, including the TEKRA 2026 worksheet and supporting documents related to the system design.

### `wokwi-simulation/`

Contains the main Wokwi simulation files for the ESP32-based BRECO prototype, including the Arduino source code, circuit diagram, and required libraries.

### `wokwi-simulation/chips/`

Contains custom Wokwi chip files used to simulate components that are not fully available as native Wokwi components, such as BH1750 and PZEM-004T.

### `screenshots/`

Contains visual documentation of the simulation, such as the Wokwi circuit preview, Serial Monitor output, and dashboard preview.

### `legacy-original-files/`

Contains the original Wokwi simulation files that were previously uploaded before the repository was reorganized. This folder is kept as a historical archive, while the maintained simulation version is located in `wokwi-simulation/`.

---

## How to Run the Simulation

1. Open [Wokwi](https://wokwi.com/).
2. Create or open an ESP32 project.
3. Copy the files from the `wokwi-simulation/` folder into the Wokwi project.
4. Make sure the custom chip files from `wokwi-simulation/chips/` are included.
5. Check the required libraries listed in `libraries.txt`.
6. Run the simulation.
7. Observe the system output through:

   * Wokwi circuit preview
   * Serial Monitor
   * Blynk dashboard, if configured

---

## Simulation Notes

For demonstration purposes, the simulation may use shortened timing values compared to the real implementation scenario. In the actual concept, the system is designed to follow classroom session duration and safety delay logic before turning off electrical devices.

The electrical load is simulated based on the active state of classroom devices:

* Lamp ON + AC ON: approximately 3,400 Watts
* Lamp OFF + AC OFF: approximately 0 Watts
* Abnormal remaining load after shutdown: anomaly condition

This approach allows the Wokwi simulation to represent the energy-saving logic of BRECO without requiring real high-power electrical hardware.

---

## Future Development

Future improvements may include:

* Full Node-RED flow implementation
* InfluxDB historical data logging
* Complete Blynk dashboard deployment
* Real MQTT broker integration
* Real IR transmitter implementation for AC control
* Multi-room monitoring support
* Energy usage analytics and reporting dashboard

---

## Project Context

This project was developed as part of the **TEKRA 2026 IoT & Embedded System Concept Challenge**.

---

## Credits

Developed by **Duo Trio HaFaFa** 

Faculty of Computer Science, Universitas Brawijaya
