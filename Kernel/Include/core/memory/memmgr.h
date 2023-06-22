#pragma once
#include <lib/types.h>

typedef enum
{
    MEM_INVALID,
    MEM_FREE,
    MEM_RESERVED,
    MEM_ACPI_RECLAIM,
    MEM_NVS,
    MEM_BADRAM,
    MEM_VRAM,
    MEM_KERNEL,
    MEM_MODULE,
    MEM_PFA,
    MEM_HEAP,
    MEM_ALLOC,
    MEM_UNUSED,
    MEM_VM_READONLY,
    MEM_VM_RW,
    MEM_COUNT,
} MEMTYPE;

typedef struct
{
    uintptr_t addr;
    size_t    sz;
    MEMTYPE   type;
} memblk_t;

void memmgr_print(void);
void memmgr_init(void);
void memmgr_gcinit(void);
int memmgr_gcmain(int argc, char** argv);

memblk_t* memmgr_map(uintptr_t addr, size_t sz, MEMTYPE type);
void memmgr_unmap(memblk_t* memblk);

memblk_t* memmgr_alloc(size_t sz, MEMTYPE type);

memblk_t* memmgr_from_addr(uintptr_t addr);
memblk_t* memmgr_from_index(int index);
memblk_t* memmgr_first_of(MEMTYPE type, size_t minsz);

uint32_t memmgr_amount_installed(int measurement);
uint32_t memmgr_amount_free(int measurement);
uint32_t memmgr_amount_used(int measurement);

const char* memmgr_typestr(MEMTYPE type);