#include <core/debug.h>
#include <core/kernel.h>

static char _dbgbuff[16384];

attr_optimize("O0") void debug_out(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    memset(_dbgbuff, 0, sizeof(_dbgbuff));
    vsprintf(_dbgbuff, fmt, args);
    va_end(args);

    uart_write(_dbgbuff);
}

attr_optimize("O0") void debug_exception(const char* fmt, ...)
{    
    va_list args;
    va_start(args, fmt);
    memset(_dbgbuff, 0, sizeof(_dbgbuff));
    vsprintf(_dbgbuff, fmt, args);
    va_end(args);

    static const char* halt_msg = "The system has been halted to prevent damage to your computer.";
    uart_write(DEBUG_ERROR);
    uart_putc(' ');
    uart_writeln(_dbgbuff);
    uart_writeln(halt_msg);
    
    display_clear(VGA_DARKRED);
    vga_writeln(_dbgbuff);
    vga_writeln(halt_msg);
      
    inline_asm("cli; hlt");
}