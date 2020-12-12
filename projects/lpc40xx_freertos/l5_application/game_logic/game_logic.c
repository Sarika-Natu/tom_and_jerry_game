#include "game_logic.h"
#include "FreeRTOS.h"
#include "delay.h"
#include "display_screen_RGB.h"
#include "game_accelerometer.h"
#include "gpio.h"
#include "gpio_isr.h"
#include "led_matrix.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"
#include "maze.h"
#include "mp3_decoder.h"
#include "semphr.h"

#define THREE_LIVES 3
#define GAME_LEVELS 3

// #define TEST
enum game_state {
  START_SCREEN = 0,
  GAME_ON = 1,
  PAUSE_PLEASE = 2,
  TOMWON = 3,
  JERRYWON = 4,
  SCORECARD = 5
};

maze maze1 = {{16, 2}, {26, 56}};
maze maze2 = {{19, 2}, {2, 54}};
maze maze3 = {{14, 2}, {27, 38}};

uint8_t game_screen_state = START_SCREEN;
bool game_on = false;
bool pause_or_stop = false;
bool game_on_after_pause = false;
bool change_level = false;

extern SemaphoreHandle_t button_pressed_signal;
extern SemaphoreHandle_t change_game_state;

uint8_t level = 0;
uint8_t tom_lives = THREE_LIVES;
maze_selection_t maze_lookup_table[3] = {maze_one_frame, maze_two_frame,
                                         maze_three_frame};

static void collision_detector(void);
static void player_failed(void);

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
        game_screen_state = GAME_ON;
      }

      break;

    case GAME_ON:
#ifdef TEST
      puts("GAME_SCREEN");
      fprintf(stderr, "Jerry.x = %d | Jerry.y =%d | motion_counter=%d \n",
              jerry.x, jerry.y, jerry_motion_counter);
#endif
      maze_lookup_table[level]();
      change_level = false;
      // level_display();
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

    case TOMWON:
#ifdef TEST
      puts("TOMWON");
#endif
      tom_lives = THREE_LIVES;
      pause_or_stop = true;
      sound.catchsuccess = true;
      change_level = true;
      if (level < (GAME_LEVELS - 1)) {
        level++;
        row_count = 1;
        col_count = 1;
        clear_screen_display();
        game_screen_state = GAME_ON;
      } else {
        level = 0;
        clear_screen_display();
        game_screen_state = SCORECARD;
      }
      break;

    case JERRYWON:
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
        // tom_lives = 3;
        clear_screen_display();
        game_screen_state = SCORECARD;
      }
      break;

    case SCORECARD:
#ifdef TEST
      puts("SCORECARD");
#endif
      jerry_won_display();
      sound.scorecard = true;
      if (change_state) {
        change_state = false;
        clear_screen_display();
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

void jerry_motion(void *params) {
  while (1) {
    if (game_on) {
      jerry_image();
#ifdef TEST
      fprintf(stderr, "jerry moving\n");
#endif
    } else {
      // printf("jerry not moving\n");
    }
    vTaskDelay(1);
  }
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

static void collision_detector(void) {
  // Tom bottom collide with Jerry coordinates
  if ((jerry.x == tom.x + 3 && jerry.y == tom.y + 2) ||
      (jerry.x + 1 == tom.x + 3 && jerry.y + 1 == tom.y + 2) ||
      (jerry.x + 2 == tom.x + 3 && jerry.y == tom.y + 2) ||
      (jerry.x + 1 == tom.x + 3 && jerry.y == tom.y + 2)) {
#ifdef TEST
    puts("Collision detect - Tom Won");
#endif
    game_screen_state = TOMWON;
    clear_screen_display();
  }
  // Tom right collide with Jerry coordinates
  else if ((jerry.x == tom.x + 2 && jerry.y == tom.y + 3) ||
           (jerry.x + 1 == tom.x + 2 && jerry.y + 1 == tom.y + 3) ||
           (jerry.x + 2 == tom.x + 2 && jerry.y == tom.y + 3) ||
           (jerry.x + 1 == tom.x + 2 && jerry.y == tom.y + 3)) {
#ifdef TEST
    puts("Collision detect - Tom Won");
#endif
    game_screen_state = TOMWON;
    clear_screen_display();
  }

  // Tom left  collide with Jerry coordinates
  else if ((jerry.x == tom.x + 2 && jerry.y == tom.y + 1) ||
           (jerry.x + 1 == tom.x + 2 && jerry.y + 1 == tom.y + 1) ||
           (jerry.x + 2 == tom.x + 2 && jerry.y == tom.y + 1) ||
           (jerry.x + 1 == tom.x + 2 && jerry.y == tom.y + 1)) {
#ifdef TEST
    puts("Collision detect - Tom Won");
#endif
    game_screen_state = TOMWON;
    clear_screen_display();
  }
  // Tom top collide with Jerry coordinates
  else if ((jerry.x == tom.x + 1 && jerry.y == tom.y + 2) ||
           (jerry.x + 1 == tom.x + 1 && jerry.y + 1 == tom.y + 2) ||
           (jerry.x + 2 == tom.x + 1 && jerry.y == tom.y + 2) ||
           (jerry.x + 1 == tom.x + 1 && jerry.y == tom.y + 2)) {
#ifdef TEST
    puts("Collision detect - Tom Won");
#endif
    game_screen_state = TOMWON;
    clear_screen_display();
  }
}

static void player_failed(void) {
  switch (level) {
  case 0:
    if ((jerry.x == maze1.end.x) && (jerry.y == maze1.end.y)) {
#ifdef TEST
      puts("Jerry reached home - Jerry Won");
#endif
      jerry.x = maze1.start.x;
      jerry.y = maze1.start.y;
      change_level = true;
      game_screen_state = JERRYWON;
      clear_screen_display();
    }
    break;
  case 1:
    if ((jerry.x == maze2.end.x) && (jerry.y == maze2.end.y)) {
#ifdef TEST
      puts("Jerry reached home - Jerry Won");
#endif
      jerry.x = maze2.start.x;
      jerry.y = maze2.start.y;
      change_level = true;
      game_screen_state = JERRYWON;
      clear_screen_display();
    }
    break;
  case 2:
    if ((jerry.x == maze3.end.x) && (jerry.y == maze3.end.y)) {
#ifdef TEST
      puts("Jerry reached home - Jerry Won");
#endif
      jerry.x = maze3.start.x;
      jerry.y = maze3.start.y;
      change_level = true;
      game_screen_state = JERRYWON;
      clear_screen_display();
    }
    break;
  default:
    break;
  }
}
