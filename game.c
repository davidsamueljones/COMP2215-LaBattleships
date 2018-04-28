#include "game.h"

#include "player.h"
#include "ship.h"

#define DEFAULT_GRID_WIDTH  (10)
#define DEFAULT_GRID_HEIGHT (10)
#define DEFAULT_SHIP_COUNT   (5)


game_t make_default_game(void) {
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

    // Create game
    game_t game = {
        .shots = 0, 
        .player_one = make_player(width, height, ships, ship_count),
        .player_two = make_player(width, height, ships, ship_count)
    };

    return game;
}