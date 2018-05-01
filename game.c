#include <stdlib.h>
#include "game.h"

#include "player.h"
#include "ship.h"

#define DEFAULT_GRID_WIDTH   (10)
#define DEFAULT_GRID_HEIGHT  (10)
#define DEFAULT_SHIP_COUNT   (5)


void make_default_game(game_t* game) {
    // Initialise game properties
    uint8_t width  = DEFAULT_GRID_WIDTH;
    uint8_t height = DEFAULT_GRID_WIDTH;

    uint8_t ship_count = 5;
    ship_t ships[DEFAULT_SHIP_COUNT] = {
        {.name = "Destroyer",  .ref = 1, .length = 2, SHIP_DEFAULTS},
        {.name = "Submarine",  .ref = 2, .length = 3, SHIP_DEFAULTS},
        {.name = "Cruiser",    .ref = 3, .length = 3, SHIP_DEFAULTS},
        {.name = "Battleship", .ref = 4, .length = 4, SHIP_DEFAULTS},
        {.name = "Carrier",    .ref = 5, .length = 5, SHIP_DEFAULTS},
    };

    // Initialise game
    game->shots = 0;
    game->player_one = malloc(sizeof(player_t));
    game->player_two = malloc(sizeof(player_t));

    make_player(game->player_one, width, height, ships, ship_count);    
    make_player(game->player_two, width, height, ships, ship_count);
}


void free_game(game_t* game) {
    free_player(game->player_one);
    free(game->player_one);

    free_player(game->player_two);
    free(game->player_two);
}


player_t* get_current_player(game_t* game) {
    switch (get_cur_player_idx(game)) {
    case (PLAYER_ONE):
        return game->player_one;
    case (PLAYER_TWO):
        return game->player_two;
    default:
        return NULL;
    }
}


player_t* get_other_player(game_t* game) {
    switch (get_cur_player_idx(game)) {
    case (PLAYER_ONE):
        return game->player_two;
    case (PLAYER_TWO):
        return game->player_one;
    default:
        return NULL;
    }
}