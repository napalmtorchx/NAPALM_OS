#pragma once
#include <lib/types.h>
#include <lib/time.h>
#include <hal/device.h>
#include <hal/ints/idt.h>

typedef struct
{
    device_t base;
    time_t   tm;
    uint32_t ticks;
    uint32_t tps, timer;
    float    seconds;
    uint32_t millis, millis_t;
} rtc_device_t;

void rtc_init(void);
bool rtc_start(rtc_device_t* dev, void* unused);
int  rtc_stop(rtc_device_t* dev);
void rtc_update(void);
void rtc_callback(irq_context_t* context);
uint8_t rtc_rdreg(uint8_t reg);
bool rtc_updating(void);