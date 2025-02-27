#ifndef TETRIS_H
#define TETRIS_H

#include <stdbool.h>
#include <stdint.h>

#define TETRIS_WIDTH  10
#define TETRIS_HEIGHT 20

void tetris_init(void);
void tetris_update(uint32_t dt_ms);

void tetris_move_left(void);
void tetris_move_right(void);
void tetris_rotate_clockwise(void);
void tetris_rotate_counter(void);
void tetris_soft_drop(void);
void tetris_hard_drop(void);

bool tetris_is_game_over(void);
uint32_t tetris_get_score(void);

// Desenha no SSD1306
void tetris_draw(void);

#endif
