#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdbool.h>

#include "grid.h"
#include "player.h"


/**
 * Structure holding state information for a game.
 */
typedef struct {
    uint8_t shots;
    player_t player_one;
    player_t player_two;
} game_t;


/**
 * Make a new game using defaults.
 * 
 * @return  Generated game
 */
game_t make_default_game(void);

#endif // GAME_H