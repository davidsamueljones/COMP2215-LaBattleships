#include "ili934x.h"
#include "lcd.h"
#include "drawing.h"

void stroke_line_straight(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t col);
void stroke_line_low(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t col);
void stroke_line_high(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t col);
uint16_t* create_circle(uint16_t r);

void draw_pixel(uint16_t x, uint16_t y, uint16_t col)
{
  write_cmd(COLUMN_ADDRESS_SET);
  write_data16(x);
  write_data16(x);
  write_cmd(PAGE_ADDRESS_SET);
  write_data16(y);
  write_data16(y);
  write_cmd(MEMORY_WRITE);

  write_data16(col);
}

void stroke_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t col)
{
  if(x1==x2)
  {
    if(y1 > y2)
    {
      stroke_line_straight(x1, y2, x2, y1, col);
    }
    else
    {
      stroke_line_straight(x1, y1, x2, y2, col);
    }
  }
  else if(y1==y2)
  {
    if(x1 > x2)
    {
      stroke_line_straight(x2, y1, x1, y2, col);
    }
    else
    {
      stroke_line_straight(x1, y1, x2, y2, col);
    }
  }
  else if(abs(y2-y1) < abs(x2-x1))
  {
    if(x1 > x2)
    {
      stroke_line_low(x2, y2, x1, y1, col);
    }
    else
    {
      stroke_line_low(x1, y1, x2, y2, col);
    }
  }
  else
  {
    if(y1 > y2)
    {
      stroke_line_high(x2, y2, x1, y1, col);
    }
    else
    {
      stroke_line_high(x1, y1, x2, y2, col);
    }
  }
}

void stroke_line_straight(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t col)
{
  write_cmd(COLUMN_ADDRESS_SET);
  write_data16(x1);
  write_data16(x2);
  write_cmd(PAGE_ADDRESS_SET);
  write_data16(y1);
  write_data16(y2);
  write_cmd(MEMORY_WRITE);

  uint16_t h,w,i,j;
  h = y2-y1;
  w = x2-x1;

  for(i = 0; i<=h; i++)
  {
    for(j = 0; j<=w; j++)
    {
      write_data16(col);
    }
  }
}

void stroke_line_low(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t col)
{
  int dx, dy, D, yi;

  dx = x2 - x1;
  dy = y2 - y1;
  yi = 1;
  if(dy < 0)
  {
    yi = -1;
    dy = -dy;
  }
  D = 2*dy - dx;

  uint16_t x, y;
  y = y1;
  for(x = x1; x<=x2; x++)
  {
    draw_pixel(x, y, col);

    if(D > 0)
    {
      y += yi;
      D -= 2*dx;
    }

    D += 2*dy;
  }
}

void stroke_line_high(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t col)
{
  int dx, dy, D, xi;

  dx = x2 - x1;
  dy = y2 - y1;
  xi = 1;
  if(dx < 0)
  {
    xi = -1;
    dx = -dx;
  }
  D = 2*dx - dy;

  uint16_t x, y;
  x = x1;
  for(y = y1; y<=y2; y++)
  {
    draw_pixel(x, y, col);

    if(D > 0)
    {
      x += xi;
      D -= 2*dy;
    }

    D += 2*dx;
  }
}

void stroke_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t col)
{
  stroke_filled_rectangle(x, y, w, h, col, display.background);
}

void stroke_filled_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t col, uint16_t col2) 
{
  write_cmd(COLUMN_ADDRESS_SET);
  write_data16(x);
  write_data16(x+w);
  write_cmd(PAGE_ADDRESS_SET);
  write_data16(y);
  write_data16(y+h);
  write_cmd(MEMORY_WRITE);

  uint16_t i,j;
  for(i = 0; i<=h; i++)
  {
    write_data16(col);
    for(j = 1; j<=w-1; j++)
    {
      if(i == 0 || i == h)
      {
        write_data16(col);
      }
      else
      {
        write_data16(col2);
      }
    }
    write_data16(col);
  }
}

void stroke_rounded_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, uint16_t col)
{
  stroke_rectangle(x, y, w, h, col);

  stroke_rectangle(x, y, r, r, display.background);
  stroke_rectangle(x+w-r, y, r, r, display.background);
  stroke_rectangle(x, y+h-r, r, r, display.background);
  stroke_rectangle(x+w-r, y+h-r, r, r, display.background);

  uint16_t *xs = create_circle(r);

  uint16_t i;
  for(i = 0; i<r; i++)
  {
    /* Top left */
    draw_pixel((x+r) - xs[i], (y+r) - i, col);
    draw_pixel((x+r) - i, (y+r) - xs[i], col);

    /* Top right */
    draw_pixel((x+w-r) + xs[i], (y+r) - i, col);
    draw_pixel((x+w-r) + i, (y+r) - xs[i], col);

    /* Bottom left */
    draw_pixel((x+r) - xs[i], (y+w-r) + i, col);
    draw_pixel((x+r) - i, (y+w-r) + xs[i], col);

    /* Bottom right */
    draw_pixel((x+w-r) + xs[i], (y+h-r) + i, col);
    draw_pixel((x+w-r) + i, (y+h-r) + xs[i], col);
  }

  if(xs)
    free(xs);
}

void fill_rounded_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, uint16_t col)
{
  rectangle rec = {x, x+w, y, y+h};
  fill_rectangle(rec, col);

  rec.left = x;
  rec.right = x+r;
  rec.top = y;
  rec.bottom = y+r;
  fill_rectangle(rec, display.background);
  rec.left = x+w-r;
  rec.right = x+w;
  fill_rectangle(rec, display.background);
  rec.top = y+h-r;
  rec.bottom = y+h;
  fill_rectangle(rec, display.background);
  rec.left = x;
  rec.right = x+r;
  fill_rectangle(rec, display.background);

  uint16_t *xs = create_circle(r);

  uint16_t i;
  for(i = 0; i<r; i++)
  {
    /* Top left */
    stroke_line((x+r) - xs[i], (y+r) - i, (x+r) + xs[i], (y+r) - i, col);

    /* Top right */
    stroke_line((x+w-r) + xs[i], (y+r) - i, (x+w-r) - xs[i], (y+r) - i, col);

    /* Bottom left */
    stroke_line((x+r) - i, (y+w-r) + xs[i], (x+r) - i, (y+w-r) - xs[i], col);

    /* Bottom right */
    stroke_line((x+w-r) + i, (y+w-r) - xs[i], (x+w-r) + i, (y+w-r) + xs[i], col);
  }

  if(xs)
    free(xs);
}

void stroke_circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t col)
{
  uint16_t *xs = create_circle(r);

  uint16_t y;
  for(y = 0; y<r; y++)
  {
    draw_pixel(x0 + xs[y], y0 + y, col);
    draw_pixel(x0 + y, y0 + xs[y], col);

    int xPos = x0 - y;
    int yPos;

    if(xPos >= 0)
    {
      draw_pixel(x0 - y, y0 + xs[y], col);

      yPos = y0 - xs[y];
      if(yPos >= 0)
      {
        draw_pixel(x0 - y, y0 - xs[y], col);
      }
    }

    xPos = x0 - xs[y];
    if(xPos >= 0)
    {
      draw_pixel(x0 - xs[y], y0 + y, col);

      yPos = y0 - y;
      if(yPos >= 0)
      {
        draw_pixel(x0 - xs[y], y0 - y, col);
      }
    }

    yPos = y0 - xs[y];
    if(yPos >= 0)
      draw_pixel(x0 + y, y0 - xs[y], col);

    yPos = y0 - y;
    if(yPos >= 0)
      draw_pixel(x0 + xs[y], y0 - y, col);
  }

  if(xs)
    free(xs);
}

void fill_circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t col)
{
  uint16_t *xs = create_circle(r);

  uint16_t y;
  for(y = 0; y<r; y++)
  {
    int xPos = x0 - xs[y];
    int yPos = y0 - y;

    if(xPos < 0)
      xPos = 0;

    stroke_line(xPos, y0 + y, x0 + xs[y], y0 + y, col);

    if(yPos >= 0)
      stroke_line(xPos, yPos, x0 + xs[y], yPos, col);
  }

  if(xs)
    free(xs);
}

uint16_t* create_circle(uint16_t r)
{
  uint16_t *xs = malloc(r*sizeof(uint16_t));

  int x = r - 1;
  int y = 0;
  int dx = 1;
  int dy = 1;
  int err = dx - (r << 1);

  while(x >= y)
  {
    xs[y] = x;
    xs[x] = y;

    if(err <= 0)
    {
      y++;
      err += dy;
      dy += 2;
    }

    if(err > 0)
    {
      x--;
      dx += 2;
      err += dx - (r << 1);
    }
  }

  return xs;
}
