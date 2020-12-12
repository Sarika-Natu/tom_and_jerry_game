#include "game_logic.h"
#include "FreeRTOS.h"
#include "delay.h"
#include "display_screen_RGB.h"
#include "game_accelerometer.h"
#include "game_level.h"
#include "gpio.h"
#include "gpio_isr.h"
#include "led_matrix.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"
#include "maze.h"
#include "mp3_decoder.h"
#include "semphr.h"

// #define TEST
enum game_state {
  START_SCREEN = 0,
  PLAYER_READY = 1,
  NEXT_LEVEL_SCREEN = 2,
  GAME_ON = 3,
  PAUSE_PLEASE = 4,
  TOM_WON = 5,
  JERRY_WON = 6,
  SCORE_CARD = 7
};
uint8_t game_screen_state = START_SCREEN;
bool game_on = false;

bool pause_or_stop = false;
bool game_on_after_pause = false;
bool change_level = false;
extern SemaphoreHandle_t button_pressed_signal;
extern SemaphoreHandle_t change_game_state;

uint8_t const jerry_end_position_1 = 211;
uint8_t const jerry_end_position_2 = 128;
uint8_t const jerry_end_position_3 = 181;

uint8_t previous_game_mode = 0;
uint8_t const jerry_end_positions[3] = {
    jerry_end_position_1, jerry_end_position_2, jerry_end_position_3};
uint8_t level = 0;
uint8_t tom_lives = 3;
maze_selection_t maze_lookup_table[3] = {maze_one_frame, maze_two_frame,
                                         maze_three_frame};
bool tom_or_jerry_won_decision = false;

void collision_detector(void);
void player_failed(void);

void game_task(void *p) {
#ifdef TEST
  puts("Game task");
#endif
  bool change_state = false;
  game_screen_state = START_SCREEN;

  while (1) {
    if (xSemaphoreTake(change_game_state, 0)) {
      change_state = true;
    }

    switch (game_screen_state) {

    case START_SCREEN:
#ifdef TEST
      puts("START_SCREEN");
#endif
      start_screen_display();
      sound.entry = true;
      if (change_state) {
        clear_screen_display();
        change_state = false;
        game_screen_state = PLAYER_READY;
      }

      break;

    case PLAYER_READY:
#ifdef TEST
      puts("LEVEL_SCREEN");
#endif
      player_ready_display();

      if (change_state) {
        clear_screen_display();
        change_state = false;
        game_screen_state = GAME_ON;
      }
      break;

    case NEXT_LEVEL_SCREEN:
#ifdef TEST
      puts("LEVEL_SCREEN");
#endif
      level_display();

      if (change_state) {
        clear_screen_display();
        change_state = false;
        game_screen_state = GAME_ON;
      }

      break;

    case GAME_ON:
#ifdef TEST
      puts("GAME_SCREEN");

#endif
      maze_lookup_table[level]();
      change_level = false;

      game_on = true;
      tom_image(row_count, col_count);
      sound.game = true;
      collision_detector();
      player_failed();

      if (change_state) {
        change_state = false;
        clear_screen_display();
        game_screen_state = PAUSE_PLEASE;
      }

      break;

    case PAUSE_PLEASE:
#ifdef TEST
      puts("PAUSE_SCREEN");
#endif
      pause_or_stop = true;
      pause_screen_display();
      sound.entry = true;
      if (change_state) {
        game_on_after_pause = true;
        change_state = false;
        clear_screen_display();
        game_screen_state = GAME_ON;
      }
      break;

    case TOM_WON:
#ifdef TEST
      puts("TOMWON");
#endif
      tom_lives = 3;
      pause_or_stop = true;
      sound.catchsuccess = true;
      change_level = true;
      if (level < 2) {
        clear_screen_display();
        level++;
        row_count = 1;
        col_count = 1;
        game_screen_state = NEXT_LEVEL_SCREEN;

      } else {
        level = 0;
        clear_screen_display();
        tom_or_jerry_won_decision = false;
        game_screen_state = SCORE_CARD;
      }
      break;

    case JERRY_WON:
#ifdef TEST
      puts("JERRYWON");

#endif

      pause_or_stop = true;
      change_level = true;
      sound.catchfail = true;

      if (tom_lives > 1) {

        tom_lives--;
        row_count = 1;
        col_count = 1;
        game_screen_state = GAME_ON;
      } else {
        tom_or_jerry_won_decision = true;
        clear_screen_display();
        game_screen_state = SCORE_CARD;
      }

      break;

    case SCORE_CARD:
#ifdef TEST
      puts("SCORECARD");
#endif
      if (tom_or_jerry_won_decision) {
        game_over_display();
      } else {
        player_won_display();
      }

      sound.scorecard = true;
      if (change_state) {
        change_state = false;
        clear_screen_display();
        game_screen_state = START_SCREEN;
      }
      break;

    default:

      puts("DEFAULT");
      if (change_state) {
        clear_screen_display();
        change_state = false;
        game_screen_state = START_SCREEN;
      }
      break;
    }
  }
  vTaskDelay(500);
}

void button_isr(void) { xSemaphoreGiveFromISR(button_pressed_signal, NULL); }

void button_task(void *p) {
  while (1) {
    if (xSemaphoreTake(button_pressed_signal, portMAX_DELAY)) {
      xSemaphoreGive(change_game_state);
    }
    vTaskDelay(10);
  }
}

void setup_button_isr(void) {
  button_pressed_signal = xSemaphoreCreateBinary();
  change_game_state = xSemaphoreCreateBinary();
  lpc_peripheral__enable_interrupt(LPC_PERIPHERAL__GPIO,
                                   gpio__interrupt_dispatcher, "gpio_intr");
  gpio__attach_interrupt(GPIO__PORT_0, 29, GPIO_INTR__FALLING_EDGE, button_isr);
}

void collision_detector(void) {

  // Tom botton collide with Jerry cordinates
  if ((jerry.x == tom.x + 3 && jerry.y == tom.y + 2) ||
      (jerry.x + 1 == tom.x + 3 && jerry.y + 1 == tom.y + 2) ||
      (jerry.x + 2 == tom.x + 3 && jerry.y == tom.y + 2) ||
      (jerry.x + 1 == tom.x + 3 && jerry.y == tom.y + 2)) {
    puts("Collision detect - Tom Won");
    game_screen_state = TOM_WON;
    clear_screen_display();
  }
  // Tom right collide with Jerry cordinates
  else if ((jerry.x == tom.x + 2 && jerry.y == tom.y + 3) ||
           (jerry.x + 1 == tom.x + 2 && jerry.y + 1 == tom.y + 3) ||
           (jerry.x + 2 == tom.x + 2 && jerry.y == tom.y + 3) ||
           (jerry.x + 1 == tom.x + 2 && jerry.y == tom.y + 3)) {
    puts("Collision detect - Tom Won");
    game_screen_state = TOM_WON;
    clear_screen_display();
  }

  // Tom left  collide with Jerry cordinates
  else if ((jerry.x == tom.x + 2 && jerry.y == tom.y + 1) ||
           (jerry.x + 1 == tom.x + 2 && jerry.y + 1 == tom.y + 1) ||
           (jerry.x + 2 == tom.x + 2 && jerry.y == tom.y + 1) ||
           (jerry.x + 1 == tom.x + 2 && jerry.y == tom.y + 1)) {
    puts("Collision detect - Tom Won");
    game_screen_state = TOM_WON;
    clear_screen_display();
  }
  // Tom top collide with Jerry cordinates
  else if ((jerry.x == tom.x + 1 && jerry.y == tom.y + 2) ||
           (jerry.x + 1 == tom.x + 1 && jerry.y + 1 == tom.y + 2) ||
           (jerry.x + 2 == tom.x + 1 && jerry.y == tom.y + 2) ||
           (jerry.x + 1 == tom.x + 1 && jerry.y == tom.y + 2)) {
    puts("Collision detect - Tom Won");
    game_screen_state = TOM_WON;
    clear_screen_display();
  }
}

void player_failed(void) {
  switch (level) {
  case 0:
    if ((jerry.x == 26) && (jerry.y == 56)) {
      puts("Jerry reached home - Jerry Won");
      jerry.x = 16;
      jerry.y = 2;
      change_level = true;
      game_screen_state = JERRY_WON;
      clear_screen_display();
    }
    break;
  case 1:
    if ((jerry.x == 2) && (jerry.y == 54)) {
      puts("Jerry reached home - Jerry Won");
      jerry.x = 19;
      jerry.y = 2;
      change_level = true;
      game_screen_state = JERRY_WON;
      clear_screen_display();
    }
    break;
  case 2:
    if ((jerry.x == 27) && (jerry.y == 38)) {
      puts("Jerry reached home - Jerry Won");
      jerry.x = 14;
      jerry.y = 2;
      change_level = true;
      game_screen_state = JERRY_WON;
      clear_screen_display();
    }
    break;

  default:

    break;
  }
}
