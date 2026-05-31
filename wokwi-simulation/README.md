# Wokwi Simulation

This folder contains the virtual simulation files for the BRECO edge-device prototype using ESP32.

## Contents

- `sketch.ino`  
  Main Arduino C++ source code for the ESP32 firmware simulation.

- `diagram.json`  
  Wokwi circuit configuration file that defines the ESP32 board, sensors, relay, LEDs, and custom chips.

- `libraries.txt`  
  List of required Arduino libraries used by the simulation.

- `chips/`  
  Folder containing custom Wokwi chip definitions for BH1750 and PZEM-004T simulation.

## Main Components

The simulation uses the following components:

- ESP32 DevKit
- PIR motion sensor
- DHT22 temperature and humidity sensor
- BH1750 light intensity sensor
- PZEM-004T electrical monitoring simulation
- Relay module as SSR representation
- LED indicators for lamp and AC status

## Simulation Purpose

The Wokwi simulation represents the edge-computing layer of the BRECO system. It demonstrates how ESP32 collects sensor data, processes room occupancy logic, controls classroom devices, generates JSON telemetry data, and sends monitoring data to the dashboard layer.

## Notes

The actual implementation of Node-RED and InfluxDB is not included in this folder. Those technologies are part of the proposed backend architecture and can be added in future development.
