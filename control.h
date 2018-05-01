#ifndef CONTROL_H
#define CONTROL_H

#include <stdio.h>
#include <stdbool.h>

#include "ship.h"
#include "player.h"
#include "game.h"
#include "grid_drawing.h"

/**
 * Initialise a game of battleships. This will use a default setup. This is the main control flow of
 * a battleships game so will not return until the game is complete.
 * 
 * @param player_one_cpu Whether player one should be a CPU
 * @param player_two_cpu Whether player two should be a CPU
 */
void play_battleships(bool player_one_cpu, bool player_two_cpu);

/**
 * Handle placement of ships for a player. If the player is a CPU, ships are placed automatically,
 * otherwise user placement is expected.
 * 
 * @param game       Game to manipulate
 * @param player_idx Player number to do placement for
 */
void placement_phase(game_t* game, uint8_t player_idx);

/**
 * Take shots from human and CPU players until one is destroyed. CPU shots are taken randomly whereas
 * for human shots user selection is expected.
 * 
 * @param game Game to manipulate
 */
void shooting_phase(game_t* game);

/**
 * Draw a separation screen that indicates who the next player is. Display the given message.
 * Progress can be made with a centre button press at which point the display is cleared.
 * 
 * @param message    Message to display
 * @param player_idx The next player
 */
void draw_next_player_screen(char* message, uint8_t player_idx);

/** 
 * Get a user input referencing which position to target with the shot. The selected position is set
 * to the last_x and last_y fields of the target. 
 * 
 * @param target     Player to target
 * @param draw_props Drawing constraints
 */
void shot_position_selector(player_t* target, draw_props_t* draw_props);

/** 
 * Allow user to manually place all player ships using player_ship_placer.
 * 
 * @param player     Player who is placing their ships
 * @param draw_props Drawing constraints
 */
void player_ships_placer(player_t* player, draw_props_t* draw_props);

/**
 * Allow the user to manually place a ship using a selection grid. A ship can only be placed where it does
 * not overlap with any other ship or goes off the board.
 * 
 * @param player     Player placing ship
 * @param cur_ship   Ship to place
 * @param draw_props Drawing constraints
 */
void player_ship_placer(player_t* player, ship_t* cur_ship, draw_props_t* draw_props);

/**
 * Loop until input is received from a given switch.
 * 
 * @param sw Switch to wait for
 */
void wait_for_press(uint8_t sw);

/** 
 * Capture user input and convert it to a direction. No input results in a no direction flag
 * being returned.
 * 
 * @return  Direction indicated by input
 */
dir_t get_move_dir(void);

#endif // CONTROL_H