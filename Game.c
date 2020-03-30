#include "Game.h"
#include "Drawing.h"

/*
    Declare sprites. These are objects that will appear in the gameplay area
*/
sprite_id player, 
blocks[MAX_SAFE + MAX_UNSAFE], 
treasure;

/* 
    Read user input from keyboard.

    return: the integer value of the key pressed
*/
int read_char()
{
    int key = get_char();
    return key;
    
}

/*
    Compares the start time of the curent game instance to the current time
    to caluclate play time

    start_time: An integer that stores the start time of the current game instance
    digits: A pointer to an integer array that stores the current time in mm:ss format
*/
void check_time(int start_time, int * digits)
{
    int sec_passed = round(get_current_time() - start_time); // Calculate play time

    // Parse play time out into the correct format
    while (sec_passed > 0)
    {
        if (sec_passed >= 60)
        {
            if (digits[1] >= 10)
            {
                digits[1] = 0;
                digits[0]++;
                sec_passed = sec_passed - 60;
            } 
            else 
            {
                digits[1]++;
                sec_passed = sec_passed - 60;
            }
        } 
        else if (sec_passed >= 10)
        {
            digits[2]++;
            sec_passed = sec_passed - 10;
        } 
        else 
        {
            digits[3] = digits[3] + sec_passed;
            sec_passed = 0;
        }
    }
}

/* 
    Creates a single block

    x: The x co-ordinate of the block to be created
    y: The y co-ordinate of the block to be created
    type: An int used to decide whether the block should be a safe block or a hazard
*/
sprite_id setup_block(int x, int y, int type)
{
    int width = (rand() % 5) + 5; // Decide the width of the block

    if (type == 0)
    {
        return sprite_create(x, y, width, 2, unsafe_block); // Create a hazard
    }
    else
    {
        return sprite_create(x, y, width, 2, safe_block); // Create a safe block
    }
}

/*
    Create a block in every column of the gameplay area

    game: A pointer to a struct which holds the game state variables
    safe: a pointer to an int which stores the amount of safe blocks that have been created
*/
void setup_safe(state *game, int *safe)
{
    
    // Declare a variable to hold the number of blocks in the top row.
    // The top row must contain at least one safe block
    int top_row = 0;

    for (; game->block_count < NUM_COL; game->block_count++)
    {
        int y = ROW_HEIGHT * ((rand() % NUM_ROW) + 1) + 5; // Generate y co-ord

        if (y == ROW_HEIGHT + 5)
        {
            top_row++; // Record a safe block being placed in the top row
        } 
        
        // If all columns have been filled and there is no safe block
        // in the top row, make the final safe block part of the top row
        if (game->block_count == NUM_COL - 1 && top_row == 0)
        {
            y = ROW_HEIGHT + 5;
        }

        // With the parameters decided, create the blocks
        blocks[game->block_count] = setup_block(game->block_count * COL_WIDTH, y, 1);
        (*safe)++;
    }
}

/*
    Creates 2 unsafe blocks, making sure they do not overlap with any safe blocks

    game: A pointer to a struct which holds the game state variables
    unsafe: A pointer to an integer which stores how many hazards have been created
*/
void setup_unsafe(state *game, int *unsafe)
{
    while(*unsafe < 2)
    {
        int x = (rand() % NUM_COL) * COL_WIDTH, 
        y = ROW_HEIGHT * ((rand() % NUM_ROW) + 1) + 5,
        invalid = 0; // Used to track whether a block is invalid

        for (int i = 0; i < game->block_count; i++)
        {
            // If a block is invalid, record it and return the the start of the while
            // loop to generate a new block
            if (blocks[i]->x == x && blocks[i]->y == y)
            {
                invalid++;
                break;
            }
        } 

        // If the block is not invalid, create the block
        if (invalid == 0)
        {
            blocks[game->block_count] = setup_block(x, y, 0);
            game->block_count++;
            (*unsafe)++;
        }
    }
}

/*
    Creates extra safe blocks, makin sure they do not overlap with existing blocks

    game: A pointer to a struct which holds the game state variables
    extra_blocks: An integer that defines how many extra blocks should be created
    safe: An integer that stores how many safe blocks have been created
    unsafe: An integer that stores how many unsafe blocks have been created
*/
void setup_extra(state *game, int extra_blocks, int safe, int unsafe)
{
    for (int i = 0; i < extra_blocks;)
    {
        int x = (rand() % NUM_COL) * COL_WIDTH, 
        y = ROW_HEIGHT * ((rand() % NUM_ROW) + 1) + 5, 
        type = rand() % 5, // Selects whether a given block will be safe or unsafe. Favours safe blocks
        invalid = 0; // Registers whether a block is valid or not

        // If the maximum amount of safe blocks has been reached, 
        // make the next block unsafe
        if (safe == MAX_SAFE)
        {
            type = 0;
        } 
        
        // If the maximum amount of unsafe blocks has been reached, 
        // make the next block safe
        if (unsafe == MAX_UNSAFE)
        {
            type = 1;
        }

        for (int j = 0; j < game->block_count; j++)
        {

            // If the position of the next block overlaps an existing block, record it
            // as invalid and return to generate a new position
            if (blocks[j]->x == x && blocks[j]->y == y)
            {
                invalid++;
                break;
            }

        }

        // If the block is not invalid, create the new block 
        if (invalid == 0)
        {
            blocks[game->block_count] = setup_block(x, y, type);
            game->block_count++;
            i++;
        }
    }
}

/*
    Initializes a piece of treasure, which moves across the bottom of the screen.
    If the player contacts the treasure, they earn a life.
*/
void setup_treasure()
{

    // Initial starting position and direction of movement
    int x = (rand() % (screen_width() - 3)) +1; // Horizontal co-ordinate
    treasure = sprite_create(x, ROW_HEIGHT * (NUM_ROW + 1) + 5, 3, 3, treasure1);
    int direction = rand() % 2;
    
    if (direction == 0)
    {
        treasure->dx = -.5; // Set treasure movement to left
    }
     else 
    {
        treasure->dx = .5; // Set treasure movement to right
    }
}

/*
    Iterates through the blocks in the game and checks to see which ones are 
    safe spawn points for the player

    block_count: The number of blocks in the current game instance
    valid_points: An integer array that holds the indexes of all valid spawn points
    num_points: A pointer to an integer that stores the number of valid spawn points
*/
void spawn_points(int block_count, int *valid_points, int *num_points){
    for (int i = 0; i < block_count; i++){
        if (blocks[i]->bitmap[0] == '=' && blocks[i]->y == ROW_HEIGHT + 5){
            valid_points[*num_points] = i;
            (*num_points)++;
        }
    }
}

/*
    Performs a spawning animation

    game: A pointer to a struct which holds the game state variables
*/
void spawn_ani(state *game){

    player->bitmap = respawn1;
    draw_all(game);
    timer_pause(200);

    player->bitmap = respawn2;
    draw_all(game);
    timer_pause(200);

    player->bitmap = respawn3;
    draw_all(game);
    timer_pause(200);

    player->bitmap = player_jump;
    draw_all(game);
    timer_pause(200);
}

/* 
    Randomly selects a spawn point for the player to spawn in

    game: A pointer to a struct which holds the game state variables
*/
void spawn_player(state *game)
{
    int valid_points[game->block_count], // Stores the indexes of all safe blocks in the blocks array
    num_points = 0;

    // Get the viable spawn points
    spawn_points(game->block_count, valid_points, &num_points);

    int spawn_block = (rand() % num_points);
    game->current_block = valid_points[spawn_block];

    player = sprite_create(blocks[valid_points[spawn_block]]->x, 
    blocks[valid_points[spawn_block]]->y - 3, 3, 3, player_stationary);
    
    spawn_ani(game);
}

/* 
    Applies horizontal movement values to the blocks in the gameplay area.
    Blocks in the top and bottom row will not move

    block_count: An int that stores the number of blocks in the current game instance
    row_speed: A double array which stores the horizontal movement values of each row of blocks
    row_dir: An int array which stores the movement direction of each row of blocks (0 for left, 1 for right)
*/
void row_displacement(int block_count, double *row_speed, int *row_dir)
{
    // Iterate through all blocks
    for (int i = 0; i < block_count; i++)
    {
        int row_num = (blocks[i]->y - 5) / ROW_HEIGHT;

        // If the block is not in the top or bottom row
        if (row_num > 1 && row_num < NUM_ROW)
        {
            row_num -= 2;

            if (row_dir[row_num] == 0)
            {
                blocks[i]->dx = + row_speed[row_num];
            } 
            if (row_dir[row_num] == 1)
            {
                blocks[i]->dx = - row_speed[row_num];
            }
        }
    }
}

/* 
    Combination of initialization functions

    game: A pointer to a struct which holds the game state variables
    extra_blocks: An int which defines how many blocks should be created on top of the minimum
    row_speed: A double array which stores the horizontal movement speed of each row
    row_dir: An int array which stores the movement direction of each row (0 for left, 1 for right)
*/
void setup(state *game, int extra_blocks, double *row_speed, int *row_dir)
{
    int safe = 0, 
    unsafe = 0;

    setup_safe(game, &safe);
    setup_unsafe(game, &unsafe);
    setup_extra(game, extra_blocks, safe, unsafe);
    row_displacement(game->block_count, row_speed, row_dir);
    setup_treasure();
    spawn_player(game);
}


/*
    Checks to see if the player has landed on a new block. If the block is in a different
    row from the block the player was previously standing on, a point it added to
    the player's score. If the player has landed on a new block, the horizontal
    movement value of the block is applied to the player, so the player
    can stay on the block

    game: A pointer to a struct which holds the game state variables
    new_block: The index of the new block the player is standing on within the block array
*/
bool landing_check(state *game, int new_block)
{

    // Check to see if the new block is the same as the old block
    if (blocks[game->current_block] == blocks[new_block])
    {
        return false;
    }

    // If the block is a new block, horizontal movement values are applied
    else
    {
        player->dx = blocks[new_block]->dx;
    }

    // If the new block is in a different row from the previous block, add a point
    if (blocks[game->current_block]->y != blocks[new_block]->y && 
    blocks[new_block]->bitmap[0] == '=')
    {
        game->score++;
    }

    // Record the new block as the most recent block the player has stood on
    game->current_block = new_block;
    return true;
}

/* 
    Check to see if player is standing on a block

    game: A pointer to a struct which holds the game state variables
*/
bool standing_check(state *game)
{
    int px = round(player->x), // player's x co-ord
    py = round(player->y); // player's y co-ord

    // Iterate through all blocks
    for (int i = 0; i < game->block_count; i++)
    {
        int bx = round(blocks[i]->x), // get the x co-ord of the current block 
        by = round(blocks[i]->y); // get the y co-ord of the current block

        // Detect whether the player is on a block
        if (px + 2 >= bx && px <= bx + blocks[i]->width - 1){
            if (py == by - 3){
                landing_check(game, i);
                game->fall = true;
                return true;
                break;
            }
        }
    }
    return false;
}

/* 
    Check user input to control the player sprite. Checks input from keyboard to get
    desired direction. In the case of left or right movement, player's horizontal movement
    value is compared to the movement value of the block the player is standing on
    to allow the player to move left or right at a speed relative to the speed of the
    block they are standing on

    game: A pointer to a struct which holds the game state variables
    key: The integer value of the key that was input from the keyboard
*/
void move_player(state *game, int key)
{
    player->dy = 0; // Player's horizontal movement value

    // Left movement
    if (key == 'a' && player->dx > blocks[game->current_block]->dx - 1)
    {
        player->dx -= .5;
        player->bitmap = player_left;
    } 

    // Right movement
    else if (key == 'd' && player->dx < blocks[game->current_block]->dx + 1)
    {
        player->dx += .5;
        player->bitmap = player_right;
    }

    // Jump 
    else if (key == 'w')
    {
        player->dy -= 1.5;
        game->fall = false;
        } 
        else
        {
        player->bitmap = player_stationary;
        player->dx = blocks[game->current_block]->dx;
    }
}

/* 
    Increase the player's vertical movement value to make them fall

    game: A pointer to a struct which holds the game state variables
*/
void gravity(state *game)
{

        if (player->dy < 1.5)
        {
            player->dy += .1;
        }
    player->bitmap = player_jump;
}


/*
    Stop player from jumping through blocks

    game: A pointer to a struct which holds the game state variables
*/
void check_up(state *game)
{
    // Stop the player from jumping through the top of the game area
    if (player->y < 5)
    {
        player->y = 5;
        player->dy = 0;
    }

    int px = round(player->x), // player x co-ord
    py = round(player->y); // player y co-ord
    
    // Iterate through all blocks
    for (int i = 0; i < game->block_count; i++)
    {
        int bx = round(blocks[i]->x), // block x co-ord
        by = round(blocks[i]->y); // block y co-ord
        
        // Horizontal position check
        if (px + 2 >= bx && px <= bx + blocks[i]->width - 1)
        {
            // Vertical position check
            if (py == by || py == by + 1)
            {
                player->dy = 0; // Cease horizontal movement
                player->y = by + 2; // Move the player to one pixel below the block
                break;
            }
        }
    }
}

/* 
    Stop player from falling through blocks

    game: A pointer to a struct which holds the game state variables
*/
void check_down(state *game)
{
    int px = round(player->x), // Player x co-ord
    py = round(player->y); // Player y co-rod

    // Iterate through all blocks
    for (int i = 0; i < game->block_count; i++)
    {
        int bx = round(blocks[i]->x), // Block x co-ord
        by = round(blocks[i]->y); // Block y co-ord

        // Horizontal position check
        if (px + 2 >= bx && px <= bx + blocks[i]->width - 1)
        {
            // Vertical position check
            if (py + 2 == by || py + 2 == by + 1)
            {
                player->dy = 0; // Cease vertical movement
                player->y = by - 3; // Move the player to one pixel above the block
                break;
            }
        }
    }
}

/*
    Stop player from jumping into the side of blocks

    game: A pointer to a struct which holds the game state variables
*/
void check_side(state *game)
{
    int px = round(player->x), // Player x co-ord
    py = round(player->y); // Player y co-ord

    for (int i = 0; i < game->block_count; i++)
    {
        int bx = round(blocks[i]->x), // Block x co-ord 
        by = round(blocks[i]->y); // Block y co-ord

        // Vertical position check
        if (py + 2 >= by && py <= by + 1)
        {
            // Check right of player and left of block
            if (px + 2 == bx - 1 && player->dx > 0)
            {
                player->dx = 0; // Cease horizontal movement
                player->x -= 1; // Move the player to one pixel left of the block
                break;

            // Check left of player and right of block
            } if (px == bx + blocks[i]->width && player->dx < 0)
            {
                player->dx = 0; // Cease horizontal movement
                player->x += 1; // Move the player to one pixel right of the block
                break;
            }
        }
    }
}

/*
    Combination of collision stoppers

    game: A pointer to a struct which holds the game state variables
*/
void collision_check(state *game)
{
    check_up(game);
    check_down(game);
    check_side(game);
}

/*
    Manage the player's movement and interactions with the stage

    game: A pointer to a struct which holds the game state variables
    key: The integer value of the key input from the keyboard
*/
void update_player(state *game, int key)
{
    // If the player is standing on a block, all the user's input to move the player
    if (standing_check(game) == true)
    {
        move_player(game, key);
    }
    else 
    {
        gravity(game);
    }

    sprite_step(player);
    collision_check(game);
}

/*
    Manages a toggle for the movement of the treasure sprite

    game: A pointer to a struct which holds the game state variables
    key: The integer value of the key input from the keyboard
*/
void treasure_toggle(state *game, int key)
{
    // Check the key input and the toggle buffer
    if (key == 't' && game->disable_tres_toggle <= 0)
    {
        if (game->treasure_moving == true)
        {
            game->treasure_moving = false;
        }
        else 
        {
            game->treasure_moving = true;
        }
        game->disable_tres_toggle = 10; // Set the toggle buffer
    }

    // Decrement the toggle buffer
    if (game->disable_tres_toggle > 0)
    {
        game->disable_tres_toggle--;
    }
}

/*
    Change the bitmap of the treasure to create an animation
*/
void cycle_treasure_ani()
{
    if (treasure->bitmap == treasure1)
    {
        treasure->bitmap = treasure2;
    } 
    else if (treasure->bitmap == treasure2)
    {
        treasure->bitmap = treasure3;
    } 
    else if (treasure->bitmap == treasure3)
    {
        treasure->bitmap = treasure4;
    } 
    else 
    {
        treasure->bitmap = treasure1;
    }
}

/*
    Manage the treasure's movement

    game: A pointer to a struct which holds the game state variables
    key: The integer value of the key input from the keyboard
*/
void update_treasure(state *game, int key)
{
    treasure_toggle(game, key);

    if (game->treasure_moving == true)
    {
        // Check to see if the treasure has reached the edge of the screen
        if ((treasure->x == 0 && treasure->dx < 0) || 
            (treasure->x + 2 == screen_width() - 1 && treasure->dx > 0))
        {
            treasure->dx -= treasure->dx * 2; // Make the treasure reverse direction
        }
        
        // If the treasure animation buffer has elapsed
        if (game->tres_ani_cycle <= 0)
        {
            cycle_treasure_ani();
            game->tres_ani_cycle = 10; // Reset the buffer
        }
        game->tres_ani_cycle--; // Decrement the buffer
        sprite_step(treasure);
    }
}

/*
    Checks to see if the player and the treasure have collided

    game: A pointer to a struct which holds the game state variables
*/
void win_check(state *game)
{
    // Check if the treasure is in play
    if (sprite_visible(treasure) == true)
    {
        int px = round(player->x), // Player horizontal co-ord
        py = round(player->y), // Player vertical co-ord
        tx = round(treasure->x), // Treasure horizontal co-ord
        ty = round(treasure->y); // Treasure vertical co-ord

        // Collision check
        if (px <= tx + 2 && px + 2 >= tx && py <= ty + 2 && py + 2 >= ty)
        {
            game->lives += 2;
            spawn_player(game);
            sprite_hide(treasure);
        }
    }
}

/*
    Removes a life and respawns the player

    game: A pointer to a struct which holds the game state variables
*/
void death(state *game)
{
    game->lives--;

    if (game->lives == 0)
    {
        game->game_over = true;
        player->dx = 0;
    }
    if (game->game_over == false)
    {
        spawn_player(game);
    }
}

/*
    Checks to see if the player is in a situation that results in death

    game: A pointer to a struct which holds the game state variables
*/
void death_check(state *game)
{
    // Check if player has left the gameplay area
    if (player->y > screen_height() - 1 || 
        player->x + 2 < 0 || 
        player->x > screen_width() - 1)
    {
        death(game); 
    }

    // Check if the block the player is standing on is a hazard block
    if (blocks[game->current_block]->bitmap[0] == 'x')
    {
        death(game);
    }
}

/*
    Check to see if a block needs to warp, and if the player is on the block.
    If the player is standing on a block that warps, the player dies

    game: A pointer to a struct which holds the game state variables
    index: The index of the block within the blocks array
*/
void warp_check(state *game, int index)
{
    // Check if the block has reached the edge of the screen
    if ((blocks[index]->x < 0 || 
        blocks[index]->x + blocks[index]->width - 1 > screen_width() - 1) && 
        game->current_block == index)
    {
        // If the player is standing on the block, kill the player
        if (standing_check(game) == true){
            death(game);
        }
    } 
    
    // Check if the block has reached the left edge of the screen
    if (blocks[index]->x + COL_WIDTH < 0)
    {
        blocks[index]->x = screen_width () - 1; // Move block to the right side of the screen
    } 
    
    // Check if the block has reached the right edge of the screen
    if (blocks[index]->x > screen_width() - 1)
    {
        blocks[index]->x = 0 - COL_WIDTH; // Move block to the left side of the screen
    }
}

/*
    Manage row movement

    game: A pointer to a struct which holds the game state variables
*/
void update_row(state *game)
{
    for (int i = 0; i < game->block_count; i++){
        sprite_step(blocks[i]);
        warp_check(game, i);
    }
}

/*
    Function to progress the game one step

    game: A pointer to a struct which holds the game state variables
*/
void process(state *game){
    int key = read_char();
    update_player(game, key);
    update_treasure(game, key);
    update_row(game);
    win_check(game);
    death_check(game);
    draw_all(game);
}

/*
    Recieve input at the game over screen. If the user presses 'q', the program exits.
    If the user presses 'r', a new game instance is created

    key: The integer value of the key input from the keyboard
    game: A pointer to a struct which holds the game state variables
    running: A pointer to abool used to keep the program loop running
    waiting: A pointer to a bool used to keep the program waiting for an input at the game over screen
*/
void go_screen_input(int key, state *game, bool *running, bool *waiting){
    if (key == 'q')
    {
        *running = false;
        *waiting = false;
    } 
    if (key == 'r')
    {
        game->game_over = false;
        *waiting = false;
    }   
}

/*
    Produces a game over screen

    game: A pointer to a struct which holds the game state variables
    running: A pointer to a bool used to keep the program loop running
*/
void game_over_screen(state *game, bool *running)
{
    int width = round(screen_width() - 1), 
    height = round(screen_height() - 1);
    
    clear_screen();
    draw_string(width / 2 - 4, height / 2, "Game Over");
    draw_string(width / 2 - 19, height / 2 + 1, "Press 'q' to quit or 'r' to play again");
    draw_string(width / 2 - 5, height / 2 + 2, "score:");
    draw_int(width / 2 + 2, height / 2 + 2, game->score);
    draw_string(width / 2 - 6, height / 2 + 3, "Playtime:");
    draw_timer(width / 2 + 4, height / 2 + 3, game->start_time);
    show_screen();
    
    bool waiting = true;
    
    // Wait for input
    while(waiting)
    {
        int key = read_char();
        go_screen_input(key, game, running, &waiting);
        timer_pause(10);
    }
}

/*
    Convert the int being used as a selection tool into 
    a double that can be used for a row's horizontal movement value

    speed: An int which is used to decide how fast a row should move
    return: A double which can directly be used for a row's horizontal movement value
*/
double convert_to_speed(int speed)
{
    // Slow
    if (speed == 0)
    {
        return .1;
    } 
    
    // Medium
    else if (speed == 1)
    {
        return .2;
    } 
    
    // Fast
    else {
        return .5;
    }
}

/*
    Initialize row movement speeds and directions. Each row must move in
    a different speed and direction than the row above it. The top and bottom rows
    must not move

    row_speed: A double array which holds thh movement speed of the rows
    row_dir: An int array which holds the movement directions for each row (0 = left, 1 = right)
*/
void init_row_movement(double *row_speed, int *row_dir)
{
    int dir = rand() % 2, 
    speed = rand() % 3;
    
    // Iterate through the rows
    for (int i = 0; i < NUM_ROW - 2; i++)
    {
        for (;;)
        {
            int new_speed = rand() % 3;

            if (new_speed != speed)
            {
                speed = new_speed;
                break;
            }
        }

        if (dir == 0)
        {
            dir = 1;
        } 
        else if (dir == 1)
        {
            dir = 0;
        }
        
        row_speed[i] = convert_to_speed(speed);
        row_dir[i] = dir;
    }
}