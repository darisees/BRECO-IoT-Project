# Custom Wokwi Chips

This folder contains custom chip files used in the BRECO Wokwi simulation.

## Contents

- `bh1750.chip.c`  
  Custom chip behavior file for the BH1750 light intensity sensor simulation.

- `bh1750.chip.json`  
  Custom chip configuration file for the BH1750 sensor.

- `pzem-004t.chip.c`  
  Custom chip behavior file for the PZEM-004T electrical monitoring simulation.

- `pzem-004t.chip.json`  
  Custom chip configuration file for the PZEM-004T module.

## Purpose

These custom chip files are used because not all required BRECO components are available as native Wokwi components. The custom chips allow the simulation to represent light intensity and electrical monitoring features more closely to the proposed system design.

## Notes

The PZEM-004T module in this simulation is used to represent electrical parameters such as voltage, current, power, and energy usage. The values may be simulated or simplified for demonstration purposes.
