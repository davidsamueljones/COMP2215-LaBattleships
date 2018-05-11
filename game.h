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

/**
 * Get a pointer to the player who should be taking the next shot.
 *  
 * @param  game Game to check
 * @return      Pointer to player in game
 */
player_t* get_current_player(game_t* game);

/**
 * Get a pointer to the player who should have taken the last shot.
 * 
 * @param  game Game to check
 * @return      Pointer to player in game
 */
player_t* get_other_player(game_t* game);

/**
 * Get a pointer to the game's player referenced by the given index.
 * 
 * @param  game Game to check
 * @return      Pointer to player in game
 */
player_t* get_player(game_t* game, uint8_t player_idx);

/**
 * For the given player, find out what player index it is for the given game.
 * This may be PLAYER_ONE, PLAYER_TWO or if the player does not belong to the game -1.
 * 
 * @param  game   Game to check for player
 * @param  player Player to find
 * @return        Index of player
 */
uint8_t get_player_idx(game_t* game, player_t* player);

#endif // GAME_H