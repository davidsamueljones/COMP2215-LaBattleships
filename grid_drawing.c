#include <string.h>

#include "grid_drawing.h"

#include "lafortuna/lcd/lcd.h"
#include "lafortuna/drawing/drawing.h"


/* Function Prototypes */
void get_ship_constraints(grid_t* grid, ship_t* ship, draw_props_t* draw_props, 
    uint16_t* draw_x, uint16_t* draw_y, uint16_t* draw_width, uint16_t* draw_height);
void pad_draw_area(int8_t pad_x, int8_t pad_y, uint16_t* x, uint16_t* y, 
    uint16_t* width, uint16_t* height);


void draw_title(char* title, draw_props_t* draw_props) {
    uint8_t len = strlen(title);

    // Clear current
    rectangle title_area = {
        .left = draw_props->x, .right = draw_props->x + draw_props->width,
        .top  = draw_props->y - TITLE_HEIGHT, .bottom = draw_props->y - 1
    };
    fill_rectangle(title_area, display.background);

    // Display new title
    display.x = draw_props->x + (draw_props->width - len * FONT_WIDTH) / 2;
    display.y = title_area.top;
    display.foreground = TEXT_COL;
    display_string(title);
}


void draw_grid(grid_t* grid, draw_props_t* draw_props) {
    int16_t pos_draw_width = draw_props->width / (grid->width + 1);
    int16_t pos_draw_height = draw_props->height / (grid->height + 1);

    for (int8_t y = 0; y <= grid->height; y++) {
        for (int8_t x = -1; x < grid->width; x++) {
            display.x = draw_props->x + (x+1) * pos_draw_width;
            display.y = draw_props->y + y * pos_draw_height;
            if (y != grid->height && x != -1) {
                stroke_rectangle(
                    display.x, 
                    display.y, 
                    pos_draw_width, 
                    pos_draw_height, 
                    (GRID_BORDER));
            } else {
                display.x += (pos_draw_width - FONT_WIDTH) / 2 ;
                display.y += (pos_draw_height) / 2;
                display.foreground = TEXT_COL;
                if (x == -1 && y != grid->height) {
                    display_char("0123456789ABCDEF"[grid->height - y - 1]);
                } else if (y == grid->height && x != -1) {
                    display_char("ABCDEFGHIJKLMNO"[x]);
                }
            }
        }
    } 
}


void draw_shot_overlay(grid_t* grid, draw_props_t* draw_props) {
    for (int8_t y = 0; y < grid->height; y++) {
        for (int8_t x = 0; x < grid->width; x++) {
            draw_shot(grid, x, y, draw_props);
        }
    } 
}


void draw_shot(grid_t* grid, uint8_t x, uint8_t y, draw_props_t* draw_props) {
    int16_t pos_draw_width = draw_props->width / (grid->width + 1);
    int16_t pos_draw_height = draw_props->height / (grid->height + 1);
    g_data data = get_grid_data(grid, x, y);
    
    uint16_t draw_x = draw_props->x + (x + 1) * pos_draw_width;
    uint16_t draw_y = draw_props->y + y * pos_draw_height;
    uint16_t draw_height = pos_draw_width;
    uint16_t draw_width  = pos_draw_height;

    int8_t x_padding = pos_draw_width / 1.6;
    int8_t y_padding = pos_draw_height / 1.6;
    pad_draw_area(x_padding, y_padding, &draw_x, &draw_y, &draw_width, &draw_height);

    uint16_t bg_colour;
    uint16_t fg_colour;
    if (IS_MISS(data)) {
        bg_colour = MISS_COLOUR_BG;
        fg_colour = MISS_COLOUR_FG;
    } else if (IS_HIT(data)) {
        bg_colour = HIT_COLOUR_BG;
        fg_colour = HIT_COLOUR_FG;
    } else {
        return;
    }

    stroke_filled_rectangle(
        draw_x, 
        draw_y, 
        draw_width, 
        draw_height, 
        (fg_colour),
        (bg_colour));

}


void draw_ships(grid_t* grid, ship_t ships[], uint8_t ship_count, draw_props_t *draw_props) {
    for (uint8_t ship_idx=0; ship_idx < ship_count; ship_idx++) {
        draw_ship(grid, &ships[ship_idx], draw_props);
    }
}


void draw_ship(grid_t* grid, ship_t* ship, draw_props_t* draw_props) {
    if (!draw_props->ships || !ship->placed) {
        return;
    }

    // Get ship draw area
    uint16_t draw_x, draw_y, draw_width, draw_height;
    get_ship_constraints(grid, ship, draw_props, &draw_x, &draw_y, &draw_width, &draw_height);
    
    // Pad the draw area so it doesn't cover the whole square
    int16_t pos_draw_width = draw_props->width / (grid->width + 1);
    int16_t pos_draw_height = draw_props->height / (grid->height + 1);
    int8_t x_padding = pos_draw_width / 2;
    int8_t y_padding = pos_draw_height / 2;
    pad_draw_area(x_padding, y_padding, &draw_x, &draw_y, &draw_width, &draw_height);

    // Draw rectangle representing the ship
    stroke_filled_rectangle(
        draw_x, 
        draw_y, 
        draw_width, 
        draw_height, 
        (SHIP_FG),
        (SHIP_BG));
}


void draw_selection(grid_t* grid, int8_t x, int8_t y, draw_props_t* draw_props, uint16_t colour) {
    int16_t pos_draw_width = draw_props->width / (grid->width + 1);
    int16_t pos_draw_height = draw_props->height / (grid->height + 1);
    uint16_t draw_x = draw_props->x + (x+1) * pos_draw_width;
    uint16_t draw_y = draw_props->y + y * pos_draw_height;
    stroke_line(draw_x, draw_y, draw_x + pos_draw_width, draw_y, colour);
    stroke_line(draw_x, draw_y, draw_x, draw_y + pos_draw_height, colour);
    stroke_line(draw_x + pos_draw_width, draw_y, draw_x + pos_draw_width, draw_y + pos_draw_height, colour);
    stroke_line(draw_x, draw_y + pos_draw_height, draw_x + pos_draw_width, draw_y + pos_draw_height, colour);
}


void clear_selection(player_t* player, int8_t x, int8_t y, draw_props_t* draw_props) {
    draw_selection(player->grid, x, y, draw_props, GRID_BORDER);
    draw_shot_overlay(player->grid, draw_props);
    // Ships must be redrawn if selection is drawn over them
    g_data sel_data = get_grid_data(player->grid, x, y);
    int8_t ship_idx = (sel_data & POS_DATA) - 1;
    if (ship_idx >= 0) {
        ship_t* ship = &player->ships[ship_idx];
        draw_ship(player->grid, ship, draw_props);
        int8_t ship_x = ship->x;
        int8_t ship_y = ship->y;
        for (uint8_t i = 0; i < ship->length; i++) {
            draw_shot(player->grid, ship_x, ship_y, draw_props);
            move_x_y(&ship_x, &ship_y, ship->dir);
        }
    }
}


void draw_ship_selection(grid_t* grid, ship_t* ship, draw_props_t* draw_props, uint16_t colour) {
    uint16_t draw_x, draw_y, draw_width, draw_height;
    get_ship_constraints(grid, ship, draw_props, &draw_x, &draw_y, &draw_width, &draw_height);

    stroke_line(draw_x, draw_y, draw_x + draw_width, draw_y, colour);
    stroke_line(draw_x, draw_y, draw_x, draw_y + draw_height, colour);
    stroke_line(draw_x + draw_width, draw_y, draw_x + draw_width, draw_y + draw_height, colour);
    stroke_line(draw_x, draw_y + draw_height, draw_x + draw_width, draw_y + draw_height, colour);
}


void clear_ship_selection(player_t* player, ship_t* cur_ship, draw_props_t* draw_props) {
    draw_ship_selection(player->grid, cur_ship, draw_props, GRID_BORDER);
    draw_ships(player->grid, player->ships, player->ship_count, draw_props);
    draw_shot_overlay(player->grid, draw_props);
}

/**
 * Map a ship to a padded rectangle that covers its defined grid positions. Ships going
 * from North or West are drawn as South or East ships respectively though this has
 * no visible difference.
 * 
 * @param grid        Grid for sizing, only uses dimensions (width/height)
 * @param ship        Ship to map to shape
 * @param draw_props  Dimensions for mapping to drawn grid
 * @param draw_x      Pointer to location to update with draw x coordinate
 * @param draw_y      Pointer to location to update with draw y coordinate 
 * @param draw_width  Pointer to location to update with draw width 
 * @param draw_height Pointer to location to update with draw height 
 */
void get_ship_constraints(grid_t* grid, ship_t* ship, draw_props_t* draw_props, 
    uint16_t* draw_x, uint16_t* draw_y, uint16_t* draw_width, uint16_t* draw_height) {
    
    int16_t pos_draw_width = draw_props->width / (grid->width + 1);
    int16_t pos_draw_height = draw_props->height / (grid->height + 1);
    
    // Determine initial constraints
    uint16_t draw_length = (ship->length) * pos_draw_width;
    *draw_width  = pos_draw_width;
    *draw_height = pos_draw_height;
    *draw_x = draw_props->x + (ship->x+1) * pos_draw_width;
    *draw_y = draw_props->y + (ship->y) * pos_draw_height;

    // Map coordinates for ship direction
    switch (ship->dir) {
        case D_North:
        *draw_y -= (draw_length - pos_draw_width);
        // fall through
        case D_South:
        *draw_height = draw_length;
        break;
        
        case D_West:
        *draw_x -= (draw_length - pos_draw_width);
        // fall through
        case D_East:
        *draw_width = draw_length;
        break;
    }
}

/**
 * Pad the given draw coordinates either inwardly (positive pad) or outwardly (negative pad).
 * The padding is applied as a centred amount (coordinates are only shifted by half the pad amount
 * but dimensions have the full padding).
 * 
 * @param pad_x       Amount to pad in the x-axis
 * @param pad_y       Amount to pad in the y-axis
 * @param draw_x      Pointer to x value to update with padding
 * @param draw_y      Pointer to y value to update with padding
 * @param draw_width  Pointer to width value to update with padding
 * @param draw_height Pointer to height value to update with padding
 */
void pad_draw_area(int8_t pad_x, int8_t pad_y, uint16_t* draw_x, uint16_t* draw_y, 
    uint16_t* draw_width, uint16_t* draw_height) {
    *draw_x += pad_x / 2;
    *draw_y += pad_y / 2;
    *draw_width -= pad_x;
    *draw_height -= pad_y;
}
