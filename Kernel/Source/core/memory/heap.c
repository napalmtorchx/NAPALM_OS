#include <core/memory/heap.h>
#include <core/kernel.h>

static memblk_t*  _memblk_ents;
static memblk_t*  _memblk_data;
static heapblk_t* _heapblks;
static size_t     _freecount;

void heap_init(void)
{
    uint32_t total = memmgr_amount_installed(0);
    uint32_t sz = total - (total / 8);

    _freecount   = 0;
    _memblk_ents = memmgr_alloc(HEAP_COUNT * sizeof(heapblk_t), MEM_HEAP);
    _memblk_data = memmgr_alloc(sz, MEM_HEAP);

    _heapblks = (memblk_t*)_memblk_ents->addr;

    heap_create((heapblk_t){ _memblk_data->addr, _memblk_data->sz, HEAPSTATE_FREE, NULL });

    debug_out("%s Initialized heap - Size:%a\n", DEBUG_OK, _memblk_data->sz); 
}

void heap_init_kthread(thread_t* thread)
{
    for (size_t i = 0; i < HEAP_COUNT; i++)
    {
        if (_heapblks[i].state == HEAPSTATE_USED) { _heapblks[i].thread = thread; }
    }
}

void heap_print(bool cmd)
{
    static const char* LN1   = "|-heap-entries-------------------------------|\n";
    static const char* LN2   = "|%s ADDR              STATE   SIZE    THREAD   %s|\n";
    static const char* LNDIV = "|--------------------------------------------|\n";

    if (cmd) { printf(LN1); printf(LNDIV); printf(LN2, ANSI_FG_DARKGRAY, ANSI_RESET); printf(LNDIV); }
    else { debug_out(LN1); debug_out(LNDIV); debug_out(LN2, ANSI_FG_DARKGRAY, ANSI_RESET); printf(LNDIV); }

    char line[47];
    for (size_t i = 0; i < HEAP_COUNT; i++)
    {
        if (_heapblks[i].state == HEAPSTATE_INVALID) { continue; }

        memset(line, 0, sizeof(line));
        sprintf(line, "| %8x-%8x %s %a", _heapblks[i].addr, _heapblks[i].addr + _heapblks[i].sz, heap_statestr(_heapblks[i].state), _heapblks[i].sz);
        while (strlen(line) < 35) { stradd(line, ' '); }
        if (cmd) { printf("%s %8x |\n", line, (_heapblks[i].thread != NULL ? _heapblks[i].thread->uid : 0)); }
        else  { debug_out("%s %8x |\n", line, (_heapblks[i].thread != NULL ? _heapblks[i].thread->uid : 0)); }
    }

    if (cmd) { printf(LNDIV); } else { debug_out(LNDIV); }
}

void* heap_alloc(size_t sz, bool zerofill)
{
    heapblk_t* blk = heap_request(memalign(sz, HEAP_ALIGN));
    if (zerofill) { memset(blk->addr, 0, blk->sz); }
    blk->thread = THIS_THREAD;
    debug_out("%s ADDR:%8x-%8x SIZE:%a(%a)\n", DEBUG_MALLOC, blk->addr, blk->addr + blk->sz, blk->sz, sz);
    return (void*)blk->addr;
}

void heap_free(void* ptr)
{
    if (ptr == NULL) { debug_exception("heap_free(%8x) - Null pointer", ptr); return; }
    if (ptr < _memblk_data->addr || ptr > _memblk_data->addr + _memblk_data->sz) { debug_exception("heap_free(%8x) - Pointer is not located on heap", ptr); return; }

    for (size_t i = 0; i < HEAP_COUNT; i++)
    {
        if (_heapblks[i].addr == (uintptr_t)ptr && _heapblks[i].state == HEAPSTATE_USED)
        {
            _heapblks[i].state = HEAPSTATE_FREE;
            debug_out("%s ADDR:%8x-%8x SIZE:%a\n", DEBUG_FREE, _heapblks[i].addr, _heapblks[i].addr + _heapblks[i].sz, _heapblks[i].sz);
            _freecount++;
            return;
        }
    }
    debug_exception("heap_free(%8x) - Failed to free pointer", ptr);
}

size_t heap_collect(void)
{
    heapblk_t* blk = NULL;
    size_t i, c = 0;

    for (size_t i = 0; i < HEAP_COUNT; i++)
    {
        if (_heapblks[i].state != HEAPSTATE_FREE) { continue; }
        blk = heap_nearest(&_heapblks[i]);
        if (blk != NULL) { c += heap_merge(&_heapblks[i], blk); }
    }

    blk = heap_nearest(&_heapblks[0]);
    if (blk != NULL) { c += heap_merge(&_heapblks[0], blk); }
    return c;
}

void heap_collect_dead(void)
{
    ptrlist_t* tasklist = taskmgr_list();
    size_t i, j;

    for (i = 0; i < HEAP_COUNT; i++)
    {
        if (_heapblks[i].state != HEAPSTATE_USED || _heapblks[i].sz == 0) { continue; }
        if (!taskmgr_exists(_heapblks[i].thread)) { free((void*)_heapblks[i].addr); }
    }
}

bool heap_merge(heapblk_t* blk1, heapblk_t* blk2)
{
    if (blk1 == NULL || blk2 == NULL) { return false; }
    if (blk1->addr > blk2->addr) { blk1->addr = blk2->addr; }
    blk1->sz += blk2->sz;
    heap_remove(blk2);
    return true;
}

heapblk_t* heap_request(size_t sz)
{
    if (sz == 0 || sz > _memblk_data->sz) { debug_exception("heap_request(%a) - Invalid size", sz); return NULL; }

    size_t i;
    for (i = 0; i < HEAP_COUNT; i++)
    {
        if (_heapblks[i].sz == sz && _heapblks[i].state == HEAPSTATE_FREE)
        {
            _heapblks[i].state = HEAPSTATE_USED;
            return &_heapblks[i];
        }
    }

    for (i = 0; i < HEAP_COUNT; i++)
    {
        if (_heapblks[i].sz > sz && _heapblks[i].state == HEAPSTATE_FREE)
        {
            heapblk_t* blk = heap_create((heapblk_t){ _heapblks[i].addr, sz, HEAPSTATE_USED, NULL });
            _heapblks[i].addr += sz;
            _heapblks[i].sz   -= sz;
            return blk;
        }
    }

    debug_exception("heap_request(%a) - Request for allocated memory block failed", sz);
    return NULL;
}

heapblk_t* heap_next(void)
{
    for (size_t i = 0; i < HEAP_COUNT; i++)
    {
        if (_heapblks[i].state == HEAPSTATE_INVALID) { return &_heapblks[i]; }
    }
    return NULL;
}

heapblk_t* heap_nearest(heapblk_t* blk)
{
    if (blk == NULL) { return NULL; }

    for (size_t i = 0; i < HEAP_COUNT; i++)
    {
        if (_heapblks[i].state != HEAPSTATE_FREE) { continue; }
        if (_heapblks[i].addr + _heapblks[i].sz == blk->addr) { return &_heapblks[i]; }
        if (blk->addr - blk->sz == _heapblks[i].addr) { return &_heapblks[i]; }
    }
    return NULL;
}

heapblk_t* heap_create(heapblk_t blk)
{
    if (!heap_validate(&blk)) { debug_exception("heap_create(%8x, %a, %d, %8x) - Invalid block", blk.addr, blk.sz, blk.state, blk.thread); return NULL; }

    heapblk_t* outblk = heap_next();
    *outblk = blk;
    return outblk;
}

void heap_remove(heapblk_t* blk)
{
    if (blk == NULL) { debug_exception("heap_remove(%8x) - Null pointer", blk); return; }

    for (size_t i = 0; i < HEAP_COUNT; i++)
    {
        if (&_heapblks[i] == blk)
        {
            _heapblks[i] = (heapblk_t){ 0, 0, HEAPSTATE_INVALID, NULL };
            return;
        }
    }
    debug_exception("heap_remove(%8x) - Failed to remove heapblk", blk);
}

heapblk_t* heap_from_addr(void* ptr)
{
    if (ptr == NULL) { return NULL; }
    for (size_t i = 0; i < HEAP_COUNT; i++)
    {
        if (_heapblks[i].state != HEAPSTATE_USED) { continue; }
        if (_heapblks[i].addr == (uintptr_t)ptr) { return &_heapblks[i]; }
    }
    return NULL;
}

heapblk_t* heap_from_index(int index)
{
    if (index < 0 || index >= HEAP_COUNT) { return NULL; }
    return &_heapblks[index];
}

bool heap_validate(heapblk_t* blk)
{
    if (blk->addr < _memblk_data->addr || blk->addr > _memblk_data->addr + _memblk_data->sz) { return false; }
    if (blk->sz == 0 || blk->state == HEAPSTATE_INVALID) { return false; }
    return true;
}

size_t heap_count_type(HEAPSTATE state)
{
    size_t count = 0;
    for (size_t i = 0; i < HEAP_COUNT; i++)
    {
        if (_heapblks[i].state == state) { count++; }
    }
    return count;
}

size_t heap_amount_total(void)
{
    return _memblk_data->sz;
}

size_t heap_amount_free(void)
{
    size_t bytes = 0;
    for (size_t i = 0; i < HEAP_COUNT; i++)
    {
        if (_heapblks[i].state == HEAPSTATE_FREE) { bytes += _heapblks[i].sz; }
    }
    return bytes;
}

size_t heap_amount_used(void)
{
    size_t bytes = 0;
    for (size_t i = 0; i < HEAP_COUNT; i++)
    {
        if (_heapblks[i].state == HEAPSTATE_USED) { bytes += _heapblks[i].sz; }
    }
    return bytes;
}

size_t heap_count_freecalls(void) { return _freecount; }

void heap_reset_count_freecalls(void) { _freecount = 0; }

const char* heap_statestr(HEAPSTATE state)
{
    switch (state)
    {
        default:             { return "invalid"; }
        case HEAPSTATE_FREE: { return "free   "; }
        case HEAPSTATE_USED: { return "used   "; }
    }
}