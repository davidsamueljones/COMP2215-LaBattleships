#include <stdio.h>
#include <stdbool.h>

#include "control.h"
#include "game.h"
#include "ui_drawing.h"
#include "ai.h"

#include "lafortuna/os.h"

void update_ship_position(player_t* player, ship_t* cur_ship, ship_t* next_ship, draw_props_t* draw_props);

void play_battleships(bool player_one_cpu, bool player_two_cpu) {
    // Initialise a new game
    game_t game;
    make_default_game(&game);
    game.player_one->cpu = player_one_cpu;
    game.player_two->cpu = player_two_cpu;

    // Placement phase
    placement_phase(&game, PLAYER_ONE);
    placement_phase(&game, PLAYER_TWO);

    // Shooting face
    shooting_phase(&game);

    // Game over
    wait_for_press(SWC);
    free_game(&game);
}


void placement_phase(game_t* game, uint8_t player_idx) {
    player_t* player = NULL;
    switch (player_idx) {
    case PLAYER_ONE:
        player = game->player_one;
        break;
    case PLAYER_TWO:
        player = game->player_two;
        break;
    }
    if (player == NULL) {
        return;
    }

    // If a CPU just auto place the ships
    if (player->cpu) {
        auto_place_ships(player->grid, player->ships, player->ship_count);
        return;
    }

    // Two player game so must display separation screen
    if (!game->player_one->cpu && !game->player_two->cpu) {
        draw_next_player_screen("Place your ships", player_idx);
    }

    // Wait for ship placement
    draw_props_t draw_props;
    generate_one_grid_view(&draw_props);
    draw_props.ships = true; 
    clear_screen();
    draw_title("SHIP PLACER", &draw_props);
    draw_grid(player->grid, &draw_props);
    player_ships_placer(player, &draw_props);
}


void shooting_phase(game_t* game) {
    
    // Split view so LHS is shooting area, RHS is shots against player
    draw_props_t grid_1_draw_props;
    draw_props_t grid_2_draw_props;   
    generate_two_grid_view(&grid_1_draw_props, &grid_2_draw_props, 0.57);
    grid_1_draw_props.ships = false;
    grid_2_draw_props.ships = true; 

    clear_screen();
    while (!is_player_destroyed(game->player_one) && !is_player_destroyed(game->player_two)) {
        player_t* cur_player = get_current_player(game);
        player_t* other_player = get_other_player(game);


        // Two player game so must display separation screen 
        if (!game->player_one->cpu && !game->player_two->cpu) {
            draw_next_player_screen("Take your shot", get_cur_player_idx(game));
        }

        // Only redraw if not a CPU turn (unless both are CPUs)
        if (!cur_player->cpu  || other_player->cpu) {
            draw_game_state(game, &grid_1_draw_props, &grid_2_draw_props);
        }
    
        // Make shot
        if (cur_player->cpu) {
            make_weighted_shot(other_player);
        } else {   
            shot_position_selector(other_player, &grid_1_draw_props);
            shoot_pos(other_player, other_player->last_x, other_player->last_y);
        }
        game->shots++;
    }

    draw_game_state(game, &grid_1_draw_props, &grid_2_draw_props);
}


void draw_next_player_screen(char* message, uint8_t player_idx) {
    clear_screen();

    char buf[50];
    char* player_str = "";
    switch (player_idx) {
    case PLAYER_ONE:
        player_str = ONE_PLAYER_STRING_UPPER;
        break;
    case PLAYER_TWO:
        player_str = TWO_PLAYER_STRING_UPPER;
        break;
    }
    sprintf(buf, "PLAYER %s: %s\n", player_str, message);
    draw_centred_string(buf, &draw_area);
    sprintf(buf, "\nPress 'Centre Key' to continue...");
    draw_centred_string(buf, &draw_area);
    wait_for_press(SWC);
    clear_screen();
}


void shot_position_selector(player_t* target, draw_props_t* draw_props) {
    bool selected = false;
    bool update = true;
    bool valid = false;

    // If no prior shot, initialise on top left
    if (target->last_x == BLOCKED_POS || target->last_y == BLOCKED_POS) {
        target->last_x = 0;
        target->last_y = 0;
    }
    // Modify copy so validation can take place
    int8_t next_x = target->last_x;
    int8_t next_y = target->last_y;
    while (!selected) {
        if (update) {
            // Check whether update is valid
            g_data next_data = get_grid_data(target->grid, next_x, next_y);
            if (next_data != BLOCKED_POS) {
                // Move the selection
                clear_selection(target, target->last_x, target->last_y, draw_props);

                valid = !(next_data & SHOT_POS);
                draw_selection(target->grid, next_x, next_y, draw_props,
                               valid ?  VALID_SEL : INVALID_SEL);

                // Update the selection coordinates
                target->last_x = next_x;
                target->last_y = next_y;
            } else {
                // Reset
                next_x = target->last_x;
                next_y = target->last_y;
            }
            update = false;
        }

        // Capture selection make event
        if (get_switch_short(_BV(SWC))) {
            if (valid) {
                selected = true;
            }
        }

        // Capture selection move events
        dir_t move_dir = get_move_dir();
        if (move_dir != NO_DIR) {
            move_x_y(&next_x, &next_y, move_dir);
            update = true;
        }
    }
    clear_selection(target, target->last_x, target->last_y, draw_props);
}


void player_ships_placer(player_t* player, draw_props_t* draw_props) {
    for (uint8_t ship_idx = 0; ship_idx < player->ship_count; ship_idx++) {
        ship_t* cur_ship = &player->ships[ship_idx];
        update_ship_position(player, NULL, cur_ship, draw_props);

        // Wait for user to place this ship
        player_ship_placer(player, cur_ship, draw_props);
    }
}


void player_ship_placer(player_t* player, ship_t* cur_ship, draw_props_t* draw_props) {
    while (!cur_ship->placed) {
        // Modify a copy so validation can take place
        ship_t next_ship = *cur_ship;

        // Capture ship move events
        dir_t move_dir = NO_DIR;
        if (get_switch_short(_BV(SWN))) {
            move_dir = D_North;
        } else if (get_switch_short(_BV(SWE))) {
            move_dir = D_East;
        } else if (get_switch_short(_BV(SWS))) {
            move_dir = D_South;
        } else if (get_switch_short(_BV(SWW))) {
            move_dir = D_West;
            // Capture ship rotate event
        } else if (get_switch_short(_BV(SWC))) {
            do {
                // Find the first valid CW rotation
                next_ship.dir = (next_ship.dir + 1) % (D_West + 1);
                if (validate_ship_position(player->grid, &next_ship) != OffGrid) {
                    break;
                }
            } while (next_ship.dir != cur_ship->dir);
            // Capture ship place event (Long centre press)
        } else if (get_switch_long(_BV(SWC))) {
            place_ship(player->grid, &next_ship, true);
        }

        // Process result of captured input
        if (move_dir != NO_DIR || next_ship.dir != cur_ship->dir || next_ship.placed) {
            move_x_y(&next_ship.x, &next_ship.y, move_dir);
            update_ship_position(player, cur_ship, &next_ship, draw_props);
        }
    }
    clear_ship_selection(player, cur_ship, draw_props);
}



void wait_for_press(uint8_t sw) {
    while (true) {
        if (get_switch_short(_BV(sw))) {
            return;
        }
    }
}



dir_t get_move_dir(void) {
    if (get_switch_short(_BV(SWN))) {
        return D_North;
    } else if (get_switch_short(_BV(SWE))) {
        return D_East;
    } else if (get_switch_short(_BV(SWS))) {
        return D_South;
    } else if (get_switch_short(_BV(SWW))) {
        return D_West;
    }
    return NO_DIR;
}


/**
 * Handle the change of ship selection. If the new position is not on the grid, nothing happens. If the ship
 * is on the grid the position is updated. However, if the position is not valid as it overlaps existing ships
 * this will be reflected in the selection colour.
 * 
 * @param player     Player doing update
 * @param cur_ship   Current ship selection
 * @param next_ship  Next ship selection
 * @param draw_props Drawing constraints
 */
void update_ship_position(player_t* player, ship_t* cur_ship, ship_t* next_ship, draw_props_t* draw_props) {
    place_valid_t validation = validate_ship_position(player->grid, next_ship);
    uint16_t colour;

    switch (validation) {
    case OffGrid:
        return; // do not draw off screen
    case PlaceValid:
        colour = VALID_SEL;
        break;

    default:
        colour = INVALID_SEL;
    }
    // Remove old ship selection
    if (cur_ship != NULL) {
        clear_ship_selection(player, cur_ship, draw_props);
    }
    // Draw new ship selection
    if (next_ship != NULL) {
        draw_ship_selection(player->grid, next_ship, draw_props, colour);
    }
    // Update current ship state to match that of next
    if (cur_ship != NULL) {
        *cur_ship = *next_ship;
    }
}