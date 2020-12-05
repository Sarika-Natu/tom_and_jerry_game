#include "FreeRTOS.h"
#include "board_io.h"
#include "common_macros.h"
#include "periodic_scheduler.h"
#include "semphr.h"
#include "sj2_cli.h"
#include "task.h"
#include <stdio.h>

#define READ_BYTES_FROM_FILE 512U
//#define TEST
SemaphoreHandle_t mp3_mutex = NULL;
SemaphoreHandle_t default_sound = NULL;
SemaphoreHandle_t game_sound = NULL;
SemaphoreHandle_t catchsuccess_sound = NULL;
SemaphoreHandle_t catchfail_sound = NULL;
SemaphoreHandle_t score_sound = NULL;
QueueHandle_t mp3_queue = NULL;

SemaphoreHandle_t button_pressed_signal = NULL;
SemaphoreHandle_t change_game_state = NULL;

void game_task(void *p);
void button_task(void *p);
void read_song(void *p);
void play_song(void *p);

int main(void) {

  mp3_init();
  setup_button_isr();
  mp3_mutex = xSemaphoreCreateMutex();
  default_sound = xSemaphoreCreateBinary();
  game_sound = xSemaphoreCreateBinary();
  catchsuccess_sound = xSemaphoreCreateBinary();
  catchfail_sound = xSemaphoreCreateBinary();
  score_sound = xSemaphoreCreateBinary();
  mp3_queue = xQueueCreate(1, sizeof(uint8_t[READ_BYTES_FROM_FILE]));

  xTaskCreate(game_task, "game_task", (512U * 4) / sizeof(void *), (void *)NULL,
              PRIORITY_LOW, NULL);
  xTaskCreate(button_task, "button_task", (512U * 4) / sizeof(void *),
              (void *)NULL, PRIORITY_LOW, NULL);
  xTaskCreate(read_song, "read_song", (512U * 10) / sizeof(void *),
              (void *)NULL, PRIORITY_LOW, NULL);
  xTaskCreate(play_song, "play_song", (512U * 4) / sizeof(void *), (void *)NULL,
              PRIORITY_LOW, NULL);

  puts("Starting RTOS");
  vTaskStartScheduler(); // This function never returns unless RTOS scheduler
                         // runs out of memory and fails

  return 0;
}