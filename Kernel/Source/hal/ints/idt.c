#include <hal/ints/idt.h>
#include <core/kernel.h>

static const char* EXCEPTION_MSGS[] = 
{
    "Divide By Zero",
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Co-processor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    "Reserved",
    "Triple Fault",
    "FPU Error Interrupt",
};

#define isr(x) extern void isr##x();
#define irq(x) extern void irq##x();

isr(0);  isr(1);  isr(2);  isr(3);  isr(4);  isr(5);  isr(6);  isr(7);
isr(8);  isr(9);  isr(10); isr(11); isr(12); isr(13); isr(14); isr(15);
isr(16); isr(17); isr(18); isr(19); isr(20); isr(21); isr(22); isr(23);
isr(24); isr(25); isr(26); isr(27); isr(28); isr(29); isr(30); isr(31);
irq(0);  irq(1);  irq(2);  irq(3);  irq(4);  irq(5);  irq(6);  irq(7);
irq(8);  irq(9);  irq(10); irq(11); irq(12); irq(13); irq(14); irq(15);
irq(128);

idt_flags_t IDT_EXCEPTION_ATTRS = (idt_flags_t) { .type = (uint8_t)IDT_TRAP32, .unused = 0, .dpl = 3, .present = 1 };
idt_flags_t IDT_IRQ_ATTRS       = (idt_flags_t) { .type = (uint8_t)IDT_INT32,  .unused = 0, .dpl = 3, .present = 1 };

static idt_reg_t      _reg                  attr_align(0x100);
static idt_entry_t    _entries[IDT_COUNT]   attr_align(0x100);
static irq_protocol_t _protocols[IDT_COUNT] attr_align(0x100);

extern void _idt_flush(uintptr_t value);

void irq_default_protocol(irq_context_t* context) { irq_ack(context); }

void idt_init_exceptions()
{
    idt_set_desc(0x00, (uint32_t)isr0,  IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x01, (uint32_t)isr1,  IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x02, (uint32_t)isr2,  IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x03, (uint32_t)isr3,  IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x04, (uint32_t)isr4,  IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x05, (uint32_t)isr5,  IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x06, (uint32_t)isr6,  IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x07, (uint32_t)isr7,  IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x08, (uint32_t)isr8,  IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x09, (uint32_t)isr9,  IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x0A, (uint32_t)isr10, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x0B, (uint32_t)isr11, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x0C, (uint32_t)isr12, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x0D, (uint32_t)isr13, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x0E, (uint32_t)isr14, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x0F, (uint32_t)isr15, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x10, (uint32_t)isr16, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x11, (uint32_t)isr17, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x12, (uint32_t)isr18, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x13, (uint32_t)isr19, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x14, (uint32_t)isr20, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x15, (uint32_t)isr21, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x16, (uint32_t)isr22, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x17, (uint32_t)isr23, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x18, (uint32_t)isr24, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x19, (uint32_t)isr25, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x1A, (uint32_t)isr26, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x1B, (uint32_t)isr27, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x1C, (uint32_t)isr28, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x1D, (uint32_t)isr29, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x1E, (uint32_t)isr30, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x1F, (uint32_t)isr31, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    debug_out("%s Initialized exception descriptors\n", DEBUG_INFO);
}

void idt_init_irqs()
{
    idt_set_desc(0x20, (uint32_t)irq0,  IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x21, (uint32_t)irq1,  IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x22, (uint32_t)irq2,  IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x23, (uint32_t)irq3,  IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x24, (uint32_t)irq4,  IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x25, (uint32_t)irq5,  IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x26, (uint32_t)irq6,  IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x27, (uint32_t)irq7,  IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x28, (uint32_t)irq8,  IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x29, (uint32_t)irq9,  IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x2A, (uint32_t)irq10, IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x2B, (uint32_t)irq11, IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x2C, (uint32_t)irq12, IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x2D, (uint32_t)irq13, IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x2E, (uint32_t)irq14, IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x2F, (uint32_t)irq15, IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x80, (uint32_t)irq128, IDT_IRQ_ATTRS, KERNEL_CS);
    debug_out("%s Initialized irq descriptors\n", DEBUG_INFO);
}

void idt_init()
{
    memset(_entries, 0, sizeof(_entries));
    memset(_protocols, 0, sizeof(_protocols));
    _reg.base = (uintptr_t)&_entries;
    _reg.limit = (sizeof(idt_entry_t) * IDT_COUNT) - 1;

    IRQ_DISABLE;
    idt_init_exceptions();
    pic_remap();
    idt_init_irqs();

    _idt_flush((uintptr_t)&_reg);
    irq_register(IRQ7,  irq_default_protocol);
    irq_register(IRQ15, irq_default_protocol);
    IRQ_ENABLE;

    debug_out("%s Initialized interrupt descriptor table and handlers\n", DEBUG_OK);
}

void idt_exception_handler(irq_context_t* context)
{
    const size_t n = sizeof(EXCEPTION_MSGS) / sizeof(char*);
    debug_out("%s System Exception %s%s\n", DEBUG_ERROR, (context->irq >= n ? "" : "- "), (context->irq >= n ? "" : EXCEPTION_MSGS[context->irq]));
    debug_exception("Fatal system exception");
}

void idt_irq_handler(irq_context_t* context)
{
    if (_protocols[context->irq] != NULL) { _protocols[context->irq](context); }
    else
    {
        if (context->irq != IRQ0) { debug_out("%s Unhandled interrupt %d\n", DEBUG_WARN, context->irq - 32); }
        irq_ack(context);
    }
}

void idt_set_desc(uint8_t n, uint32_t base, idt_flags_t flags, uint16_t seg)
{
    _entries[n].offset_low    = LOW_16(base);
    _entries[n].offset_high   = HIGH_16(base);
    _entries[n].reserved      = 0;
    _entries[n].flags.type    = flags.type;
    _entries[n].flags.dpl     = flags.dpl;
    _entries[n].flags.present = flags.present;
    _entries[n].flags.unused  = 0;
    _entries[n].segment       = seg;
}

void irq_register(IRQ irq, irq_protocol_t protocol)
{
    if (protocol == NULL) { debug_exception("irq_register(%2x, 00000000) - Attempt to register interrupt with null protocol"); return; }
    _protocols[irq] = protocol;
    debug_out("%s Registered IRQ protocol - IRQ:%2x Protocol:%p\n", DEBUG_INFO, irq, protocol);
}

void irq_unregister(IRQ irq)
{
    irq_protocol_t protocol = _protocols[irq];
    _protocols[irq] = NULL;
    debug_out("%s Unregistered IRQ protocol - IRQ:%2x Protocol:%p\n", DEBUG_INFO, irq, protocol);
}

void irq_ack(irq_context_t* context)
{
    if (context->irq >= 40) { port_outb(0xA0, 0x20); }
    port_outb(0x20, 0x20);
}

bool irqs_enabled(void)
{
    uint32_t eflags = _regrd_eflags();
    return (eflags & 0x200) >> 9;
}