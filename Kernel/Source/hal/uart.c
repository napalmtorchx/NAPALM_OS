#include "hal/uart.h"
#include "core/kernel.h"

static UARTPORT _uartport;

void uart_setport(UARTPORT port)
{
    _uartport = port;
    if (port == UARTPORT_DISABLED) { return; }

    // send port data to serial controller
    port_outb(_uartport + 1, 0x00);    // disable all interrupts
    port_outb(_uartport + 3, 0x80);    // set baud rate divisor
    port_outb(_uartport + 0, 0x03);    // set divisor to 3 (lo uint8_t) 38400 baud
    port_outb(_uartport + 1, 0x00);    // hi uint8_t
    port_outb(_uartport + 3, 0x03);    // 8 bits, no parity, one stop bit
    port_outb(_uartport + 2, 0xC7);    // enable fifo, clear them, with 14-uint8_t threshold
    port_outb(_uartport + 4, 0x0B);    // irqs enabled, rts/dsr set
    port_outb(_uartport + 4, 0x1E);    // set in loopback mode, test the serial chip
    port_outb(_uartport + 0, 0xAE);    // test serial chip (send uint8_t 0xAE and check if serial returns same uint8_t)

    // check if serial is faulty
    if (port_inb(_uartport) != 0xAE) { _uartport = UARTPORT_DISABLED; return; }
    
    // ff serial is not faulty set it in normal operation mode
    port_outb(_uartport + 4, 0x0F);
}

char uart_read(void)
{
    if (_uartport == UARTPORT_DISABLED) { return 0; }
    while (!uart_canrd());
    return port_inb(_uartport);
}

void uart_putc(char c)
{
    if (_uartport == UARTPORT_DISABLED) { return; }
    while (!uart_canwr());
    port_outb(_uartport, c);
}

void uart_write(const char* str)
{
    if (_uartport == UARTPORT_DISABLED) { return; }
    char* wrstr = (str == NULL ? NULLSTR : str);
    
    int i = 0;
    while (wrstr[i] != 0) { uart_putc(wrstr[i++]); }
}

void uart_writeln(const char* str)
{
    if (_uartport == UARTPORT_DISABLED) { return; }
    uart_write(str);
    uart_putc(NEWLINE);
}

bool uart_canrd(void)
{
    if (_uartport == UARTPORT_DISABLED) { return false; }
    return port_inb(_uartport + 5) & 1;
}

bool uart_canwr(void)
{
    if (_uartport == UARTPORT_DISABLED) { return false; }
    return port_inb(_uartport + 5) & 0x20;
}

const char* uart_portstr(UARTPORT port)
{
    switch (port)
    {
        default:                { return "UNKNOWN"; }
        case UARTPORT_DISABLED: { return "DISABLED"; }
        case UARTPORT_COM1:     { return "COM1"; }
        case UARTPORT_COM2:     { return "COM2"; }
        case UARTPORT_COM3:     { return "COM3"; }
        case UARTPORT_COM4:     { return "COM4"; }
    }
}

const UARTPORT uart_getport(void) { return _uartport; }