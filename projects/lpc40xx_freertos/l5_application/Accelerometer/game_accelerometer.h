#pragma once

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
  Front_Orientation = 0,
  Back_Orientation = 1,
  Portrait_DOWN = 2,
  Landscape_RIGHT = 4,
  Landscape_LEFT = 6,
} orientation_e;

extern uint8_t col_count;
extern uint8_t row_count;
extern bool right_move;
extern bool left_move;
extern bool up_move;
extern bool down_move;
extern SemaphoreHandle_t movement_counter;

bool acceleration__init(void);
void action_on_orientation(void *p);
