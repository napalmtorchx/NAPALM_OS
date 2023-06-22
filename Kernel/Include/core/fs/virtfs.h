#pragma once
#include <lib/types.h>
#include <lib/ptrlist.h>

#define FILENAME_MAX 260

typedef struct virtfs_t virtfs_t;

typedef enum
{
    VFS_INVALID,
    VFS_RAMFS,
    VFS_FAT,
} VFSTYPE;

typedef struct
{
    char*     name;
    bool      hidden;
    virtfs_t* vfs;
} attr_pack directory_t;

typedef struct
{
    char*     name;
    bool      hidden;
    virtfs_t* vfs;
    size_t    sz;
    uintptr_t offset;
} attr_pack file_t;

struct virtfs_t
{
    VFSTYPE type;
    char    letter;
    char    label[32];
    void*   fs;
};

void virtfs_init(void);
void virtfs_register(virtfs_t* vfs);
void virtfs_unregister(virtfs_t* vfs);

directory_t* virtfs_opendir(const char* dirname);
int virtfs_closedir(directory_t* dir);

file_t* virtfs_fopen(const char* fname, const char* mode);
size_t  virtfs_fread(void* ptr, size_t sz, size_t nmemb, file_t* file);
int     virtfs_fclose(file_t* file);

bool  virtfs_fexists(const char* fname);
bool  virtfs_dexists(const char* dirname);
char* virtfs_getparent(const char* path);
ptrlist_t virtfs_getfiles(const char* path);
ptrlist_t virtfs_getdirs(const char* path);
char* virts_formatpath(char* path, bool dir);

virtfs_t* virtfs_from_letter(uint8_t uid);
virtfs_t* virtfs_from_label(const char* label);
virtfs_t* virtfs_get(int index);
size_t virtfs_count(void);