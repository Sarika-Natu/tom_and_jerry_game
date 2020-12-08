#include "display_screen_RGB.h"
#include "ff.h"
#include "led_matrix.h"
#include "stdio.h"

uint8_t row_counter_maze_one = 0;
uint8_t row_counter_set_maze_two = 0;
uint8_t row_counter_set_maze_three = 0;
uint8_t row_counter_set_maze_four = 0;

void maze_one_frame(void) {

  if (row_counter_maze_one < LEDMATRIX_HEIGHT) {
    for (uint8_t col = 0; col < LEDMATRIX_WIDTH; col++) {
      if ((maze_one_lookup_table[row_counter_maze_one][col]) == 4) {
        set_pixel(row_counter_maze_one, col, BLUE);

      } else if ((maze_one_lookup_table[row_counter_maze_one][col]) == 2) {
        set_pixel(row_counter_maze_one, col, GREEN);

      } else if ((maze_one_lookup_table[row_counter_maze_one][col]) == 5) {
        set_pixel(row_counter_maze_one, col, PINK);

      } else if ((maze_one_lookup_table[row_counter_maze_one][col]) == 3) {
        set_pixel(row_counter_maze_one, col, YELLOW);
      }
    }
    row_counter_maze_one++;
  } else {
    row_counter_maze_one = 0;
  }
  vTaskDelay(1);
}

void maze_two_display(void) {
  if (row_counter_set_maze_two < LEDMATRIX_HEIGHT) {
    for (uint8_t col = 0; col < LEDMATRIX_WIDTH; col++) {
      if ((maze_two_lookup_table[row_counter_set_maze_two][col]) == 4) {
        set_pixel(row_counter_set_maze_two, col, BLUE);

      } else if ((maze_two_lookup_table[row_counter_set_maze_two][col]) == 5) {
        set_pixel(row_counter_set_maze_two, col, PINK);
      }
    }
    row_counter_set_maze_two++;
  } else {
    row_counter_set_maze_two = 0;
  }
  vTaskDelay(2);
}

void maze_three_display(void) {
  if (row_counter_set_maze_three < LEDMATRIX_HEIGHT) {
    for (uint8_t col = 0; col < LEDMATRIX_WIDTH; col++) {
      if ((maze_three_lookup_table[row_counter_set_maze_three][col]) == 4) {
        set_pixel(row_counter_set_maze_three, col, BLUE);

      } else if ((maze_three_lookup_table[row_counter_set_maze_three][col]) ==
                 5) {
        set_pixel(row_counter_set_maze_three, col, PINK);
      }
    }
    row_counter_set_maze_three++;
  } else {
    row_counter_set_maze_three = 0;
  }
  vTaskDelay(2);
}

void maze_four_display(void) {
  if (row_counter_set_maze_four < LEDMATRIX_HEIGHT) {
    for (uint8_t col = 0; col < LEDMATRIX_WIDTH; col++) {
      if ((maze_four_lookup_table[row_counter_set_maze_four][col]) == 4) {
        set_pixel(row_counter_set_maze_four, col, BLUE);

      } else if ((maze_four_lookup_table[row_counter_set_maze_four][col]) ==
                 5) {
        set_pixel(row_counter_set_maze_four, col, PINK);
      }
    }
    row_counter_set_maze_four++;

  }

  else {
    row_counter_set_maze_four = 0;
  }
  vTaskDelay(2);
}