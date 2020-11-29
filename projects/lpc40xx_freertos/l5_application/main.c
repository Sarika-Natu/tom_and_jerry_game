#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "acceleration.h"
#include "board_io.h"
#include "common_macros.h"
#include "delay.h"
#include "gpio.h"
#include "led_matrix.h"
#include "periodic_scheduler.h"
#include "sj2_cli.h"

#include "ff.h"
#include "mp3_decoder.h"
#include "queue.h"
#include "semphr.h"

//#define CODE
#define READ_BYTES_FROM_FILE 512U
#define MAX_BYTES_TX 32U
//#define TEST
static SemaphoreHandle_t mp3_mutex = NULL;
static QueueHandle_t mp3_queue = NULL;
void action_on_orientation(void *p);

static void RGB_task(void *params);
void read_song(void *p);
void play_song(void *p);

int main(void) {

#ifdef CODE
  mp3_init();

  mp3_mutex = xSemaphoreCreateMutex();
  mp3_queue = xQueueCreate(1, sizeof(uint8_t[READ_BYTES_FROM_FILE]));
  xTaskCreate(RGB_task, "RGB_task", 4096, NULL, PRIORITY_HIGH, NULL);
  // xTaskCreate(read_song, "read_song", (512U * 8) / sizeof(void *), (void
  // *)NULL,PRIORITY_LOW, NULL);
  // xTaskCreate(play_song, "play_song", (512U * 4) / sizeof(void
  // *), (void *)NULL,      PRIORITY_HIGH, NULL);

#endif
  xTaskCreate(action_on_orientation, "Performing_Action",
              4096 / (sizeof(void *)), NULL, PRIORITY_LOW, NULL);

  puts("Starting RTOS");
  vTaskStartScheduler(); // This function never returns unless RTOS
                         // scheduler runs out of memory and fails

  return 0;
}

void action_on_orientation(void *p) {
  orientation_e value;

  while (1) {
    value = acceleration_get_data();
    switch (value) {
    case Landscape_LEFT:
      printf("TURN LEFT\n\n");
      break;
    case Landscape_RIGHT:
      printf("TURN RIGHT\n\n");
      break;
    case Back_Orientation:
      printf("Back Orientation\n");
      break;
    case Front_Orientation:
      printf("STABLE ORIENTATION\n\n");
      break;
    case Portrait_DOWN:
      printf("MOVE FORWARD\n\n");
      break;
    default:
      printf("STEP BACK\n\n");
      break;
    }
    vTaskDelay(100);
  }
}

static void RGB_task(void *params) {

  // uint8_t LEDMATRIX_HALF_HEIGHT = 32;
  // uint8_t LEDMATRIX_WIDTH = 64;
  gpio_init();
  // clear_display();
  while (1) {
    // update_display();
    // display_rectangle_pink();
    // update_display();
    // display_rectangle_red();
    // display_moving_image(2, 29);
    // move_rectangle();
    display_maze();
    display_image();
    // update_display();
    // set_pixel(4, 6, PINK);
    // update_display();
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
