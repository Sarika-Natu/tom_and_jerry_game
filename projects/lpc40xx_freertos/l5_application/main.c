#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "board_io.h"
#include "common_macros.h"
#include "delay.h"
#include "gpio.h"
#include "led_matrix.h"
#include "periodic_scheduler.h"
#include "sj2_cli.h"

//#include "accelerometer_orientation.h"
#include "ff.h"
#include "i2c.h"
#include "mp3_decoder.h"
#include "queue.h"
#include "semphr.h"

#define READ_BYTES_FROM_FILE 512U
#define MAX_BYTES_TX 32U
//#define TEST
static SemaphoreHandle_t mp3_mutex = NULL;
static QueueHandle_t mp3_queue = NULL;

typedef enum {
  Front_Orientation = 0,
  Back_Orientation = 1,
  Portrait_DOWN = 2,
  Landscape_RIGHT = 4,
  Landscape_LEFT = 6,
} orientation_e;

typedef enum {
  acceleration__PL_status_address = 0x10,
  acceleration__PL_CFG_address = 0x11,
  acceleration__PL_count_address = 0x12,
  acceleration__memory_control = 0x2A,
  acceleration__Stand_by = 254,
} acceleration__orientation;

static const i2c_e acceleration__sensor_bus = I2C__2;
static const uint8_t acceleration__address = 0x38;
uint8_t enable_orientation_detection = 192;
uint8_t decrement_debounce_counter = 5;

void enable_orientation(void);
void action_on_orientation(void *p);
orientation_e acceleration_get_data(void);
static void RGB_task(void *params);
static void RGB_frame(void *params);

int main(void) {

  // mp3_init();

  // mp3_mutex = xSemaphoreCreateMutex();
  // mp3_queue = xQueueCreate(1, sizeof(uint8_t[READ_BYTES_FROM_FILE]));
  // const uint8_t active_mode_with_100Hz = (1 << 0) | (1 << 5);

  // i2c__write_single(acceleration__sensor_bus, acceleration__address,
  //                   acceleration__memory_control, acceleration__Stand_by);

  // enable_orientation();

  // i2c__write_single(acceleration__sensor_bus, acceleration__address,
  //                   acceleration__memory_control, active_mode_with_100Hz);

  xTaskCreate(RGB_frame, "RGB_task", 4096, NULL, PRIORITY_HIGH, NULL);
  xTaskCreate(RGB_task, "RGB_task", 4096, NULL, PRIORITY_HIGH, NULL);
  // xTaskCreate(read_song, "read_song", (512U * 8) / sizeof(void *), (void
  // *)NULL,PRIORITY_LOW, NULL);
  // xTaskCreate(play_song, "play_song", (512U * 4) / sizeof(void
  // *), (void *)NULL,PRIORITY_HIGH, NULL);
  // xTaskCreate(action_on_orientation, "Performing_Action",4096 / (sizeof(void
  // *)), NULL, PRIORITY_LOW, NULL);
  puts("Starting RTOS");
  vTaskStartScheduler(); // This function never returns unless RTOS
                         // scheduler runs out of memory and fails

  return 0;
}
uint8_t x = 0;
static void RGB_frame(void *params) {

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
    // display_maze();
    // display_image();
    // display_acc_value(x);
    // game_frame();
    // display_rectangle_cyan();
    // display_rectangle_cyan_small();
    // display_rectangle_cyan_big();

    // final logic
    display_maze_frame1();

    // display_up_arrow();
    // display_moving_point(27, 2, RED);
    // display_moving_point(1, 2, BLUE);

    // display_moving_point(6, 7, RED);
    // x++;
    // update_display();
    // set_pixel(4, 6, PINK);
    // update_display();
  }
}

void RGB_task(void *params) {
  while (1) {
    display_moving_point(25, 3, RED);
    // display_moving_point(24, 15, RED);
    // vTaskDelay(2000);
    // fprintf(stderr, "in");
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

void enable_orientation(void) {
  i2c__write_single(acceleration__sensor_bus, acceleration__address,
                    acceleration__PL_CFG_address, enable_orientation_detection);

  i2c__write_single(acceleration__sensor_bus, acceleration__address,
                    acceleration__PL_count_address, decrement_debounce_counter);
}

orientation_e acceleration_get_data(void) {
  orientation_e orientation_status;

  uint8_t PL_status_reg =
      i2c__read_single(acceleration__sensor_bus, acceleration__address,
                       acceleration__PL_status_address);

  printf("STATUS REG AFTER READING = 0x%x\n", PL_status_reg);

  if (PL_status_reg & (~(1 << 0)) & (~(1 << 1)) & (~(1 << 2))) {
    orientation_status = 0; // FRONT
  } else if (PL_status_reg == 2) {
    orientation_status = 2; // DOWN
  } else if (PL_status_reg == 4) {
    orientation_status = 4; // RIGHT
  } else if (PL_status_reg == 6) {
    orientation_status = 6; // LEFT
  } else if (PL_status_reg & (1 << 0)) {
    orientation_status = 1; // BACK
  } else {
    orientation_status = 0xA;
  }
  printf("STATUS = %d\n", orientation_status);
  return orientation_status;
}

void action_on_orientation(void *p) {
  orientation_e value;

  while (1) {
    value = acceleration_get_data();
    switch (value) {
    case Landscape_LEFT:
      printf("Direction LEFT\n\n");
      break;
    case Landscape_RIGHT:
      printf("Direction RIGHT\n\n");
      break;
    case Back_Orientation:
      printf("Back Orientation\n\n");
      break;
    case Front_Orientation:
      printf("Front orientation\n\n");
      break;
    case Portrait_DOWN:
      printf("Down orientation\n\n");
      break;
    default:
      printf("Incorrect Orientation\n\n");
      break;
    }
    vTaskDelay(100);
  }
}
