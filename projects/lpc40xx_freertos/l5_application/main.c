#include "FreeRTOS.h"
#include "board_io.h"
#include "common_macros.h"
#include "delay.h"
#include "display_screen_RGB.h"
#include "game_accelerometer.h"
#include "game_logic.h"
#include "led_matrix.h"
#include "mp3_decoder.h"
#include "periodic_scheduler.h"
#include "queue.h"
#include "semphr.h"
#include "sj2_cli.h"
#include "task.h"

#define READ_BYTES_FROM_FILE 512U
#define MAX_BYTES_TX 32U
#define JERRY_START_POSITION 12U

// #define TEST

extern struct object_axis tom;

SemaphoreHandle_t mp3_mutex = NULL;
QueueHandle_t mp3_queue = NULL;
xTaskHandle jerry_motion_suspend;

SemaphoreHandle_t button_pressed_signal = NULL;
SemaphoreHandle_t change_game_state = NULL;
SemaphoreHandle_t movement_counter = NULL;

static void RGB_task(void *params);
void button_task(void *p);
void read_song(void *p);
void play_song(void *p);

int main(void) {

  printf("game starts here");
  movement_counter = xSemaphoreCreateMutex();
  mp3_mutex = xSemaphoreCreateMutex();
  mp3_queue = xQueueCreate(1, sizeof(uint8_t[READ_BYTES_FROM_FILE]));

  gpio_init();
  clear_display();
  acceleration__init();
  setup_button_isr();
  mp3_init();
  xTaskCreate(RGB_task, "RGB_task", 4096 / (sizeof(void *)), NULL,
              PRIORITY_HIGH, NULL);
  xTaskCreate(jerry_motion, "jerry_motion", 4096 / (sizeof(void *)), NULL,
              PRIORITY_MEDIUM, &jerry_motion_suspend);

  xTaskCreate(action_on_orientation, "Performing_Action",
              4096 / (sizeof(void *)), NULL, PRIORITY_LOW, NULL);

  /***************** Game Logic ***************************/
  xTaskCreate(game_task, "game_task", (512U * 4) / sizeof(void *), (void *)NULL,
              PRIORITY_MEDIUM, NULL);
  xTaskCreate(button_task, "button_task", (512U * 4) / sizeof(void *),
              (void *)NULL, PRIORITY_LOW, NULL);

  /***************** MP3 DECODER***************************/

  xTaskCreate(read_song, "read_song", (512U * 10) / sizeof(void *),
              (void *)NULL, PRIORITY_LOW, NULL);
  xTaskCreate(play_song, "play_song", (512U * 4) / sizeof(void *), (void *)NULL,
              PRIORITY_LOW, NULL);
  /*********************************************************/

  vTaskStartScheduler();

  return 0;
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
