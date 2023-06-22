#include <hal/pci.h>
#include <core/kernel.h>

static pci_hdr_t* _devices[PCI_COUNT];
static size_t     _count = 0;

void pci_print(bool cmd)
{
    static const char* LN1   = "|-pci-devices------------------------------------------------|\n";
    static const char* LN2   = "|%s VENDOR DEVICE CLASS NAME                                   %s|\n";
    static const char* LNDIV = "|------------------------------------------------------------|\n";

    if (cmd) { printf(LN1); printf(LNDIV); printf(LN2, ANSI_FG_DARKGRAY, ANSI_RESET); printf(LNDIV); }
    else { debug_out(LN1); debug_out(LNDIV); debug_out(LN2, ANSI_FG_DARKGRAY, ANSI_RESET); printf(LNDIV); }

    char line[72];
    for (size_t i = 0; i < PCI_COUNT; i++)
    {
        if (_devices[i] == NULL) { continue; }
        memset(line, 0, sizeof(line));
        sprintf(line, "| %4x   %4x   %2x    %s",  _devices[i]->vendor_id, _devices[i]->device_id, _devices[i]->class_code, pci_getname(_devices[i]->vendor_id, _devices[i]->device_id));
        while (strlen(line) < 60) { stradd(line, ' '); }
        strcat(line, " |");
        if (cmd) { printf("%s\n", line); } else { debug_out("%s\n", line); }
    }

    if (cmd) { printf(LNDIV); } else { debug_out(LNDIV); }
}

void pci_probe(void)
{
    memset(_devices, 0, sizeof(_devices));
    _count = 0;

    for (uint16_t bus = 0; bus < PCI_BUS_COUNT; bus++)
    {
        for (uint8_t slot = 0; slot < PCI_SLOT_COUNT; slot++)
        {
            for (uint8_t func = 0; func < PCI_FUNC_COUNT; func++)
            {
                uint16_t vendor_id = pci_read(bus, slot, func, 0x00);
                if (vendor_id == PCI_VENDOR_INVALID) { continue; }

                PCIROLE type = (PCIROLE)(pci_read(bus, slot, func, 0x0E) & 0xFF) & ~(1 << 7);
                if (type == PCIROLE_DEVICE)
                {
                    pci_device_t* pcidev = (pci_device_t*)malloc(sizeof(pci_device_t));
                    *(pci_hdr_t*)pcidev = pci_probedev(bus, slot, func);
                    _devices[_count++] = pcidev;
                }
            }
        }
    }

    debug_out("%s Finished probing PCI bus - %d devices detected\n", DEBUG_OK, _count);
}

pci_hdr_t pci_probedev(uint16_t bus, uint16_t slot, uint16_t func)
{
    pci_hdr_t pcidev;
    pcidev.vendor_id     = pci_read(bus, slot, func, 0x00);
    pcidev.device_id     = pci_read(bus, slot, func, 0x02);
    pcidev.command       = pci_read(bus, slot, func, 0x04);
    pcidev.status        = pci_read(bus, slot, func, 0x06);
    pcidev.rev_id        = (uint8_t)(pci_read(bus, slot, func, 0x08) &  0xFF);
    pcidev.prog_if       = (uint8_t)(pci_read(bus, slot, func, 0x08) >> 0x08);
    pcidev.subclass      = (uint8_t)(pci_read(bus, slot, func, 0x0A) &  0xFF);
    pcidev.class_code    = (uint8_t)(pci_read(bus, slot, func, 0x0A) >> 0x08);
    pcidev.cacheln_sz    = (uint8_t)(pci_read(bus, slot, func, 0x0C) &  0xFF);
    pcidev.latency_timer = (uint8_t)(pci_read(bus, slot, func, 0x0C) >> 0x08);
    pcidev.hdr_type      = (PCITYPE)(pci_read(bus, slot, func, 0x0E) &  0xFF);
    pcidev.bist          = (uint8_t)(pci_read(bus, slot, func, 0x0E) >> 0x08);
    debug_out("%s PCI device(%2x,%2x,%2x) - Vendor:%4x Dev:0x%4x\n", DEBUG_INFO, bus, slot, func, pcidev.vendor_id, pcidev.device_id);
    return pcidev;
}

void pci_write(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset, uint16_t data)
{
    uint32_t addr = (uint32_t)((bus << 16) | (slot << 1) | (func << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
    port_outl(PCI_CONFIG_ADDRESS, addr);
    port_outl(PCI_CONFIG_DATA, data);
}

uint16_t pci_read(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset)
{
    uint32_t addr = (uint32_t)(((uint32_t)bus << 16) | ((uint32_t)slot << 11) | ((uint32_t)func << 8) | ((uint32_t)offset & 0xFC) | 0x80000000);
    port_outl(PCI_CONFIG_ADDRESS, addr);
    return (uint16_t)(port_inl(PCI_CONFIG_DATA) >> ((offset & 2) * 8) & 0xFFFF);
}

pci_hdr_t** pci_list(void) { return _devices; }

size_t pci_count(void) { return _count; }

const char* pci_getname(uint16_t vendor_id, uint16_t device_id)
{
    // intel devices
    if (vendor_id == PCIVENDOR_INTEL)
    {
        switch (device_id)
        {
            case 0x1237: { return "Intel 440FX Chipset"; }
            case 0x7000: { return "Intel 82371SB PIIX3 ISA"; }
            case 0x7010: { return "Intel 82371SB PIIX3 IDE"; }
            case 0x7020: { return "Intel 82371SB PIIX3 USB"; }
            case 0x7111: { return "Intel 82371AB/EB/MB PIIX4 IDE"; }
            case 0x7113: { return "Intel 82371AB/EB/MB PIIX4 ACPI"; }
            case 0x100E: { return "Intel 82540EM Ethernet Controller"; }
            case 0x0041: { return "Intel Core PCI Express x16 Root Port"; }
            case 0x0042: { return "Intel Core IGPU Controller"; }
            case 0x0044: { return "Intel Core DRAM Controller"; }
            case 0x0600: { return "Intel RAID Controller"; }
            case 0x061F: { return "Intel 80303 I/O Processor"; }
            case 0x2415: { return "Intel 82801AA AC'97 Audio"; }
            case 0x2668: { return "Intel 82801(ICH6 Family) HD Audio"; }
            case 0x2770: { return "Intel 82945G Memory Controller Hub"; }
            case 0x2771: { return "Intel 82945G PCI Express Root Port"; }
            case 0x2772: { return "Intel 82945G Integrated Graphics"; }
            case 0x2776: { return "Intel 82945G Integrated Graphics"; }
            case 0x27D0: { return "Intel NM10/ICH7 PCI Express Port 1"; }
            case 0x27D2: { return "Intel NM10/ICH7 PCI Express Port 2"; }
            case 0x27C8: { return "Intel NM10/ICH7 USB UHCI Controller 1"; }
            case 0x27C9: { return "Intel NM10/ICH7 USB UHCI Controller 2"; }
            case 0x27CA: { return "Intel NM10/ICH7 USB UHCI Controller 3"; }
            case 0x27CB: { return "Intel NM10/ICH7 USB UHCI Controller 4"; }
            case 0x27CC: { return "Intel NM10/ICH7 USB2 EHCI Controller"; }
            case 0x244E: { return "Intel 82801 PCI Bridge"; }
            case 0x27DE: { return "Intel 82801G/ICH7 AC'97 Audio"; }
            case 0x27B8: { return "Intel 82801GB/ICH7 LPC Interface Bridge"; }
            case 0x27C0: { return "Intel NM10/ICH7 SATA Controller(IDE MODE)"; }
            case 0x27DA: { return "Intel NM10/ICH7 SMBus Controller"; }
            case 0x0F00: { return "Intel Atom Z36xxx/Z37xxx SoC Trans-Reg"; }
            case 0x0F31: { return "Intel Atom Z36xxx/Z37xxx Series Graphics"; }
            case 0x0F23: { return "Intel Atom E3800 Series SATA AHCI"; }
            case 0x0F18: { return "Intel Atom Z36xxx/Z37xxx Trusted Exec."; }
            case 0x0F04: { return "Intel Atom Z36xxx/Z37xxx HD Audio"; }
            case 0x0F48: { return "Intel Atom E3800 Series PCI Express Port 1"; }
            case 0x0F4A: { return "Intel Atom E3800 Series PCI Express Port 2"; }
            case 0x0F34: { return "Intel Atom Z36xxx/Z37xxx USB EHCI"; }
            case 0x0F1C: { return "Intel Atom Z36xxx/Z37xxx Series PCU"; }
            case 0x0F12: { return "Intel Atom E3800 Series SMBus Controller"; }
            case 0x7190: { return "Intel 440BX/ZX/DX Host Bridge"; }
            case 0x7110: { return "Intel 82371AB/EB/MB PIIX4 ISA"; }
            default:     { return "Unrecognized Intel Device"; }
        }
    }

    // newer intel devices
    if (vendor_id == PCIVENDOR_INTEL_ALT)
    {
        switch (device_id)
        {
            default: { return "Unrecognized Intel Device"; }
        }
    }

    // vmware
    if (vendor_id == PCIVENDOR_VMWARE)
    {
        switch (device_id)
        {
            case 0x0405: { return "VMWare SVGAII Adapter"; }
            case 0x0710: { return "VMWare SVGA Adapter"; }
            case 0x0770: { return "VMWare USB2 EHCI Controller"; }
            case 0x0720: { return "VMWare VMXNET Ethernet Controller"; }
            case 0x0740: { return "VMWare VM Communication Interface"; }
            case 0x0774: { return "VMWare USB1.1 UHCI Controller"; }
            case 0x0778: { return "VMWare USB3 xHCI 0.96 Controller"; }
            case 0x0779: { return "VMWare USB3 xHCI 1.00 Controller"; }
            case 0x0790: { return "VMWare PCI Bridge"; }
            case 0x07A0: { return "VMWare PCI Express Root Port"; }
            case 0x07B0: { return "VMWare VMXNET3 Ethernet Controller"; }
            case 0x07C0: { return "VMWare PVSCSI SCSI Controller"; }
            case 0x07E0: { return "VMWare SATA AHCI Controller"; }
            case 0x0801: { return "VMWare VM Interface"; }
            case 0x0820: { return "VMWare Paravirtual RDMA Controller"; }
            case 0x0800: { return "VMWare Hypervisor ROM Interface"; }
            case 0x1977: { return "VMWare HD Audio Controller"; }
            default: { return "Unrecognized VMWare Device"; }
        }
    }

    // vmware
    if (vendor_id == PCIVENDOR_VMWARE_ALT)
    {
        switch (device_id)
        {
            default: { return "Unrecognized VMWare Device"; }
        }
    }

    // innotek
    if (vendor_id == PCIVENDOR_INNOTEK)
    {
        switch (device_id)
        {
            case 0xBEEF: { return "VirtualBox Graphics Adapter"; }
            case 0xCAFE: { return "VirtualBox Guest Service"; }
            default: { return "Unrecognized InnoTek Device"; }
        }
    }

    // amd
    if (vendor_id == PCIVENDOR_AMD)
    {
        switch (device_id)
        {
            case 0x2000: { return "AMD 79C970 Ethernet Controller"; }
            default: { return "Unrecognized AMD Device"; }
        }
    }

    // ensoniq
    if (vendor_id == PCIVENDOR_ENSONIQ)
    {
        switch (device_id)
        {
            case 0x5000: { return "Ensoniq ES1370 [AudioPCI]"; }
            case 0x1371: { return "Ensoniq ES1371/ES1373/CT2518"; }
            default: { return "Unrecognized Ensoniq Device"; }
        }
    }

    // broadcom
    if (vendor_id == PCIVENDOR_BROADCOM)
    {
        switch (device_id)
        {
            case 0x1677: { return "NetXtreme BCM5751 Ethernet PCI-E"; }
            default: { return "Unrecognized Realtek Device"; }
        }
    }

    // realtek
    if (vendor_id == PCIVENDOR_REALTEK)
    {
        switch (device_id)
        {
            case 0x5289: { return "Realtek RTL8411 PCI Express Card Reader"; }
            case 0x8168: { return "Realtek RTL8111/8168/8411 Ethernet PCI-E"; }
            default: { return "Unrecognized Realtek Device"; }
        }
    }

    // atheros
    if (vendor_id == PCIVENDOR_ATHEROS)
    {
        switch (device_id)
        {
            case 0x0036: { return "Atheros AR9485 WiFi Adapter"; }
            default: { return "Unrecognized Atheros Device"; }
        }
    }

    // qemu graphics controller
    if (vendor_id == 0x1234 && device_id == 0x1111) { return "QEMU VGA Controller"; }

    // unknown
    return "Unrecognized Device";
}