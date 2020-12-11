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
#include "semphr.h"

// #define TEST
enum game_state {
  START_SCREEN = 0,
  GAME_ON = 1,
  PAUSE_PLEASE = 2,
  TOMWON = 3,
  JERRYWON = 4,
  SCORECARD = 5
};
uint8_t game_screen_state = START_SCREEN;
bool game_on = false;

bool change_level = false;
extern SemaphoreHandle_t button_pressed_signal;
extern SemaphoreHandle_t change_game_state;
extern SemaphoreHandle_t default_sound;
extern SemaphoreHandle_t game_sound;
extern SemaphoreHandle_t catchsuccess_sound;
extern SemaphoreHandle_t catchfail_sound;
extern SemaphoreHandle_t score_sound;

uint8_t const jerry_start_position = 12;
uint8_t const jerry_end_position_1 = 60;
uint8_t const jerry_end_position_2 = 80;
uint8_t const jerry_end_position_3 = 89;
uint8_t const jerry_end_position_4 = 85;
uint8_t const jerry_end_position_5 = 56;

uint8_t previous_game_mode = 0;
uint8_t const jerry_end_positions[5] = {
    jerry_end_position_1, jerry_end_position_2, jerry_end_position_3,
    jerry_end_position_4, jerry_end_position_5};
uint8_t level = 0;

maze_selection_t maze_lookup_table[] = {maze_one_frame, maze_two_frame,
                                        maze_three_frame, maze_four_frame,
                                        maze_one_frame};

maze_selection_t display_game_level[] = {level_one_display, level_one_display,
                                         level_one_display, level_one_display,
                                         level_one_display};
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
      xSemaphoreGive(default_sound);
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
      // if (previous_game_mode == JERRYWON) {
      //   game_mode_on();
      //   clear_screen_display();
      // }
      maze_lookup_table[level]();

      change_level = false;
      game_on = true;
      tom_image(row_count, col_count);
      xSemaphoreGive(game_sound);
      collision_detector();
      player_failed();

      if (change_state) {
        change_state = false;
        clear_screen_display();
        game_screen_state = PAUSE_PLEASE;
      }
#ifdef TEST
      puts("GAME_SCREEN_EXIT");
#endif
      break;

    case PAUSE_PLEASE:
#ifdef TEST
      puts("PAUSE_SCREEN");
#endif
      pause_screen_display();
      xSemaphoreGive(default_sound);
      if (change_state) {
        change_state = false;
        clear_screen_display();
        game_screen_state = GAME_ON;
      }
      break;

    case TOMWON:
#ifdef TEST
      puts("TOMWON");
#endif
      tom_won_display();
      // Call function for led_matrix TOM-WON screen here.
      change_level = true;
      if (level < 3) {
        level++;
        row_count = 1;
        col_count = 1;
        clear_screen_display();
        game_screen_state = GAME_ON;

      } else {
        clear_screen_display();
        game_screen_state = SCORECARD;
      }
      xSemaphoreGive(catchsuccess_sound);

      // if (change_state) {
      //   change_state = false;
      //   game_screen_state = START_SCREEN;
      //   clear_screen_display();
      // }

      break;

    case JERRYWON:
#ifdef TEST
      puts("JERRYWON");
#endif
      change_level = true;
      jerry_won_display();
      xSemaphoreGive(catchfail_sound);
      if (change_state) {
        previous_game_mode = JERRYWON;
        change_state = false;
        clear_screen_display();
        game_screen_state = START_SCREEN;
      }

      break;

    case SCORECARD:
#ifdef TEST
      puts("SCORECARD");
#endif
      tom_won_display();
      xSemaphoreGive(game_sound);
      if (change_state) {
        change_state = false;
        game_screen_state = START_SCREEN;
      }
      break;

    default:
      clear_screen_display();
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
  // setup_button_isr();

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
    game_screen_state = TOMWON;
    clear_screen_display();
  }
  // Tom right collide with Jerry cordinates
  else if ((jerry.x == tom.x + 2 && jerry.y == tom.y + 3) ||
           (jerry.x + 1 == tom.x + 2 && jerry.y + 1 == tom.y + 3) ||
           (jerry.x + 2 == tom.x + 2 && jerry.y == tom.y + 3) ||
           (jerry.x + 1 == tom.x + 2 && jerry.y == tom.y + 3)) {
    puts("Collision detect - Tom Won");
    game_screen_state = TOMWON;
    clear_screen_display();
  }

  // Tom left  collide with Jerry cordinates
  else if ((jerry.x == tom.x + 2 && jerry.y == tom.y + 1) ||
           (jerry.x + 1 == tom.x + 2 && jerry.y + 1 == tom.y + 1) ||
           (jerry.x + 2 == tom.x + 2 && jerry.y == tom.y + 1) ||
           (jerry.x + 1 == tom.x + 2 && jerry.y == tom.y + 1)) {
    puts("Collision detect - Tom Won");
    game_screen_state = TOMWON;
    clear_screen_display();
  }
  // Tom top collide with Jerry cordinates
  else if ((jerry.x == tom.x + 1 && jerry.y == tom.y + 2) ||
           (jerry.x + 1 == tom.x + 1 && jerry.y + 1 == tom.y + 2) ||
           (jerry.x + 2 == tom.x + 1 && jerry.y == tom.y + 2) ||
           (jerry.x + 1 == tom.x + 1 && jerry.y == tom.y + 2)) {
    puts("Collision detect - Tom Won");
    game_screen_state = TOMWON;
    clear_screen_display();
  }
}

void player_failed(void) {
  switch (level) {
  case 0:
    if (maze_one_lookup_table[jerry.x][jerry.y] == jerry_end_positions[0]) {
      puts("Jerry reached home - Jerry Won");
      game_screen_state = JERRYWON;
      clear_screen_display();
    }
    break;
  case 1:
    if (maze_two_lookup_table[jerry.x][jerry.y] == jerry_end_positions[1]) {
      puts("Jerry reached home - Jerry Won");
      game_screen_state = JERRYWON;
      clear_screen_display();
    }
    break;
  case 2:
    if (maze_three_lookup_table[jerry.x][jerry.y] == jerry_end_positions[2]) {
      puts("Jerry reached home - Jerry Won");
      game_screen_state = JERRYWON;
      clear_screen_display();
    }
    break;

  default:
    puts("No case");
    break;
  }
}

void game_mode_on(void) { display_game_level[level](); }