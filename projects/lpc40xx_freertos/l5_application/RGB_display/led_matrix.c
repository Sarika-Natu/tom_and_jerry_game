#include "led_matrix.h"
#include "delay.h"
#include "ff.h"
#include "gpio.h"

bool jerry_check = false;
uint8_t display_matrix[LEDMATRIX_HALF_HEIGHT][LEDMATRIX_WIDTH];
RGB_gpio RGB = {{2, 0}, {2, 1}, {2, 2},  {2, 5},  {2, 4},  {0, 15}, {2, 7},
                {2, 8}, {2, 9}, {0, 16}, {1, 23}, {1, 20}, {1, 28}};

void disable_display(void) { gpio__set(RGB.OE); }

void enable_display(void) { gpio__reset(RGB.OE); }

void enable_latch_data(void) { gpio__set(RGB.STB); }

void disable_latch_data(void) { gpio__reset(RGB.STB); }

uint8_t maze_one_lookup_table[LEDMATRIX_HEIGHT][LEDMATRIX_WIDTH] = {
    {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5},
    {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5},
    {5, 5, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5},
    {5, 5, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5},
    {5, 5, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5},
    {5, 5, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0,
     0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 5, 5},
    {5, 5, 0, 4, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0,
     0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 5, 5},
    {5, 5, 0, 4, 0, 4, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 0, 3, 0, 0,
     0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 5, 5},
    {5, 5, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 3, 0, 3, 3, 3, 3, 3, 3, 3,
     3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 2, 0, 0, 5, 5},
    {5, 5, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 5, 5},
    {5, 5, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 5, 5},
    {5, 5, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 5, 5},
    {5, 5, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5},
    {5, 5, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5},
    {5, 5, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5},
    {5, 5, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 5, 5},
    {5, 5, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 5, 5},
    {5, 5, 4, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2,
     2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 5, 5},
    {5, 5, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
     3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0,
     0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 5, 5},
    {5, 5, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0,
     0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 5, 5},
    {5, 5, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0,
     0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 5, 5},
    {5, 5, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0,
     0, 0, 2, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 5, 5},
    {5, 5, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0,
     0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 5, 5},
    {5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0,
     0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 5, 5},
    {5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4,
     4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0,
     0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 5, 5},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0,
     0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 3, 0, 0, 0, 5, 5},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 5, 5},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 5, 5},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 5, 5},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 5, 5},
    {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5},
    {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5},
};

void clear_display(void) {
  gpio__reset(RGB.R1);
  gpio__reset(RGB.G1);
  gpio__reset(RGB.B1);
  gpio__reset(RGB.B2);
  gpio__reset(RGB.R2);
  gpio__reset(RGB.G2);
}
void gpio_init(void) {

  gpio__set_function(RGB.B2, GPIO__FUNCITON_0_IO_PIN);
  gpio__set_function(RGB.B1, GPIO__FUNCITON_0_IO_PIN);
  gpio__set_function(RGB.G2, GPIO__FUNCITON_0_IO_PIN);
  gpio__set_function(RGB.G1, GPIO__FUNCITON_0_IO_PIN);
  gpio__set_function(RGB.R2, GPIO__FUNCITON_0_IO_PIN);
  gpio__set_function(RGB.R1, GPIO__FUNCITON_0_IO_PIN);
  gpio__set_function(RGB.STB, GPIO__FUNCITON_0_IO_PIN);
  gpio__set_function(RGB.CLK, GPIO__FUNCITON_0_IO_PIN);
  gpio__set_function(RGB.OE, GPIO__FUNCITON_0_IO_PIN);
  gpio__set_function(RGB.A, GPIO__FUNCITON_0_IO_PIN);
  gpio__set_function(RGB.B, GPIO__FUNCITON_0_IO_PIN);
  gpio__set_function(RGB.C, GPIO__FUNCITON_0_IO_PIN);
  gpio__set_function(RGB.D, GPIO__FUNCITON_0_IO_PIN);

  gpio__set_as_output(RGB.B2);
  gpio__set_as_output(RGB.B1);
  gpio__set_as_output(RGB.G2);
  gpio__set_as_output(RGB.G1);
  gpio__set_as_output(RGB.R2);
  gpio__set_as_output(RGB.R1);
  gpio__set_as_output(RGB.STB);
  gpio__set_as_output(RGB.CLK);
  gpio__set_as_output(RGB.OE);
  gpio__set_as_output(RGB.A);
  gpio__set_as_output(RGB.B);
  gpio__set_as_output(RGB.C);
  gpio__set_as_output(RGB.D);

  clear_display();
}

uint8_t RGB_A, RGB_B, RGB_C, RGB_D;

void select_row(uint8_t row) {
  RGB_A = (row >> 0) & 1; // 1
  RGB_B = (row >> 1) & 1;
  RGB_C = (row >> 2) & 1;
  RGB_D = (row >> 3) & 1;

  RGB_A ? (LPC_GPIO2->SET = (1 << RGB.A.pin_number))
        : (LPC_GPIO2->CLR = (1 << RGB.A.pin_number));
  RGB_B ? (LPC_GPIO2->SET = (1 << RGB.B.pin_number))
        : (LPC_GPIO2->CLR = (1 << RGB.B.pin_number));
  RGB_C ? (LPC_GPIO2->SET = (1 << RGB.C.pin_number))
        : (LPC_GPIO2->CLR = (1 << RGB.C.pin_number));
  RGB_D ? (LPC_GPIO0->SET = (1 << RGB.D.pin_number))
        : (LPC_GPIO0->CLR = (1 << RGB.D.pin_number));
}

void data_clock_in(uint8_t row) {

  for (uint8_t col = 0; col < LEDMATRIX_WIDTH; col++) {

    ((display_matrix[row][col] >> 0) & 0x1) ? gpio__set(RGB.R1)
                                            : gpio__reset(RGB.R1);
    ((display_matrix[row][col] >> 1) & 0x1) ? gpio__set(RGB.G1)
                                            : gpio__reset(RGB.G1);
    ((display_matrix[row][col] >> 2) & 0x1) ? gpio__set(RGB.B1)
                                            : gpio__reset(RGB.B1);
    ((display_matrix[row][col] >> 4) & 0x1) ? gpio__set(RGB.R2)
                                            : gpio__reset(RGB.R2);
    ((display_matrix[row][col] >> 5) & 0x1) ? gpio__set(RGB.G2)
                                            : gpio__reset(RGB.G2);
    ((display_matrix[row][col] >> 6) & 0x1) ? gpio__set(RGB.B2)
                                            : gpio__reset(RGB.B2);

    gpio__set(RGB.CLK);
    gpio__reset(RGB.CLK);
    enable_latch_data();
    disable_latch_data();
  }
}

void set_pixel(int8_t row, int8_t col, color_t color) {

  if ((row < 0) || (row >= LEDMATRIX_HEIGHT))
    return;
  if ((col < 0) || (col >= LEDMATRIX_WIDTH))
    return;

  if (row >= LEDMATRIX_HALF_HEIGHT) {

    display_matrix[row % LEDMATRIX_HALF_HEIGHT][col] |= (color << 4);

  } else {

    display_matrix[row][col] |= (color);
  }
}

void clear_pixel(int8_t row, int8_t col) {

  if ((row < 0) || (row >= LEDMATRIX_HEIGHT))
    return;
  if ((col < 0) || (col >= LEDMATRIX_WIDTH))
    return;

  if (row >= LEDMATRIX_HALF_HEIGHT) {
    display_matrix[row % LEDMATRIX_HALF_HEIGHT][col] &= ~(0xF << 4);
  } else {
    display_matrix[row][col] &= ~(0xF);
  }
}

void update_display(void) {
  for (uint8_t row = 0; row < LEDMATRIX_HALF_HEIGHT; row++) {

    disable_display();
    disable_latch_data();
    select_row(row);
    data_clock_in(row);
    enable_latch_data();
    enable_display();
    delay__us(150);
    disable_display();
  }
}

void display_rectangle_width(uint8_t x, uint8_t y, uint8_t width_x,
                             uint8_t width_y, color_t color) {

  for (uint8_t i = x; i <= width_x; i++) {
    for (uint8_t j = y; j <= width_y; j++) {
      set_pixel(i, j, color);
      update_display();
    }
  }
}

void display_maze_frame1(void) {
  display_rectangle_width(0, 0, 1, 50, PINK);
  display_rectangle_width(2, 0, 24, 1, PINK);
  display_rectangle_width(30, 0, 31, 63, PINK);
  display_rectangle_width(2, 62, 31, 63, PINK);
  display_rectangle_width(0, 55, 1, 63, PINK);
  display_rectangle_width(2, 2, 24, 14, CYAN);
  display_rectangle_width(10, 20, 20, 50, GREEN);
  display_rectangle_width(27, 20, 29, 50, BLUE);
  display_rectangle_width(10, 55, 29, 61, YELLOW);
  display_rectangle_width(2, 15, 5, 50, CYAN);
  display_rectangle_width(2, 55, 5, 61, RED);
}

uint8_t row_counter = 0;

void maze_one_frame(void) {
  //  for (uint8_t row = 0; row < LEDMATRIX_HALF_HEIGHT; row++) {

  if (row_counter < LEDMATRIX_HEIGHT) {
    for (uint8_t col = 0; col < LEDMATRIX_WIDTH; col++) {
      if ((maze_one_lookup_table[row_counter][col]) == 4) {
        set_pixel(row_counter, col, BLUE);

      } else if ((maze_one_lookup_table[row_counter][col]) == 2) {
        set_pixel(row_counter, col, GREEN);

      } else if ((maze_one_lookup_table[row_counter][col]) == 5) {
        set_pixel(row_counter, col, PINK);

      } else if ((maze_one_lookup_table[row_counter][col]) == 3) {
        set_pixel(row_counter, col, YELLOW);
      }
    }
    row_counter++;

  }

  else {
    row_counter = 0;
  }
  vTaskDelay(1);
}

void tom_image_2(uint8_t x, uint8_t y) {

  set_pixel(x, y, BLUE);

  set_pixel(x + 1, y, RED);

  set_pixel(x + 2, y, BLUE);

  set_pixel(x + 1, y + 1, BLUE);

  set_pixel(x + 1, y - 1, BLUE);

  update_display();
  tom_image_2_clear(x, y);
}

void jerry_image(uint8_t x, uint8_t y) {

  set_pixel(x, y, YELLOW);
  set_pixel(x - 2, y, YELLOW);
  set_pixel(x - 1, y + 1, YELLOW);
  delay__ms(1);
  clear_pixel(x, y);
  clear_pixel(x - 2, y);
  clear_pixel(x - 1, y + 1);
}

void jerry_image_clear(uint8_t x, uint8_t y) {

  clear_pixel(x, y);
  clear_pixel(x - 2, y);
  clear_pixel(x - 1, y + 1);
}

void tom_image_2_clear(uint8_t x, uint8_t y) {
  clear_pixel(x, y);
  update_display();
  clear_pixel(x + 1, y);
  update_display();
  clear_pixel(x + 2, y);
  update_display();
  clear_pixel(x + 1, y + 1);
  update_display();
  clear_pixel(x + 1, y - 1);
  update_display();
}
