#pragma once
// led_display_driver.h
#include "FreeRTOS.h"
#include "gpio.h"
#include "lpc40xx.h"
#include "stdbool.h"
#include "stdint.h"
#include "task.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LEDMATRIX_HEIGHT 32
#define LEDMATRIX_WIDTH 64
#define LEDMATRIX_HALF_HEIGHT 16
#define LEDMATRIX_HALF_WIDTH 32

extern bool command_up;
extern bool command_down;
extern bool command_left;
extern bool command_right;

extern uint8_t display_matrix[LEDMATRIX_HALF_HEIGHT][LEDMATRIX_WIDTH];
extern uint8_t col_count;
extern uint8_t row_count;

extern struct object_axis tom;

typedef void (*maze_table_t)(void);

typedef enum {
  BLACK = 0,
  RED = 1,
  GREEN = 2,
  YELLOW = 3,
  BLUE = 4,
  PINK = 5,
  CYAN = 6,
  WHITE = 7,

} color_t;

typedef struct {
  gpio_s R1;
  gpio_s G1;
  gpio_s B1;
  gpio_s R2;
  gpio_s G2;
  gpio_s B2;
  gpio_s A;
  gpio_s B;
  gpio_s C;
  gpio_s D;
  gpio_s STB;
  gpio_s OE;
  gpio_s CLK;

} RGB_gpio;

extern uint8_t jerry_motion_counter;
extern xTaskHandle jerry_motion_suspend;

void gpio_init(void);

void update_display(void);
void select_row(uint8_t row);
void disable_display(void);
void enable_display(void);
void enable_latch_data(void);
void disable_latch_data(void);
void data_clock_in(uint8_t row);

void set_pixel(int8_t x, int8_t y, color_t color);
void clear_pixel(int8_t x, int8_t y);
void clear_display(void);
void display_maze(void);
void display_image(void);
void game_frame(void);
void display_rectangle_width(uint8_t x, uint8_t y, uint8_t width_x,
                             uint8_t width_y, color_t color);
void display_maze_frame1(void);
void maze_one_frame(void);
// void start_screen_display(void);
void jerry_image(void);
void jerry_image_clear(uint8_t x, uint8_t y);
void game_start_display_frame(void);
void tom_image_2_clear(uint8_t x, uint8_t y);
void tom_image_2(uint8_t x, uint8_t y);
void tom_image(uint8_t x, uint8_t y);
void tom_move_on_maze(uint8_t x, uint8_t y);
void maze_one_frame_one(void);

extern bool command_up;
extern bool command_down;
extern bool command_left;
extern bool command_right;