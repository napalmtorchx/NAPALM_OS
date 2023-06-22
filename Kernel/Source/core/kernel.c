#include <core/kernel.h>

extern uint32_t _kernel_start;
extern uint32_t _kernel_end;
extern uint32_t _stack_top;
extern uint32_t _stack_bottom;

static multiboot_t* _mbootptr;
static ramfs_t      _bootfs;
static kbd_state_t  _kbstate;

void boot_sequence(void)
{
    uart_setport(UARTPORT_COM1);
    gdt_init();
    idt_init();
    memmgr_init();
    heap_init();
    devmgr_init();
    virtfs_init();
    taskmgr_init();
    cmdhost_init();
    memmgr_gcinit();
    taskmgr_toggle(true);
    cmdhost_prompt();

    display_clear(COLOR_DARKMAGENTA);
    display_drawstr(0, 0, "Hello world\nTesting 123", 0xFFFFFFFF, 0xFF7F0000, font_get_default());
    while (true);
}

void kernel_main(multiboot_t* mboot)
{
    _mbootptr = mboot;
    boot_sequence();

    int last = 0;
    while (true)
    {
        lock();
        time_t now = time(NULL);
        if (last != now.second)
        {
            last = now.second;
            krnl_redraw();
        }
        cmdhost_update();
        unlock();
    }
}

void krnl_redraw(void)
{
    time_t now = time(NULL);
    for (int i = 0; i < vga_get()->w; i++) { vga_drawchar(i, 0, 0x20, VGA_WHITE, VGA_DARKRED); }
    char* tmstr[64];
    memset(tmstr, 0, sizeof(tmstr));
    timestr(&now, tmstr, TIMEFORMAT_STANDARD, true);
    vga_drawstr(vga_get()->w - strlen(tmstr), 0, tmstr, VGA_WHITE, VGA_DARKRED);
    vga_drawstr(0, 0, "napalm-os", VGA_RED, VGA_DARKRED);
}

uintptr_t krnl_get_addr(void) { return (uint32_t)&_kernel_start; }

uintptr_t krnl_get_addr_end(void) { return (uint32_t)&_kernel_end; }

size_t krnl_get_sz(void) { return krnl_get_addr_end() - krnl_get_addr(); }

uintptr_t krnl_get_stk_btm(void) { return (uint32_t)&_stack_bottom; }

uintptr_t krnl_get_stk_top(void) { return (uint32_t)&_stack_top; }

size_t krnl_get_stk_sz(void) { return krnl_get_stk_top() - krnl_get_stk_btm(); }

multiboot_t* mboot_get(void) { return _mbootptr; }

ramfs_t* bootfs_get(void) { return &_bootfs; }