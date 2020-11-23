#pragma once
// led_display_driver.h
#include "gpio.h"
#include "lpc40xx.h"
#include "stdbool.h"
#include "stdint.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LEDMATRIX_HEIGHT 32
#define LEDMATRIX_WIDTH 64
#define LEDMATRIX_HALF_HEIGHT 16
#define LEDMATRIX_HALF_WIDTH 32

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

void gpio_init(void);

void update_display(void);
void select_row(uint8_t row);
void disable_display(void);
void enable_display(void);
void enable_latch_data(void);
void disable_latch_data(void);
void data_clock_in(uint8_t row);
void selectMux(uint8_t row);

void set_pixel(int8_t x, int8_t y, color_t color);
void clear_pixel(int8_t x, int8_t y);
void clear_display(void);
void display_rectangle_pink(void);
void display_rectangle_red(void);
void display_moving_image(uint8_t x, uint8_t y);
void move_rectangle(void);
void display_maze(void);
void display_image(void);
