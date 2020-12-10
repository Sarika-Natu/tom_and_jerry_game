#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define LEDMATRIX_HEIGHT 32
#define LEDMATRIX_WIDTH 64

extern const uint8_t maze_one_lookup_table[LEDMATRIX_HEIGHT][LEDMATRIX_WIDTH];

extern void setup_button_isr(void);

extern void button_isr(void);

extern void game_task(void *p);

extern void jerry_motion(void *p);

struct object_axis {
  uint8_t x;
  uint8_t y;
};
struct object_axis tom;
struct object_axis jerry;
extern bool game_on;
extern bool game_on_after_pause;

extern bool pause_or_stop;
