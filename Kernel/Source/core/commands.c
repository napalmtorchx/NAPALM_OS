#include <core/commands.h>
#include <core/kernel.h>

int cmd_cls(int argc, char** argv)
{
    display_clear(0xFF202020);
    vga_setpos(0, 1);
    return 0;
}

int cmd_info(int argc, char** argv)
{
    printf("NapalmOS version 1.0\nDeveloped by: napalmtorch\n");
    return 0;
}

int cmd_help(int argc, char** argv)
{
    printf("Showing list of commands:\n");

    vga_device_t* vga = (vga_device_t*)devmgr_from_name("vga_controller");
    char name[vga->w];

    for (size_t i = 0; i < COMMAND_COUNT; i++)
    {
        memset(name, 0, vga->w);
        strcat(name, COMMANDS[i].name);
        while (strlen(name) < 20) { stradd(name, ' '); }
        printf("- %s %s%s%s\n", name, ANSI_FG_CYAN, COMMANDS[i].help, ANSI_RESET);
    }
    return 0;
}

int cmd_heap(int argc, char** argv)
{
    if (argc < 2) { heap_print(true); }
    else 
    {
        if (!strcmp(argv[1], "-i"))
        {
            size_t used = heap_amount_used(), total = heap_amount_total();

            printf("Installed:   %a\n", memmgr_amount_installed(NULL));
            printf("Usage:       %u/%u bytes\n             %u/%u KB\n             %u/%u MB\n", used, total, used / KILOBYTE, total / KILOBYTE, used / MEGABYTE, total / MEGABYTE);
            printf("Heap Blocks: %u/%u\n", heap_count_type(HEAPSTATE_FREE) + heap_count_type(HEAPSTATE_USED), HEAP_COUNT);
        }
        else { printf("%s Invalid argument '%s'\n", DEBUG_ERROR, argv[1]); return 1; }
    }
    return 0;
}

int cmd_threads(int argc, char** argv)
{
    taskmgr_print(true);
    return 0;
}

int cmd_cd(int argc, char** argv)
{
    char fullpath[FILENAME_MAX];
    memset(fullpath, 0, FILENAME_MAX);
    for (size_t i = 1; i < argc; i++) { strcat(fullpath, argv[i]); if (i < argc - 1) { stradd(fullpath, ' '); } }
    virts_formatpath(fullpath, true);
    if (strlen(fullpath) == 0) { return 0; }
    if (!virtfs_dexists(fullpath)) { printf("%s Unknown path '%s'\n", DEBUG_ERROR, fullpath); return 1; }
    cmdhost_setdir(fullpath);
    return 0;
}

int cmd_ls(int argc, char** argv)
{
    char fullpath[FILENAME_MAX];
    memset(fullpath, 0, FILENAME_MAX);
    for (size_t i = 1; i < argc; i++) { strcat(fullpath, argv[i]); if (i < argc - 1) { stradd(fullpath, ' '); } }
    virts_formatpath(fullpath, true);
    if (strlen(fullpath) == 0) { strcpy(fullpath, cmdhost_dir()); }

    ptrlist_t files = virtfs_getfiles(fullpath);
    if (files.count == 0) { printf("No files found.\n"); }
    else
    {
        printf("Listing contents of directory '%s'\n", fullpath);

        char temp[32];
        for (size_t i = 0; i < files.count; i++)
        {
            if (i % 2 == 0)
            {
                memset(temp, 0, 32);
                char* fname = (char*)ptrlist_get(&files, i);
                for (int j = 0; j < 31; j++) { if (j < strlen(fname)) { stradd(temp, fname[j]); } else { stradd(temp, ' '); } }
                if (strlen(fname) >= 28) { memset(temp + 28, '.', 3); }
                temp[31] = 0;
                printf("%s ", temp);
                free(fname);
            }
            else
            {
                printf("%a\n", (uint32_t)ptrlist_get(&files, i));
            }
        }
    }
    free(files.entries);
    return 0;
}

int cmd_view(int argc, char** argv)
{
    if (argc < 3) { printf("%s Invalid arguments\n", DEBUG_ERROR); return 1; }
    char fullpath[FILENAME_MAX];
    memset(fullpath, 0, FILENAME_MAX);
    for (size_t i = 2; i < argc; i++) { strcat(fullpath, argv[i]); if (i < argc - 1) { stradd(fullpath, ' '); } }
    virts_formatpath(fullpath, false);
    if (strlen(fullpath) == 0) { strcpy(fullpath, cmdhost_dir()); }

    file_t* file = virtfs_fopen(fullpath, "r");
    if (file == NULL) { printf("%s Unknown file '%s'\n", DEBUG_ERROR, fullpath); virtfs_fclose(file); return 1; }    

    void* data = malloc(file->sz);
    virtfs_fread(data, file->sz, 1, file);

    if (!strcmp(argv[1], "-t"))
    {
        printf("%s\n", (char*)data);
    }
    else { printf("%s Unknown type for file '%s'\n", DEBUG_ERROR, fullpath); }

    virtfs_fclose(file);
    free(data);
    return 0;
}

int cmd_txtedit(int argc, char** argv)
{
    if (argc < 2) { printf("%s Expected filename\n", DEBUG_ERROR); return 1; }

    char fullpath[FILENAME_MAX];
    memset(fullpath, 0, FILENAME_MAX);
    for (size_t i = 2; i < argc; i++) { strcat(fullpath, argv[i]); if (i < argc - 1) { stradd(fullpath, ' '); } }
    virts_formatpath(fullpath, false);

    txtedit_t* txtedit = txtedit_create(fullpath, true);
    return 0;
}

int cmd_kill(int argc, char** argv)
{
    if (argc < 2) { printf("%s Expected UID of thread to terminate\n", DEBUG_ERROR); return 1; }

    uint32_t uid = atox(argv[1]);
    if (uid == 0 || uid == 0xFFFFFFFF) { printf("%s Invalid thread UID\n", DEBUG_ERROR); return 1; }
    if (uid == KERNEL_UID) { printf("%s Cannot terminal kernel thread\n", DEBUG_ERROR); return 1; }
    thread_t* thread = taskmgr_from_uid(uid);
    if (thread == NULL) { printf("%s No thread exists with specified UID\n", DEBUG_ERROR); return 1; }
    taskmgr_kill(thread);
    return 0;
}

int cmd_lspci(int argc, char** argv)
{
    pci_print(true);
}