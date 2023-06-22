#pragma once
#include <lib/types.h>
#include <core/memory/memmgr.h>
#include <core/threading/thread.h>

#define HEAP_ALIGN 0x1000
#define HEAP_COUNT 32768

typedef enum
{
    HEAPSTATE_INVALID,
    HEAPSTATE_FREE,
    HEAPSTATE_USED,
} HEAPSTATE;

typedef struct
{
    uintptr_t addr;
    size_t    sz;
    HEAPSTATE state;
    thread_t* thread;
} attr_pack heapblk_t;

void heap_init(void);
void heap_init_kthread(thread_t* thread);
void heap_print(bool cmd);

void* heap_alloc(size_t sz, bool zerofill);
void heap_free(void* ptr);
size_t heap_collect(void);
void heap_collect_dead(void);
bool heap_merge(heapblk_t* blk1, heapblk_t* blk2);

heapblk_t* heap_request(size_t sz);
heapblk_t* heap_next(void);
heapblk_t* heap_nearest(heapblk_t* blk);
heapblk_t* heap_create(heapblk_t blk);
void       heap_remove(heapblk_t* blk);

heapblk_t* heap_from_addr(void* ptr);
heapblk_t* heap_from_index(int index);

bool heap_validate(heapblk_t* blk);

size_t heap_count_type(HEAPSTATE state);
size_t heap_amount_total(void);
size_t heap_amount_free(void);
size_t heap_amount_used(void);
size_t heap_count_freecalls(void);
void heap_reset_count_freecalls(void);

const char* heap_statestr(HEAPSTATE state);



