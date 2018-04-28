#ifndef AI_H
#define AI_H

#include <stdio.h>
#include <stdbool.h>

#include "grid.h"
#include "ship.h"
#include "player.h"

/**
 * Attempt a shot on a target player using the statistically most likely 'hit' position. Position
 * is determined as the maximum location in a probability grid. If multiple equally weighted positions
 * exist, one position is targeted randomly.
 * 
 * @param  player Player to target with shot
 * @return        Whether a shot could be made
 */
bool make_weighted_shot(player_t* target);

/**
 * Find the max probability, and the number of occurrences, in a pre-generated probability grid. 
 * 
 * @param  prob_grid Probability grid to search
 * @param  max_val   Return pointer for max value
 * @return           The number of times max_val was found
 */
uint16_t get_max_probability(grid_t* prob_grid, int16_t* max_val);

/**
 * Assign values to each position on a grid where the values are a estimation of how likely a ship
 * goes through it. If a predicted ship placement passes through it and a previous (undestroyed) hit,
 * the position is weighted higher than if it were just passing through un-shot squares.
 * 
 * @param  target_grid Grid that is being targeted with previous hits/misses identified
 * @param  prob_grid   Grid with memory allocated (equal to target_grid) for return probabilities
 * @param  ships       Ships that are known to be on the board (destroyed are ignored)
 * @param  ship_count  Number of ships passed
 * @return             The total weighting of the grid
 */
uint16_t gen_probability_grid(grid_t* target_grid, grid_t* prob_grid, ship_t ships[], uint8_t ship_count);


#endif // AI_H