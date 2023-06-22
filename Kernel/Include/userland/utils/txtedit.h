#pragma once
#include <lib/types.h>
#include <core/threading/thread.h>
#include <hal/devices/kbd.h>

typedef struct
{
    thread_t*    thread;
    kbd_state_t  kbstate;
    uint32_t     w, h;
    int          cx, cy;
    char*        fname;
    char*        data;
    size_t       sz;
} txtedit_t;

txtedit_t* txtedit_create(const char* fname, bool start);
void txtedit_free(txtedit_t* txtedit);
int txtedit_main(int argc, char** argv);