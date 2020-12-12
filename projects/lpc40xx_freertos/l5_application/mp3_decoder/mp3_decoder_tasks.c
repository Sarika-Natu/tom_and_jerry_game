#include "ff.h"
#include "mp3_decoder.h"
#include "queue.h"
#include <stdio.h>

// #define TEST
#ifdef TEST
#include <stdio.h>
#endif
#define READ_BYTES_FROM_FILE 512U
#define MAX_BYTES_TX 32U

extern SemaphoreHandle_t mp3_mutex;
extern QueueHandle_t mp3_queue;

FIL file;
enum music {
  DEFAULT = 0,
  GAME = 1,
  CATCHSUCCESS = 2,
  CATCHFAIL = 3,
  SCORECARD = 4
};

void read_song(void *p) {
  uint8_t current_state = DEFAULT;
  const char *filename = "1.mp3";
  static uint8_t bytes_to_read[READ_BYTES_FROM_FILE];
  FRESULT result;

  DIR dj;             /* Directory search object */
  static FILINFO fno; /* File information */

  result = f_open(&file, filename, FA_OPEN_EXISTING | FA_READ);
  UINT bytes_read;

  while (1) {

    if ((sound.scorecard) && (current_state != SCORECARD)) {
      result = f_close(&file);
      result = f_findfirst(&dj, &fno, "", "5.mp3");
#ifdef TEST
      if (0 != result) {
        printf("Result of song found is %i\n", result);
      }
#endif
      result = f_open(&file, fno.fname, FA_OPEN_EXISTING | FA_READ);
#ifdef TEST
      if (0 != result) {
        printf("Result of open is %i\n", result);
      }
      printf("Song name of %s\n", fno.fname);
#endif
      sound.scorecard = false;
      current_state = SCORECARD;
    }

    else if ((sound.catchfail) && (current_state != CATCHFAIL)) {
      result = f_close(&file);
      result = f_findfirst(&dj, &fno, "", "4.mp3");
#ifdef TEST
      if (0 != result) {
        printf("Result of song found is %i\n", result);
      }
#endif
      result = f_open(&file, fno.fname, FA_OPEN_EXISTING | FA_READ);
#ifdef TEST
      if (0 != result) {
        printf("Result of open is %i\n", result);
      }
      printf("Song name of %s\n", fno.fname);
#endif
      sound.catchfail = false;
      current_state = CATCHFAIL;
    }

    else if ((sound.catchsuccess) && (current_state != CATCHSUCCESS)) {
      result = f_close(&file);
      result = f_findfirst(&dj, &fno, "", "3.mp3");
#ifdef TEST
      if (0 != result) {
        printf("Result of song found is %i\n", result);
      }
#endif
      result = f_open(&file, fno.fname, FA_OPEN_EXISTING | FA_READ);
#ifdef TEST
      if (0 != result) {
        printf("Result of open is %i\n", result);
      }
      printf("Song name of %s\n", fno.fname);
#endif
      sound.catchsuccess = false;
      current_state = CATCHSUCCESS;
    }

    else if ((sound.game) && (current_state != GAME)) {
      result = f_close(&file);
      if (0 != result) {
        // printf("File not closed %i\n", result);
      }
      result = f_findfirst(&dj, &fno, "", "2.mp3");
#ifdef TEST
      if (0 != result) {
        printf("Result of song found is %i\n", result);
      }
#endif
      result = f_open(&file, fno.fname, FA_OPEN_EXISTING | FA_READ);
#ifdef TEST
      if (0 != result) {
        printf("Result of open is %i\n", result);
      }
      printf("Song name of %s\n", fno.fname);
#endif
      sound.game = false;
      current_state = GAME;
    }

    else if ((sound.entry) && (current_state != DEFAULT)) {
      result = f_close(&file);
      if (0 != result) {
        // printf("File not closed %i\n", result);
      }
      result = f_findfirst(&dj, &fno, "", "1.mp3");
#ifdef TEST
      if (0 != result) {
        printf("Result of song found is %i\n", result);
      }
#endif
      result = f_open(&file, fno.fname, FA_OPEN_EXISTING | FA_READ);
#ifdef TEST
      if (0 != result) {
        printf("Result of open is %i\n", result);
      }
      printf("Song name of %s\n", fno.fname);
#endif
      sound.entry = false;
      current_state = DEFAULT;
    }

    xSemaphoreTake(mp3_mutex, portMAX_DELAY);
    result =
        f_read(&file, &bytes_to_read[0], READ_BYTES_FROM_FILE, &bytes_read);
    if (0 != result) {
      // printf("Result of read is %i\n", result);
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
      // printf("data not requested\n");
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
        // printf("count = %d\n", current_count);
#endif
      }
    }
  }
}
