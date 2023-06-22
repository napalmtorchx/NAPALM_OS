#include <hal/devices/ata.h>
#include <core/kernel.h>

// ports
#define ATA_PRIMARY_DATA         0x1F0
#define ATA_PRIMARY_ERR          0x1F1
#define ATA_PRIMARY_SECCOUNT     0x1F2
#define ATA_PRIMARY_LBA_LO       0x1F3
#define ATA_PRIMARY_LBA_MID      0x1F4
#define ATA_PRIMARY_LBA_HI       0x1F5
#define ATA_PRIMARY_DRIVE_HEAD   0x1F6
#define ATA_PRIMARY_COMM_REGSTAT 0x1F7
#define ATA_PRIMARY_ALTSTAT_DCR  0x3F6

// status flags
#define ATA_STAT_ERR  (1 << 0) 
#define ATA_STAT_DRQ  (1 << 3)
#define ATA_STAT_SRV  (1 << 4)
#define ATA_STAT_DF   (1 << 5) 
#define ATA_STAT_RDY  (1 << 6)
#define ATA_STAT_BSY  (1 << 7)

#define SECTOR_SZ 512

static ata_device_t _ata;
static uint8_t      _sector_cache[SECTOR_SZ];

void ata_init(void)
{
    _ata = (ata_device_t)
    {
        .base       = (device_t){ "ata_controller", NULL_DUID, ata_start, ata_stop, false },
        .identified = false,
    };
    devmgr_register(&_ata);
    devmgr_start(_ata.base.uid, NULL);
}

bool ata_start(ata_device_t* dev, void* unused)
{
    memset(_sector_cache, 0, SECTOR_SZ);
    irq_register(IRQ14, ata_callback);
    dev->identified = ata_identify();
    if (!dev->identified) { debug_out("%s Primary ATA disk not detected\n", DEBUG_WARN); return false; }
    return true;
}

int ata_stop(ata_device_t* dev)
{
    irq_unregister(IRQ14);
    dev->identified = false;
    return true;
}

void ata_callback(irq_context_t* context)
{
    irq_ack(context);
}

bool ata_identify(void)
{
    if (!_ata.base.running) { return false; }
    port_inb(ATA_PRIMARY_COMM_REGSTAT);
    port_outb(ATA_PRIMARY_DRIVE_HEAD, 0xA0);
    port_inb(ATA_PRIMARY_COMM_REGSTAT);
    port_outb(ATA_PRIMARY_SECCOUNT, 0);
    port_inb(ATA_PRIMARY_COMM_REGSTAT);
    port_outb(ATA_PRIMARY_LBA_LO, 0);
    port_inb(ATA_PRIMARY_COMM_REGSTAT);
    port_outb(ATA_PRIMARY_LBA_MID, 0);
    port_inb(ATA_PRIMARY_COMM_REGSTAT);
    port_outb(ATA_PRIMARY_LBA_HI, 0);
    port_inb(ATA_PRIMARY_COMM_REGSTAT);
    port_outb(ATA_PRIMARY_COMM_REGSTAT, 0xEC);
    port_outb(ATA_PRIMARY_COMM_REGSTAT, 0xE7);

    uint8_t status = port_inb(ATA_PRIMARY_COMM_REGSTAT);
    while (status & ATA_STAT_BSY) 
    {
        uint32_t i = 0;
        while(1) { i++; }
        for(i = 0; i < 0x0FFFFFFF; i++) { }
        status = port_inb(ATA_PRIMARY_COMM_REGSTAT);
    }

    if (status == 0) { return false; }
    while (status & ATA_STAT_BSY) { status = port_inb(ATA_PRIMARY_COMM_REGSTAT); }

    uint8_t mid = port_inb(ATA_PRIMARY_LBA_MID);
    uint8_t hi = port_inb(ATA_PRIMARY_LBA_HI);
    if (mid || hi) { return false; }

    while (!(status & (ATA_STAT_ERR | ATA_STAT_DRQ))) { status = port_inb(ATA_PRIMARY_COMM_REGSTAT); }
    if (status & ATA_STAT_ERR) { return false; }
    port_insw(ATA_PRIMARY_DATA, _sector_cache, 256);
    memset(_sector_cache, 0, SECTOR_SZ);
    return true;
}

void ata_read(uint64_t sector, size_t count, void* src)
{
    if (!_ata.base.running) { return; }

    port_outb(ATA_PRIMARY_DRIVE_HEAD, 0x40);                        // Select master
    port_outb(ATA_PRIMARY_SECCOUNT, (count >> 8) & 0xFF );          // sectorcount high
    port_outb(ATA_PRIMARY_LBA_LO, (sector >> 24) & 0xFF);           // LBA4
    port_outb(ATA_PRIMARY_LBA_MID, (sector >> 32) & 0xFF);          // LBA5
    port_outb(ATA_PRIMARY_LBA_HI, (sector >> 40) & 0xFF);           // LBA6
    port_outb(ATA_PRIMARY_SECCOUNT, count & 0xFF);                  // sectorcount low
    port_outb(ATA_PRIMARY_LBA_LO, sector & 0xFF);                   // LBA1
    port_outb(ATA_PRIMARY_LBA_MID, (sector >> 8) & 0xFF);           // LBA2
    port_outb(ATA_PRIMARY_LBA_HI, (sector >> 16) & 0xFF);           // LBA3
    port_outb(ATA_PRIMARY_COMM_REGSTAT, 0x24);                      // READ SECTORS EXT

    uint8_t* buff = (uint8_t*)src;
    for (uint32_t i = 0; i < count; i++) 
    {
        while (true)
        {
            uint8_t status = port_inb(ATA_PRIMARY_COMM_REGSTAT);
            if (status & ATA_STAT_DRQ) { break; }
            if (status & ATA_STAT_ERR) { debug_exception("ata_read(%d, %d, %p) - Disk read error", (uint32_t)sector, count, src); return; }
        }
        port_insw(ATA_PRIMARY_DATA, buff, 256);
        buff += 256;
    }

}

void ata_write(uint64_t sector, size_t count, void* dest)
{
    if (!_ata.base.running) { return; }

    port_outb(ATA_PRIMARY_DRIVE_HEAD, 0x40);                        // Select master
    port_outb(ATA_PRIMARY_SECCOUNT, (count >> 8) & 0xFF );          // sectorcount high
    port_outb(ATA_PRIMARY_LBA_LO, (sector >> 24) & 0xFF);           // LBA4
    port_outb(ATA_PRIMARY_LBA_MID, (sector >> 32) & 0xFF);          // LBA5
    port_outb(ATA_PRIMARY_LBA_HI, (sector >> 40) & 0xFF);           // LBA6
    port_outb(ATA_PRIMARY_SECCOUNT, count & 0xFF);                  // sectorcount low
    port_outb(ATA_PRIMARY_LBA_LO, sector & 0xFF);                   // LBA1
    port_outb(ATA_PRIMARY_LBA_MID, (sector >> 8) & 0xFF);           // LBA2
    port_outb(ATA_PRIMARY_LBA_HI, (sector >> 16) & 0xFF);           // LBA3
    port_outb(ATA_PRIMARY_COMM_REGSTAT, 0x34);                      // READ SECTORS EXT

    uint8_t* buff = (uint8_t*)dest;
    for (uint8_t i = 0; i < count; i++) 
    {
        while (true) 
        {
            uint8_t status = port_inb(ATA_PRIMARY_COMM_REGSTAT);
            if (status & ATA_STAT_DRQ) { break; }
            else if (status & ATA_STAT_ERR) { debug_exception("ata_write(%d, %d, %p) - Disk write error", (uint32_t)sector, count, dest); return; }
        }
        port_outsw(ATA_PRIMARY_DATA, buff, 256);
        buff += 256;
    }
    port_outb(ATA_PRIMARY_COMM_REGSTAT, 0xE7);
    while (port_inb(ATA_PRIMARY_COMM_REGSTAT) & ATA_STAT_BSY);
}
