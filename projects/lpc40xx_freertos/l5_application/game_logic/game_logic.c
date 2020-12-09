#include "game_logic.h"
#include "FreeRTOS.h"
#include "delay.h"
#include "display_screen_RGB.h"
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
uint8_t curr_game_state = START_SCREEN;

extern SemaphoreHandle_t button_pressed_signal;
extern SemaphoreHandle_t change_game_state;
extern SemaphoreHandle_t default_sound;
extern SemaphoreHandle_t game_sound;
extern SemaphoreHandle_t catchsuccess_sound;
extern SemaphoreHandle_t catchfail_sound;
extern SemaphoreHandle_t score_sound;

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
        game_screen_state = GAME_ON;
        change_state = false;
        clear_screen_display();
      }
      break;

    case GAME_ON:
#ifdef TEST
      puts("GAME_SCREEN");
#endif
      maze_one_frame();
      xSemaphoreGive(game_sound);
      if (change_state) {
        game_screen_state = PAUSE_PLEASE;
        change_state = false;
        clear_screen_display();
      }
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
      tom_won_display();
      xSemaphoreGive(catchsuccess_sound);
      if (change_state) {
        change_state = false;
        clear_screen_display();
        game_screen_state = SCORECARD;
      }
      break;

    case JERRYWON:
#ifdef TEST
      puts("JERRYWON");
#endif
      jerry_won_display();
      xSemaphoreGive(catchfail_sound);
      if (change_state) {
        change_state = false;
        clear_screen_display();
        game_screen_state = SCORECARD;
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
