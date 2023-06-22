#pragma once
#include <lib/types.h>
#include <core/commands.h>

typedef int (*command_protocol_t)(int argc, char** argv);

typedef struct
{
    const char*        name;
    const char*        help;
    const char*        usage;
    command_protocol_t fn_exec;
} attr_pack command_t;

static const command_t COMMANDS[] = 
{
    (command_t){ "cls",         "Clear the screen",                     "cls [bg?: backcolor] [fg?: forecolor]", cmd_cls },
    (command_t){ "help",        "Show list of available commands",      "help [u?: usage]", cmd_help },
    (command_t){ "info",        "Show operating system information",    "info", cmd_info },
    (command_t){ "heap",        "Show list of heapblk entries",         "heap", cmd_heap },
    (command_t){ "threads",     "Show list of threads",                 "threads", cmd_threads },
    (command_t){ "cd",          "Set the current directory",            "cd", cmd_cd },
    (command_t){ "ls",          "List contents of directory",           "ls", cmd_ls },
    (command_t){ "view",        "Print the contents of specified file", "view [t: text,b: binary]", cmd_view },
    (command_t){ "txtedit",     "Create or modify text file",           "txtedit", cmd_txtedit },
    (command_t){ "kill",        "Terminate a running thread",           "kill [uid]", cmd_kill },
    (command_t){ "lspci",       "Show list of detected PCI devices",    "lspci", cmd_lspci },
};

static const size_t COMMAND_COUNT = sizeof(COMMANDS) / sizeof(command_t);

void cmdhost_init(void);
void cmdhost_prompt(void);
void cmdhost_update(void);
int  cmdhost_execute(const char* input);
void cmdhost_setdir(const char* dir);
const char* cmdhost_dir(void);
