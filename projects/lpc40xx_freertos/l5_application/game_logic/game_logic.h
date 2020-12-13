#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define LEDMATRIX_HEIGHT 32U
#define LEDMATRIX_WIDTH 64U

struct object_axis {
  uint8_t x;
  uint8_t y;
};

typedef struct {
  uint8_t x;
  uint8_t y;
} axis;

typedef struct {
  axis start;
  axis end;
} maze;

struct object_axis tom;
struct object_axis jerry;
extern struct sounds sound;
extern bool game_on_after_pause;
extern bool pause_or_stop;
extern bool change_level;
extern uint8_t level;
extern const uint8_t maze_one_lookup_table[LEDMATRIX_HEIGHT][LEDMATRIX_WIDTH];

typedef void (*maze_selection_t)(void);
extern void setup_button_isr(void);
extern void button_isr(void);
extern void game_task(void *p);
extern void jerry_motion(void *p);
void game_mode_on(void);
