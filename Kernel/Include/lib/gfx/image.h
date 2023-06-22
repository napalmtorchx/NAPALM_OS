#pragma once
#include <lib/types.h>
#include <lib/gfx/color.h>
#include <lib/gfx/font.h>

typedef struct image_t image_t;
struct image_t
{
    int w, h;
    COLOR* buffer;
};

void image_drawchar(image_t* img, int x, int y, char c, COLOR fg, COLOR bg, const font_t* font);