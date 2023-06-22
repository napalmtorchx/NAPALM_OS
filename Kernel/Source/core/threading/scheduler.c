#include <core/threading/scheduler.h>
#include <core/kernel.h>

#define SCHEDULER_INTERVAL 30

extern void kernel_main(multiboot_t* mboot);
extern void _context_switch(void);

static ptrlist_t  _threads;
static uint32_t   _uid;
static uint32_t   _timer;
static uint32_t   _index;
static bool       _lock, _ready;
static char       _temp[64];

void taskmgr_init(void)
{
    _uid     = 0x10000000;
    _timer   = 0;
    _index   = 0;
    _lock    = false;
    _ready   = false;
    _threads = ptrlist_create();

    taskmgr_init_kthread();
    debug_out("%s Initialized task manager\n", DEBUG_OK);
}

void taskmgr_init_kthread(void)
{
    thread_t* kthread = (thread_t*)malloc(sizeof(thread_t));
    strcpy(kthread->name, "kernel");
    kthread->argc = 0;
    kthread->argv = NULL;
    kthread->uid  = taskmgr_generate_uid();

    kthread->context.protocol = (thread_protocol_t)kernel_main;
    kthread->context.priority = THREAD_PRIORITY_NORMAL;
    kthread->context.lock     = UNLOCKED;
    kthread->context.state    = THREAD_RUNNING;
    kthread->context.stacksz  = krnl_get_stk_sz();
    kthread->context.stack    = krnl_get_stk_btm();
    thread_print(kthread);
    taskmgr_load(kthread);

    THIS_THREAD = kthread;

    heap_init_kthread(kthread);
}

void taskmgr_schedule(bool unlock)
{
    if (_lock || !_ready) { return; }
    if (_index >= _threads.count) { _index = 0; return; }
    IRQ_DISABLE;
    _lock = true;

    THIS_THREAD = ptrlist_get(&_threads, _index);
    if (THIS_THREAD == NULL) { IRQ_ENABLE; _lock = false; return; }

    if (THIS_THREAD->context.lock == LOCKED)
    {
        if (!unlock) { IRQ_ENABLE; _lock = false; return; }
        THIS_THREAD->context.lock = UNLOCKED;
    }

    while (true)
    {
        _index++;
        if ((size_t)_index >= _threads.count) { _index = 0; NEXT_THREAD = ptrlist_get(&_threads, 0); break; }

        thread_t* thread = ptrlist_get(&_threads, _index);
        if (thread == NULL) { debug_exception("taskmgr_schedule(%d) - Null thread located in scheduler", unlock); return; }
        if (thread->context.state == THREAD_HALTED || thread->context.state == THREAD_WAITING) { continue; }
        if (thread->context.state == THREAD_TERMINATED) { taskmgr_unload(thread); continue; }
        NEXT_THREAD = thread;
        break;
    }

    _lock = false;
    _context_switch();
}

void taskmgr_callback(irq_context_t* context)
{
    if (THIS_THREAD == NULL || !_ready) { _timer = 0; return; }
    thread_clock(THIS_THREAD);

    _timer++;
    if (_timer >= SCHEDULER_INTERVAL) { _timer = 0; taskmgr_schedule(false); }
}

void taskmgr_load(thread_t* thread)
{
    if (thread == NULL) { debug_exception("taskmgr_load(%p) - Null thread pointer", thread); return; }
    ptrlist_add(&_threads, thread);
    debug_out("%s Loaded thread - UID:%p Name:%s\n", DEBUG_INFO, thread->uid, thread->name);
}

void taskmgr_unload_index(int index)
{
    _lock = true;
    if (index < 0 || index >= _threads.count) { debug_exception("taskmgr_unload_index(%d) - Index out of bounds", index); _lock = false; return; }

    thread_t* thread = (thread_t*)ptrlist_get(&_threads, index);
    if (thread->uid == KERNEL_UID) { debug_out("%s Attempt to unload kernel thread was detected\n", DEBUG_WARN); _lock = false; return; }

    uint32_t uid = thread->uid;
    memset(_temp, 0, sizeof(_temp));
    strcpy(_temp, thread->name);

    thread_free(thread);
    ptrlist_remove(&_threads, index, false);
    debug_out("%s Unloaded thread - UID:%p Name:%s\n", DEBUG_INFO, uid, _temp);
    _lock = false;
}

void taskmgr_unload(thread_t* thread)
{
    _lock = true;
    if (thread == NULL) { debug_exception("taskmgr_unload(%p) - Null thread pointer", thread); _lock = false; return; }

    for (size_t i = 0; i < _threads.count; i++)
    {
        if (_threads.entries[i] == thread) { taskmgr_unload_index(i); return; }
    }

    debug_exception("taskmgr_unload(%p) - Failed to unload thread", thread);
    _lock = false;
}

void taskmgr_start(thread_t* thread)
{
    _lock = true;
    if (thread == NULL) { debug_exception("taskmgr_start(%p) - Null thread pointer", thread); _lock = false; return; }
    thread->context.state = THREAD_RUNNING;
    debug_out("%s Started thread - UID:%p Name:%s\n", DEBUG_INFO, thread->uid, thread->name);
    _lock = false;
}

void taskmgr_kill(thread_t* thread)
{
    _lock = true;
    if (thread == NULL) { debug_exception("taskmgr_kill(%p) - Null thread pointer", thread); _lock = false; return; }
    thread->context.state = THREAD_TERMINATED;
    debug_out("%s Terminated thread - UID:%p Name:%s\n", DEBUG_INFO, thread->uid, thread->name);
    _lock = false;
}

void taskmgr_toggle(bool enabled) { _ready = enabled; _lock = false; IRQ_ENABLE; }

void taskmgr_print(bool cmd)
{
    static const char* LN1   = "|-threads---------------------------------------------------------------|\n";
    static const char* LN2   = "|%s UID       STACK                     PROTOCOL NAME                     %s|\n";
    static const char* LNDIV = "|-----------------------------------------------------------------------|\n";

    if (cmd) { printf(LN1); printf(LNDIV); printf(LN2, ANSI_FG_DARKGRAY, ANSI_RESET); printf(LNDIV); }
    else { debug_out(LN1); debug_out(LNDIV); debug_out(LN2, ANSI_FG_DARKGRAY, ANSI_RESET); printf(LNDIV); }

    char line[80];
    char name[60];
    for (size_t i = 0; i < _threads.count; i++)
    {
        thread_t* t = (thread_t*)ptrlist_get(&_threads, i);

        memset(line, 0, sizeof(line));
        memset(name, 0, sizeof(name));
        sprintf(line, "| %p  %p-%p(%a)", t->uid, t->context.stack, (uint32_t)t->context.stack + t->context.stacksz, t->context.stacksz);
        while (strlen(line) < 38) { stradd(line, ' '); }
        sprintf(name, "%p ", t->context.protocol);
        for (int i = 0; i < 24; i++) { if (i < strlen(t->name)) { stradd(name, t->name[i]); } }
        strcat(line, name);
        while (strlen(line) < 72) { stradd(line, ' '); }
        strcat(line, "|\n");
        if (cmd) { printf(line); }
        else { debug_out(line); }
    }

    if (cmd) { printf(LNDIV); } else { debug_out(LNDIV); }
}

uint32_t taskmgr_generate_uid(void) { return _uid++; }

bool taskmgr_exists(thread_t* thread)
{
    if (thread == NULL) { return false; }
    for (size_t i = 0; i < _threads.count; i++) { if (ptrlist_get(&_threads, i) == thread) { return true; } }
    return false;
}

ptrlist_t* taskmgr_list(void) { return &_threads; }

thread_t* taskmgr_from_uid(uint32_t uid)
{
    for (size_t i = 0; i < _threads.count; i++) 
    {
        if (((thread_t*)ptrlist_get(&_threads, i))->uid == uid) { return _threads.entries[i]; } 
    }
    return NULL;
}

void lock()
{
    if (THIS_THREAD == NULL) { return; }
    THIS_THREAD->context.lock = LOCKED;
}

void unlock()
{
    if (THIS_THREAD == NULL) { return; }
    THIS_THREAD->context.lock = UNLOCKED;
}