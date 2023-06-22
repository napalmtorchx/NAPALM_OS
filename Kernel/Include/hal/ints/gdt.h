#pragma once
#include <lib/types.h>

#define GDT_CONF    0
#define GDT_NONCONF 1
#define GDT_COUNT   3

#define KERNEL_CS 0x08
#define KERNEL_DS 0x10

typedef enum { GDT_UPWARDS, GDT_DOWNWARDS } GDT_DIR;
typedef enum { GDT_ALIGN_1B, GDT_ALIGN_4K } GDT_GRAN;
typedef enum { GDT_16BIT, GDT_32BIT }       GDT_SIZE;

typedef struct
{
    uint8_t accessed        : 1;
    uint8_t read_write      : 1;
    bool    dc_flag         : 1;
    uint8_t executable      : 1;
    uint8_t type            : 1;
    uint8_t privilege_level : 2;
    uint8_t present         : 1;
} attr_pack gdt_access_t;

typedef struct
{
    uint8_t  reserved    : 1;
    bool     long_mode   : 1;
    GDT_SIZE size        : 1;
    GDT_GRAN granularity : 1;		
} attr_pack gdt_flags_t;

typedef struct
{
    uint16_t  limit_low;
    uint16_t  base_low;
    uint8_t   base_middle;
    uint8_t   accessed        : 1;
    uint8_t   read_write      : 1;
    bool      dc_flag         : 1;
    uint8_t   executable      : 1;
    uint8_t   type            : 1;
    uint8_t   privilege_level : 2;
    uint8_t   present         : 1;
    uint8_t   limit_high      : 4;
    uint8_t   reserved        : 1;
    bool      long_mode       : 1;
    GDT_SIZE  size            : 1;
    GDT_GRAN  granularity     : 1;	
    uint8_t   base_high;
} attr_pack gdt_entry_t;

typedef struct 
{
    uint16_t limit;
    uint32_t base;
} attr_pack gdt_reg_t;

void gdt_init(void);
void gdt_set_desc(uint8_t n, uint32_t base, uint32_t limit, gdt_access_t access, gdt_flags_t flags);