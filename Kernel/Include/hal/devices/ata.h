#pragma once
#include <lib/types.h>
#include <hal/device.h>
#include <hal/ints/idt.h>

typedef struct
{
    device_t  base;
    bool      identified;
} ata_device_t;

void ata_init(void);
bool ata_start(ata_device_t* dev, void* unused);
int  ata_stop(ata_device_t* dev);
void ata_callback(irq_context_t* context);

bool ata_identify(void);
void ata_read(uint64_t sector, size_t count, void* src);
void ata_write(uint64_t sector, size_t count, void* dest);