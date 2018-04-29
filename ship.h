#ifndef SHIP_H
#define SHIP_H

#include <stdio.h>
#include <stdbool.h>

#include "grid.h"

/* Default clear values for non-property values */
#define SHIP_DEFAULTS .hits = 0, .x = 0, .y = 0, .dir = D_South, .placed = false

/* Wrapper macrocs for methods */
#define place_ship_valid(grid, ship) (!validate_ship_position(grid, ship))
#define move_x_y(x, y, dir) (move_x_y_n(x, y, dir, 1))

/**
 * Enumeration of ship directions.
 */
typedef enum {
    D_North,
    D_East,
    D_South,
    D_West
} dir_t;

/**
 * Structure holding a ships identifying data, positioning data and
 * its current state.
 */
typedef struct {
    char* name;
    int8_t ref;
    int8_t x;
    int8_t y;
    dir_t dir;
    uint8_t length;
    uint8_t hits;
    bool placed;
} ship_t;

/**
 * Enumeration of placement validation results.
 */
typedef enum {
    PlaceValid,
    OffGrid,
    Overlap
} place_valid_t;

/**
 * Print the states of the given ship list.
 *
 * @param ships Ship array
 * @param count Number of ships in array
 */
void print_ships(ship_t ships[], uint8_t count);

/**
 * Using a grid that may be filled with ships as a basis, attempt to place the provided list of ships randomly.
 * This requires a memory allocation equal to that of the provided grid.
 * 
 * @param  grid  Currently placed ships (no shot bits allowed to be set)
 * @param  ships Ships to attempt to auto-place
 * @param  count Number of ships in provided list
 * @return       Whether all ships were placed
 */
bool auto_place_ships(grid_t* grid, ship_t ships[], uint8_t count);

/**
 * Using a grid filled with ships as a basis, attempt to place another ship randomly. If possible, the
 * ship's positioning fields are set and the ship will be placed on the ship_grid.
 *
 * @param  ship_grid  Currently placed ships (no shot bits allowed to be set)
 * @param  alloc_grid Grid with pre-allocated memory equal in size to ship_grid (will be cleared)
 * @param  ship       Ship to attempt to auto-place
 * @return            Whether ship could be auto-placed
 */
bool auto_place_ship(grid_t* ship_grid, grid_t* alloc_grid, ship_t* ship);

/**
 * Using an allocation grid, attempt to allocate a position and direction to a given ship.
 * This is achieved by counting each set flag in the allocation grid as an available allocation,
 * The selected flag is the n'th one (where n == allocation).
 *
 * @param  alloc_grid A pre-generated allocation grid
 * @param  ship       The ship who's position should be updated
 * @param  allocation Of the allocatable places, the one to select
 * @return            Whether a position was successfully allocated, if this returns false
 *                    the current state of the ship's position fields should be ignored
 */
bool allocate_ship_pos(grid_t* alloc_grid, ship_t* ship, uint16_t allocation);

/**
 * Using a grid filled with ships as a basis, generate another grid with flags set that represent which directions
 * from the respective position support the given ship length. A position can have multiple flags set if multiple
 * directions are supported
 *
 * Flags:
 * 15-4 [NOTHING], 3 [West], 2 [South], 1 [East], 0 [North]
 *
 * @param ship_grid  Currently placed ships (no shot bits allowed to be set)
 * @param alloc_grid Grid with pre-allocated memory equal in size to ship_grid (will be cleared)
 * @param length     Length of space required for move to be available
 * @return           The number of available placements
 */
uint16_t gen_availability_grid(grid_t* ship_grid, grid_t* alloc_grid, uint8_t length);

/**
 * First verify if the position is valid before placing a ship on the grid.
 * The ship is placed as its reference field in all positions it covers.
 *
 * @param  grid   Grid configuration to update
 * @param  ship   Ship configuration to use for update
 * @param  verify Whether ship placement should be verified first
 * @return        Whether the ship was placed
 */
bool place_ship(grid_t* grid, ship_t* ship, bool verify);

/**
 * Check if a ship can be placed in its stored position on the given grid.
 * A ship must fit within the bounds of the grid and not cross any tile that is
 * not empty.
 *
 * @param  grid Grid configuration to check on
 * @param  ship Ship configuration to check with
 * @return      Return code reflecting whether valid.
 */
place_valid_t validate_ship_position(grid_t* grid, ship_t* ship);

/**
 * Check if an input ship is in a destroyed state. A destroyed state is where the number of hits equals the
 * number of hitpoints (length).
 * 
 * @param  ship Ship to check
 * @return      Whether ship is in a destroyed state
 */
bool is_ship_destroyed(ship_t* ship);

/**
 * Shift an x y coordinate in a given direction by a given amount.
 * The macro move_x_y(x, y, dir) can be used to do a single shift.
 * 
 * @param x   Pointer to x coordinate to shift
 * @param y   Pointer to y coordinate to shift
 * @param dir Direction in which to shift
 * @param n   Amount by which to shift
 */
void move_x_y_n(int8_t* x, int8_t* y, dir_t dir, uint8_t n);

#endif // SHIP_H