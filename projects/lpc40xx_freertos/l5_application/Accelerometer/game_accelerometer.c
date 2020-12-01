#include "game_accelerometer.h"
#include <stdio.h>

#include "i2c.h"

static const i2c_e acceleration__sensor_bus = I2C__2;
static const uint8_t acceleration__address = 0x38;
uint8_t enable_orientation_detection = 192;
uint8_t decrement_debounce_counter = 5;

void enable_orientation(void);

typedef enum {
  acceleration__PL_status_address = 0x10,
  acceleration__PL_CFG_address = 0x11,
  acceleration__PL_count_address = 0x12,
  acceleration__memory_control = 0x2A,
  acceleration__Stand_by = 254,
} acceleration__orientation;

bool acceleration__init(void) {
  const uint8_t active_mode_with_50Hz = (1 << 0) | (1 << 5);

  i2c__write_single(acceleration__sensor_bus, acceleration__address,
                    acceleration__memory_control, acceleration__Stand_by);

  enable_orientation();

  i2c__write_single(acceleration__sensor_bus, acceleration__address,
                    acceleration__memory_control, active_mode_with_50Hz);
  return 0;
}

void enable_orientation(void) {
  i2c__write_single(acceleration__sensor_bus, acceleration__address,
                    acceleration__PL_CFG_address, enable_orientation_detection);

  i2c__write_single(acceleration__sensor_bus, acceleration__address,
                    acceleration__PL_count_address, decrement_debounce_counter);
}

orientation_e acceleration_get_data(void) {
  orientation_e orientation_status;

  uint8_t PL_status_reg =
      i2c__read_single(acceleration__sensor_bus, acceleration__address,
                       acceleration__PL_status_address);

  printf("STATUS REG AFTER READING = 0x%x\n", PL_status_reg);

  if (PL_status_reg & (~(1 << 0)) & (~(1 << 1)) & (~(1 << 2))) {
    orientation_status = 0; // FRONT
  } else if (PL_status_reg == 2) {
    orientation_status = 2; // DOWN
  } else if (PL_status_reg == 4) {
    orientation_status = 4; // RIGHT
  } else if (PL_status_reg == 6) {
    orientation_status = 6; // LEFT
  } else if (PL_status_reg & (1 << 0)) {
    orientation_status = 1; // BACK
  } else {
    orientation_status = 0xA;
  }
  printf("STATUS = %d\n", orientation_status);
  return orientation_status;
}

