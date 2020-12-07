#include "display_start_screen.h"
#include "delay.h"
#include "led_matrix.h"
#include "matrix_look_up_table.h"

uint8_t row_counter_start_screen = 0;
uint8_t row_counter_pause_screen = 0;

void start_screen_display(void) {

  if (row_counter_start_screen < LEDMATRIX_HEIGHT) {
    for (uint8_t col_counter_start_screen = 0;
         col_counter_start_screen < LEDMATRIX_WIDTH;
         col_counter_start_screen++) {
      if ((game_start_display_lookup_table[row_counter_start_screen]
                                          [col_counter_start_screen]) == 4) {
        set_pixel(row_counter_start_screen, col_counter_start_screen, BLUE);

      } else if ((game_start_display_lookup_table[row_counter_start_screen]
                                                 [col_counter_start_screen]) ==
                 2) {
        set_pixel(row_counter_start_screen, col_counter_start_screen, GREEN);

      } else if ((game_start_display_lookup_table[row_counter_start_screen]
                                                 [col_counter_start_screen]) ==
                 5) {
        set_pixel(row_counter_start_screen, col_counter_start_screen, PINK);

      } else if ((game_start_display_lookup_table[row_counter_start_screen]
                                                 [col_counter_start_screen]) ==
                 1) {
        set_pixel(row_counter_start_screen, col_counter_start_screen, RED);

      } else if ((game_start_display_lookup_table[row_counter_start_screen]
                                                 [col_counter_start_screen]) ==
                 6) {
        set_pixel(row_counter_start_screen, col_counter_start_screen, CYAN);

      } else if ((game_start_display_lookup_table[row_counter_start_screen]
                                                 [col_counter_start_screen]) ==
                 7) {
        set_pixel(row_counter_start_screen, col_counter_start_screen, WHITE);

      } else if ((game_start_display_lookup_table[row_counter_start_screen]
                                                 [col_counter_start_screen]) ==
                 3) {
        set_pixel(row_counter_start_screen, col_counter_start_screen, YELLOW);
      }
    }
    row_counter_start_screen++;

  }

  else {
    row_counter_start_screen = 0;
  }
  vTaskDelay(1);
  // jerry_image_start_screen();
  // tom_image_start_screen();
}

void jerry_image_start_screen(void) {

  for (uint8_t counter = 12; counter < 19; counter++) {
    for (uint8_t y = 0; y < 64; y++) {
      for (uint8_t x = 0; x < 32; x++) {
        if (game_start_display_lookup_table[x][y] == counter) {
          set_pixel(x, y, YELLOW);
          set_pixel(x + 2, y, YELLOW);
          set_pixel(x + 1, y, YELLOW);
          set_pixel(x + 1, y + 1, YELLOW);
          delay__ms(200);
          clear_pixel(x, y);
          clear_pixel(x + 2, y);
          clear_pixel(x + 1, y);
          clear_pixel(x + 1, y + 1);
        }
      }
    }
  }
  delay__ms(1);
}

void tom_image_start_screen(void) {

  for (uint8_t counter = 19; counter < 26; counter++) {
    for (uint8_t y = 0; y < 64; y++) {
      for (uint8_t x = 0; x < 32; x++) {
        if (game_start_display_lookup_table[x][y] == counter) {
          set_pixel(x, y, RED);
          set_pixel(x + 2, y, RED);
          set_pixel(x + 1, y, RED);
          set_pixel(x + 1, y + 1, RED);
          delay__ms(200);
          clear_pixel(x, y);
          clear_pixel(x + 2, y);
          clear_pixel(x + 1, y);
          clear_pixel(x + 1, y + 1);
        }
      }
    }
  }
  delay__ms(1);
}

void pause_screen_display(void) {

  if (row_counter_pause_screen < LEDMATRIX_HEIGHT) {
    for (uint8_t col_counter_pause_screen = 0;
         col_counter_pause_screen < LEDMATRIX_WIDTH;
         col_counter_pause_screen++) {
      if ((game_start_display_lookup_table[row_counter_pause_screen]
                                          [col_counter_pause_screen]) == 4) {
        set_pixel(row_counter_pause_screen, col_counter_pause_screen, BLUE);

      } else if ((game_start_display_lookup_table[row_counter_pause_screen]
                                                 [col_counter_pause_screen]) ==
                 2) {
        set_pixel(row_counter_pause_screen, col_counter_pause_screen, GREEN);

      } else if ((game_start_display_lookup_table[row_counter_pause_screen]
                                                 [col_counter_pause_screen]) ==
                 5) {
        set_pixel(row_counter_pause_screen, col_counter_pause_screen, PINK);

      } else if ((game_start_display_lookup_table[row_counter_pause_screen]
                                                 [col_counter_pause_screen]) ==
                 1) {
        set_pixel(row_counter_pause_screen, col_counter_pause_screen, RED);

      } else if ((game_start_display_lookup_table[row_counter_pause_screen]
                                                 [col_counter_pause_screen]) ==
                 6) {
        set_pixel(row_counter_pause_screen, col_counter_pause_screen, CYAN);

      } else if ((game_start_display_lookup_table[row_counter_pause_screen]
                                                 [col_counter_pause_screen]) ==
                 7) {
        set_pixel(row_counter_pause_screen, col_counter_pause_screen, WHITE);

      } else if ((game_start_display_lookup_table[row_counter_pause_screen]
                                                 [col_counter_pause_screen]) ==
                 3) {
        set_pixel(row_counter_pause_screen, col_counter_pause_screen, YELLOW);
      }
    }
    row_counter_pause_screen++;

  }

  else {
    row_counter_pause_screen = 0;
  }
  vTaskDelay(1);
}