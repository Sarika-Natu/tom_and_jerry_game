#include <stdbool.h>

extern void setup_button_isr(void);

extern void button_isr(void);

extern void game_task(void *p);

extern bool game_on;