# BRECO: Brawijaya Smart Energy Conservation System

**BRECO** is an innovative IoT-based Smart Energy and Environment Monitoring System developed specifically for the Faculty of Computer Science (FILKOM), Universitas Brawijaya. This repository is officially submitted for the **TEKRA 2026 Competition** (Technology & Creative Arena) under the IoT & Embedded System Concept Challenge.

## 👥 Team Information: Duo Trio HaFaFa
- **Faculty:** Faculty of Computer Science (FILKOM), Universitas Brawijaya

- **Team Members:**
1. Muhammad Haidar Abiyyah Yahya - 255150301111017 - Leader
2. Muhammad Faiz - 255150301111018 - Member 1
3. Fathir Rachman Wicaksono - 255150300111029 - Member 2

---

## 🚀 Concept & Problem Analysis
Based on the official problem identification, FILKOM Universitas Brawijaya manages at least 65 classrooms equipped with high-power Central AC units (~3,000 Watts each) and lighting (~400 Watts). Due to manual checking constraints, these electronics are often left running overnight in empty rooms. 

Mathematically, this operational oversight leaks approximately **Rp 323,119,680.- per semester** in unnecessary electrical expenses (calculated under the official PLN P-2/TM government tariff of Rp 1,523/kWh). BRECO is engineered to completely eradicate this financial and energy waste.

## 🧠 System Architecture & Solution Flow
BRECO utilizes an integrated hardware-software ecosystem with a lightweight network framework:

1. **Data Acquisition:** A single PIR sensor actively detects human occupancy in the classroom, while DHT22 and BH1750 monitor temperature, humidity, and ambient light. 
2. **Edge Processing (ESP32):** The microcontroller processes raw signals. To bypass simulation hardware limits safely on Wokwi, the firmware automatically models a **3,400 Watt active load** whenever motion is detected (`HIGH`), and drops to **0 Watts** when empty (`LOW`).
3. **Data Telemetry:** The compiled telemetry data is packed into compact JSON format and transmitted over the campus network via the **MQTT Protocol** (Publish/Subscribe framework) to minimize bandwidth.
4. **Middleware Gateway:** A backend Node-RED flow intercepts the data, simultaneously archiving it into an **InfluxDB** time-series database for auditing and pushing real-time updates to the **Blynk Cloud API**.
5. **Automated Actuation:** When zero occupancy is confirmed, the server triggers an immediate interlocked automation flow: ESP32 switches the Solid State Relay (SSR) to turn off the lights and commands the IR Transmitter to initiate a soft-shutdown on the Central AC.

## 📁 Repository Structure
- `/src` : Contains `sketch.ino` and firmware configuration files for Wokwi simulation.
- `/data` : Optional simple dummy data notebook or visualization tracking.
- `README.md` : Core concept analysis, solution workflow, and project documentation.

---
*Developed by Duo Trio HaFaFa — TEKRA 2026 Challenge*
