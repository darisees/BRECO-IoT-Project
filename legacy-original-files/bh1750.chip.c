// Wokwi Custom Chip - For information and examples see:
// https://link.wokwi.com/custom-chips-alpha
//
// SPDX-License-Identifier: MIT
// Copyright (C) 2023 Gatul / wokwi.com
//
// Luxometer BH1750 beta version

#include "wokwi-api.h"
#include <stdio.h>
#include <stdlib.h>

const int ADDRESS = 0x23;

typedef enum
{
  ST_EXPECTING_READ_BYTE_1,
  ST_EXPECTING_READ_BYTE_2,
} States;

typedef struct {
  pin_t pin_int;
  uint32_t lightlevel_attr;
  States state;
} chip_state_t;


static bool on_i2c_connect(void *user_data, uint32_t address, bool connect);
static uint8_t on_i2c_read(void *user_data);
static bool on_i2c_write(void *user_data, uint8_t data);
static void on_i2c_disconnect(void *user_data);

void chip_init() {
  chip_state_t *chip = malloc(sizeof(chip_state_t));

  chip->pin_int = pin_init("ADDR", INPUT_PULLUP);
  
  const i2c_config_t i2c_config = {
    .user_data = chip,
    .address = ADDRESS,
    .scl = pin_init("SCL", INPUT),
    .sda = pin_init("SDA", INPUT),
    .connect = on_i2c_connect,
    .read = on_i2c_read,
    .write = on_i2c_write,
    .disconnect = on_i2c_disconnect, // Optional
  };
  i2c_init(&i2c_config);

  // This attribute can be edited by the user. It's defined in wokwi-custom-part.json:
  chip->lightlevel_attr = attr_init("LightLevel", 10000);
}
/*
static void counter_updated(chip_state_t *chip) {
  const uint32_t threshold = attr_read(chip->threshold_attr);
  if (chip->counter > threshold) {
    pin_write(chip->pin_int, LOW);
    pin_mode(chip->pin_int, OUTPUT);
  } else {
    pin_mode(chip->pin_int, INPUT);
  }
}
*/
bool on_i2c_connect(void *user_data, uint32_t address, bool connect) {
  chip_state_t *chip = user_data;
//  if (read)
//  {
    chip->state = ST_EXPECTING_READ_BYTE_1;
//  }
  return true; /* Ack */
}

uint8_t on_i2c_read(void *user_data) {
  chip_state_t *chip = user_data;
  const uint32_t lightlevel = attr_read(chip->lightlevel_attr) * 1.2;
  uint8_t level;
//  chip->counter++;
//  counter_updated(chip);
  switch (chip->state)
  {
    case ST_EXPECTING_READ_BYTE_1:
      level = lightlevel / 256;
      chip->state = ST_EXPECTING_READ_BYTE_2;
      break;
    case ST_EXPECTING_READ_BYTE_2:
      level = lightlevel % 256;
      chip->state = ST_EXPECTING_READ_BYTE_1;
      break;
  }
  return level; //threshold; //chip->threshold_attr;
}

bool on_i2c_write(void *user_data, uint8_t data) {
  chip_state_t *chip = user_data;
//  chip->counter = data;
//  counter_updated(chip);
  return true; // Ack
}

void on_i2c_disconnect(void *user_data) {
  // Do nothing
}
