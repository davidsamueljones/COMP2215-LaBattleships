#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdbool.h>

#include "grid.h"
#include "player.h"

#define PLAYER_ONE (1)
#define PLAYER_TWO (2)

#define get_cur_player_idx(game) ((game->shots % 2) + 1)

/**
 * Structure holding state information for a game.
 */
typedef struct {
    uint8_t shots;
    player_t* player_one;
    player_t* player_two;
} game_t;


/**
 * Update the current game with a copy of the defaults.
 * 
 * @param game Pointer to game to update
 */
void make_default_game(game_t* game);


/**
 * Free all memory used by a game, this will stop the game being usable.
 *
 * @param game Game for which to free
 */
void free_game(game_t* game);

player_t* get_current_player(game_t* game);
player_t* get_other_player(game_t* game);

#endif // GAME_H