#ifndef GRID_DRAWING_H
#define GRID_DRAWING_H

#include <stdio.h>
#include <stdbool.h>

#include "grid.h"
#include "ship.h"
#include "player.h"

/* Colour definitions */
#define MISS_COLOUR_BG (0xFFFF)
#define MISS_COLOUR_FG (0xF79E)
#define HIT_COLOUR_BG  (0xF000)
#define HIT_COLOUR_FG  (0xD000)

#define SHIP_FG        (0x94B2)
#define SHIP_BG        (0xD6BA)

#define VALID_SEL      (0x4FE0)
#define INVALID_SEL    (0xF800)
#define GRID_BORDER    (0xEF5D)

#define MESSAGE_BOX_BG (0x2124)
#define MESSAGE_BOX_FG (0xFFFF)

#define TEXT_COL       (0xFFFF)
#define PLAYER_ONE_COL (0x01FF)
#define PLAYER_TWO_COL (0xF800)

/* Drawing constants */
#define FONT_WIDTH  (5)
#define FONT_HEIGHT (7)

#define TITLE_HEIGHT (FONT_HEIGHT * 1.5)

/**
 * Structure for configuring positioning, sizing and what is actually
 * drawn whilst utilising drawing functions.
 */
typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    bool ships;
} draw_props_t;


/**
 * Centre a string a above a drawn grid.
 * 
 * @param title      Title to display
 * @param draw_props Dimensions for mapping to drawn grid
 */
void draw_title(char* title, draw_props_t* draw_props);

/**
 * Draw a grid into the given area, this includes a LHS number axis and a bottom letter
 * axis, for this reason the size of squares is {draw property dimension} / (n + 1).
 * Grid squares are filled so contents will be overwritten.
 * 
 * 
 * @param grid        Grid to display, only uses dimensions (width/height)
 * @param draw_props  Dimensions to fit grid into
 */
void draw_grid(grid_t* grid, draw_props_t* draw_props);


/**
 * Draw the shot contents for all grid positions using draw_shot.
 * 
 * @param grid       Grid to use for reference information
 * @param draw_props Dimensions for mapping to drawn grid
 */
void draw_shot_overlay(grid_t* grid, draw_props_t* draw_props);

/**
 * Draw the shot contents of a single grid position, reflecting whether the the grid
 * indicates this as a hit or miss. If there is no shot, nothing is drawn.
 * 
 * @param grid       Grid to use for reference information
 * @param x          x coordinate to check
 * @param y          y coordinate to check
 * @param draw_props Dimensions for mapping to drawn grid
 */
void draw_shot(grid_t* grid, uint8_t x, uint8_t y, draw_props_t* draw_props);

/**
 * Draw all ships provided using draw_ship.
 * 
 * @param grid       Grid for sizing, only uses dimensions (width/height)
 * @param ships      Array of ships to draw
 * @param ship_count Number of ships in array
 * @param draw_props Dimensions for mapping to drawn grid
 */
void draw_ships(grid_t* grid, ship_t ships[], uint8_t ship_count, draw_props_t *draw_props);

/**
 * Draw a single ship as a rectangle (sized for length) mapped to a drawn grid.
 * If drawing properties indicate not to draw ships, nothing will be drawn.
 * 
 * @param grid       Grid for sizing, only uses dimensions (width/height)
 * @param ship       Pointer to ship to draw
 * @param draw_props Dimensions for mapping to drawn grid
 */
void draw_ship(grid_t* grid, ship_t* ship, draw_props_t* draw_props);

/**
 * Draw a single selection square, this uses a stroke so contents of squares will not be overwritten.
 * However, contents that goes over grid boundaries such as ships will need to be redrawn if they
 * want to be on top.
 * 
 * @param grid       Grid for sizing, only uses dimensions (width/height)
 * @param x          x coordinate to draw selection at
 * @param y          y coordinate to draw selection at
 * @param draw_props Dimensions for mapping to drawn grid
 * @param colour     Colour to draw selection
 */
void draw_selection(grid_t* grid, int8_t x, int8_t y, draw_props_t* draw_props, uint16_t colour);

/**
 * Clear a single selection square. The functionality involves redrawing a grid coloured square over
 * any previous selections. As this may cause overlap on ships, any affected ships are redrawn 
 * alongside their shots, this is localised for performance.
 * 
 * @param player     Player who's ships are on the grid (for redrawing)
 * @param x          x coordinate to clear selection at
 * @param y          y coordinate to clear selection at
 * @param draw_props Dimensions for mapping to drawn grid (ships not redrawn if hidden)
 */
void clear_selection(player_t* player, int8_t x, int8_t y, draw_props_t* draw_props);

/**
 * Draw a selection square/rectangle that covers the whole, this uses a stroke so contents of squares
 * will not be overwritten. However, contents that goes over grid boundaries such as ships will need 
 * to be redrawn if they want to be on top.
 * 
 * @param grid       Grid for sizing, only uses dimensions (width/height)
 * @param ship       Ship reference for coordinates, length and direction
 * @param draw_props Dimensions for mapping to drawn grid 
 * @param colour     Colour to draw selection
 */
void draw_ship_selection(grid_t* grid, ship_t* ship, draw_props_t* draw_props, uint16_t colour);

/**
 * Clear a ship selection square/rectangle. The functionality involves redrawing a grid coloured 
 * square/rectangle over any previous selections. As this may cause overlap on ships, all ships 
 * and shots are redrawn (FIXME: Only redraw required).
 * 
 * @param player     Player who's ships are on the grid (for redrawing)
 * @param cur_ship   Ship selection to clear
 * @param draw_props Dimensions for mapping to drawn grid (ships not redrawn if hidden)
 */
void clear_ship_selection(player_t* player, ship_t* cur_ship, draw_props_t* draw_props);

#endif // GRID_DRAWING_H