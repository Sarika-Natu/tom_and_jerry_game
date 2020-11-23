#include "led_matrix.h"
#include "delay.h"
#include "gpio.h"

uint8_t display_matrix[LEDMATRIX_HALF_HEIGHT][LEDMATRIX_WIDTH];
RGB_gpio RGB = {{2, 0}, {2, 1}, {2, 2},  {2, 5},  {2, 4},  {0, 15}, {2, 7},
                {2, 8}, {2, 9}, {0, 16}, {1, 23}, {1, 20}, {1, 28}};

void disable_display(void) { gpio__set(RGB.OE); }

void enable_display(void) { gpio__reset(RGB.OE); }

void enable_latch_data(void) { gpio__set(RGB.STB); }

void disable_latch_data(void) { gpio__reset(RGB.STB); }

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

    clear_display();

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
  }
}

void set_pixel(int8_t row, int8_t col, color_t color) {

  if ((row < 0) || (row >= LEDMATRIX_HEIGHT))
    return;
  if ((col < 0) || (col >= LEDMATRIX_WIDTH))
    return;

  if (row >= LEDMATRIX_HALF_HEIGHT) {
    fprintf(stderr, "row %d col %d\n", row, col);
    display_matrix[row % LEDMATRIX_HALF_HEIGHT][col] |= (color << 4);
    fprintf(stderr, "row %d col %d\n", row, col);
  } else {
    fprintf(stderr, "row %d col %d\n", row, col);
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

    select_row(row);
    disable_display();    // gpio__set(OE);
    disable_latch_data(); // gpio__reset(LAT);
    data_clock_in(row);
    enable_latch_data(); // gpio__set(LAT);
    enable_display();    // gpio__reset(OE);
    delay__us(150);      // Change Brightness
    disable_display();
  }
}

void display_rectangle_pink(void) {
  for (uint8_t x = 2; x < 12; x++) {
    for (uint8_t y = 2; y < 13; y++) {
      set_pixel(x, y, PINK);
      update_display();
    }
  }
}

void display_rectangle_red(void) {
  for (uint8_t x = 24; x < 32; x++) {
    for (uint8_t y = 29; y < 54; y++) {
      set_pixel(x, y, RED);
      update_display();
    }
  }
}

void display_moving_image(uint8_t x, uint8_t y) {
  for (x = 2; x < 23; x++) {
    for (y = 29; y < 54; y++) {
      if (x == 22) {
        clear_pixel(22, y);
        update_display();
      } else if (y == 53) {
        clear_pixel(x, 53);
        update_display();
      } else {
        set_pixel(x, y, GREEN);
        clear_pixel(x - 1, y - 1);
        update_display();
      }
    }
  }
  clear_pixel(x, y);
  update_display();
}

void move_rectangle(void) {
  for (uint8_t x = 5; x < 30; x++) {
    for (uint8_t y = 29; y < 54; y++) {
      set_pixel(x, y, GREEN);

      set_pixel(x + 3, y, GREEN);
      clear_pixel(x - 1, y - 1);
      set_pixel(x, y + 3, BLUE);
      clear_pixel(x - 1, y - 1);
      set_pixel(x + 3, y + 3, PINK);
      clear_pixel(x - 1, y - 1);

      update_display();
    }
  }
}

void display_maze(void) {
  for (uint8_t x = 0; x < LEDMATRIX_HEIGHT; x++) {
    for (uint8_t y = 0; y < LEDMATRIX_WIDTH; y++) {
      if ((x < 4)) {
        set_pixel(x, y, GREEN);

        set_pixel((LEDMATRIX_HEIGHT - 1) - x, y, GREEN);
        update_display();
      } else if ((x > 7) && (y < 5)) {
        set_pixel(x, y, RED);
        set_pixel(x + 7, (LEDMATRIX_WIDTH - 1) - y, RED);
        update_display();
      } else if ((x > 3) && (y < 50) && (y >= 40) &&
                 (x < LEDMATRIX_HEIGHT - 10)) {
        set_pixel(x, y, RED);

        update_display();
      } else if ((x > 7) && (x < 15) && (y >= 13) && (y < 16)) {
        set_pixel(x, y, RED);

        update_display();
      }
    }
  }
}

void display_image(void) {
  uint8_t i = 19;
  set_pixel(i, 15, PINK);
  set_pixel(i, 14, PINK);
  set_pixel(i, 16, PINK);
  set_pixel(i - 1, 14, PINK);
  set_pixel(i + 1, 14, PINK);

  update_display();
}
