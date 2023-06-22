#include <userland/utils/txtedit.h>
#include <core/kernel.h>

void txtedit_load(txtedit_t* txtedit, const char* fname);
void txtedit_redraw(txtedit_t* txtedit);
void txtedit_newline(txtedit_t* txtedit);

void txtedit_onchar(txtedit_t* txtedit, char c)
{
    char* data = NULL;
    if (txtedit->data == NULL) 
    { 
        txtedit->sz = 2; 
        data = (char*)malloc(2); 
        txtedit->data = data;
    }
    else 
    { 
        txtedit->sz = strlen(txtedit->data) + 2;
        data = (char*)malloc(txtedit->sz);
        strcpy(data, txtedit->data);
        free(txtedit->data);
        txtedit->data = data;
    }
    stradd(txtedit->data, c);

    if (c == '\n') { vga_newline(); txtedit_newline(txtedit); }
    else if (c >= 32 && c <= 126)
    {
        vga_drawchar(txtedit->cx, txtedit->cy, c, VGA_WHITE, VGA_DARKBLUE);
        txtedit->cx++;
        if (txtedit->cx >= txtedit->w) { txtedit_newline(txtedit); }
        else { vga_setpos(txtedit->cx, txtedit->cy); }
    }
}

void txtedit_onret(txtedit_t* txtedit, void* unused)
{

}

void txtedit_ondel(txtedit_t* txtedit, void* unused)
{

}

txtedit_t* txtedit_create(const char* fname, bool start)
{
    txtedit_t* txtedit = (txtedit_t*)malloc(sizeof(txtedit_t));

    int    argc = 2;
    char** argv = (char**)malloc(sizeof(char*) * 2);        
    argv[0] = (char*)txtedit;
    argv[1] = strdup(fname);

    txtedit->thread = thread_create("txtedit", 0x10000, txtedit_main, THREAD_PRIORITY_NORMAL, argc, argv);
    taskmgr_load(txtedit->thread);
    if (start) { taskmgr_start(txtedit->thread); }
    return txtedit;
}

void txtedit_free(txtedit_t* txtedit)
{
    if (txtedit == NULL) { debug_exception("txtedit_free(%p) - Null pointer", txtedit); return; }
    thread_t* t = txtedit->thread;
    free(txtedit->thread->argv[1]);
    free(txtedit->thread->argv);
    free(txtedit);
    taskmgr_kill(t);
}

int txtedit_main(int argc, char** argv)
{
    txtedit_t*    txtedit = (txtedit_t*)argv[0];
    vga_device_t* vga     = (vga_device_t*)devmgr_from_name("vga_controller");
    char*         fname   = argv[1];

    if (fname != NULL && virtfs_fexists(fname)) { txtedit_load(txtedit, fname); }
    else
    {
        txtedit->kbstate = kbd_state_create(NULL, 0, txtedit, false, txtedit_onchar, txtedit_ondel, txtedit_onret);
        txtedit->fname   = fname;
        txtedit->data    = NULL;
        txtedit->sz      = 0;
        txtedit->cx      = 0;
        txtedit->cy      = 1;
        txtedit->w       = vga->w;
        txtedit->h       = vga->h;   
        vga->fg          = VGA_WHITE;
        display_clear(VGA_DARKBLUE);     
        vga_setpos(0, 1);
    }

    debug_out("%s Started txtedit\n", DEBUG_INFO);
    lock();

    bool running = true;
    while (running)
    {
        txtedit->kbstate.data = NULL;
        kbd_setstate(&txtedit->kbstate);        
    }

    unlock();
    txtedit_free(txtedit);
    return 0;
}

void txtedit_load(txtedit_t* txtedit, const char* fname)
{

}

void txtedit_newline(txtedit_t* txtedit)
{
    txtedit->cx = 0;
    txtedit->cy++;
    if (txtedit->cy >= txtedit->h) 
    {
        vga_scroll(); 
        txtedit->cx = 0; 
        txtedit->cy = txtedit->h - 1; 
        vga_setpos(txtedit->cx, txtedit->cy);
    }
    else { vga_setpos(txtedit->cx, txtedit->cy); }
}

void txtedit_redraw(txtedit_t* txtedit)
{

}