#pragma once
#include <lib/types.h>
#include <hal/device.h>

typedef enum
{
    VGA_BLACK,
    VGA_DARKBLUE,
    VGA_DARKGREEN,
    VGA_DARKCYAN,
    VGA_DARKRED,
    VGA_DARKMAGENTA,
    VGA_DARKYELLOW,
    VGA_GRAY,
    VGA_DARKGRAY,
    VGA_BLUE,
    VGA_GREEN,
    VGA_CYAN,
    VGA_RED,
    VGA_MAGENTA,
    VGA_YELLOW,
    VGA_WHITE,
} VGA_COLOR;

typedef struct
{
    device_t  base;
    uint16_t  w,  h;
    uint16_t  cx, cy;
    VGA_COLOR fg, bg;
    bool      text_mode;
    uint8_t*  fbptr;
} vga_device_t;

void vga_init(void);
bool vga_start(vga_device_t* dev, VGA_COLOR bg);
int  vga_stop(vga_device_t* dev);
void vga_clear(VGA_COLOR bg);
void vga_drawchar(int x, int y, char c, VGA_COLOR fg, VGA_COLOR bg);
void vga_drawstr(int x, int y, const char* str, VGA_COLOR fg, VGA_COLOR bg);
void vga_newline(void);
void vga_delete(void);
void vga_scroll(void);
void vga_putc(char c);
void vga_write(const char* str);
void vga_writeln(const char* str);
void vga_setpos(int x, int y);

uint8_t vga_pack(VGA_COLOR fg, VGA_COLOR bg);

vga_device_t* vga_get(void);