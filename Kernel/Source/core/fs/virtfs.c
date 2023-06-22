#include <core/fs/virtfs.h>
#include <core/kernel.h>

static ptrlist_t _list;
static virtfs_t  _bootvfs;

void virtfs_init(void)
{
    _list = ptrlist_create();

    memblk_t* bootfs_module = memmgr_first_of(MEM_MODULE, 0);
    ramfs_init(bootfs_get(), (void*)bootfs_module->addr, bootfs_module->sz);

    _bootvfs = (virtfs_t)
    {
        .type   = VFS_RAMFS,
        .letter = 'A',
        .label  = "boot",
        .fs     = bootfs_get()
    };
    virtfs_register(&_bootvfs);

    debug_out("%s Initialized VFS manager - %d filesystems detected\n", DEBUG_OK, _list.count);
}

void virtfs_register(virtfs_t* vfs)
{
    if (vfs == NULL) { debug_exception("virtfs_register(%p) - Null pointer", vfs); return; }
    ptrlist_add(&_list, vfs);
    debug_out("%s Registered VFS - MNT:%2x Type:%2x Label:%s\n", DEBUG_INFO, vfs->letter, vfs->type, vfs->label);
}

void virtfs_unregister(virtfs_t* vfs)
{
    
}

virtfs_t* virtfs_from_path(const char* path)
{
    if (strlen(path) < 3 || path[1] != ':' || path[2] != '/') { return NULL; }
    if (!isalpha(path[0])) { return NULL; }
    return virtfs_from_letter(path[0]);
}

directory_t* virtfs_opendir(const char* dirname)
{
    virtfs_t* vfs = virtfs_from_path(dirname);
    if (vfs == NULL) { return NULL; }

    if (vfs->type == VFS_RAMFS && dirname[0] == vfs->letter && dirname[1] == ':' && dirname[2] == '/' && strlen(dirname) == 3)
    {
        directory_t* dir = (directory_t*)malloc(sizeof(directory_t));
        dir->hidden = false;
        dir->vfs    = vfs;
        dir->name   = strdup(vfs->label);
        return dir;
    }

    char path[strlen(dirname)];
    memset(path, 0, strlen(dirname));
    strcpy(path, dirname + 3);
    return NULL;
}

int virtfs_closedir(directory_t* dir)
{
    if (dir == NULL) { return 1; }
    free(dir->name);
    free(dir);
    return 0;
}

file_t* virtfs_fopen(const char* fname, const char* mode)
{
    virtfs_t* vfs = virtfs_from_path(fname);
    if (vfs == NULL) { return NULL; }

    char path[strlen(fname)];
    memset(path, 0, strlen(fname));
    strcpy(path, fname + 3);

    if (vfs->type == VFS_RAMFS)
    {
        ramfs_file_t* ramfile = ramfs_fopen(vfs->fs, path);
        if (ramfile == NULL) { return NULL; }

        file_t* file = (file_t*)malloc(sizeof(file_t));
        file->name   = strdup(ramfile->name);
        file->sz     = ramfile->size;
        file->hidden = ramfile->hidden;
        file->offset = ramfile->offset;
        file->vfs    = vfs;
        debug_out("%s virtfs_fopen('%s', '%s')\n", DEBUG_INFO, mode, fname);
        return file;
    }
    return NULL;
}

size_t virtfs_fread(void* ptr, size_t sz, size_t nmemb, file_t* file)
{
    if (ptr == NULL || file == NULL || sz == 0 || nmemb == 0) { return 0; }
    size_t written = 0;

    if (file->vfs->type == VFS_RAMFS)
    {
        ramfs_t* ramfs = (ramfs_t*)file->vfs->fs;
        uint8_t* src   = (uint8_t*)((uintptr_t)ramfs->data + ramfs->hdr->data_offset + file->offset);
        while (written < sz * nmemb)
        {
            ((uint8_t*)ptr)[written] = src[written];
            written++;
        }
    }
    debug_out("%s virtfs_fread(%p, %u, %u, %p) - %a copied\n", DEBUG_INFO, ptr, sz, nmemb, file, written);
    return written;
}

int virtfs_fclose(file_t* file)
{
    if (file == NULL) { return 1; }
    free(file->name);
    free(file);
    debug_out("%s virtfs_fclose(%p)\n", DEBUG_INFO, file);
    return 0;
}

bool virtfs_fexists(const char* fname)
{
    file_t* file = virtfs_fopen(fname, "r");
    if (file == NULL) { return false; }
    virtfs_fclose(file);
    return true;
}

bool virtfs_dexists(const char* dirname)
{
    directory_t* dir = virtfs_opendir(dirname);
    if (dir == NULL) { return false; }
    if (dir->vfs->type == VFS_RAMFS && strlen(dirname) == 3 && dirname[0] == dir->vfs->letter && dirname[1] == ':' && dirname[2] == '/') { virtfs_closedir(dir); return true; }
    virtfs_closedir(dir);
    return false;
}

char* virtfs_getparent(const char* path)
{
    if (strlen(path) == 0 || strchr(path, '/') == NULL) { return; }
    if (path[strlen(path) - 1] == '/') { strback(path); }

    char* outpath = (char*)malloc(strlen(path) + 1);
    int i, j = -1;
    for (i = 0; i < strlen(path); i++) { if (path[i] == '/') { j = i; } }
    if (j > 0) { memcpy(outpath, path, j); }
    return outpath;
}

ptrlist_t virtfs_getfiles(const char* path)
{
    ptrlist_t list = ptrlist_create();
    virtfs_t* vfs  = virtfs_from_path(path);
    if (vfs == NULL || strlen(path) < 3) { return list; }

    if (vfs->type == VFS_RAMFS)
    {
        if (path[0] == vfs->letter && path[1] == ':' && path[2] == '/' && strlen(path) == 3)
        {
            size_t count = 0;
            ramfs_file_t** files = ramfs_getfiles(vfs->fs, &count);
            for (size_t i = 0; i < count; i++)
            {
                char* name = strdup(files[i]->name);
                ptrlist_add(&list, name);
                ptrlist_add(&list, (void*)files[i]->size);
            }
            free(files);
        }
    }
    return list;
}

ptrlist_t virtfs_getdirs(const char* path)
{

}

char* virts_formatpath(char* path, bool dir)
{
    size_t len = strlen(path);
    if (len == 0) { return; }
    if (len == 2 && path[1] == ':') { stradd(path, '/'); return path; }

    for (size_t i = 0; i < len; i++) { if (path[i] == '\\') { path[i] = '/'; } }
    if (dir && path[len - 1] != '/') { stradd(path, '/'); }
    return path;
}

virtfs_t* virtfs_from_letter(uint8_t letter)
{
    for (size_t i = 0; i < _list.count; i++)
    {
        virtfs_t* vfs = (virtfs_t*)ptrlist_get(&_list, i);
        if (vfs->letter == letter) { return vfs; }
    }
    return NULL;
}

virtfs_t* virtfs_from_label(const char* label)
{
    if (strlen(label) == 0) { return NULL; }
    for (size_t i = 0; i < _list.count; i++)
    {
        virtfs_t* vfs = (virtfs_t*)ptrlist_get(&_list, i);
        if (!strcmp(vfs->label, label)) { return vfs; }
    }
    return NULL;
}

virtfs_t* virtfs_get(int index) { return (virtfs_t*)ptrlist_get(&_list, index); }

size_t virtfs_count(void) { return _list.count; }