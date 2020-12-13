#include "display_screen_RGB.h"
#include "delay.h"
#include "ff.h"
#include "led_matrix.h"
#include "maze.h"

static uint8_t row_counter_start_screen = 0;
static uint8_t row_counter_pause_screen = 0;
static uint8_t row_counter_level_screen = 0;
static uint8_t row_counter_game_over_screen = 0;
static uint8_t row_counter_player_won_screen = 0;
static uint8_t row_counter_player_ready_screen = 0;

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
}

void pause_screen_display(void) {

  if (row_counter_pause_screen < LEDMATRIX_HEIGHT) {
    for (uint8_t col_counter_pause_screen = 0;
         col_counter_pause_screen < LEDMATRIX_WIDTH;
         col_counter_pause_screen++) {
      if ((pause_screen_display_lookup_table[row_counter_pause_screen]
                                            [col_counter_pause_screen]) == 4) {
        set_pixel(row_counter_pause_screen, col_counter_pause_screen, BLUE);

      } else if ((pause_screen_display_lookup_table
                      [row_counter_pause_screen][col_counter_pause_screen]) ==
                 7) {
        set_pixel(row_counter_pause_screen, col_counter_pause_screen, WHITE);

      } else if ((pause_screen_display_lookup_table
                      [row_counter_pause_screen][col_counter_pause_screen]) ==
                 3) {
        set_pixel(row_counter_pause_screen, col_counter_pause_screen, YELLOW);

      } else if ((pause_screen_display_lookup_table
                      [row_counter_pause_screen][col_counter_pause_screen]) ==
                 5) {
        set_pixel(row_counter_pause_screen, col_counter_pause_screen, PINK);

      } else if ((pause_screen_display_lookup_table
                      [row_counter_pause_screen][col_counter_pause_screen]) ==
                 1) {
        set_pixel(row_counter_pause_screen, col_counter_pause_screen, RED);
      } else if ((pause_screen_display_lookup_table
                      [row_counter_pause_screen][col_counter_pause_screen]) ==
                 4) {
        set_pixel(row_counter_pause_screen, col_counter_pause_screen, BLUE);
      } else if ((pause_screen_display_lookup_table
                      [row_counter_pause_screen][col_counter_pause_screen]) ==
                 2) {
        set_pixel(row_counter_pause_screen, col_counter_pause_screen, GREEN);
      }
    }
    row_counter_pause_screen++;

  }

  else {
    row_counter_pause_screen = 0;
  }
  vTaskDelay(1);
}

void clear_screen_display(void) {
  for (uint8_t y = 0; y < 64; y++) {
    for (uint8_t x = 0; x < 32; x++) {
      clear_pixel(x, y);
    }
  }
  vTaskDelay(1);
}

void level_display(void) {
  if (row_counter_level_screen < LEDMATRIX_HEIGHT) {
    for (uint8_t col = 0; col < LEDMATRIX_WIDTH; col++) {
      if ((next_level_display_lookup_table[row_counter_level_screen][col]) ==
          2) {
        set_pixel(row_counter_level_screen, col, GREEN);

      } else if ((next_level_display_lookup_table[row_counter_level_screen]
                                                 [col]) == 6) {
        set_pixel(row_counter_level_screen, col, CYAN);

      } else if ((next_level_display_lookup_table[row_counter_level_screen]
                                                 [col]) == 4) {
        set_pixel(row_counter_level_screen, col, BLUE);

      } else if ((next_level_display_lookup_table[row_counter_level_screen]
                                                 [col]) == 5) {
        set_pixel(row_counter_level_screen, col, PINK);
      } else if ((next_level_display_lookup_table[row_counter_level_screen]
                                                 [col]) == 1) {
        set_pixel(row_counter_level_screen, col, RED);
      }
    }
    row_counter_level_screen++;

  }

  else {
    row_counter_level_screen = 0;
  }
  vTaskDelay(1);
}

void game_over_display(void) {
  if (row_counter_game_over_screen < LEDMATRIX_HEIGHT) {
    for (uint8_t col = 0; col < LEDMATRIX_WIDTH; col++) {
      if ((game_over_display_lookup_table[row_counter_game_over_screen][col]) ==
          1) {
        set_pixel(row_counter_game_over_screen, col, RED);

      } else if ((game_over_display_lookup_table[row_counter_game_over_screen]
                                                [col]) == 7) {
        set_pixel(row_counter_game_over_screen, col, WHITE);

      } else if ((game_over_display_lookup_table[row_counter_game_over_screen]
                                                [col]) == 3) {
        set_pixel(row_counter_game_over_screen, col, YELLOW);

      } else if ((game_over_display_lookup_table[row_counter_game_over_screen]
                                                [col]) == 5) {
        set_pixel(row_counter_game_over_screen, col, PINK);
      }
    }
    row_counter_game_over_screen++;

  }

  else {
    row_counter_game_over_screen = 0;
  }
  vTaskDelay(1);
}

void player_won_display(void) {
  if (row_counter_player_won_screen < LEDMATRIX_HEIGHT) {
    for (uint8_t col = 0; col < LEDMATRIX_WIDTH; col++) {
      if ((player_won_display_lookup_table[row_counter_player_won_screen]
                                          [col]) == 2) {
        set_pixel(row_counter_player_won_screen, col, GREEN);

      } else if ((player_won_display_lookup_table[row_counter_player_won_screen]
                                                 [col]) == 7) {
        set_pixel(row_counter_player_won_screen, col, WHITE);

      } else if ((player_won_display_lookup_table[row_counter_player_won_screen]
                                                 [col]) == 3) {
        set_pixel(row_counter_player_won_screen, col, YELLOW);

      } else if ((player_won_display_lookup_table[row_counter_player_won_screen]
                                                 [col]) == 5) {
        set_pixel(row_counter_player_won_screen, col, PINK);

      } else if ((player_won_display_lookup_table[row_counter_player_won_screen]
                                                 [col]) == 1) {
        set_pixel(row_counter_player_won_screen, col, RED);
      }
    }
    row_counter_player_won_screen++;

  }

  else {
    row_counter_player_won_screen = 0;
  }
  vTaskDelay(1);
}

void player_ready_display(void) {
  if (row_counter_player_ready_screen < LEDMATRIX_HEIGHT) {
    for (uint8_t col = 0; col < LEDMATRIX_WIDTH; col++) {
      if ((player_ready_display_lookup_table[row_counter_player_ready_screen]
                                            [col]) == 2) {
        set_pixel(row_counter_player_ready_screen, col, GREEN);

      } else if ((player_ready_display_lookup_table
                      [row_counter_player_ready_screen][col]) == 7) {
        set_pixel(row_counter_player_ready_screen, col, WHITE);

      } else if ((player_ready_display_lookup_table
                      [row_counter_player_ready_screen][col]) == 3) {
        set_pixel(row_counter_player_ready_screen, col, YELLOW);

      } else if ((player_ready_display_lookup_table
                      [row_counter_player_ready_screen][col]) == 5) {
        set_pixel(row_counter_player_ready_screen, col, PINK);

      } else if ((player_ready_display_lookup_table
                      [row_counter_player_ready_screen][col]) == 1) {
        set_pixel(row_counter_player_ready_screen, col, RED);
      }
    }
    row_counter_player_ready_screen++;

  }

  else {
    row_counter_player_ready_screen = 0;
  }
  vTaskDelay(1);
}
