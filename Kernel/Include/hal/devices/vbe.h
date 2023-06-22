#pragma once
#include <lib/types.h>
#include <core/multiboot.h>
#include <hal/device.h>

typedef uint32_t pixel_t;

typedef struct
{
    device_t    base;
    uint16_t    w, h;
    uint32_t*   fbptr;
    vbe_mode_t* mode_hdr;
    vbe_ctrl_t* ctrl_hdr;
} vbe_device_t;

void vbe_init(void);
bool vbe_start(vbe_device_t* dev, pixel_t bg);
int  vbe_stop(vbe_device_t* dev);
bool vbe_setmode(int w, int h);
void vbe_swap(pixel_t* src);
void vbe_clear(pixel_t bg);
void vbe_blit(int x, int y, pixel_t color);
void vbe_fill(int x, int y, int w, int h, pixel_t color);