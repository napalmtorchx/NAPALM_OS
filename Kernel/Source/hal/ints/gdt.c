#include <hal/ints/gdt.h>
#include <core/kernel.h>

#define KERNEL_CS_INDEX 1
#define KERNEL_DS_INDEX 2

extern void _gdt_flush(uint32_t value);

const gdt_access_t KERNEL_CS_ACCESS = (gdt_access_t)
{
    .accessed        = 0,
    .read_write      = 1,
    .dc_flag         = GDT_NONCONF,
    .executable      = true,
    .type            = 1,
    .privilege_level = 0,
    .present         = 1,
};

const gdt_access_t KERNEL_DS_ACCESS = (gdt_access_t)
{
    .accessed        = 0,
    .read_write      = 1,
    .dc_flag         = GDT_UPWARDS,
    .executable      = false,
    .type            = 1,
    .privilege_level = 0,
    .present         = 1,
};

const gdt_flags_t KERNEL_CS_FLAGS = (gdt_flags_t)
{
    .reserved    = 0,
    .long_mode   = false,
    .size        = GDT_32BIT,
    .granularity = GDT_ALIGN_4K,
};

const gdt_flags_t KERNEL_DS_FLAGS = (gdt_flags_t)
{
    .reserved    = 0,
    .long_mode   = false,
    .size        = GDT_32BIT,
    .granularity = GDT_ALIGN_4K,
};

static gdt_reg_t   _reg                attr_align(0x100);
static gdt_entry_t _entries[GDT_COUNT] attr_align(0x100);

void gdt_init(void)
{
    memset(_entries, 0, sizeof(_entries));
    _reg.base  = (uintptr_t)&_entries;
    _reg.limit = (uint16_t)((GDT_COUNT * sizeof(gdt_entry_t)) - 1);

    gdt_set_desc(0, 0, 0, (gdt_access_t){ 0, 0, 0, 0, 0, 0, 0 }, (gdt_flags_t){ 0, 0, 0, 0 });
    gdt_set_desc(KERNEL_CS_INDEX, 0, 0xFFFFFFFF, KERNEL_CS_ACCESS, KERNEL_CS_FLAGS);
    gdt_set_desc(KERNEL_DS_INDEX, 0, 0xFFFFFFFF, KERNEL_DS_ACCESS, KERNEL_DS_FLAGS);
    _gdt_flush((uintptr_t)&_reg);

    debug_out("%s Initialized global descriptor table\n", DEBUG_OK);
}

void gdt_set_desc(uint8_t n, uint32_t base, uint32_t limit, gdt_access_t access, gdt_flags_t flags)
{
    _entries[n].base_low    = base & 0xFFFF;
    _entries[n].base_middle = (base >> 16) & 0xFF;
    _entries[n].base_high   = (base >> 24) & 0xFF;

    _entries[n].limit_low  = limit & 0xFFFF;
    _entries[n].limit_high = (limit >> 16) & 0x0F;

    _entries[n].accessed        = access.accessed;
    _entries[n].read_write      = access.read_write;
    _entries[n].dc_flag         = access.dc_flag;
    _entries[n].executable      = access.executable;
    _entries[n].type            = access.type;
    _entries[n].privilege_level = access.privilege_level;
    _entries[n].present         = access.present;

    _entries[n].reserved    = 0;
    _entries[n].long_mode   = flags.long_mode;
    _entries[n].size        = flags.size;
    _entries[n].granularity = flags.granularity;

    debug_out("%s GDT[%d] - Base:%8x Limit:%8x Access:%2x Flags:%2x\n", DEBUG_INFO, n, base, limit, access, flags);
}