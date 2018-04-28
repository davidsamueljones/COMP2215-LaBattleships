#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "ai.h"

bool make_weighted_shot(player_t* target) {
    // Generate probability grid for state
    grid_t prob_grid = {.width = target->grid.width, .height = target->grid.height};
    allocate_grid_data(&prob_grid, false);
    gen_probability_grid(&target->grid, &prob_grid, target->ships, target->ship_count);

    // Determine optimal shot
    int16_t max;
    uint16_t occurrence = get_max_probability(&prob_grid, &max);
    if (occurrence > 0) {
        // Shoot at the highest probability square (randomly if multiple have max)
        uint16_t ongoing = 0;
        uint16_t shot = 1 + rand() % occurrence;
        for (int8_t x = 0; x < target->grid.width; x++) {
            for (int8_t y = 0; y < target->grid.height; y++) {
                if (get_grid_data(&prob_grid, x, y) == max) {
                    ongoing++;
                }
                if (ongoing == shot) {
                    shoot_pos(target, x, y);
                    printf("SHOT\n");
                    free(prob_grid.data);
                    return true;
                }
            }
        }
    }
    free(prob_grid.data);
    printf("NO SHOT\n");
    return false;
}


uint16_t get_max_probability(grid_t* prob_grid, int16_t* max_val) {
    uint16_t max_occurences = 0;
    *max_val = BLOCKED_POS;

    for (int8_t x = 0; x < prob_grid->width; x++) {
        for (int8_t y = 0; y < prob_grid->height; y++) {
            int16_t data = get_grid_data(prob_grid, x, y);
            if (data > *max_val) {
                *max_val = data;
                max_occurences = 1;
            } else if (data == *max_val) {
                max_occurences++;
            }
        }
    }
    return max_occurences;
}


uint16_t gen_probability_grid(grid_t* target_grid, grid_t* prob_grid, ship_t ships[], uint8_t ship_count) {
    // Initialise
    zero_grid_data(prob_grid);
    uint16_t grid_weight = 0;

    // Attempt placement for all ships that are alive
    for (uint8_t idx_ship = 0; idx_ship < ship_count; idx_ship++) {
        ship_t ship = ships[idx_ship];
        if (is_ship_destroyed(&ship)) {
            continue;
        }
        // Attempt placement in all configurations
        for (ship.x = 0; ship.x < target_grid->width; ship.x++) {
            for (ship.y = 0; ship.y < target_grid->height; ship.y++) {
                for (ship.dir = D_North; ship.dir <= D_West; ship.dir++) {
                    uint8_t weight = 1;
                    bool valid = true;
                    // Validate that ship is placeable
                    int8_t x = ship.x;
                    int8_t y = ship.y;
                    for (uint8_t i = 0; i < ship.length; i++) {
                        int16_t data = get_grid_data(target_grid, x, y);
                        // Not valid if off grid or through miss or through confirmed destroy
                        if (data == BLOCKED_POS || IS_MISS(data) || data & DESTROY_POS) {
                            valid = false;
                            break;
                        } else if (IS_HIT(data)) {
                            weight += 5;
                        }
                        move_x_y(&x, &y, ship.dir);
                    }
                    if (valid) {
                        // Increment probabilities for each non-hit position under ship
                        x = ship.x;
                        y = ship.y;
                        for (uint8_t i = 0; i < ship.length; i++) {
                            int16_t data = get_grid_data(target_grid, x, y);
                            uint16_t pos = map_grid_pos(prob_grid, x, y);
                            if (!(data & SHOT_POS)) {
                                prob_grid->data[pos] += weight;
                                grid_weight += weight;
                            }
                            move_x_y(&x, &y, ship.dir);
                        }
                    }
                }
            }
        }
    }
    return grid_weight;
}
