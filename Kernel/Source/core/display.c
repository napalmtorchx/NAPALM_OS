#include <core/display.h>
#include <core/kernel.h>

static vga_device_t* _vga;
static vbe_device_t* _vbe;

void display_init(void)
{
    _vga = devmgr_try_from_name("vga_controller");
    _vbe = devmgr_try_from_name("vbe_controller");
    if (_vbe != NULL) { _vga = NULL; }

    debug_out("%s Initialized display controller - Device:%s\n", DEBUG_OK, (_vbe != NULL ? _vbe->base.name : _vga->base.name));

    font_init_default();
    display_clear(0xFFFF00FF);
}

void display_clear(COLOR color)
{
    if (_vga != NULL)
    {
        color4_t c = color4_from_argb(color_to_argb(color));
        if (c.index >= 8) { c.index -= 8; }
        vga_clear(c.index);
    }
    else { vbe_clear(color); }
}

void display_blit(int x, int y, COLOR color)
{
    if (_vga != NULL)
    {
        color4_t c = color4_from_argb(color_to_argb(color));
        vga_drawchar(x, y, 0x20, c.index, c.index);
    }
    else { vbe_blit(x, y, color); }
}

void display_rect_fill(rect_t* bounds, COLOR color)
{
    if (bounds == NULL) { return; }

    if (_vga != NULL)
    {
        color4_t c = color4_from_argb(color_to_argb(color));
        for (int i = 0; i < bounds->w * bounds->h; i++)
        {
            vga_drawchar(bounds->x + (i % bounds->w), bounds->y + (i / bounds->w), 0x20, c.index, c.index);
        }
    }
    else { vbe_fill(bounds->x, bounds->y, bounds->w, bounds->h, color); }
}

void display_rect(rect_t* bounds, int pen, COLOR color)
{
    if (bounds == NULL || pen == 0) { return; }

    rect_t r = (rect_t){ bounds->x, bounds->y, bounds->w, pen };
    display_rect_fill(&r, color);

    r = (rect_t){ bounds->x, bounds->y + bounds->h - pen, bounds->w, pen };
    display_rect_fill(&r, color);

    r = (rect_t){ bounds->x, bounds->y + pen, pen, bounds->h - (pen * 2) };
    display_rect_fill(&r, color);

    r = (rect_t){ bounds->x + bounds->w - pen, bounds->y + pen, pen, bounds->h - (pen * 2) };
    display_rect_fill(&r, color);
}

void display_drawchar(int x, int y, char c, COLOR fg, COLOR bg, const font_t* font)
{
    if (_vga != NULL)
    {
        color4_t fg4 = color4_from_argb(color_to_argb(fg));
        color4_t bg4 = color4_from_argb(color_to_argb(bg));
        vga_drawchar(x, y, c, fg4.index, bg4.index >= 8 ? bg4.index - 8 : bg4.index);
    }
    else
    {
        if (font->psf != NULL)
        {
            int fh = font_getsz(font, false).y;
            uint8_t* glyphs = (uint8_t*)((uintptr_t)font->psf + sizeof(psf_hdr_t));

            if (((bg & 0xFF000000) >> 24) > 0) 
            {
                rect_t r = { x, y, font_getsz(font, true).x, font_getsz(font, true).y };
                display_rect_fill(&r, bg); 
            }

            int xx = x;
            for (int j = 0; j < fh; j++)
            {
                int glyph = glyphs[c * fh + j];
                for (int i = 0; i < 8; i++)
                {
                    if ((glyph & 0x80) >= 1) { display_blit(xx, y, fg); }
                    glyph <<= 1;
                    xx++;
                }
                y++;
                xx = x;
            }
        }
    }
}

void display_drawstr(int x, int y, const char* str, COLOR fg, COLOR bg, const font_t* font)
{
    if (str == NULL || strlen(str) == 0) { return; }

    if (_vga != NULL)
    {
        int i = 0, xx = x, yy = y;
        while (str[i] != 0)
        {
            if (str[i] == '\n') { xx = x; yy++; }
            else { display_drawchar(xx, yy, str[i], fg, bg, font); xx++; }
            i++;
        }
    }
    else
    {
        point_t fntsz = font_getsz(font, true);
        int i = 0, xx = x, yy = y;
        while (str[i] != 0)
        {
            if (str[i] == '\n') { xx = x; yy += fntsz.y; }
            else { display_drawchar(xx, yy, str[i], fg, bg, font); xx += fntsz.x; }
            i++;
        }
    }
}

void display_load(COLOR* buffer)
{
    if (_vbe == NULL) { return; }
    memcpy(_vbe->fbptr, buffer, _vbe->w * _vbe->h * 4);
}