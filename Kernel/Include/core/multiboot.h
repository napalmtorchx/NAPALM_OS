#pragma once
#include <lib/types.h>

typedef struct
{
    uint16_t attributes;
	uint8_t  win_a, win_b;
	uint16_t granularity;
	uint16_t winsize;
	uint16_t segment_a, segment_b;
	uint32_t real_fct_ptr;
	uint16_t pitch;
	uint16_t res_x, res_y;
	uint8_t  char_w, char_h, planes, bpp, banks;
	uint8_t  memory_model, bank_size, image_pages;
	uint8_t  reserved0;
	uint8_t  red_mask, red_position;
	uint8_t  green_mask, green_position;
	uint8_t  blue_mask, blue_position;
	uint8_t  rsv_mask, rsv_position;
	uint8_t  color_attrs;
	uint32_t physical_base;
	uint32_t reserved1;
	uint16_t reserved2;
} attr_pack vbe_mode_t;

typedef struct
{
    char     signature[4];
    uint16_t version;
    uint16_t oemstr[2];
    uint8_t  capabilities[4];
    uint16_t videomode[2];
    uint16_t totalmem;
} attr_pack vbe_ctrl_t;

typedef struct
{
    uintptr_t addr_start;
    uintptr_t addr_end;
    char*     str;
    uint32_t  reserved;
} attr_pack module_t;

typedef struct
{
    uint32_t ent_size;
    uint64_t address;
    uint64_t size;
    uint32_t type;  
} attr_pack mmap_entry_t;

typedef struct
{
    uintptr_t       flags;
	uintptr_t       mem_lower;
	uintptr_t       mem_upper;
	uintptr_t       boot_device;
	const char*     cmdline;
	size_t          mods_count;
	module_t*       mods_addr;
	uintptr_t       num;
	size_t          size;
	uintptr_t       addr;
	uintptr_t       shndx;
	size_t          mmap_length;
	mmap_entry_t*   mmap_addr;
	uintptr_t       drives_length;
	uintptr_t       drives_addr;
	uintptr_t       config_table;
	const char*     boot_loader_name;
	uintptr_t       apm_table;
	vbe_ctrl_t*     vbe_control_info;
	uintptr_t       vbe_mode_info;
	vbe_mode_t*     vbe_mode;
	uintptr_t       vbe_interface_seg;
	uintptr_t       vbe_interface_off;
	uintptr_t       vbe_interface_len;
} attr_pack multiboot_t;
