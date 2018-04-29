#include <stdint.h>
#include <stdlib.h>

void draw_pixel(uint16_t x, uint16_t y, uint16_t col);
void stroke_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t col);
void stroke_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t col);
void stroke_filled_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t col, uint16_t col2);
void stroke_rounded_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, uint16_t col);
void fill_rounded_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, uint16_t col);
void stroke_circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t col);
void fill_circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t col);
