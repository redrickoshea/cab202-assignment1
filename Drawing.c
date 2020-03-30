#include <cab202_sprites.h>
#include <cab202_graphics.h>
#include "Game.h"

// Draws a border at the top of the screen, inside of which gameplay info will be displayed
void draw_border(void)
{
    int top = 0, left = 0, right = screen_width() - 1, bottom = 4;
    draw_line(left, top, right, top, '~');
    draw_line(left, bottom, right, bottom, '~');
}

// Draw a timer. Takes x and y co-ordinates for where to draw the timer, and a
// start time in order to compare to the current time to calculate play time
void draw_timer(int x, int y, int start_time)
{
    int digits[] = {0, 0, 0, 0};
    check_time(start_time, digits);
    draw_int(x, y, digits[0]);
    draw_int(x + 1, y, digits[1]);
    draw_char(x + 2, y, ':');
    draw_int(x + 3, y, digits[2]);
    draw_int(x + 4, y, digits[3]);
}

// Draws information into the info display area 
void draw_content(state *game)
{
    char * student_number = "n10139281",
    * lives_label = "Lives remaining:",
    * score_label = "score:",
    * timer_label = "Time:";

    draw_string(2, 2, student_number);
    draw_string(15, 2, lives_label);
    draw_int(32, 2, game->lives);
    draw_string(38, 2, score_label);
    draw_int(45, 2, game->score);
    draw_string(50, 2, timer_label);
    draw_timer(56, 2, game->start_time);
}

// Combines the functions for drawing the display area
void draw_display(state *game)
{
    draw_border();
    draw_content(game);
}

// Draws blocks into the game area. Takes an int which specifies the amount of blocks
// in the game
void draw_blocks(int block_count)
{
    for (int i = 0; i < block_count; i++)
    {
        sprite_draw(blocks[i]);
    }
}

// Combines all draw functions
void draw_all(state *game)
{
    clear_screen();
    draw_display(game);
    draw_blocks(game->block_count);
    sprite_draw(player);
    sprite_draw(treasure);
    show_screen();
}