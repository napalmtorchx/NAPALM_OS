#include <hal/ps2.h>
#include <core/kernel.h>

#define PS2_TIMEOUT        100000L
#define PS2_DATA           0x60
#define PS2_STATUS         0x64
#define PS2_COMMAND        0x64
#define MOUSE_WRITE        0xD4
#define MOUSE_V_BIT        0x08

#define PS2_PORT1_IRQ      0x01
#define PS2_PORT2_IRQ      0x02
#define PS2_PORT1_TLATE    0x40

#define PS2_READ_CONFIG    0x20
#define PS2_WRITE_CONFIG   0x60

#define PS2_DISABLE_PORT2  0xA7
#define PS2_ENABLE_PORT2   0xA8
#define PS2_DISABLE_PORT1  0xAD
#define PS2_ENABLE_PORT1   0xAE

#define MOUSE_SET_REMOTE   0xF0
#define MOUSE_DEVICE_ID    0xF2
#define MOUSE_SAMPLE_RATE  0xF3
#define MOUSE_DATA_ON      0xF4
#define MOUSE_DATA_OFF     0xF5
#define MOUSE_SET_DEFAULTS 0xF6

#define MOUSE_DEFAULT         0
#define MOUSE_SCROLLWHEEL     1
#define MOUSE_BUTTONS         2

#define KBD_SET_SCANCODE   0xF0

void ps2_init(void)
{
    bool ints = irqs_enabled();
    IRQ_DISABLE;

    kbd_init();

    ps2_sendcmd(PS2_DISABLE_PORT1);
    ps2_sendcmd(PS2_DISABLE_PORT2);
    while (port_inb(PS2_STATUS) & 1) { port_inb(PS2_DATA); }

    uint8_t status = ps2_comm(PS2_READ_CONFIG);
    status |= (PS2_PORT1_IRQ | PS2_PORT2_IRQ | PS2_PORT1_TLATE);
    ps2_sendcmdex(PS2_WRITE_CONFIG, status);

    ps2_sendcmd(PS2_ENABLE_PORT1);
    ps2_sendcmd(PS2_ENABLE_PORT2);

    irq_register(IRQ1, ps2_callback);
    irq_register(IRQ12, ps2_callback);
    debug_out("%s Registered PS/2 interrupts\n", DEBUG_INFO);

    ps2_wrkbd(KBD_SET_SCANCODE);
    ps2_wrkbd(2);
    ps2_wrmouse(MOUSE_SET_DEFAULTS);
    ps2_wrmouse(MOUSE_DATA_ON);

    if (ints) { IRQ_ENABLE; }
    debug_out("%s Initialized PS/2 controller\n", DEBUG_OK);
}

int ps2_wait_in(void)
{
    uint32_t timeout = PS2_TIMEOUT;
    while (--timeout) { if (!(port_inb(PS2_STATUS) & (1 << 1))) { return 0; } }
    return 1;
}

int ps2_wait_out(void)
{
    uint32_t timeout = PS2_TIMEOUT;
    while (--timeout) { if ((port_inb(PS2_STATUS) & (1 << 0))) { return 0; } }
    return 1;
}

void ps2_setsample(uint8_t rate)
{
    ps2_wrmouse(MOUSE_SAMPLE_RATE);
    while (!port_inb(PS2_STATUS) & 1);
    port_inb(PS2_DATA);
    ps2_wrmouse(rate);
    while (!port_inb(PS2_STATUS) & 1);
    port_inb(PS2_DATA);
}

void ps2_sendcmd(uint8_t cmd)
{
    ps2_wait_in();
    port_outb(PS2_COMMAND, cmd);
}

void ps2_sendcmdex(uint8_t cmd, uint8_t arg)
{
    ps2_wait_in();
    port_outb(PS2_COMMAND, cmd);
    ps2_wait_in();
    port_outb(PS2_DATA, arg);
}

uint8_t ps2_comm(uint8_t cmd)
{
    ps2_wait_in();
    port_outb(PS2_COMMAND, cmd);
    ps2_wait_out();
    return port_inb(PS2_DATA);
}

void ps2_wrmouse(uint8_t data)
{
    ps2_sendcmdex(MOUSE_WRITE, data);
}

void ps2_wrkbd(uint8_t data)
{
    ps2_wait_in();
    port_outb(PS2_DATA, data);
}

void ps2_callback(irq_context_t* context)
{
    ps2_sendcmd(PS2_DISABLE_PORT1);
    ps2_sendcmd(PS2_DISABLE_PORT2);

    uint8_t status = port_inb(PS2_STATUS);
    uint8_t data   = port_inb(PS2_DATA);

    ps2_sendcmd(PS2_ENABLE_PORT1);
    ps2_sendcmd(PS2_ENABLE_PORT2);

    irq_ack(context);

    if (!(status & 0x01))  { return; }
    else if (!(status & 0x20))  
    {
        kbd_device_t* kbd = (kbd_device_t*)devmgr_from_name("ps2_keyboard");
        if (kbd != NULL) { kbd_handle(data); } 
        return;
    }
    else if (status & 0x21) 
    { 
        //kbd_device_t* ms = (kbd_device_t*)devmgr_from_name("ps2_mouse");
        //if (ms != NULL) { } 
    }
}
