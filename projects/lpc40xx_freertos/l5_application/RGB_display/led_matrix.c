#include "led_matrix.h"
#include "delay.h"
#include "ff.h"
#include "game_logic.h"
#include "gpio.h"
#include "maze.h"
RGB_gpio RGB = {{2, 0}, {2, 1}, {2, 2},  {2, 5},  {2, 4},  {0, 15}, {2, 7},
                {2, 8}, {2, 9}, {0, 16}, {1, 23}, {1, 20}, {1, 28}};
uint8_t RGB_A, RGB_B, RGB_C, RGB_D;

uint8_t display_matrix[LEDMATRIX_HALF_HEIGHT][LEDMATRIX_WIDTH] = {0};

bool right_move;
bool left_move;
bool up_move;
bool down_move;
extern struct object_axis jerry;
const uint8_t jerry_start_position = 12;
uint8_t const jerry_end_positions[3] = {211, 128, 181};
uint8_t jerry_motion_counter = 0;

static void update_display(void);
static void select_row(uint8_t row);
static void disable_display(void);
static void enable_display(void);
static void enable_latch_data(void);
static void disable_latch_data(void);
static void data_clock_in(uint8_t row);

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

void RGB_task(void *params) {
  while (1) {
    update_display();
    vTaskDelay(5);
    if (change_level) {
      vTaskSuspend(jerry_motion_suspend);
      jerry_motion_counter = JERRY_START_POSITION;
    } else {

      vTaskResume(jerry_motion_suspend);
    }
    if (pause_or_stop) {
      vTaskSuspend(jerry_motion_suspend);
      left_move = false;
      right_move = false;
      down_move = false;
      up_move = false;
      pause_or_stop = false;
    } else if (game_on_after_pause) {
      vTaskResume(jerry_motion_suspend);
      left_move = true;
      right_move = true;
      down_move = true;
      up_move = true;
      game_on_after_pause = false;
    }
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

static void disable_display(void) { gpio__set(RGB.OE); }

static void enable_display(void) { gpio__reset(RGB.OE); }

static void enable_latch_data(void) { gpio__set(RGB.STB); }

static void disable_latch_data(void) { gpio__reset(RGB.STB); }

void clear_display(void) {
  gpio__reset(RGB.R1);
  gpio__reset(RGB.G1);
  gpio__reset(RGB.B1);
  gpio__reset(RGB.B2);
  gpio__reset(RGB.R2);
  gpio__reset(RGB.G2);
  update_display();
}

void jerry_image(void) {

  for (jerry_motion_counter = JERRY_START_POSITION;
       jerry_motion_counter <= jerry_end_positions[level];
       jerry_motion_counter++) {
    for (uint8_t y = 0; y < LEDMATRIX_WIDTH; y++) {
      for (uint8_t x = 0; x < LEDMATRIX_HEIGHT; x++) {
        switch (level) {
        case 0:
          if (maze_one_lookup_table[x][y] == jerry_motion_counter) {
            jerry.x = x;
            jerry.y = y;
            set_pixel(x, y, YELLOW);         // top
            set_pixel(x + 2, y, YELLOW);     // bottom
            set_pixel(x + 1, y, YELLOW);     // middle_left
            set_pixel(x + 1, y + 1, YELLOW); // middle_right
            delay__ms(150);
            clear_pixel(x, y);
            clear_pixel(x + 2, y);
            clear_pixel(x + 1, y);
            clear_pixel(x + 1, y + 1);
          }
          break;
        case 1:
          if (maze_two_lookup_table[x][y] == jerry_motion_counter) {
            jerry.x = x;
            jerry.y = y;
            set_pixel(x, y, YELLOW);         // top
            set_pixel(x + 2, y, YELLOW);     // bottom
            set_pixel(x + 1, y, YELLOW);     // middle_left
            set_pixel(x + 1, y + 1, YELLOW); // middle_right
            delay__ms(150);
            clear_pixel(x, y);
            clear_pixel(x + 2, y);
            clear_pixel(x + 1, y);
            clear_pixel(x + 1, y + 1);
          }
          break;
        case 2:
          if (maze_three_lookup_table[x][y] == jerry_motion_counter) {
            jerry.x = x;
            jerry.y = y;
            set_pixel(x, y, YELLOW);         // top
            set_pixel(x + 2, y, YELLOW);     // bottom
            set_pixel(x + 1, y, YELLOW);     // middle_left
            set_pixel(x + 1, y + 1, YELLOW); // middle_right
            delay__ms(150);
            clear_pixel(x, y);
            clear_pixel(x + 2, y);
            clear_pixel(x + 1, y);
            clear_pixel(x + 1, y + 1);
          }
          break;

        default:
          // puts("No maze exists");
          break;
        }
      }
    }
  }
}

void tom_image(uint8_t x, uint8_t y) {

  tom.x = x;
  tom.y = y;
  set_pixel(x + 1, y + 2, RED); // top
  set_pixel(x + 2, y + 1, RED); // left
  set_pixel(x + 2, y + 2, RED); // middle
  set_pixel(x + 2, y + 3, RED); // right
  set_pixel(x + 3, y + 2, RED); // bottom

  delay__ms(1);
  clear_pixel(x + 1, y + 2);
  clear_pixel(x + 2, y + 1);
  clear_pixel(x + 2, y + 2);
  clear_pixel(x + 2, y + 3);
  clear_pixel(x + 3, y + 2);
}

void tom_move_on_maze(uint8_t x, uint8_t y) {

  switch (level) {
  case 0:
    if (command_up) {
      if (maze_one_lookup_table[x + 1][y + 3] == 5 ||
          maze_one_lookup_table[x + 1][y + 3] == 4 ||
          maze_one_lookup_table[x + 1][y + 1] == 5 ||
          maze_one_lookup_table[x + 1][y + 1] == 4) {
        up_move = false;
      } else if (maze_one_lookup_table[x][y + 2] == 5 ||
                 maze_one_lookup_table[x][y + 2] == 4) {
        up_move = false;
      }
    }

    if (command_left) {
      if (maze_one_lookup_table[x + 1][y + 1] == 5 ||
          maze_one_lookup_table[x + 1][y + 1] == 4 ||
          maze_one_lookup_table[x + 3][y + 1] == 5 ||
          maze_one_lookup_table[x + 3][y + 1] == 4) {
        left_move = false;
      } else if (maze_one_lookup_table[x + 2][y] == 5 ||
                 maze_one_lookup_table[x + 2][y] == 4) {
        left_move = false;
      }
    }

    if (command_right) {
      if (maze_one_lookup_table[x + 1][y + 3] == 5 ||
          maze_one_lookup_table[x + 1][y + 3] == 4 ||
          maze_one_lookup_table[x + 3][y + 3] == 5 ||
          maze_one_lookup_table[x + 3][y + 3] == 4) {
        right_move = false;
      } else if (maze_one_lookup_table[x + 2][y + 4] == 5 ||
                 maze_one_lookup_table[x + 2][y + 4] == 4) {
        right_move = false;
      }
    }

    if (command_down) {
      if (maze_one_lookup_table[x + 3][y + 3] == 5 ||
          maze_one_lookup_table[x + 3][y + 3] == 4 ||
          maze_one_lookup_table[x + 3][y + 1] == 5 ||
          maze_one_lookup_table[x + 3][y + 1] == 4) {
        down_move = false;
      } else if (maze_one_lookup_table[x + 4][y + 2] == 5 ||
                 maze_one_lookup_table[x + 4][y + 2] == 4) {
        down_move = false;
      }
    }
    break;

  case 1:
    if (command_up) {
      if (maze_two_lookup_table[x + 1][y + 3] == 5 ||
          maze_two_lookup_table[x + 1][y + 3] == 4 ||
          maze_two_lookup_table[x + 1][y + 1] == 5 ||
          maze_two_lookup_table[x + 1][y + 1] == 4) {
        up_move = false;
      } else if (maze_two_lookup_table[x][y + 2] == 5 ||
                 maze_two_lookup_table[x][y + 2] == 4) {
        up_move = false;
      }
    }

    if (command_left) {
      if (maze_two_lookup_table[x + 1][y + 1] == 5 ||
          maze_two_lookup_table[x + 1][y + 1] == 4 ||
          maze_two_lookup_table[x + 3][y + 1] == 5 ||
          maze_two_lookup_table[x + 3][y + 1] == 4) {
        left_move = false;
      } else if (maze_two_lookup_table[x + 2][y] == 5 ||
                 maze_two_lookup_table[x + 2][y] == 4) {
        left_move = false;
      }
    }

    if (command_right) {
      if (maze_two_lookup_table[x + 1][y + 3] == 5 ||
          maze_two_lookup_table[x + 1][y + 3] == 4 ||
          maze_two_lookup_table[x + 3][y + 3] == 5 ||
          maze_two_lookup_table[x + 3][y + 3] == 4) {
        right_move = false;
      } else if (maze_two_lookup_table[x + 2][y + 4] == 5 ||
                 maze_two_lookup_table[x + 2][y + 4] == 4) {
        right_move = false;
      }
    }

    if (command_down) {
      if (maze_two_lookup_table[x + 3][y + 3] == 5 ||
          maze_two_lookup_table[x + 3][y + 3] == 4 ||
          maze_two_lookup_table[x + 3][y + 1] == 5 ||
          maze_two_lookup_table[x + 3][y + 1] == 4) {
        down_move = false;
      } else if (maze_two_lookup_table[x + 4][y + 2] == 5 ||
                 maze_two_lookup_table[x + 4][y + 2] == 4) {
        down_move = false;
      }
    }
    break;

  case 2:

    if (command_up) {
      if (maze_three_lookup_table[x + 1][y + 3] == 5 ||
          maze_three_lookup_table[x + 1][y + 3] == 4 ||
          maze_three_lookup_table[x + 1][y + 1] == 5 ||
          maze_three_lookup_table[x + 1][y + 1] == 4) {
        up_move = false;
      } else if (maze_three_lookup_table[x][y + 2] == 5 ||
                 maze_three_lookup_table[x][y + 2] == 4) {
        up_move = false;
      }
    }

    if (command_left) {
      if (maze_three_lookup_table[x + 1][y + 1] == 5 ||
          maze_three_lookup_table[x + 1][y + 1] == 4 ||
          maze_three_lookup_table[x + 3][y + 1] == 5 ||
          maze_three_lookup_table[x + 3][y + 1] == 4) {
        left_move = false;
      } else if (maze_three_lookup_table[x + 2][y] == 5 ||
                 maze_three_lookup_table[x + 2][y] == 4) {
        left_move = false;
      }
    }

    if (command_right) {
      if (maze_three_lookup_table[x + 1][y + 3] == 5 ||
          maze_three_lookup_table[x + 1][y + 3] == 4 ||
          maze_three_lookup_table[x + 3][y + 3] == 5 ||
          maze_three_lookup_table[x + 3][y + 3] == 4) {
        right_move = false;
      } else if (maze_three_lookup_table[x + 2][y + 4] == 5 ||
                 maze_three_lookup_table[x + 2][y + 4] == 4) {
        right_move = false;
      }
    }

    if (command_down) {
      if (maze_three_lookup_table[x + 3][y + 3] == 5 ||
          maze_three_lookup_table[x + 3][y + 3] == 4 ||
          maze_three_lookup_table[x + 3][y + 1] == 5 ||
          maze_three_lookup_table[x + 3][y + 1] == 4) {
        down_move = false;
      } else if (maze_three_lookup_table[x + 4][y + 2] == 5 ||
                 maze_three_lookup_table[x + 4][y + 2] == 4) {
        down_move = false;
      }
    }
    break;

  default:

    break;
  }
}

static void select_row(uint8_t row) {
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

static void data_clock_in(uint8_t row) {

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
static void update_display(void) {
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
  disable_display();
}