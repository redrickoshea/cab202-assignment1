#ifndef DRAWING_FUNCTIONS_H_
#define DRAWING_FUNCTIONS_H_

#include "Game.h"

// Draw a border around the display aread
void draw_border(void);

// Draw a timer into the info display screen
void draw_timer(int x, int y, int start_time);

// Draw the content of the info display area
void draw_content(state *game);

// Draw the info display area
void draw_display(state *game);

// Draw the blocks into the game area
void draw_blocks(int block_count);

// Combine all draw functions
void draw_all(state *game);

#endif