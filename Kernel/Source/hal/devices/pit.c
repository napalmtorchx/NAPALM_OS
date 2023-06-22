#include <hal/devices/pit.h>
#include <core/kernel.h>

#define PIT_DIVISOR 1193182
#define PIT_CMD     0x43
#define PIT_DATA    0x40

static pit_device_t _pit;

void pit_init(void)
{
    _pit = (pit_device_t)
    {
        .base     = (device_t){ "pit_timer", NULL_DUID, pit_start, pit_stop, false },
        .ticks    = 0,
        .tps      = 0,
        .timer    = 0,
        .freq     = 2000,
        .millis   = 0,
        .millis_t = 0,
    };
    devmgr_register(&_pit);
    devmgr_start(_pit.base.uid, NULL);
}

bool pit_start(pit_device_t* dev, void* unused)
{
    bool ints = irqs_enabled();
    IRQ_DISABLE;

    uint32_t f = PIT_DIVISOR / _pit.freq;
    uint8_t  h = (uint8_t)((f & 0xFF00) >> 8);
    uint8_t  l = (uint8_t)((f & 0x00FF));

    port_outb(PIT_CMD, 0x34);
    port_outb(PIT_DATA, l);
    port_outb(PIT_DATA, h);

    irq_register(IRQ0, pit_callback);
    if (ints) { IRQ_ENABLE; }
    return true;
}

int pit_stop(pit_device_t* dev)
{
    irq_unregister(IRQ0);
    _pit.ticks    = 0;
    _pit.timer    = 0;
    _pit.millis   = 0;
    _pit.millis_t = 0;
    return true;
}

void pit_callback(irq_context_t* context)
{
    if (_pit.base.running)
    {
        _pit.ticks++;
        _pit.timer++;

        if (_pit.timer >= (_pit.freq / 1000))
        {
            _pit.timer = 0;
            _pit.millis++;
            _pit.millis_t++;
        }

        if (_pit.millis > 1000) { _pit.millis = 0; }
    }

    irq_ack(context);
    taskmgr_callback(context);
}