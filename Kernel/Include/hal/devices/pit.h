#pragma once
#include <lib/types.h>
#include <hal/device.h>
#include <hal/ints/idt.h>

typedef struct
{
    device_t  base;
    uint32_t  ticks;
    uint32_t  tps, timer, freq;
    uint32_t  millis, millis_t;
} pit_device_t;

void pit_init(void);
bool pit_start(pit_device_t* dev, void* unused);
int  pit_stop(pit_device_t* dev);
void pit_callback(irq_context_t* context);