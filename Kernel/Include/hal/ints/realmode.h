#pragma once
#include <lib/types.h>

typedef struct
{
    uint16_t di, si, bp, sp, bx, dx, cx, ax;
    uint16_t gs, fs, es, ds, eflags;
} attr_pack irq_context16_t;

extern void int32(uint8_t irq, irq_context16_t* context);