#include "game_accelerometer.h"
#include "led_matrix.h"
#include <stdio.h>

#include "i2c.h"

static const i2c_e acceleration__sensor_bus = I2C__2;
static const uint8_t acceleration__address = 0x38;
uint8_t enable_orientation_detection = 192;
uint8_t decrement_debounce_counter = 5;
bool command_up = true;
bool command_down = true;
bool command_left = true;
bool command_right = true;
uint8_t col_count = 1;
uint8_t row_count = 1;

static void enable_orientation(void);
static orientation_e acceleration_get_data(void);
static void left_movement(void);
static void right_movement(void);
static void up_movement(void);
static void down_movement(void);

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

void action_on_orientation(void *p) {
  orientation_e value;

  while (1) {
    value = acceleration_get_data();

    switch (value) {
    case Landscape_LEFT:
      command_left = true;
      if (left_move) {
        left_movement();
      }
      left_move = true;
      break;
    case Landscape_RIGHT:
      command_right = true;
      if (right_move) {
        right_movement();
      }
      right_move = true;
      break;
    case Back_Orientation:

      break;
    case Front_Orientation:

      break;
    case Portrait_DOWN:
      command_down = true;
      if (down_move) {
        down_movement();
      }
      down_move = true;
      break;
    default:
      command_up = true;
      if (up_move) {
        up_movement();
      }
      up_move = true;
      break;
    }
    vTaskDelay(100);
    tom_move_on_maze(row_count, col_count);
  }
}

static void enable_orientation(void) {
  i2c__write_single(acceleration__sensor_bus, acceleration__address,
                    acceleration__PL_CFG_address, enable_orientation_detection);

  i2c__write_single(acceleration__sensor_bus, acceleration__address,
                    acceleration__PL_count_address, decrement_debounce_counter);
}

static orientation_e acceleration_get_data(void) {
  orientation_e orientation_status;

  uint8_t PL_status_reg =
      i2c__read_single(acceleration__sensor_bus, acceleration__address,
                       acceleration__PL_status_address);

  if (PL_status_reg & (~(1 << 0)) & (~(1 << 1)) & (~(1 << 2))) {
    orientation_status = Front_Orientation;
  } else if (PL_status_reg == 2) {
    orientation_status = Portrait_DOWN;
  } else if (PL_status_reg == 4) {
    orientation_status = Landscape_RIGHT;
  } else if (PL_status_reg == 6) {
    orientation_status = Landscape_LEFT;
  } else if (PL_status_reg & (1 << 0)) {
    orientation_status = Back_Orientation;
  } else {
    orientation_status = 0xA;
  }
  return orientation_status;
}

static void left_movement(void) {
  if (xSemaphoreTake(movement_counter, portMAX_DELAY)) {

    col_count--;
  }

  xSemaphoreGive(movement_counter);
}

static void right_movement(void) {
  if (xSemaphoreTake(movement_counter, portMAX_DELAY)) {

    col_count++;
  }

  xSemaphoreGive(movement_counter);
}
static void down_movement(void) {
  if (xSemaphoreTake(movement_counter, portMAX_DELAY)) {

    row_count++;
  }

  xSemaphoreGive(movement_counter);
}

static void up_movement(void) {
  if (xSemaphoreTake(movement_counter, portMAX_DELAY)) {

    row_count--;
  }

  xSemaphoreGive(movement_counter);
}
/*******************************************************************/
