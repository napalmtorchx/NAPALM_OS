#pragma once
#include <lib/types.h>

#define KERNEL_UID 0x10000000

typedef enum { UNLOCKED, LOCKED } THREAD_LOCK;

typedef enum
{
    THREAD_HALTED,
    THREAD_WAITING,
    THREAD_RUNNING,
    THREAD_TERMINATED,
} THREAD_STATE;

typedef enum
{
    THREAD_PRIORITY_LOW,
    THREAD_PRIORITY_NORMAL,
    THREAD_PRIORITY_HIGH,
} THREAD_PRIORITY;

typedef int (*thread_protocol_t)(int argc, char** argv);

typedef struct
{
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax, eflags;
} attr_pack thread_regs_t;

typedef struct
{
    uint64_t ticks;
} attr_pack thread_time_t;

typedef struct
{
    thread_regs_t     regs;
    thread_time_t     time;
    thread_protocol_t protocol;
    THREAD_STATE      state;
    THREAD_PRIORITY   priority;
    THREAD_LOCK       lock;
    void*             stack;
    size_t            stacksz;
} attr_pack thread_context_t;

typedef struct
{
    thread_context_t context;
    char             name[64];
    uint32_t         uid;
    int              argc;
    char**           argv;
} attr_pack thread_t;

thread_t* thread_create(const char* name, size_t stacksz, thread_protocol_t protocol, THREAD_PRIORITY priority, int argc, char** argv);
void thread_free(thread_t* thread);
void thread_clock(thread_t* thread);
void thread_print(thread_t* thread);

extern thread_t* THIS_THREAD;
extern thread_t* NEXT_THREAD;
