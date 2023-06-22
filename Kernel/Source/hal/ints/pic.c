#include <hal/ints/pic.h>
#include <core/kernel.h>

void pic_remap(void)
{
    port_outb(0x20, 0x11);
    port_wait(100);
    port_outb(0xA0, 0x11);
    port_wait(100);
    port_outb(0x21, 0x20);
    port_wait(100);
    port_outb(0xA1, 0x28);
    port_wait(100);
    port_outb(0x21, 0x04);
    port_wait(100);
    port_outb(0xA1, 0x02);
    port_wait(100);
    port_outb(0x21, 0x01);
    port_wait(100);
    port_outb(0xA1, 0x01);
    port_wait(100);
    port_outb(0x21, 0x0);
    port_wait(100);
    port_outb(0xA1, 0x0);
    port_wait(100);
}