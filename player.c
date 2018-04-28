#include <stdio.h>
#include <stdbool.h>

#include "player.h"

player_t make_player(uint8_t width, uint8_t height, ship_t ships[], uint8_t ship_count) {
    // Create a grid for the player
    grid_t player_grid = {.width = width, .height = height};
    allocate_grid_data(&player_grid, true);

    // Copy the ships for the player
    ship_t player_ships[ship_count];
    for (uint8_t ship_idx=0; ship_idx < ship_count; ship_idx++) {
        player_ships[ship_idx] = ships[ship_idx];
    }

    // Create player
    player_t player = {
        .grid = player_grid, .ships = player_ships, .ship_count = ship_count
    };
    return player;
}


bool is_player_destroyed(player_t* player) {
    bool destroyed = true;
    for (uint8_t ship_idx = 0; ship_idx < player->ship_count; ship_idx++) {
        destroyed &= is_ship_destroyed(&player->ships[ship_idx]);
    }
    return destroyed;
}


shot_res_t shoot_pos(player_t* target, int8_t x, int8_t y) {
    g_data data = get_grid_data(&target->grid, x, y);
    //printf("Shooting: %d, %d - ", x, y);
    shot_res_t ret_code;

    if (data == BLOCKED_POS) {
        //printf("Not valid position");
        ret_code = Invalid;
    } if (data & SHOT_POS) {
        //printf("Already hit");
        ret_code = Invalid;
    } else {

        if (data != 0) {
            uint8_t idx = data - 1;
            target->ships[idx].hits++;
            //printf("HIT");
            ret_code = Hit;
            if (is_ship_destroyed(&target->ships[idx])) {
                // TODO: This is a slight cheat, should be probabilistically marking confirmed
                // destroy locations. This uses knowledge of where ships are but doesn't lend
                // massive advantage. (FIXME)
                int8_t x = target->ships[idx].x;
                int8_t y = target->ships[idx].y;
                for (uint8_t i = 0; i < target->ships[idx].length; i++) {
                    set_grid_data(&target->grid, x, y,
                                  (g_data) target->ships[idx].ref | DESTROY_POS);
                    move_x_y(&x, &y, target->ships[idx].dir);
                }
                //printf(" & DESTROYED");
                ret_code = HitAndDestroyed;
            }
        } else {
            ret_code = Miss;
            //printf("MISS");
        }
        mark_shot(&target->grid, x, y);
    }
    printf("\n");
    return ret_code;
}