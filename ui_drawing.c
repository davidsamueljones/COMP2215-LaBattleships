#include "ui_drawing.h"

#include "lafortuna/lcd/lcd.h"
#include "lafortuna/drawing/drawing.h"

/* Define header and footer for external use */
rectangle header;
rectangle footer;

void init_ui_drawing(void) {
    header.left   = 0;
    header.right  = display.width - 1;
    header.top    = 0;
    header.bottom = FONT_HEIGHT + 14;

    footer.left   = 0;
    footer.right  = display.width - 1;
    footer.top    = display.height - (FONT_HEIGHT + 14);
    footer.bottom = display.height;
}

void draw_game_state(game_t* game, draw_props_t* grid_1_draw_props, draw_props_t* grid_2_draw_props) {
    player_t* playing = get_current_player(game);
    player_t* other = get_other_player(game);

    draw_header(header);
    draw_footer(footer);

    write_current_player(get_cur_player_idx(game));

    draw_grid(&playing->grid, grid_2_draw_props);
    draw_ships(&playing->grid, playing->ships, playing->ship_count, grid_2_draw_props);
    draw_shot_overlay(&playing->grid, grid_2_draw_props);
    

    draw_grid(&other->grid, grid_1_draw_props);
    draw_shot_overlay(&other->grid, grid_1_draw_props);
}

void draw_header() {
    fill_rectangle(header, TITLE_BOX_BG);
    stroke_line(header.left, header.bottom+1, header.right, header.bottom+1, MESSAGE_BOX_FG);
}

void draw_footer() {
    fill_rectangle(footer, MESSAGE_BOX_BG);
    stroke_line(footer.left, footer.top-1, footer.right, footer.top-1, MESSAGE_BOX_FG);
}

void write_current_player(uint8_t player) {
    char* str = "";
    uint16_t temp_bg = display.background;
    uint16_t temp_fg = display.foreground;
    display.background = TITLE_BOX_BG;
    display.foreground = TEXT_COL;
    display_string_xy("Current Turn: ", 10, 7);
    

    switch (player) {
        case PLAYER_ONE:
        str = "ONE";
        display.foreground = PLAYER_ONE_COL;
        break;
        case PLAYER_TWO:
        str = "TWO";
        display.foreground = PLAYER_TWO_COL;
        break;
    }
    display_string("PLAYER ");
    display_string(str);

    display.background = temp_bg;
    display.foreground = temp_fg;
}
