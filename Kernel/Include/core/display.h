#pragma once
#include <lib/types.h>
#include <lib/gfx/color.h>
#include <lib/gfx/point.h>
#include <lib/gfx/rect.h>
#include <lib/gfx/font.h>

void display_init(void);

void display_clear(COLOR color);
void display_blit(int x, int y, COLOR color);
void display_rect_fill(rect_t* bounds, COLOR color);
void display_rect(rect_t* bounds, int pen, COLOR color);
void display_drawchar(int x, int y, char c, COLOR fg, COLOR bg, const font_t* font);
void display_drawstr(int x, int y, const char* str, COLOR fg, COLOR bg, const font_t* font);
void display_load(COLOR* buffer);