#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

//#include "acceleration.h"
#include "board_io.h"
#include "common_macros.h"
#include "delay.h"
#include "ff.h"
#include "game_accelerometer.h"
#include "gpio.h"
#include "i2c.h"
#include "led_matrix.h"
#include "mp3_decoder.h"
#include "periodic_scheduler.h"
#include "queue.h"
#include "semphr.h"
#include "sj2_cli.h"

#define CODE
#define READ_BYTES_FROM_FILE 512U
#define MAX_BYTES_TX 32U

static SemaphoreHandle_t mp3_mutex = NULL;
static QueueHandle_t mp3_queue = NULL;

void action_on_orientation(void *p);
SemaphoreHandle_t movement_counter = NULL;
void left_movement(void);
void right_movement(void);
void up_movement(void);
void down_movement(void);
volatile uint8_t xcount = 0;
volatile uint8_t ycount = 0;

static void RGB_task(void *params);
static void RGB_frame(void *params);
void read_song(void *p);
void play_song(void *p);

int main(void) {
  // mp3_init();
  movement_counter = xSemaphoreCreateMutex();

  gpio_init();
  // mp3_mutex = xSemaphoreCreateMutex();
  // mp3_queue = xQueueCreate(1, sizeof(uint8_t[READ_BYTES_FROM_FILE]));

  // xTaskCreate(RGB_frame, "RGB_task", 4096, NULL, PRIORITY_HIGH, NULL);
  // xTaskCreate(RGB_task, "RGB_task", 4096, NULL, PRIORITY_HIGH, NULL);
  // xTaskCreate(read_song, "read_song", (512U * 8) / sizeof(void *), (void
  // *)NULL, PRIORITY_LOW, NULL);
  // xTaskCreate(play_song, "play_song", (512U * 4) / sizeof(void
  // *), (void *)NULL, PRIORITY_HIGH, NULL);

  xTaskCreate(action_on_orientation, "Performing_Action",
              4096 / (sizeof(void *)), NULL, PRIORITY_HIGH, NULL);

  puts("Starting RTOS");
  vTaskStartScheduler(); // This function never returns unless RTOS
                         // scheduler runs out of memory and fails

  return 0;
}

static void RGB_frame(void *params) {
  gpio_init();
  while (1) {
    // display_maze_frame1();
    maze_one_frame();
  }
}

void RGB_task(void *params) {
  while (1) {
    for (uint8_t x = 0; x < 23; x++) {
      for (uint8_t y = 0; y < 50; y++) {
        // display_moving_point(25, 3, RED);
        tom_image(x, y);
        vTaskDelay(25);
        tom_clear_image(x, y);
      }
    }
  }
}

void read_song(void *p) {
  const char *filename = "RangDeBasanti.mp3";
  static uint8_t bytes_to_read[READ_BYTES_FROM_FILE];
  FRESULT result;
  FIL file;

  result = f_open(&file, filename, FA_OPEN_EXISTING | FA_READ);
  UINT bytes_read;
  while (1) {
    xSemaphoreTake(mp3_mutex, portMAX_DELAY);

    result =
        f_read(&file, &bytes_to_read[0], READ_BYTES_FROM_FILE, &bytes_read);
    if (0 != result) {
      printf("Result of %s is %i\n", filename, result);
    }

    xSemaphoreGive(mp3_mutex);
    xQueueSend(mp3_queue, &bytes_to_read[0], portMAX_DELAY);
  }
}

void play_song(void *p) {
  static uint8_t bytes_to_read[READ_BYTES_FROM_FILE];
  static uint8_t current_count = 0;
  uint32_t start_index = 0;
  while (1) {

    if (current_count == 0) {
      xQueueReceive(mp3_queue, &bytes_to_read[0], portMAX_DELAY);
    }
    start_index = (current_count * MAX_BYTES_TX);

    while (!mp3_dreq_get_status()) {
#ifdef TEST
      printf("data not requested\n");
#endif
      vTaskDelay(2);
    }
    if (xSemaphoreTake(mp3_mutex, portMAX_DELAY)) {

      send_bytes_to_decoder(start_index, &bytes_to_read[0]);
      xSemaphoreGive(mp3_mutex);
      if (current_count == (READ_BYTES_FROM_FILE / MAX_BYTES_TX) - 1) {
        current_count = 0;
      } else {
        current_count += 1;
#ifdef TEST
        printf("count = %d\n", current_count);
#endif
      }
    }
  }
}

void action_on_orientation(void *p) {
  orientation_e value;
  // uint8_t tom_movement;

  while (1) {
    value = acceleration_get_data();
    switch (value) {
    case Landscape_LEFT:
      left_movement();
      break;
    case Landscape_RIGHT:
      right_movement();
      break;
    case Back_Orientation:
      printf("Back orientation\n\n");
      break;
    case Front_Orientation:
      printf("Front orientation\n\n");
      break;
    case Portrait_DOWN:
      down_movement();
      break;
    default:
      up_movement();
      break;
    }
    vTaskDelay(100);
  }
  // return tom_movement;
}

void left_movement(void) {
  if (xSemaphoreTake(movement_counter, portMAX_DELAY)) {
    fprintf(stderr, "Direction LEFT = %d\n\n", xcount--);
  }
  vTaskDelay(100);
  xSemaphoreGive(movement_counter);
}

void right_movement(void) {
  if (xSemaphoreTake(movement_counter, portMAX_DELAY)) {
    fprintf(stderr, "Direction RIGHT = %d\n\n", xcount++);
  }
  vTaskDelay(100);
  xSemaphoreGive(movement_counter);
}
void down_movement(void) {
  if (xSemaphoreTake(movement_counter, portMAX_DELAY)) {
    fprintf(stderr, "Direction DOWN = %d\n\n", ycount--);
  }
  vTaskDelay(100);
  xSemaphoreGive(movement_counter);
}

void up_movement(void) {
  if (xSemaphoreTake(movement_counter, portMAX_DELAY)) {
    fprintf(stderr, "Direction UP = %d\n\n", ycount++);
  }
  vTaskDelay(100);
  xSemaphoreGive(movement_counter);
}