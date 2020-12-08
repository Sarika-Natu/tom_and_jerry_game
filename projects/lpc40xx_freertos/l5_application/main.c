#include "FreeRTOS.h"
#include "game_logic.h"
#include "task.h"
//#include "acceleration.h"
#include "board_io.h"
#include "common_macros.h"
#include "delay.h"
#include "display_screen_RGB.h"
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

bool command_up = true;
bool command_down = true;
bool command_left = true;
bool command_right = true;

void action_on_orientation(void *p);
SemaphoreHandle_t movement_counter = NULL;
SemaphoreHandle_t rgb_owner = NULL;
void left_movement(void);
void right_movement(void);
void up_movement(void);
void down_movement(void);
uint8_t col_count = 0;
uint8_t row_count = 0;

static void RGB_task(void *params);
static void RGB_frame(void *params);

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

// void game_task(void *p);
void button_task(void *p);
void read_song(void *p);
void play_song(void *p);
void RGB_clear(void *p);

void RGB_task_2(void *params);
void tom_motion_update(void *params);

int main(void) {

  movement_counter = xSemaphoreCreateMutex();
  mp3_mutex = xSemaphoreCreateMutex();
  default_sound = xSemaphoreCreateBinary();
  game_sound = xSemaphoreCreateBinary();
  catchsuccess_sound = xSemaphoreCreateBinary();
  catchfail_sound = xSemaphoreCreateBinary();
  score_sound = xSemaphoreCreateBinary();
  mp3_queue = xQueueCreate(1, sizeof(uint8_t[READ_BYTES_FROM_FILE]));

  gpio_init();
  clear_display();
  acceleration__init();
  setup_button_isr();

  xTaskCreate(RGB_task, "RGB_task", 4096 / (sizeof(void *)), NULL,
              PRIORITY_HIGH, NULL);

  // xTaskCreate(RGB_task_2, "Tom_Move", 4096 / (sizeof(void *)),
  // NULL,PRIORITY_MEDIUM, NULL);

  // xTaskCreate(tom_motion_update, "tom_motion_update", 4096 / (sizeof(void
  // *)),     NULL, PRIORITY_MEDIUM, NULL);
  // xTaskCreate(action_on_orientation, "Performing_Action",4096 / (sizeof(void
  // *)), NULL, PRIORITY_MEDIUM, NULL);

  /***************** Game Logic ***************************/
  xTaskCreate(game_task, "game_task", (512U * 4) / sizeof(void *), (void *)NULL,
              PRIORITY_MEDIUM, NULL);
  xTaskCreate(button_task, "button_task", (512U * 4) / sizeof(void *),
              (void *)NULL, PRIORITY_LOW, NULL);

  /***************** MP3 DECODER***************************/
  mp3_init();
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
  }
}

void RGB_task_2(void *params) {

  while (1) {

    tom_image(row_count, col_count);
    vTaskDelay(1);
  }
}

/*******************************************************************/
/*                       ACCELEROMETER                             */
/*******************************************************************/
void action_on_orientation(void *p) {
  orientation_e value;

  while (1) {
    value = acceleration_get_data();

    switch (value) {
    case Landscape_LEFT:
      command_left = true;
      if (left_move) {
        left_movement();
      }
      left_move = true;
      break;
    case Landscape_RIGHT:
      command_right = true;
      if (right_move) {
        right_movement();
      }
      right_move = true;
      break;
    case Back_Orientation:

      break;
    case Front_Orientation:

      break;
    case Portrait_DOWN:
      command_down = true;
      if (down_move) {
        down_movement();
      }
      down_move = true;
      break;
    default:
      command_up = true;
      if (up_move) {
        up_movement();
      }
      up_move = true;
      break;
    }
    vTaskDelay(100);
    tom_move_on_maze(row_count, col_count);
  }
}

void left_movement(void) {
  if (xSemaphoreTake(movement_counter, portMAX_DELAY)) {

    col_count--;
  }

  xSemaphoreGive(movement_counter);
}

void right_movement(void) {
  if (xSemaphoreTake(movement_counter, portMAX_DELAY)) {

    col_count++;
  }

  xSemaphoreGive(movement_counter);
}
void down_movement(void) {
  if (xSemaphoreTake(movement_counter, portMAX_DELAY)) {

    row_count++;
  }

  xSemaphoreGive(movement_counter);
}

void up_movement(void) {
  if (xSemaphoreTake(movement_counter, portMAX_DELAY)) {

    row_count--;
  }

  xSemaphoreGive(movement_counter);
}
/*******************************************************************/
