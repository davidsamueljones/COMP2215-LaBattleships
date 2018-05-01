#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "player.h"

void make_player(player_t* player, uint8_t width, uint8_t height, ship_t ships[], uint8_t ship_count) {
    // Create a grid for the player
    grid_t* player_grid = malloc(sizeof(player_grid));
    player_grid->width = width;
    player_grid->height = height;

    allocate_grid_data(player_grid, true);

    // Copy the ships for the player
    ship_t* player_ships = malloc(ship_count * sizeof(ship_t));
    for (uint8_t ship_idx=0; ship_idx < ship_count; ship_idx++) {
        player_ships[ship_idx] = ships[ship_idx];
    }
    // Create player
    player->grid = player_grid;
    player->ships = player_ships;
    player->ship_count = ship_count;
    player->last_x = BLOCKED_POS;
    player->last_y = BLOCKED_POS;
}


void free_player(player_t* player) {
    free(player->grid);
    free(player->grid->data);
    free(player->ships);
}


bool is_player_destroyed(player_t* player) {
    bool destroyed = true;
    for (uint8_t ship_idx = 0; ship_idx < player->ship_count; ship_idx++) {
        destroyed &= is_ship_destroyed(&player->ships[ship_idx]);
    }
    return destroyed;
}


shot_res_t shoot_pos(player_t* target, int8_t x, int8_t y) {
    g_data data = get_grid_data(target->grid, x, y);
    shot_res_t ret_code;

    if (data == BLOCKED_POS) {
        ret_code = Invalid;
    } if (data & SHOT_POS) {
        ret_code = Invalid;
    } else {
        // Mark shot
        mark_shot(target->grid, x, y);

        if (data != 0) {
            // Do ship hit behaviour
            uint8_t idx = data - 1;
            target->ships[idx].hits++;
            ret_code = Hit;
            // Check if ship is also destroyed
            if (is_ship_destroyed(&target->ships[idx])) {
                // TODO: This is a slight cheat, should be probabilistically marking confirmed
                // destroy locations. This uses knowledge of where ships are but doesn't lend
                // massive advantage. (FIXME)
                int8_t x1 = target->ships[idx].x;
                int8_t y1 = target->ships[idx].y;
                for (uint8_t i = 0; i < target->ships[idx].length; i++) {
                    mark_destroyed(target->grid, x1, y1);
                    move_x_y(&x1, &y1, target->ships[idx].dir);
                }
                ret_code = HitAndDestroyed;
            }
        } else {
            ret_code = Miss;
        }
       
    }
    printf("\n");
    return ret_code;
}