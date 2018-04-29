#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "ship.h"


void print_ships(ship_t ships[], uint8_t count) {
    printf("Ships:\n");
    for (uint8_t ship = 0; ship < count; ship++) {
        printf("  (%d) %-10s - X:%d, Y:%d, D:%d, L:%d, H:%d\n",
               ships[ship].ref, ships[ship].name, ships[ship].x,
               ships[ship].y, ships[ship].dir, ships[ship].length, ships[ship].hits);
    }
}

bool auto_place_ships(grid_t* grid, ship_t ships[], uint8_t count) {
    bool allplaced = false;
    // Allocate data for auto grid placements
    grid_t alloc_grid = {.width = grid->width, .height = grid->height};
    allocate_grid_data(&alloc_grid, false);

    if (alloc_grid.data != NULL) {
        // Auto place ships
        allplaced = true;
        for (uint8_t ship = 0; ship < count; ship++) {
            bool placed = auto_place_ship(grid, &alloc_grid, &ships[ship]);
            allplaced &= placed;
        };
        free(alloc_grid.data);
    }

    return allplaced;
}

bool auto_place_ship(grid_t* ship_grid, grid_t* alloc_grid, ship_t* ship) {
    uint16_t available = gen_availability_grid(ship_grid, alloc_grid, ship->length);
    if (available > 0) {
        uint16_t allocation = 1 + rand() % available;
        if (allocate_ship_pos(alloc_grid, ship, allocation)) {
            place_ship(ship_grid, ship, false);
            return true;
        }
    }
    return false;
}

bool allocate_ship_pos(grid_t* alloc_grid, ship_t* ship, uint16_t allocation) {
    uint16_t ongoing = 0;
    if (allocation > 0) {
        for (ship->x = 0; ship->x < alloc_grid->width; ship->x++) {
            for (ship->y = 0; ship->y < alloc_grid->height; ship->y++) {
                g_data data = get_grid_data(alloc_grid, ship->x, ship->y);
                for (ship->dir = D_North; ship->dir <= D_West; ship->dir++) {
                    // Check if flag is set
                    if (data & (1 << ship->dir)) {
                        ongoing++;
                        if (ongoing == allocation) {
                            // Allocation found
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

uint16_t gen_availability_grid(grid_t* ship_grid, grid_t* alloc_grid, uint8_t length) {
    uint16_t count = 0;
    ship_t ship = {.length = length};
    for (ship.x = 0; ship.x < ship_grid->width; ship.x++) {
        for (ship.y = 0; ship.y < ship_grid->height; ship.y++) {
            // Clear current allocation
            int16_t pos = map_grid_pos(alloc_grid, ship.x, ship.y);
            alloc_grid->data[pos] = 0;
            for (ship.dir = D_North; ship.dir <= D_West; ship.dir++) {
                // Set the direction bit high if placeable
                if (place_ship_valid(ship_grid, &ship)) {
                    alloc_grid->data[pos] |= 1 << ship.dir;
                    count++;
                }
            }
        }
    }
    return count;
}

bool place_ship(grid_t* grid, ship_t* ship, bool verify) {
    if (verify && !place_ship_valid(grid, ship)) {
        return false;
    }
    int8_t x = ship->x;
    int8_t y = ship->y;
    for (uint8_t i = 0; i < ship->length; i++) {
        set_grid_data(grid, x, y, (g_data) ship->ref);
        move_x_y(&x, &y, ship->dir);
    }
    return true;
}

place_valid_t validate_ship_position(grid_t* grid, ship_t* ship) {
    int8_t x = ship->x;
    int8_t y = ship->y;
    place_valid_t validation = PlaceValid;

    for (uint8_t i = 0; i < ship->length; i++) {
        g_data data = get_grid_data(grid, x, y);

        if (data == BLOCKED_POS) {
            return OffGrid;
        } else if (data & POS_DATA) {
            validation = Overlap;
        }
        move_x_y(&x, &y, ship->dir);
    }
    return validation;
}

bool is_ship_destroyed(ship_t* ship) {
    return ship->length == ship->hits;
}

void move_x_y_n(int8_t* x, int8_t* y, dir_t dir, uint8_t n) {
    switch (dir) {
    case D_North:
        *y -= n;
        break;
    case D_East:
        *x += n;
        break;
    case D_South:
        *y += n;
        break;
    case D_West:
        *x -= n;
        break;
    }
}
