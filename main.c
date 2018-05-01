#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <avr/eeprom.h>

#include "ui_drawing.h"
#include "menu.h"

#include "lafortuna/os.h"
#include "lafortuna/lcd/lcd.h"

uint16_t EEMEM stored_seed;

void update_seed(void);

int main() {
    os_init();
    update_seed();

    // Load menu
    clear_screen();
    init_ui_drawing();
    loop_main_menu_input();

    // Never exit (should be stuck in menu anyway)
    while (true) {}
}

/**
 * Load the current seed stored in EEPROM, use it as the current seed. 
 * Increment it and write it back.
 */
void update_seed(void) {
  uint16_t seed = eeprom_read_word(&stored_seed);
  srand(seed);
  eeprom_write_word(&stored_seed, seed + 1);
}
