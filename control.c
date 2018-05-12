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
    finish_phase(&game);
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
        show_next_player_screen("Place your ships", player_idx);
    }

    // Wait for ship placement
    draw_props_t draw_props;
    generate_one_grid_view(&draw_props);
    draw_props.ships = true; 
    clear_screen();
    draw_header();
    write_current_player(player_idx);
    draw_footer();
    uint8_t temp = display.background;
    display.background = MESSAGE_BOX_BG;
    draw_centred_string("Arrows: Movement - Centre: Rotate\n", &footer);
    draw_centred_string("\nLong Press Centre: Place", &footer);
    display.background = temp;
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
        player_t* enemy_player = get_next_player(game);

        // Two player game so must display separation screen 
        if (!game->player_one->cpu && !game->player_two->cpu) {
            show_next_player_screen("Take your shot", game->turn);
        }

        // Require redraw if both CPU's, if both are not CPUs, or first shot
        bool full_redraw = (cur_player->cpu && enemy_player->cpu) ||
            (!cur_player->cpu && !enemy_player->cpu) || 
            (game->shots == 0);
        if (full_redraw) {
            draw_game_state(game, &grid_1_draw_props, &grid_2_draw_props);
        }

        // Only draw updates if not a CPU turn (unless both are CPUs)
        bool draw_updates = !cur_player->cpu || enemy_player->cpu;
        if (draw_updates) {
            write_current_turn(game->shots / 2 + 1);
            draw_shot_overlay(cur_player->grid, &grid_2_draw_props);
            char buf[50];
            bool message = get_last_action_message(buf, game);
            show_footer_message(message ? buf : NULL, false);
        }

        // Make shot
        if (cur_player->cpu) {
            make_weighted_shot(enemy_player);
        } else {   
            shot_position_selector(enemy_player, &grid_1_draw_props);
            shoot_pos(enemy_player, enemy_player->last_x, enemy_player->last_y);
        }
        game->shots++;

        // Wait if not a CPU turn (unless both are CPUs)
        if (!cur_player->cpu  || enemy_player->cpu) {
            draw_shot_overlay(enemy_player->grid, &grid_1_draw_props);
            char buf[50];
            bool message = get_last_action_message(buf, game);
            show_footer_message(message ? buf : NULL, !cur_player->cpu);
        }

        // Increment turn
        game->turn = next_player_idx(game);
    }
    draw_game_state(game, &grid_1_draw_props, &grid_2_draw_props);
}


void finish_phase(game_t* game) {
    clear_screen();
    draw_props_t grid_1_draw_props;
    draw_props_t grid_2_draw_props;   
    generate_two_grid_view(&grid_1_draw_props, &grid_2_draw_props, 0.5);
    grid_1_draw_props.ships = true;
    grid_2_draw_props.ships = true;
    draw_finish_state(game, &grid_1_draw_props, &grid_2_draw_props);
    show_footer_message(NULL, true);
}


bool get_last_action_message(char* buf, game_t* game) {
    uint8_t last_shooter_idx = game->shots % 2 + 1;
    player_t* last_shooter = get_player(game, last_shooter_idx);
    if (game->shots == 0) {
        return false;
    }
    g_data data = get_grid_data(last_shooter->grid, 
        last_shooter->last_x, last_shooter->last_y);
    bool hit = IS_HIT(data);
    bool destroy = data & DESTROY_POS;
    const char* ship = last_shooter->ships[(data & POS_DATA) - 1].name;
    if (game->turn == last_shooter_idx) {
        if (!hit) {
            sprintf(buf, "The enemy missed!");
        } else {
            sprintf(buf, "The enemy %s your '%s'", destroy ? "destroyed" : "hit", ship);
        }
    } else {
        if (destroy) {
            sprintf(buf, "You destroyed the enemy '%s'", ship);
        } else {
            sprintf(buf, hit ? "Hit!" : "Miss!");
        }
    }
    return true;
}


void show_footer_message(char* message, bool wait) {
    uint8_t temp = display.background;
    display.background = MESSAGE_BOX_BG;
    draw_footer();
    char buf[50];
    if (message) {
        sprintf(buf, "%s%s", message, wait ? "\n" : "");
        draw_centred_string(buf, &footer);
    }

    if (wait) {
        sprintf(buf, "%sPress 'Centre' to continue...",  message ? "\n" : "");
        draw_centred_string(buf, &footer);
        wait_for_press(SWC);
    }
    display.background = temp;
}


void show_next_player_screen(char* message, uint8_t player_idx) {
    clear_screen();

    char buf[50];
    char* player_str = "";
    switch (player_idx) {
    case PLAYER_ONE:
        player_str = PLAYER_ONE_STR_UP;
        break;
    case PLAYER_TWO:
        player_str = PLAYER_TWO_STR_UP;
        break;
    }
    sprintf(buf, "PLAYER %s: %s\n", player_str, message);
    draw_centred_string(buf, &draw_area);
    sprintf(buf, "\nPress 'Centre' to continue...");
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
