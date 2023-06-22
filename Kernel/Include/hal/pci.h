#pragma once
#include <lib/types.h>

typedef enum
{
    PCIVENDOR_INTEL                = 0x8086,
    PCIVENDOR_INTEL_ALT            = 0x8087,
    PCIVENDOR_VMWARE               = 0x15AD,
    PCIVENDOR_VMWARE_ALT           = 0x0E0F,
    PCIVENDOR_DELL                 = 0x1028,
    PCIVENDOR_DELL_ALT             = 0x413C,
    PCIVENDOR_ATI                  = 0x1002,
    PCIVENDOR_AMD                  = 0x1022,
    PCIVENDOR_VIA                  = 0x2109,
    PCIVENDOR_BENQ                 = 0x04A5,
    PCIVENDOR_ACER                 = 0x5986,
    PCIVENDOR_HP                   = 0x03F0,
    PCIVENDOR_HP_ALT               = 0x103C,
    PCIVENDOR_HP_ENT               = 0x1590,
    PCIVENDOR_ASUS                 = 0x1043,
    PCIVENDOR_ASUS_ALT             = 0x0B05,
    PCIVENDOR_INNOTEK              = 0x80EE,
    PCIVENDOR_REALTEK              = 0x10EC,
    PCIVENDOR_REAKLTEK_ALT         = 0x0BDA,
    PCIVENDOR_ENSONIQ              = 0x1274,
    PCIVENDOR_BROADCOM             = 0x14E4,
    PCIVENDOR_ATHEROS              = 0x168C,
} PCIVENDOR;



typedef enum
{
    PCITYPE_UNCLASSIFIED        = 0x00,
    PCITYPE_STORAGE             = 0x01,
    PCITYPE_NETWORK             = 0x02,
    PCITYPE_DISPLAY             = 0x03,
    PCITYPE_MULTIMEDIA          = 0x04,
    PCITYPE_MEMORY              = 0x05,
    PCITYPE_BRIDGE              = 0x06,
    PCITYPE_SIMPLE_COMM         = 0x07,
    PCITYPE_SYS_PERIPHERAL      = 0x08,
    PCITYPE_INPUT               = 0x09,
    PCITYPE_DOCKING_STATION     = 0x0A,
    PCITYPE_PROCESSOR           = 0x0B,
    PCITYPE_SERIAL_BUS          = 0x0C,
    PCITYPE_WIRELESS            = 0x0D,
    PCITYPE_INTELLIGENT         = 0x0E,
    PCITYPE_SATELLITE_COMM      = 0x0F,
    PCITYPE_ENCRYPTION          = 0x10,
    PCITYPE_SIGNAL_PROCESSOR    = 0x11,
    PCITYPE_PROCESSING_ACCEL    = 0x12,
    PCITYPE_NON_ESSENTIAL_INSTR = 0x13,
    PCITYPE_CO_PROCESSOR        = 0x40,
    PCITYPE_UNASSIGNED          = 0xFF,
} PCITYPE;

typedef enum
{
    PCIROLE_DEVICE,
    PCIROLE_BRIDGE,
    PCIROLE_CARDBUS,
} PCIROLE;

typedef struct
{
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t status;
    uint16_t command;
    uint8_t  class_code;
    uint8_t  subclass;
    uint8_t  prog_if;
    uint8_t  rev_id;
    uint8_t  bist;
    PCIROLE  hdr_type;
    uint8_t  latency_timer;
    uint8_t  cacheln_sz;
} attr_pack pci_hdr_t;

typedef struct
{
    pci_hdr_t hdr;
    uint32_t bar0;
    uint32_t bar1;
    uint32_t bar2;
    uint32_t bar3;
    uint32_t bar4;
    uint32_t bar5;
    uint32_t p_cardbus_cis;
    uint16_t subsys_id;
    uint16_t subsys_vendor_id;
    uint32_t exprom_bar;
    uint8_t  reserved0[3];
    uint8_t  p_capabilities;
    uint8_t  reserved1[4];
    uint8_t  max_latency;
    uint8_t  min_grant;
    uint8_t  irq_pin;
    uint8_t  irq_line;
} attr_pack pci_device_t;

typedef struct
{
    pci_hdr_t hdr;
    uint32_t  bar0;
    uint32_t  bar1;
    uint8_t   secondary_latency_timer;
    uint8_t   subcoord_busnum;
    uint8_t   secondary_busnum;
    uint8_t   primary_busnum;
    uint16_t  secondary_status;
    uint8_t   io_limit;
    uint8_t   io_base;
    uint16_t  mem_limit;
    uint16_t  mem_base;
    uint16_t  prefetch_mem_limit;
    uint16_t  prefetch_mem_base;
    uint32_t  prefetch_base_upper;
    uint32_t  prefetch_limit_upper;
    uint16_t  io_limit_upper;
    uint16_t  io_base_upper;
    uint8_t   reserved[3];
    uint8_t   p_capabilities;
    uint32_t  exprom_bar;
    uint16_t  bridge_ctrl;
    uint8_t   irq_pin;
    uint8_t   irq_line;
} attr_pack pci_bridge_t;

typedef struct
{
    pci_hdr_t hdr;
    uint32_t  cardbus_socket_bar;
    uint16_t  secondary_status;
    uint8_t   reserved;
    uint8_t   p_list_caps;
    uint8_t   cardbus_latency_timer;
    uint8_t   subcoord_busnum;
    uint8_t   cardbus_busnum;
    uint8_t   pcibus_num;
    uint32_t  mem_base0;
    uint32_t  mem_limit0;
    uint32_t  mem_base1;
    uint32_t  mem_limit1;
    uint32_t  io_base0;
    uint32_t  io_limit0;
    uint32_t  io_base1;
    uint32_t  io_limit1;
    uint16_t  bridge_ctrl;
    uint8_t   irq_pin;
    uint8_t   irq_line;
    uint16_t  subsys_vendor_id;
    uint16_t  subsys_device_id;
    uint32_t  legacy16_base;
} attr_pack pci_cardbus_t;

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

#define PCI_COUNT 1024
#define PCI_BUS_COUNT  256
#define PCI_SLOT_COUNT 32
#define PCI_FUNC_COUNT 8

#define PCI_VENDOR_INVALID 0xFFFF\

void pci_probe(void);
void pci_print(bool cmd);
pci_hdr_t pci_probedev(uint16_t bus, uint16_t slot, uint16_t func);
void pci_write(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset, uint16_t data);
uint16_t pci_read(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset);
pci_hdr_t** pci_list(void);
size_t pci_count(void);
const char* pci_getname(uint16_t vendor_id, uint16_t device_id);
