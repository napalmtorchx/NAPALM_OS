#include <core/threading/thread.h>
#include <core/kernel.h>

thread_t* THIS_THREAD;
thread_t* NEXT_THREAD;

void thread_return(void)
{
    register int eax asm("eax");
    int exitcode = eax;

    IRQ_DISABLE;
    taskmgr_kill(THIS_THREAD);
    debug_out("%s Thread %p(%s) has exited with code %d\n", DEBUG_INFO, THIS_THREAD->uid, THIS_THREAD->name, exitcode);

    IRQ_ENABLE;
    while (true) { taskmgr_schedule(true); }
}

thread_t* thread_create(const char* name, size_t stacksz, thread_protocol_t protocol, THREAD_PRIORITY priority, int argc, char** argv)
{
    thread_t* thread = (thread_t*)malloc(sizeof(thread_t));
    strcpy(thread->name, name);

    thread->uid  = taskmgr_generate_uid();
    thread->argc = argc;
    thread->argv = argv;

    thread->context.lock     = UNLOCKED;
    thread->context.priority = priority;
    thread->context.protocol = protocol;
    thread->context.stacksz  = memalign(stacksz, 0x1000);
    thread->context.stack    = malloc(thread->context.stacksz);
    thread->context.state    = THREAD_HALTED;

    uint32_t* stk = (uint32_t*)((uintptr_t)thread->context.stack + (thread->context.stacksz - sizeof(thread_regs_t) - 20));
    *--stk = (uint32_t)thread;
    *--stk = (uint32_t)thread->argv;
    *--stk = (uint32_t)thread->argc;
    *--stk = (uint32_t)thread_return;
    *--stk = (uint32_t)thread->context.protocol;

    thread->context.regs.edx    = (uint32_t)thread;
    thread->context.regs.esp    = (uint32_t)stk;
    thread->context.regs.eflags = (uint32_t)0x202;

    thread_print(thread);
    return thread;
}

void thread_free(thread_t* thread)
{
    if (thread == NULL) { debug_exception("thread_free(%p) - Null thread pointer", thread); return; }
    free(thread->context.stack);
    free(thread);
}

void thread_clock(thread_t* thread)
{
    if (thread == NULL) { debug_exception("thread_clock(%p) - Null thread pointer", thread); return; }
    thread->context.time.ticks++;
}

void thread_print(thread_t* thread)
{
    debug_out("%s UID:%p Stack:%p-%p Entry:%p Args:%d Name:%s\n", 
            DEBUG_THREAD, 
            thread->uid,
            (uint32_t)thread->context.stack,
            (uint32_t)(thread->context.stack + thread->context.stacksz),
            thread->context.protocol,
            thread->argc,
            thread->name);
}
