#ifndef UI_DRAWING_H
#define UI_DRAWING_H

#include <stdio.h>
#include <stdbool.h>

#include "grid.h"
#include "ship.h"
#include "player.h"
#include "game.h"

#include "grid_drawing.h"
#include "lafortuna/lcd/lcd.h"

#define PLAYER_ONE_COL (0x01FF)
#define PLAYER_TWO_COL (0xF800)

#define TITLE_BOX_BG   (0x0000)
#define TITLE_BOX_FG   (0xFFFF)

#define MESSAGE_BOX_BG (0x2124)
#define MESSAGE_BOX_FG (0xFFFF)

extern rectangle header;
extern rectangle footer;

/**
 * Initialise UI drawing. This ensures sizing of elements such as the header and footer
 * adjust correctly to the screen.
 */
void init_ui_drawing(void);

/**
 * Draw the full game state. This is a full redraw so is not suggested for minor updates.
 * The game state is defined the current player's shot grid positioned on 'Grid 2' and
 * their target grid positioned on 'Grid 1', alongside a header and footer.
 * 
 * @param game              Game to draw the state of.
 * @param grid_1_draw_props Sizing constraints for 'Grid 1'
 * @param grid_2_draw_props Sizing constraints for 'Grid 2'
 */
void draw_game_state(game_t* game, draw_props_t* grid_1_draw_props, draw_props_t* grid_2_draw_props);

/**
 * Draw a box for the header. Exceeds provided bounds by 1px South.
 * 
 * @param rec Size and position of box.
 */
void draw_header();

/**
 * Draw a box for the footer. Exceeds provided bounds by 1px North.
 * 
 * @param rec Size and position of box.
 */
void draw_footer();

/**
 * Write the current player description for the given player in the header.
 * 
 * @param player Player index
 */
void write_current_player(uint8_t player);

#endif // UI_DRAWING_H