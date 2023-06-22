#include <lib/gfx/color.h>
#include <core/kernel.h>

static const color4_t COLOR4_PALETTE[] = 
{
    { 0x0, 0x00, 0x00, 0x00 },  
    { 0x1, 0x00, 0x00, 0x7F },
    { 0x2, 0x00, 0x7F, 0x00 },  
    { 0x3, 0x00, 0x7F, 0x7F },
    { 0x4, 0x7F, 0x00, 0x00 },  
    { 0x5, 0x7F, 0x00, 0x7F },
    { 0x6, 0x7F, 0x7F, 0x00 },  
    { 0x7, 0x7F, 0x7F, 0x7F },
    { 0x8, 0x7F, 0x7F, 0x7F }, 
    { 0x9, 0x7F, 0x7F, 0xFF },
    { 0xA, 0x7F, 0xFF, 0x7F }, 
    { 0xB, 0x7F, 0xFF, 0xFF },
    { 0xC, 0xFF, 0x7F, 0x7F }, 
    { 0xD, 0xFF, 0x7F, 0xFF },
    { 0xE, 0xFF, 0xFF, 0x7F }, 
    { 0xF, 0xFF, 0xFF, 0xFF },
};

COLOR color_create(uint8_t a, uint8_t r, uint8_t g, uint8_t b) { return (a << 24) | (r << 16) | (g << 8) | b; }

COLOR color_from_argb(argb_t color) { return color.value; }

argb_t color_to_argb(COLOR color) { return (argb_t){ (color & 0xFF000000) >> 24, (color & 0xFF0000) >> 16, (color & 0xFF00) >> 8, color & 0xFF }; }

uint32_t color_dist_squared(argb_t argb, color4_t col4)
{
    int diffR = argb.desc.r - col4.r;
    int diffG = argb.desc.g - col4.g;
    int diffB = argb.desc.b - col4.b;

    return (diffR * diffR) + (diffG * diffG) + (diffB * diffB);
}

color4_t color4_from_argb(argb_t color)
{
    color4_t closest = COLOR4_PALETTE[0];
    unsigned int min = color_dist_squared(color, closest);

    for (int i = 0; i < sizeof(COLOR4_PALETTE) / sizeof(color4_t); i++) 
    {
        uint32_t distance = color_dist_squared(color, COLOR4_PALETTE[i]);        
        if (distance < min) { min = distance; closest = COLOR4_PALETTE[i]; }
    }
    return closest;
}

uint8_t color4_bg_index(color4_t color)
{
    if (color.index == VGA_DARKGRAY) { color.index = 0; }
    return 0;
}