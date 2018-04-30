#ifndef MENU_H
#define MENU_H

#include <stdio.h>
#include <stdbool.h>

#include "lafortuna/lcd/lcd.h"

/**
 * Enumeration of main menu items.
 */
typedef enum {
    OnePlayer,
    TwoPlayerHotseat,
    BothAIs,
} main_menu_option_t;

/**
 * Handle the selection of a main menu item.
 * 
 * @param selection The selected menu item
 */
void handle_main_menu_selection(main_menu_option_t selection);

/**
 * Wait for selection of a main menu item. Redraws the menu on a selection change.
 * Once selection behaviour has finished the loop will be re-entered.
 */
void loop_main_menu_input(void);

/**
 * Draw the main menu screen. This includes a title and all buttons. The selected button
 * will be highlighted.
 * 
 * @param selection The button to highlight
 * @param clear     Whether to first clear the display
 */
void draw_main_menu(main_menu_option_t selection, bool clear);

/**
 * Draw a single menu button, with text centred on it.
 * 
 * @param rec      Size/Positioning of button
 * @param text     Text to display on button
 * @param selected Whether to paint it as selected
 */
void draw_menu_button(rectangle* rec, char* text, bool selected);

/**
 * Draw a string central to a given rectangle. It will centre in both width and height, 
 * even when there are multiple lines. (TODO: Text is currently left-justified only)
 * @param text Text to draw
 * @param rec  Rectangle to use for positioning
 */
void draw_centred_string(char* text, rectangle* rec);

#endif // MENU_H