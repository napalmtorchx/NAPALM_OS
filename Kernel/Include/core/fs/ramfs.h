#pragma once
#include <lib/types.h>

typedef struct
{
    char     name[64];
    uint8_t  hidden;
    uint32_t offset;
    size_t   size;
} attr_pack ramfs_file_t;

typedef struct
{
    uint32_t table_offset;
    uint32_t table_size;
    uint32_t data_offset;
    uint32_t data_size;
    uint32_t data_pos;
    uint32_t count;
    uint32_t max;
} attr_pack ramfs_hdr_t;

typedef struct
{
    ramfs_hdr_t*  hdr;
    uint8_t*      data;
    size_t        sz;
    ramfs_file_t* files;
} attr_pack ramfs_t;


#define RAMFS_ENTSZ (sizeof(ramfs_file_t))
#define RAMFS_ROOT  0

void ramfs_init(ramfs_t* fs, void* data, size_t sz);
ramfs_file_t* ramfs_fopen(ramfs_t* fs, const char* name);
void* ramfs_fgetptr(ramfs_t* fs, ramfs_file_t* file);
bool ramfs_fexists(ramfs_t* fs, const char* name);

ramfs_file_t** ramfs_getfiles(ramfs_t* fs, size_t* sz);