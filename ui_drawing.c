#include "ui_drawing.h"

#include "lafortuna/lcd/lcd.h"
#include "lafortuna/drawing/drawing.h"

void draw_game_state(game_t* game, draw_props_t* grid_1_draw_props, draw_props_t* grid_2_draw_props, 
        rectangle* header, rectangle* footer) {
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

void draw_header(rectangle* rec) {
    fill_rectangle(*rec, TITLE_BOX_BG);
    stroke_line(rec->left, rec->bottom+1, rec->right, rec->bottom+1, MESSAGE_BOX_FG);
}

void draw_footer(rectangle* rec) {
    fill_rectangle(*rec, MESSAGE_BOX_BG);
    stroke_line(rec->left, rec->top-1, rec->right, rec->top-1, MESSAGE_BOX_FG);
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
