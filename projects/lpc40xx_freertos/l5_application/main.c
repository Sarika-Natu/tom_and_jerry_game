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

// #define TEST

SemaphoreHandle_t mp3_mutex = NULL;
QueueHandle_t mp3_queue = NULL;
xTaskHandle jerry_motion_suspend;

SemaphoreHandle_t button_pressed_signal = NULL;
SemaphoreHandle_t change_game_state = NULL;
SemaphoreHandle_t movement_counter = NULL;

void game_task(void *p);
void RGB_task(void *params);
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
  xTaskCreate(game_task, "game_task", (512U * 4) / sizeof(void *), (void *)NULL,
              PRIORITY_MEDIUM, NULL);
  xTaskCreate(button_task, "button_task", (512U * 4) / sizeof(void *),
              (void *)NULL, PRIORITY_LOW, NULL);
  xTaskCreate(read_song, "read_song", (512U * 10) / sizeof(void *),
              (void *)NULL, PRIORITY_LOW, NULL);
  xTaskCreate(play_song, "play_song", (512U * 4) / sizeof(void *), (void *)NULL,
              PRIORITY_LOW, NULL);

  vTaskStartScheduler();

  return 0;
}
