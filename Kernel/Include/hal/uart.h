#pragma once
#include <lib/types.h>

typedef enum
{
    UARTPORT_DISABLED = 0x000,
    UARTPORT_COM1     = 0x3F8,
    UARTPORT_COM2     = 0x2F8,
    UARTPORT_COM3     = 0x3E8,
    UARTPORT_COM4     = 0x2E8,
} UARTPORT;

void uart_setport(UARTPORT port);
char uart_read(void);
void uart_putc(char c);
void uart_write(const char* str);
void uart_writeln(const char* str);
bool uart_canrd(void);
bool uart_canwr(void);

const char* uart_portstr(UARTPORT port);
const UARTPORT uart_getport(void);