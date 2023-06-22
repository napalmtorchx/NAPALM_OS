#pragma once
#include <lib/types.h>

typedef struct device_t device_t;

typedef bool (*device_start_t)(device_t* dev, void* arg);
typedef int  (*device_stop_t)(device_t* dev);

typedef uint32_t duid_t;

#define NULL_DUID 0x00000000

struct attr_pack device_t
{
    char           name[64];
    duid_t         uid;
    device_start_t fn_start;
    device_stop_t  fn_stop;
    bool           running;
};