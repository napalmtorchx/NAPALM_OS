#include <hal/ports.h>

attr_optimize("O0") void port_wait(int iters)
{
    while (iters > 0) 
    { 
        uint8_t dummy_data_to_slow_the_os_lol[32];
        memset(dummy_data_to_slow_the_os_lol, 0xFF, sizeof(dummy_data_to_slow_the_os_lol));
        iters--; 
    }
}

uint8_t port_inb(uint16_t port)
{
    uint8_t result;
    asm volatile("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

uint16_t port_inw(uint16_t port)
{
    uint16_t result;
    asm volatile("in %%dx, %%ax" : "=a" (result) : "d" (port));
    return result;
}

uint32_t port_inl(uint16_t port)
{
    uint32_t result;
    asm volatile("inl %%dx, %%eax" : "=a" (result) : "dN" (port));
    return result;
}

void port_insw(uint16_t port, uint8_t* buff, size_t size) { asm volatile("rep insw" : "+D" (buff), "+c" (size) : "d" (port) : "memory"); }

void port_outb(uint16_t port, uint8_t data) { asm volatile("out %%al, %%dx" : : "a" (data), "d" (port)); }

void port_outw(uint16_t port, uint16_t data) { asm volatile("out %%ax, %%dx" : : "a" (data), "d" (port)); }

void port_outl(uint16_t port, uint32_t data) { asm volatile("outl %%eax, %%dx" : : "dN" (port), "a" (data)); }

void port_outsw(uint16_t port, uint8_t* data, size_t size) { asm volatile("rep outsw" : "+S" (data), "+c" (size) : "d" (port)); }