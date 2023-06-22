#include <hal/devices/rtc.h>
#include <core/kernel.h>

#define RTC_CMD         0x70
#define RTC_DATA        0x71
#define RTC_MILLIS_TIME 0.9765625f
#define RTC_FREQ        1024

static rtc_device_t _rtc;

void rtc_init(void)
{
    _rtc = (rtc_device_t)
    {
        .base     = (device_t){ "rtc_timer", NULL_DUID, rtc_start, rtc_stop, false },
        .tm       = time_create(0, 0, 0, 0, 0, 0, 0, 0),
        .ticks    = 0,
        .tps      = 0,
        .timer    = 0,
        .seconds  = 0,
        .millis   = 0,
        .millis_t = 0,
    };
    devmgr_register(&_rtc);
    devmgr_start(_rtc.base.uid, NULL);
}

bool rtc_start(rtc_device_t* dev, void* unused)
{
    bool ints = irqs_enabled();
    IRQ_DISABLE;

    port_outb(RTC_CMD, 0x8B);	
    char prev = port_inb(RTC_DATA);
    port_outb(RTC_CMD, 0x8B);
    port_outb(RTC_DATA, prev | 0x40);
    port_outb(RTC_CMD, 0x0C);
    port_inb(RTC_DATA);

    irq_register(IRQ8, rtc_callback);
    if (ints) { IRQ_ENABLE; }
    return true;
}

int rtc_stop(rtc_device_t* dev)
{
    irq_unregister(IRQ8);
    port_outb(RTC_CMD, 0x0C);
    port_wait(100);
    port_inb(RTC_DATA);
    return true;
}

void rtc_callback(irq_context_t* context)
{
    irq_ack(context);

    if (_rtc.base.running)
    {
        _rtc.ticks++;
        _rtc.timer++;
        _rtc.seconds += RTC_MILLIS_TIME / 1000.0f;
        _rtc.millis_t = (uint32_t)(_rtc.seconds * 1000.0f);
        rtc_update();
    }

    port_outb(RTC_CMD, 0x0C);
    port_wait(5);
    port_inb(RTC_DATA);
}

void rtc_update(void)
{
    _rtc.tm.second = rtc_rdreg(0x00);
    _rtc.tm.minute = rtc_rdreg(0x02);
    _rtc.tm.hour   = rtc_rdreg(0x04);
    _rtc.tm.mday   = rtc_rdreg(0x07);
    _rtc.tm.month  = rtc_rdreg(0x08);
    _rtc.tm.year   = rtc_rdreg(0x09);

    uint8_t bcd = rtc_rdreg(0x0B);

    if (!(bcd & 0x04))
    {
        _rtc.tm.second  = (_rtc.tm.second & 0x0F) + (_rtc.tm.second / 16) * 10;
        _rtc.tm.minute  = (_rtc.tm.minute & 0x0F) + (_rtc.tm.minute / 16) * 10;
        _rtc.tm.hour    = ((_rtc.tm.hour  & 0x0F) + (_rtc.tm.hour   / 16) * 10) | (_rtc.tm.hour & 0x80);
        _rtc.tm.mday    = (_rtc.tm.mday   & 0x0F) + (_rtc.tm.mday   / 16) * 10;
        _rtc.tm.month   = (_rtc.tm.month  & 0x0F) + (_rtc.tm.month  / 16) * 10;
        _rtc.tm.year    = (_rtc.tm.year   & 0x0F) + (_rtc.tm.year   / 16) * 10;
    }
}

uint8_t rtc_rdreg(uint8_t reg)
{
    port_outb(RTC_CMD, reg);
    return port_inb(RTC_DATA);
}

bool rtc_updating(void)
{
    port_outb(RTC_CMD, 0x8A);
    port_wait(100);
    uint8_t status = port_inb(RTC_DATA);
    return (status & 0x80);
}