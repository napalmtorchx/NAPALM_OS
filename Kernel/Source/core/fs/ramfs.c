#include <core/fs/ramfs.h>
#include <core/kernel.h>

void ramfs_init(ramfs_t* fs, void* data, size_t sz)
{
    fs->data  = (uint8_t*)data;
    fs->sz    = sz;
    fs->hdr   = (ramfs_hdr_t*)fs->data;
    fs->files = (ramfs_file_t*)((uintptr_t)fs->data + sizeof(ramfs_hdr_t));
    debug_out("%s Initialized RAMFS - Ptr:%p Count:%d/%d Size:%a\n", DEBUG_OK, fs->data, fs->hdr->count, fs->hdr->max, fs->hdr->data_size);
}

ramfs_file_t* ramfs_fopen(ramfs_t* fs, const char* name)
{
    if (name == NULL || strlen(name) == 0) { return NULL; }
    for (size_t i = 0; i < fs->hdr->max; i++)
    {
        if (fs->files[i].name[0] == 0) { continue; }
        if (!strcmp(fs->files[i].name, name)) { return &fs->files[i]; }
    }
    return NULL;
}

void* ramfs_fgetptr(ramfs_t* fs, ramfs_file_t* file)
{
    if (fs == NULL || file == NULL) { return NULL; }
    void* filedata = (void*)((uintptr_t)fs->data + fs->hdr->data_offset + file->offset);
    return filedata;
}

bool ramfs_fexists(ramfs_t* fs, const char* name)
{
    ramfs_file_t* file = ramfs_fopen(fs, name);
    return (file != NULL);
}

ramfs_file_t** ramfs_getfiles(ramfs_t* fs, size_t* sz)
{
    size_t c = 0;
    for (size_t i = 0; i < fs->hdr->max; i++) { if (fs->files[i].name[0] == 0) { continue; } c++; }
    if (c == 0) { return NULL; }
    
    ramfs_file_t** files = (ramfs_file_t**)malloc(sizeof(ramfs_file_t*) * c);
    size_t j = 0;
    for (size_t i = 0; i < fs->hdr->max; i++)
    {
        if (fs->files[i].name[0] == 0) { continue; }
        files[j++] = &fs->files[i];
    }
    *sz = c;
    return files;
}