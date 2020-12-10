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
#include "semphr.h"

//#define TEST
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

bool jerry_wins = false;
extern SemaphoreHandle_t button_pressed_signal;
extern SemaphoreHandle_t change_game_state;
extern SemaphoreHandle_t default_sound;
extern SemaphoreHandle_t game_sound;
extern SemaphoreHandle_t catchsuccess_sound;
extern SemaphoreHandle_t catchfail_sound;
extern SemaphoreHandle_t score_sound;

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
        game_screen_state = GAME_ON;
        change_state = false;
      }

      break;

    case GAME_ON:
#ifdef TEST
      puts("GAME_SCREEN");
#endif

      maze_one_frame();
      game_on = true;
      tom_image(row_count, col_count);
      xSemaphoreGive(game_sound);
      collision_detector();
      player_failed();

      if (change_state) {
        game_screen_state = PAUSE_PLEASE;
        change_state = false;
        clear_screen_display();
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
        game_screen_state = GAME_ON;
        change_state = false;
        clear_screen_display();
      }
      break;

    case TOMWON:
#ifdef TEST
      puts("TOMWON");
#endif
      // clear_screen_display();
      tom_won_display();
      // Call function for led_matrix TOM-WON screen here.
      jerry_wins = true;
      xSemaphoreGive(catchsuccess_sound);
      if (change_state) {
        change_state = false;
        game_screen_state = START_SCREEN;
        clear_screen_display();
      }
      break;

    case JERRYWON:
#ifdef TEST
      puts("JERRYWON");
#endif
      jerry_wins = true;
      jerry_won_display();
      xSemaphoreGive(catchfail_sound);
      if (change_state) {
        change_state = false;
        game_screen_state = START_SCREEN;
        clear_screen_display();
      }

      break;

    case SCORECARD:
#ifdef TEST
      puts("SCORECARD");
#endif
      xSemaphoreGive(game_sound);
      if (change_state) {
        game_screen_state = START_SCREEN;
        change_state = false;
      }
      break;

    default:
      puts("DEFAULT");
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
  if (maze_one_lookup_table[jerry.x][jerry.y] == 60) {
    puts("Jerry reached home - Jerry Won");
    game_screen_state = JERRYWON;
    clear_screen_display();
  }
}
