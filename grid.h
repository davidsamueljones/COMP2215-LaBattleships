#ifndef GRID_H
#define GRID_H

#include <stdio.h>
#include <stdbool.h>

#define EMPTY_POS   (0)
#define BLOCKED_POS ((g_data) (0xFFFF))

#define SHOT_POS    ((g_data) (0x8000)) // Flag indicating position shot
#define DESTROY_POS ((g_data) (0x4000)) // Flag for confirmed destroys at shot pos  
#define POS_DATA    ((g_data) (0x3FFF)) // Mask for getting data part

#define IS_HIT(x)  (x & SHOT_POS && x & POS_DATA)
#define IS_MISS(x) (x & SHOT_POS && !IS_HIT(x))

/**
 * Type of data stored by a grid.
 */
typedef int16_t g_data;

/**
 * Structure representing a grid, a grid should be allocated data equivalent
 * to its grid/width.
 */
typedef struct {
    g_data* data;
    uint8_t width;
    uint8_t height;
} grid_t;

/**
 * Structure holding a single x y coordinate.
 */
typedef struct {
    uint8_t x;
    uint8_t y;
} pos_t;

/**
 * Print the raw data state of the given grid.
 *
 * @param grid Pointer to grid that should be printed
 */
void print_grid(grid_t* grid);

/**
 * Print the data state of the given grid, interpreting data as a ship board.
 * Format per square is: {DATA (Ship Ref)}{Hit/Miss}{Confirmed Destroy}
 * 
 * @param grid Pointer to grid that should be printed
 */
void print_ship_grid(grid_t* grid);

/**
 * Check whether a given position on a grid has been marked as shot. It should be noted
 * that a blocked grid position will appear as shot.
 * 
 * @param  grid Grid to check
 * @param x     x coordinate
 * @param y     y coordinate
 * @return      Whether position was shot
 */
bool is_pos_shot(grid_t* grid, int8_t x, int8_t y);

/**
 * Check whether a given position on a grid has been marked as destroyed. It should be noted
 * that a blocked grid position will appear as destroyed.
 * 
 * @param  grid Grid to check
 * @param x     x coordinate
 * @param y     y coordinate
 * @return      Whether position was destroyed
 */
bool is_pos_destroyed(grid_t* grid, int8_t x, int8_t y);

/**
 * Mark that the given position on a grid has been shot. No checks are applied to
 * see whether it has already been shot.
 * 
 * @param grid Grid to mark
 * @param x    x coordinate
 * @param y    y coordinate
 */
void mark_shot(grid_t* grid, int8_t x, int8_t y);

/**
 * Mark that the given position on a grid has a destroyed ship on. No checks are applied to
 * see whether it has already been destroyed or is actually shot.
 * 
 * @param grid Grid to mark
 * @param x    x coordinate
 * @param y    y coordinate
 */
void mark_destroyed(grid_t* grid, int8_t x, int8_t y);

/**
 * Allocate the required memory for a grid.
 * 
 * @param grid  Grid with width/height set
 * @param clear Whether to clear the data to 0's
 */
void allocate_grid_data(grid_t* grid, bool clear);

/**
 * Set all grid locations to 0.
 * 
 * @param grid Grid to update
 */
void zero_grid_data(grid_t* grid);

/**
 * Get the grid date for a given x y coordinate. If the x y coordinate is not on the grid
 * a blocked position indicator is returned.
 *
 * @param  grid Grid configuration to fetch from
 * @param  x    x coordinate
 * @param  y    y coordinate
 * @return      Data currently in grid
 */
g_data get_grid_data(grid_t* grid, int8_t x, int8_t y);

/**
 * Set the grid data for a given x y coordinate. If the x y coordinate is not on the grid
 * no data is set.
 *
 * @param grid Grid configuration to update
 * @param x    x coordinate
 * @param y    y coordinate
 * @param data Date to set on grid
 */
void set_grid_data(grid_t* grid, int8_t x, int8_t y, g_data data);


/**
 * Map an x y coordinate to its respective data location for the given grid configuration.
 * If the coordinate cannot be mapped, a blocked position indicator is returned.
 *
 * @param  grid Grid configuration to use for mapping
 * @param  x    x coordinate
 * @param  y    y coordinate
 * @return      Mapped x y coordinate
 */
int16_t map_grid_pos(grid_t* grid, int8_t x, int8_t y);

#endif // GRID_H