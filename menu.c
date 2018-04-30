#include <math.h>
#include <string.h>

#include "menu.h"
#include "ui_drawing.h"

#include "lafortuna/os.h"
#include "lafortuna/lcd/lcd.h"
#include "lafortuna/drawing/drawing.h"

void handle_main_menu_selection(main_menu_option_t selection) {
    switch (selection) {
        case OnePlayer:
            //play_battleships(false, true);
            break;
        case TwoPlayerHotseat:
            //play_battleships(false, false);
            break;
        break;
        case BothAIs:
            //play_battleships(true, true);
            break;
    }
}


void loop_main_menu_input(void) {
    int8_t cur_selection = OnePlayer;
    draw_main_menu(cur_selection, true);
    int8_t tracked_delta = 0;
    while (true) {
        // Handle current selection
        if (get_switch_short(_BV(SWC))) {
            handle_main_menu_selection(cur_selection);
            cur_selection = 0;
            draw_main_menu(cur_selection, true);
        }

        // Use rotary encoder to capture selection
        int8_t last_selection = cur_selection;
        int8_t delta = os_enc_delta(); 
        if (delta != 0) {
            if (signbit(delta) != signbit(tracked_delta)) {
                tracked_delta = 0;
            }
            tracked_delta += delta;
            if (abs(tracked_delta) >= 3) {
                cur_selection += (tracked_delta > 0 ? 1 : -1);
                tracked_delta = 0;
            }
        }

        // Use buttons to capture selection
        if (get_switch_short(_BV(SWN))) {
            cur_selection--;
        } else if (get_switch_short(_BV(SWS))) {
            cur_selection++;
        }

        // Update screen for current selection
        if (last_selection != cur_selection) {
            cur_selection = (cur_selection) % (BothAIs + 1);
            if (cur_selection < 0) {
                cur_selection = BothAIs - (cur_selection + 1);
            } 
            draw_main_menu(cur_selection, false);   
        }
    }
}


void draw_main_menu(main_menu_option_t selection, bool clear) {
    if (clear) {
        clear_screen();
    }

    int8_t item_count = BothAIs + 1; // (last enum value)

    int16_t button_width = 100;
    int16_t button_height = 30;
    int16_t button_spacing = 10;
    int16_t height_required = button_height * item_count + button_spacing * (item_count - 1);

    for (uint8_t item=0; item < item_count; item++) {
        char* text;

        rectangle button;
        button.left = (display.width - button_width) / 2;
        button.right = button.left + button_width;
        button.top = (display.height - height_required) / 2 + (button_height + button_spacing) * item;
        button.bottom = button.top + button_height;
        
        switch (item) {
            case OnePlayer:
                text = "One Player";
                break;
            case TwoPlayerHotseat:
                text = "Two Player\n (Hotseat)";
                break;
            case BothAIs:
                text = "AI vs AI";
                break;
            default:
                text = "?";
        }
        draw_menu_button(&button, text, selection == item);
    }

    rectangle title_rec = {.left = 0, .right = display.width, 
        .top = 0, .bottom = (display.height - height_required) / 2};

    display.background = 0x0000;
    display.foreground = 0xFFFF;
    draw_centred_string(" BATTLESHIPS\n(David Jones)", &title_rec);
}


void draw_menu_button(rectangle* rec, char* text, bool selected) {
    // Draw menu button
    stroke_filled_rectangle(
        rec->left,
        rec->top,
        rec->right - rec->left,
        rec->bottom - rec->top,
        (selected ? VALID_SEL : MESSAGE_BOX_FG),
        MESSAGE_BOX_BG
    );
    

    // Draw menu button text
    display.foreground = MESSAGE_BOX_FG;
    display.background = MESSAGE_BOX_BG;
    draw_centred_string(text, rec);
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