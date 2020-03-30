#include <cab202_graphics.h>
#include <cab202_timers.h>
#include <cab202_sprites.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Game.c"
#include "Drawing.c"

int main(void)
{

    // Setup conitions for gameplay
    setup_screen();
    show_screen();
    bool running = true;
    state game; // A struct that holds data for the current state of a game instance
    srand(round(get_current_time()));

    while(running)
    {

        // Setup conditions for a new game instance
        game.start_time = get_current_time(); 
        game.lives = 10;
        game.score = 0;
        game.block_count = 0; // Number of blocks in a game instance
        game.disable_tres_toggle = 0; // A buffer that stops the treasure movement toggle 
                                      // from triggering evey game cycle if the treasure
                                      // toggle is held down
        game.tres_ani_cycle = 0; // A buffer that slows the cycling of the treasure's animation
        game.treasure_moving = true;
        game.fall = true;

        // Generate a random number for the number of blocks to add to the game
        // on top of the minimum number of blocks
        int extra_blocks = rand() % NUM_COL * NUM_ROW / 5,
        row_dir[NUM_ROW - 1];
        double row_speed[NUM_ROW - 2];

        init_row_movement(row_speed, row_dir);
        setup(&game, extra_blocks, row_speed, row_dir);

        while(game.game_over == false)
        {
            process(&game);
            timer_pause(30);
        }
        game_over_screen(&game, &running);
    }
}