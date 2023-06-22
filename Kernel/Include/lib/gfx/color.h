#pragma once
#include <lib/types.h>

typedef uint32_t COLOR;

#define COLOR_BLACK         0xFF000000
#define COLOR_DARKBLUE      0xFF00007F
#define COLOR_DARKGREEN     0xFF007F00
#define COLOR_DARKCYAN      0xFF007F7F
#define COLOR_DARKRED       0xFF7F0000
#define COLOR_DARKMAGENTA   0xFF7F007F
#define COLOR_DARKYELLOW    0xFF7F7F00
#define COLOR_GRAY          0xFFAFAFAF
#define COLOR_DARKGRAY      0xFF7F7F7F
#define COLOR_BLUE          0xFF0000FF
#define COLOR_GREEN         0xFF00FF00
#define COLOR_CYAN          0xFF00FFFF
#define COLOR_RED           0xFFFF0000
#define COLOR_MAGENTA       0xFFFF00FF
#define COLOR_YELLOW        0xFFFF0000
#define COLOR_WHITE         0xFFFFFFFF

typedef struct
{
    uint8_t index;
    uint8_t r, g, b;
} color4_t;

typedef struct
{
    uint8_t a, r, g, b;
} argb_desc_t;

typedef union
{
    argb_desc_t desc;
    COLOR       value;
} argb_t;

COLOR  color_create(uint8_t a, uint8_t r, uint8_t g, uint8_t b);
COLOR  color_from_argb(argb_t color);
argb_t color_to_argb(COLOR color);

uint32_t color_dist_squared(argb_t argb, color4_t col4);
color4_t color4_from_argb(argb_t color);
uint8_t  color4_bg_index(color4_t color);