#pragma once
#include <hal/device.h>

#define DEVICE_COUNT 1024
#define DUID_MIN     0xD0000400
#define DUID_MAX     (DUID_MIN + DEVICE_COUNT)

void devmgr_init(void);
duid_t devmgr_register(device_t* dev);
void devmgr_unregister(duid_t dev);
bool devmgr_start(duid_t dev, void* arg);
int devmgr_stop(duid_t dev);

device_t* devmgr_from_uid(duid_t dev);
device_t* devmgr_from_name(const char* name);
device_t* devmgr_try_from_name(const char* name);

bool devmgr_validate(duid_t duid, bool registered);

uint32_t devmgr_count(void);