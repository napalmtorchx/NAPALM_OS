#pragma once
#include <lib/types.h>
#include <lib/string.h>
#include <lib/math.h>
#include <lib/stdio.h>
#include <lib/stdlib.h>
#include <lib/time.h>
#include <lib/ctype.h>
#include <lib/ansi.h>
#include <lib/ptrlist.h>
#include <lib/gfx/color.h>
#include <lib/gfx/font.h>
#include <lib/gfx/image.h>
#include <lib/gfx/point.h>
#include <lib/gfx/rect.h>
#include <core/multiboot.h>
#include <core/debug.h>
#include <core/cmdhost.h>
#include <core/commands.h>
#include <core/display.h>
#include <core/memory/memmgr.h>
#include <core/memory/heap.h>
#include <core/fs/ramfs.h>
#include <core/fs/virtfs.h>
#include <core/threading/thread.h>
#include <core/threading/scheduler.h>
#include <hal/device.h>
#include <hal/devmgr.h>
#include <hal/ports.h>
#include <hal/uart.h>
#include <hal/ps2.h>
#include <hal/pci.h>
#include <hal/ints/realmode.h>
#include <hal/ints/gdt.h>
#include <hal/ints/idt.h>
#include <hal/ints/pic.h>
#include <hal/devices/vga.h>
#include <hal/devices/vbe.h>
#include <hal/devices/pit.h>
#include <hal/devices/rtc.h>
#include <hal/devices/kbd.h>
#include <hal/devices/ata.h>
#include <userland/utils/txtedit.h>

void krnl_redraw(void);

uintptr_t krnl_get_addr(void);
uintptr_t krnl_get_addr_end(void);
size_t krnl_get_sz(void);

uintptr_t krnl_get_stk_btm(void);
uintptr_t krnl_get_stk_top(void);
size_t krnl_get_stk_sz(void);

multiboot_t* mboot_get(void);

ramfs_t* bootfs_get(void);