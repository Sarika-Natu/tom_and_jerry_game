#include "led_matrix.h"

extern const uint8_t maze_one_lookup_table[LEDMATRIX_HEIGHT][LEDMATRIX_WIDTH];

extern const uint8_t maze_two_lookup_table[LEDMATRIX_HEIGHT][LEDMATRIX_WIDTH];

extern const uint8_t maze_three_lookup_table[LEDMATRIX_HEIGHT][LEDMATRIX_WIDTH];

extern const uint8_t game_start_display_lookup_table[LEDMATRIX_HEIGHT]
                                                    [LEDMATRIX_WIDTH];

extern const uint8_t pause_screen_display_lookup_table[LEDMATRIX_HEIGHT]
                                                      [LEDMATRIX_WIDTH];

extern const uint8_t player_ready_display_lookup_table[LEDMATRIX_HEIGHT]
                                                      [LEDMATRIX_WIDTH];

extern const uint8_t player_won_display_lookup_table[LEDMATRIX_HEIGHT]
                                                    [LEDMATRIX_WIDTH];
extern const uint8_t game_over_display_lookup_table[LEDMATRIX_HEIGHT]
                                                   [LEDMATRIX_WIDTH];

extern const uint8_t next_level_display_lookup_table[LEDMATRIX_HEIGHT]
                                                    [LEDMATRIX_WIDTH];
extern void maze_one_frame(void);
extern void maze_two_frame(void);
extern void maze_three_frame(void);