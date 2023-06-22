#include <core/cmdhost.h>
#include <core/kernel.h>

#define KBD_BUFFSZ 0x10000

static kbd_state_t _kb;
static char        _buff[KBD_BUFFSZ];
static char        _dir[FILENAME_MAX];
static bool        _process;

void cmdhost_on_char(void* handle, char c) { printf("%c", c); krnl_redraw(); }

void cmdhost_on_del(void* handle, void* unused) 
{ 
    if (strlen(_buff) > 0) { vga_delete(); }
    krnl_redraw(); 
}

void cmdhost_on_ret(void* handle, void* unsued)
{
    
}

void cmdhost_init(void)
{
    _kb = kbd_state_create(_buff, KBD_BUFFSZ, NULL, false, cmdhost_on_char, cmdhost_on_del, cmdhost_on_ret);
    memset(_buff, 0, KBD_BUFFSZ);
    memset(_dir, 0, FILENAME_MAX);
    strcpy(_dir, "A:/");

    cmdhost_execute("cls");
    debug_out("%s Initialized command host\n", DEBUG_OK);
}

void cmdhost_prompt(void)
{
    printf("%s%s%s>%s ", ANSI_FG_CYAN, _dir, ANSI_FG_DARKGRAY, ANSI_RESET);
}

void cmdhost_update(void)
{
    kbd_setstate(&_kb);

    if (kbd_keydown(KEY_ENTER) && !_process) 
    { 
        _process = true; 
        printf("%c", '\n');
        cmdhost_execute(_buff);
        memset(_buff, 0, KBD_BUFFSZ);
        cmdhost_prompt();
        krnl_redraw();    
    }
    if (kbd_keyup(KEY_ENTER)) { _process = false; }
}

int cmdhost_execute(const char* input)
{
    if (strlen(input) == 0) { return 0; }

    size_t i, res, count = 0, valid = false;
    char** parts = strsplit(input, ' ', &count);
    if (count == 0 || parts == NULL) { return 0; }

    if (strlen(parts[0]) > 128) { valid = false; }
    else
    {
        char cmd[128];
        memset(cmd, 0, 128);
        strcpy(cmd, parts[0]);
        strlower(cmd);

        for (i = 0; i < COMMAND_COUNT; i++)
        {
            if (!strcmp(COMMANDS[i].name, cmd) && COMMANDS[i].fn_exec != NULL)
            {
                res = COMMANDS[i].fn_exec(count, parts);
                valid = true;
                break;
            }
        }
    }

    if (!valid) { printf("%s Invalid command '%s'\n", DEBUG_ERROR, parts[0]); }

    for (i = 0; i < count; i++) { free(parts[i]); }
    free(parts);
    return res;
}

void cmdhost_setdir(const char* dir)
{
    memset(_dir, 0, FILENAME_MAX);
    if (strlen(dir) > 0) { strcpy(_dir, dir); }
}

const char* cmdhost_dir(void) { return _dir; }