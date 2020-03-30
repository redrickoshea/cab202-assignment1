#ifndef GAME_H_
#define GAME_H_

#include <cab202_sprites.h>
#include <stdbool.h>

/*
            Macros
*/

#define PLAYER_WIDTH (3)
#define PLAYER_HEIGHT (3)
#define BLOCK_HEIGHT (2)
#define MAX_UNSAFE (160)
#define MAX_SAFE (40)
#define MIN_UNSAFE (2)
#define COL_WIDTH (11)
#define ROW_HEIGHT (BLOCK_HEIGHT + PLAYER_HEIGHT + 2)
#define NUM_COL (screen_width() / COL_WIDTH)
#define NUM_ROW (((screen_height() - 5) / ROW_HEIGHT) - 1)

/*
            Typedefs
*/

// Holds gamestate variables
typedef struct state
{
    int lives,
    score, 
    start_time, 
    disable_tres_toggle, // A buffer that stops the treasure movement toggle 
                         // from triggering evey game cycle if the treasure
                         // toggle is held down
    tres_ani_cycle, // A buffer that slows the cycling of the treasure's animation
    current_block, // The index of the block that the player has stood on most recently
    block_count;
    bool treasure_moving, 
    game_over, 
    fall; // When true, a gravity function will be apllied to the player to make them fall
}state;

/*
            Graphics
*/
char *player_stationary =
    " o "
    "/O\\"
    "/ \\"
;

char *player_left =
    " o "
    "/O\\"
    " \\\\"
;

char *player_right = 
    " o "
    "/O\\"
    "// "
;

char *player_jump = 
    "\\o/"
    " O "
    "/ \\"
;

char *respawn1 =
    "   "
    "   "
    "***"
;

char *respawn2 =
    "   "
    "***"
    "/ \\"
;

char *respawn3 =
    "***"
    " O "
    "/ \\"
;

char *safe_block =
    "=========="
    "=========="
;

char *unsafe_block = 
    "xxxxxxxxxx"
    "xxxxxxxxxx"
;

char *treasure1 =
    " * "
    " 0 "
    " * "
;

char *treasure2 =
    "*  "
    " 0 "
    "  *"
;

char *treasure3 = 
    "   "
    "*0*"
    "   "
;

char *treasure4 =
    "  *"
    " 0 "
    "*  "
;

/*
			Functions
*/

// Read a user input from the keyboard
int read_char();

// Compare the start time of the game to the current time in order to calculate play time
void check_time(int start_time, int * digits);

// Create a single block
sprite_id setup_block(int x, int y, int type);

// Initialize a safe block in every column
void setup_safe(state *game, int *safe);

// Initialize 2 unsafe blocks, making sure they dont overlap with the existing safe blocks
void setup_unsafe(state *game, int *unsafe);

// Initialize extra blocks, making sure they don't overlap with existing blocks
void setup_extra(state *game, int extra_blocks, int safe, int unsafe);

// Initialize the treasure
void setup_treasure();

// Get an array of blocks that are viable spawnpoints for the player
void spawn_points(int block_count, int *valid_points, int *num_points);

// Spawn animation
void spawn_ani(state *game);

// Choose a random viable spawn point for the player to spawn on
void spawn_player(state *game);

// Give the blocks their vertical movement values
void row_displacement(int block_count, double *row_speed, int *row_dir);

// Combination of initialization functions
void setup(state *game, int extra_blocks, double *row_speed, int *row_dir);

// Checks to see if the block under foot is a new block. Adds a point if the new block is
// in a new row.
bool landing_check(state *game, int new_block);

// Check to see if player is standing on a block
bool standing_check(state *game);

// Check user input to control the player sprite
void move_player(state *game, int key);

// Increase the player's vertical movement value to make them fall
void gravity(state *game);

// Stop player from jumping through blocks
void check_up(state *game);

// Stop player from falling through blocks
void check_down(state *game);

// Stop player from jumping into the side of blocks
void check_side(state *game);

// Combination of collision stoppers
void collision_check(state *game);

// Manage the player's movement and interactions with the stage
void update_player(state *game, int key);

// Manage the treasure movement toggle
void treasure_toggle(state *game, int key);

// Change the bitmap of the treasure to create an animation
void cycle_treasure_ani();

// Manage the treasure's movement
void update_treasure(state *game, int key);

// Checks to see if the player and the treasure have collided
void win_check(state *game);

// Removes a life and respawns the player
void death(state *game);

// Checks to see if the player is in a situation that results in death
void death_check(state *game);

// Check to see if a block needs to warp, and if the player is on the block
void warp_check(state *game, int index);

// Manage row movement
void update_row(state *game);

// Function to progress the game one step
void process(state *game);

void go_screen_input(int key, state *game, bool *running, bool *waiting);

// Produces a game over screen
void game_over_screen(state *game, bool *running);

// Convert the int being used as a selection tool into a double that can be used for a row's dx
double convert_to_speed(int speed);

// Initialize row movement speeds and directions
void init_row_movement(double *row_speed, int *row_dir);

#endif