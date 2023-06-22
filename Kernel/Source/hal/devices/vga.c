#include <hal/devices/vga.h>
#include <core/kernel.h>

static vga_device_t _vga;

void vga_init(void)
{
    _vga = (vga_device_t)
    {
        .base      = (device_t){ "vga_controller", NULL_DUID, vga_start, vga_stop, false },
        .w         = 80, 
        .h         = 25,  
        .cx        = 0, 
        .cy        = 0,
        .fg        = VGA_WHITE, 
        .bg        = VGA_BLACK,
        .text_mode = true,
        .fbptr     = 0xB8000,
    };
    devmgr_register(&_vga);
    devmgr_start(_vga.base.uid, VGA_BLACK);
}

bool vga_start(vga_device_t* dev, VGA_COLOR bg)
{
    vga_clear(bg);
    return true;
}

int vga_stop(vga_device_t* dev)
{
    return true;
}

void vga_clear(VGA_COLOR bg)
{
    if (!_vga.base.running) { return; }
    _vga.bg = bg;

    if (_vga.text_mode)
    {
        uint16_t chunk = (vga_pack(_vga.fg, _vga.bg) << 8) | 0x20;
        uint32_t data  = (chunk << 16) | chunk;
        memset(_vga.fbptr, data, _vga.w * _vga.h * 2);
    }
    vga_setpos(0, 0);
}

void vga_drawchar(int x, int y, char c, VGA_COLOR fg, VGA_COLOR bg)
{
    if (!_vga.base.running) { return; }
    if ((uint32_t)x >= _vga.w || (uint32_t)y >= _vga.h) { return; }
    uintptr_t offset       = (y * _vga.w + x) * 2;
    _vga.fbptr[offset]     = c;
    _vga.fbptr[offset + 1] = vga_pack(fg, bg);
}

void vga_drawstr(int x, int y, const char* str, VGA_COLOR fg, VGA_COLOR bg)
{
    if (!_vga.base.running) { return; }

    int xx = x, yy = y, i = 0;
    while (str[i] != 0)
    {
        if (str[i] == '\n') { xx = x; yy += 1; }
        else
        {
            vga_drawchar(xx, yy, str[i], fg, bg);
            xx++;
        }
        i++;
    }
}

void vga_newline(void)
{
    if (!_vga.base.running) { return; }

    vga_setpos(0, _vga.cy + 1);
    if (_vga.cy >= _vga.h) { vga_scroll(); }
}

void vga_delete(void)
{
    if (!_vga.base.running) { return; }

    if (_vga.cx > 0)
    {
        vga_setpos(_vga.cx - 1, _vga.cy);
        vga_drawchar(_vga.cx, _vga.cy, 0x20, _vga.fg, _vga.bg);
    }
    else if (_vga.cy > 0)
    {
        vga_setpos(_vga.w - 1, _vga.cy - 1);
        vga_drawchar(_vga.cx, _vga.cy, 0x20, _vga.fg, _vga.bg);
    }
}

void vga_scroll(void)
{
    if (!_vga.base.running) { return; }

    size_t line = _vga.w * 2;
    size_t size = _vga.w * _vga.h * 2;
    memcpy(_vga.fbptr, (void*)((uintptr_t)_vga.fbptr + line), size - line);
    memset16((void*)((uintptr_t)_vga.fbptr + (size - line)), (vga_pack(_vga.fg, _vga.bg) << 8) | 0x20, line);
    vga_setpos(0, _vga.h - 1);
}

void vga_putc(char c)
{
    if (!_vga.base.running) { return; }
    if (c == '\r' || c == '\b') { return; }

    if (c == '\n') { vga_newline(); }
    else if (c == '\t') { vga_write("    "); }
    else
    {
        vga_drawchar(_vga.cx, _vga.cy, c, _vga.fg, _vga.bg);
        vga_setpos(_vga.cx + 1, _vga.cy);
        if (_vga.cx >= _vga.w) { vga_newline(); }
    }
}

void vga_write(const char* str)
{
    if (!_vga.base.running || strlen(str) == 0) { return; }

    VGA_COLOR ofg = _vga.fg, obg = _vga.bg;
    int i = 0;

    while (str[i] != 0)
    {
        if (str[i] == '\x1b')
        {
            i++;
            if (str[i] == '[' && (str[i + 2] == 'm' || str[i + 3] == 'm' || str[i + 4] == 'm'))
            {
                i++;
                char code[4] = { 0, 0, 0, 0 };
                int j = 0;
                while (str[i] != 'm')
                {
                    code[j++] = str[i++];
                }
                i++;

                uint32_t c = atol(code);
                if (c == 0) { _vga.fg = ofg; _vga.bg = obg; }
                else if ((c >= 30 && c <= 37) || (c >= 90  && c <= 97))  { _vga.fg = ansi_fg_to_vga(c, ofg); }
                else if ((c >= 40 && c <= 47) || (c >= 100 && c <= 107)) { _vga.bg = ansi_bg_to_vga(c, obg); }
            }
        }
        else { vga_putc(str[i++]); }
    }

    _vga.fg = ofg;
    _vga.bg = obg;
}

void vga_writeln(const char* str)
{
    if (!_vga.base.running) { return; }
    vga_write(str);
    vga_newline();
}

void vga_setpos(int x, int y)
{
    if (!_vga.base.running) { return; }
    _vga.cx = x;
    _vga.cy = y;

    uint16_t pos = y * _vga.w + x;
	port_outb(0x3D4, 0x0F);
	port_outb(0x3D5, (uint8_t) (pos & 0xFF));
	port_outb(0x3D4, 0x0E);
	port_outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

uint8_t vga_pack(VGA_COLOR fg, VGA_COLOR bg) { return (bg << 4) | fg; }

vga_device_t* vga_get(void) { return &_vga; }