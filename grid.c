#include <stdlib.h>
#include <string.h>

#include "grid.h"

void print_grid(grid_t* grid) {
    for (int8_t y = 0; y < grid->height; y++) {
        for (int8_t x = 0; x < grid->width; x++) {
            printf("%d ", get_grid_data(grid, x, y));
        }
        printf("\n");
    }
}


void print_ship_grid(grid_t* grid) {
    for (int8_t y = 0; y < grid->height; y++) {
        for (int8_t x = 0; x < grid->width; x++) {
            int16_t data = get_grid_data(grid, x, y);
            printf("%0d%s%s ", data & POS_DATA, data & SHOT_POS ?
                   (IS_HIT(data) ? "H" : "M") : " ", data & DESTROY_POS ? "D" : " ");
        }
        printf("\n");
    }
}


bool is_pos_shot(grid_t* grid, int8_t x, int8_t y) {
    g_data data = get_grid_data(grid, x, y);
    return data & SHOT_POS;
}


bool is_pos_destroyed(grid_t* grid, int8_t x, int8_t y) {
    g_data data = get_grid_data(grid, x, y);
    return data & DESTROY_POS;
}


void mark_shot(grid_t* grid, int8_t x, int8_t y) {
    uint16_t pos = map_grid_pos(grid, x, y);
    grid->data[pos] |= SHOT_POS;
}


void mark_destroyed(grid_t* grid, int8_t x, int8_t y) {
    uint16_t pos = map_grid_pos(grid, x, y);
    grid->data[pos] |= DESTROY_POS;
}


void allocate_grid_data(grid_t* grid, bool clear) {
    // Attempt memory allocation
    uint16_t length = grid->width * grid->height * sizeof(g_data);
    grid->data = malloc(length);
    if (clear) {
        zero_grid_data(grid);
    }
}


void zero_grid_data(grid_t* grid) {
    uint16_t length = grid->width * grid->height * sizeof(g_data);
    if (grid->data != NULL) {
        memset((void*) grid->data, 0, length);
    }
}


g_data get_grid_data(grid_t* grid, int8_t x, int8_t y) {
    int16_t pos = map_grid_pos(grid, x, y);
    return pos == BLOCKED_POS ? BLOCKED_POS : grid->data[pos];
}


void set_grid_data(grid_t* grid, int8_t x, int8_t y, g_data data) {
    int16_t pos = map_grid_pos(grid, x, y);
    if (pos != BLOCKED_POS) {
        grid->data[pos] = data;
    }
}


int16_t map_grid_pos(grid_t* grid, int8_t x, int8_t y) {
    if (x >= 0 && x < grid->width && y >= 0 && y < grid->height) {
        return x * grid->height + y;
    } else {
        return BLOCKED_POS;
    }
}
