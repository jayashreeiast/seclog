#include "fspi.h"

/* MMIO helper */
#define REG32(x) (*(volatile uint32_t *)(x))

/* FlexSPI registers (subset) */
#define MCR0        REG32(FSPI_BASE + 0x000)
#define MCR1        REG32(FSPI_BASE + 0x004)
#define AHBCR       REG32(FSPI_BASE + 0x008)
#define INTEN       REG32(FSPI_BASE + 0x018)
#define INTR        REG32(FSPI_BASE + 0x014)
#define LUTKEY      REG32(FSPI_BASE + 0x300)
#define LUTCR       REG32(FSPI_BASE + 0x304)
#define LUT_BASE    (FSPI_BASE + 0x310)
#define IPCR        REG32(FSPI_BASE + 0xA0)
#define IPCMD       REG32(FSPI_BASE + 0xB0)
#define IPTXFCR     REG32(FSPI_BASE + 0x80)
#define IPRXFCR     REG32(FSPI_BASE + 0x90)
#define TFDR        REG32(FSPI_BASE + 0x180)
#define RFDR        REG32(FSPI_BASE + 0x100)

/* Interrupt bits */
#define INTR_IPCMDDONE   (1 << 1)

/* LUT */
#define LUT_KEY_VALUE    0x5AF05AF0
#define LUT_UNLOCK      0x02

/* LUT sequence IDs */
#define LUT_READ_ID     0
#define LUT_READ        1
#define LUT_WREN        2
#define LUT_PP          3
#define LUT_SE          4


/* MC_ME Register Offsets */
#define MC_ME_BASE             0x40088000
#define PRTN0_PCONF            (MC_ME_BASE + 0x100)
#define PRTN0_PUPD             (MC_ME_BASE + 0x104)
#define PRTN0_PSTAT            (MC_ME_BASE + 0x108)
#define PRTN0_COFB1_CLKEN      (MC_ME_BASE + 0x144)

/* FlexSPI is typically Bit 28 in COFB1 */
#define REQ_FLEXSPI_BIT        (1 << 28)
#define PUPD_PCUD              (1 << 0)
#define PSTAT_PSTATUS          (1 << 0)

#include <common/debug.h> // For ERROR/INFO macros in TF-A
#include <lib/mmio.h>     // Use TF-A standard IO functions

#define mmio_write_32(addr, val) (*(volatile uint32_t *)(addr) = (val))
#define mmio_read_32(addr)       (*(volatile uint32_t *)(addr))

void fspi_enable_clock(void) {
    /* 1. Set the clock enable bit for FlexSPI in Partition 0, COFB1 */
    uint32_t val = reg_read(PRTN0_COFB1_CLKEN);
    val |= REQ_FLEXSPI_BIT;
    reg_write(PRTN0_COFB1_CLKEN, val);

    /* 2. Set the Partition 0 Power Confirmation */
    reg_write(PRTN0_PCONF, 1); 

    /* 3. Trigger the update (PUPD) */
    reg_write(PRTN0_PUPD, PUPD_PCUD);

    /* 4. Poll PSTAT until the partition update is complete */
    while (!(reg_read(PRTN0_PSTAT) & PSTAT_PSTATUS));
    
    /* 5. Small delay to let the clock stabilize */
    for(volatile int i=0; i<1000; i++); 
}

static void fspi_wait(void)
{
    while (!(INTR & INTR_IPCMDDONE))
        ;
    INTR = INTR_IPCMDDONE;
}

static void fspi_lut_write(uint32_t index, uint32_t val)
{
    REG32(LUT_BASE + index * 4) = val;
}

void fspi_init(void)
{
    /* Reset controller */
    MCR0 |= (1 << 1);
    while (MCR0 & (1 << 1));

    /* Disable AHB buffering */
    AHBCR = 0;

    /* Unlock LUT */
    LUTKEY = LUT_KEY_VALUE;
    LUTCR = LUT_UNLOCK;

    /* READ ID (0x9F) */
    fspi_lut_write(0,  0x081F0001); /* CMD 0x9F */
    fspi_lut_write(1,  0x24000003); /* READ 3 bytes */

    /* READ (0x03) */
    fspi_lut_write(4,  0x081F0001); /* CMD 0x03 */
    fspi_lut_write(5,  0x08200003); /* ADDR 3 bytes */
    fspi_lut_write(6,  0x24000000); /* READ */

    /* WRITE ENABLE (0x06) */
    fspi_lut_write(8,  0x081F0001); /* CMD 0x06 */

    /* PAGE PROGRAM (0x02) */
    fspi_lut_write(12, 0x081F0001); /* CMD 0x02 */
    fspi_lut_write(13, 0x08200003); /* ADDR */
    fspi_lut_write(14, 0x20000000); /* WRITE */

    /* SECTOR ERASE (0x20) */
    fspi_lut_write(16, 0x081F0001); /* CMD 0x20 */
    fspi_lut_write(17, 0x08200003); /* ADDR */
}

int fspi_read_id(uint8_t id[3])
{
    IPRXFCR = 0;
    IPCR = (LUT_READ_ID << 24) | 3;
    IPCMD = 1;
    fspi_wait();

    uint32_t v = RFDR;
    id[0] = v & 0xFF;
    id[1] = (v >> 8) & 0xFF;
    id[2] = (v >> 16) & 0xFF;
    return 0;
}

int fspi_read(uint32_t addr, uint8_t *buf, size_t len)
{
    volatile uint8_t *flash = (volatile uint8_t *)(FSPI_AHB_BASE + addr);
    for (size_t i = 0; i < len; i++)
        buf[i] = flash[i];
    return 0;
}

static void fspi_wren(void)
{
    IPCR = (LUT_WREN << 24);
    IPCMD = 1;
    fspi_wait();
}

int fspi_write(uint32_t addr, const uint8_t *buf, size_t len)
{
    fspi_wren();

    IPTXFCR = 0;
    for (size_t i = 0; i < len; i++)
        ((volatile uint8_t *)&TFDR)[i] = buf[i];

    IPCR = (LUT_PP << 24) | len;
    IPCMD = 1;
    fspi_wait();
    return 0;
}

int fspi_erase_sector(uint32_t addr)
{
    fspi_wren();
    IPCR = (LUT_SE << 24) | addr;
    IPCMD = 1;
    fspi_wait();
    return 0;
}

