#include <string.h>
#include <stdio.h>

#include "ui_drawing.h"

#include "lafortuna/lcd/lcd.h"
#include "lafortuna/drawing/drawing.h"

#define DRAW_AREA_LEFT_MARGIN   (10)
#define DRAW_AREA_RIGHT_MARGIN  (10)

/* Define header and footer for external use */
rectangle header;
rectangle footer;
rectangle draw_area;

void init_ui_drawing(void) {
    header.left   = 0;
    header.right  = display.width - 1;
    header.top    = 0;
    header.bottom = FONT_HEIGHT + 14;

    footer.left   = 0;
    footer.right  = display.width - 1;
    footer.top    = display.height - (FONT_HEIGHT + 14);
    footer.bottom = display.height;

    draw_area.left = DRAW_AREA_LEFT_MARGIN;
    draw_area.right = display.width - DRAW_AREA_RIGHT_MARGIN;
    draw_area.top = header.bottom;
    draw_area.bottom = footer.top;
}


void draw_game_state(game_t* game, draw_props_t* grid_1_draw_props, draw_props_t* grid_2_draw_props) {
    player_t* playing = get_current_player(game);
    player_t* next = get_next_player(game);

    draw_header(header);
    draw_footer(footer);

    write_current_player(game->turn);
    write_current_turn(game->shots / 2 + 1);

    draw_title("YOUR SHOTS", grid_1_draw_props);
    draw_title("YOUR SHIPS", grid_2_draw_props);
    draw_player_boards(grid_1_draw_props, grid_2_draw_props, next, playing);
}

void draw_finish_state(game_t* game, draw_props_t* grid_1_draw_props, draw_props_t* grid_2_draw_props) {
    player_t* player_one = get_player(game, PLAYER_ONE);
    player_t* player_two = get_player(game, PLAYER_TWO);


    draw_header(header);
    draw_footer(footer);

    uint8_t winner = get_player_idx(game, get_next_player(game));
    write_winner(winner);


    draw_title("PLAYER " PLAYER_ONE_STR_UP "'S SHIPS", grid_1_draw_props);
    draw_title("PLAYER " PLAYER_TWO_STR_UP "'S SHIPS", grid_2_draw_props);
    draw_player_boards(grid_1_draw_props, grid_2_draw_props, player_two, player_one);
}

void draw_player_boards(draw_props_t* grid_1_draw_props, draw_props_t* grid_2_draw_props, 
        player_t* left_player, player_t* right_player) {

    draw_grid(left_player->grid, grid_1_draw_props);
    draw_ships(left_player->grid, left_player->ships, left_player->ship_count, grid_1_draw_props);
    draw_shot_overlay(left_player->grid, grid_1_draw_props);

    draw_grid(right_player->grid, grid_2_draw_props);
    draw_ships(right_player->grid, right_player->ships, right_player->ship_count, grid_2_draw_props);
    draw_shot_overlay(right_player->grid, grid_2_draw_props);
}

void draw_header() {
    fill_rectangle(header, TITLE_BOX_BG);
    stroke_line(header.left, header.bottom + 1, header.right, header.bottom + 1, MESSAGE_BOX_FG);
}


void draw_footer() {
    fill_rectangle(footer, MESSAGE_BOX_BG);
    stroke_line(footer.left, footer.top - 1, footer.right, footer.top - 1, MESSAGE_BOX_FG);
}

void write_current_turn(uint8_t turn) {
    uint16_t temp_bg = display.background;
    uint16_t temp_fg = display.foreground;
    display.background = TITLE_BOX_BG;
    display.foreground = TEXT_COL;
    char buf[20];
    sprintf(buf, "(Turn %d)", turn);
    display_string_xy(buf, display.width - 60, 7);

    display.background = temp_bg;
    display.foreground = temp_fg;
}


void write_current_player(uint8_t player) {
    uint16_t temp_bg = display.background;
    uint16_t temp_fg = display.foreground;
    display.background = TITLE_BOX_BG;
    display.foreground = TEXT_COL;
    display_string_xy("Current Turn: ", 10, 7);
    write_player(player);

    display.background = temp_bg;
    display.foreground = temp_fg;
}

void write_winner(uint8_t player) {

    uint16_t temp_bg = display.background;
    uint16_t temp_fg = display.foreground;
    display.background = TITLE_BOX_BG;
    display.foreground = TEXT_COL;
    display_string_xy("WINNER: ", 10, 7);
    write_player(player);

    display.background = temp_bg;
    display.foreground = temp_fg;
}

void write_player(uint8_t player) {
    char* str = "";
    switch (player) {
    case PLAYER_ONE:
        str = PLAYER_ONE_STR_UP;
        display.foreground = PLAYER_ONE_COL;
        break;
    case PLAYER_TWO:
        str = PLAYER_TWO_STR_UP;
        display.foreground = PLAYER_TWO_COL;
        break;
    }
    display_string("PLAYER ");
    display_string(str);
}

void generate_two_grid_view(draw_props_t* grid_1, draw_props_t* grid_2, double weight) {
    uint16_t middle_split = 10;
    uint16_t draw_width = draw_area.right - draw_area.left - middle_split;
    uint16_t draw_height = draw_area.bottom - draw_area.top;

    // Create Grid 1 (aligned left)
    grid_1->width = draw_width * weight;
    grid_1->height = grid_1->width;
    grid_1->x = draw_area.left,
            grid_1->y = header.bottom + (draw_height - grid_1->height + TITLE_HEIGHT) / 2;

    // Create Grid 2 (aligned right)
    grid_2->width = draw_width * (1 - weight);
    grid_2->height = grid_2->width;
    grid_2->x = draw_area.right - grid_2->width;
    grid_2->y = header.bottom + (draw_height - grid_2->height + TITLE_HEIGHT) / 2;
}


void generate_one_grid_view(draw_props_t* grid) {
    uint16_t draw_height = draw_area.bottom - draw_area.top;

    // Create Grid 1 (aligned left)
    grid->width = draw_height * 0.8;
    grid->height = grid->width;
    grid->x = (draw_area.right - grid->width) / 2;
    grid->y = draw_area.top + (draw_height - grid->height + TITLE_HEIGHT) / 2;
}

void draw_centred_string(char* text, rectangle* rec) {
    uint8_t len = strlen(text);
    uint8_t text_width = 0;
    uint8_t line_width = 0;
    uint8_t text_height = FONT_HEIGHT;
    for (uint8_t i=0; i < len; i++) {
        if (text[i] == '\n') {
            if (line_width > text_width) {
                text_width = line_width;
            }
            line_width = 0;
            text_height += LINE_FEED_HEIGHT;
        } else {
            line_width += (FONT_WIDTH + 1);
        }
    }
    if (line_width > text_width) {
        text_width = line_width;
    }

    display.x = rec->left + (rec->right - rec->left - text_width) / 2;
    display.y = rec->top + (rec->bottom - rec->top - text_height) / 2;
    display_string_xy(text, display.x, display.y);
}