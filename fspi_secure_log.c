#include "fspi.h"

/* Register Offsets */
#define FSPI_MCR0        REG32(FSPI_BASE + 0x00)
#define FSPI_LUTKEY      REG32(FSPI_BASE + 0x18)
#define FSPI_LUTCR       REG32(FSPI_BASE + 0x1C)
#define FSPI_AHBCR       REG32(FSPI_BASE + 0x60)
#define FSPI_IPRXFCR     REG32(FSPI_BASE + 0xB8)
#define FSPI_IPTXFCR     REG32(FSPI_BASE + 0xBC)
#define FSPI_IPCR0       REG32(FSPI_BASE + 0xA0)
#define FSPI_IPCR1       REG32(FSPI_BASE + 0xA4)
#define FSPI_IPCMD       REG32(FSPI_BASE + 0xB0)
#define FSPI_INTR        REG32(FSPI_BASE + 0x14)
#define FSPI_RFDR        (FSPI_BASE + 0x100)
#define FSPI_TFDR        (FSPI_BASE + 0x180)
#define FSPI_LUT_BASE    (FSPI_BASE + 0x200)

/* Opcodes for Macronix 64MB (4-Byte Address Mode) */
#define CMD_READ_ID      0x9F
#define CMD_4B_READ      0x0C
#define CMD_WREN         0x06
#define CMD_4B_PP        0x12
#define CMD_4B_SE        0x21

static void fspi_enable_clock(void) {
    /* Enable FlexSPI Clock in MC_ME Partition 0 */
    REG32(MC_ME_BASE + 0x144) |= (1 << 28); // PRTN0_COFB1_CLKEN
    REG32(MC_ME_BASE + 0x100) = 1;          // PRTN0_PCONF
    REG32(MC_ME_BASE + 0x104) = 1;          // PRTN0_PUPD (Trigger)
    while (!(REG32(MC_ME_BASE + 0x108) & 1)); // Poll PRTN0_PSTAT
}

static void fspi_wait(void) {
    /* Bit 0 is IPCMDDONE on S32G3 */
    while (!(FSPI_INTR & 0x01));
    FSPI_INTR |= 0x01; // Clear flag
}

static int fspi_wait_wip(void) {
    /* Poll Status Register (WIP bit) */
    uint32_t status;
    int timeout = 1000000;
    while (timeout--) {
        FSPI_IPRXFCR = 0x01;
        FSPI_IPCR0 = 0;
        FSPI_IPCR1 = (0x05 << 24) | 1; // Assuming Status Read is manually triggered or add to LUT
        /* For simplicity in this driver, we assume WIP polling is handled 
           by checking the ready state via LUT if you add SEQ_READ_STATUS */
        return 0; // Placeholder: In production, poll Opcode 0x05
    }
    return -1;
}

void fspi_init(void) {
    fspi_enable_clock();

    /* Software Reset */
    FSPI_MCR0 |= (1 << 1);
    while (FSPI_MCR0 & (1 << 1));

    /* Unlock LUT */
    FSPI_LUTKEY = 0x5AF05AF0;
    FSPI_LUTCR = 0x02;

    /* LUT 0: READ ID */
    REG32(FSPI_LUT_BASE + 0) = LUT_DEF(LUT_CMD, LUT_PAD(1), CMD_READ_ID) | 
                               (LUT_DEF(LUT_NXP_READ, LUT_PAD(1), 3) << 16);

    /* LUT 1: FAST READ (4-Byte Addr) */
    REG32(FSPI_LUT_BASE + 4) = LUT_DEF(LUT_CMD, LUT_PAD(1), CMD_4B_READ) | 
                               (LUT_DEF(LUT_ADDR, LUT_PAD(1), 32) << 16);
    REG32(FSPI_LUT_BASE + 5) = LUT_DEF(LUT_DUMMY, LUT_PAD(1), 8) | 
                               (LUT_DEF(LUT_NXP_READ, LUT_PAD(1), 0) << 16);

    /* LUT 2: WREN */
    REG32(FSPI_LUT_BASE + 8) = LUT_DEF(LUT_CMD, LUT_PAD(1), CMD_WREN);

    /* LUT 3: PAGE PROGRAM (4-Byte Addr) */
    REG32(FSPI_LUT_BASE + 12) = LUT_DEF(LUT_CMD, LUT_PAD(1), CMD_4B_PP) | 
                                (LUT_DEF(LUT_ADDR, LUT_PAD(1), 32) << 16);
    REG32(FSPI_LUT_BASE + 13) = LUT_DEF(LUT_NXP_WRITE, LUT_PAD(1), 0);

    /* LUT 4: SECTOR ERASE (4-Byte Addr) */
    REG32(FSPI_LUT_BASE + 16) = LUT_DEF(LUT_CMD, LUT_PAD(1), CMD_4B_SE) | 
                                (LUT_DEF(LUT_ADDR, LUT_PAD(1), 32) << 16);
}

int fspi_read_id(uint8_t id[3]) {
    FSPI_IPRXFCR = 0x01;
    FSPI_IPCR0 = 0;
    FSPI_IPCR1 = (LUT_READ_ID << 24) | 3;
    FSPI_IPCMD = 0x01;
    fspi_wait();
    uint32_t val = REG32(FSPI_RFDR);
    id[0] = val & 0xFF;
    id[1] = (val >> 8) & 0xFF;
    id[2] = (val >> 16) & 0xFF;
    return 0;
}

int fspi_erase_sector(uint32_t addr) {
    /* 1. Write Enable */
    FSPI_IPCR1 = (LUT_WREN << 24);
    FSPI_IPCMD = 0x01;
    fspi_wait();

    /* 2. Erase */
    FSPI_IPCR0 = addr;
    FSPI_IPCR1 = (LUT_SE << 24);
    FSPI_IPCMD = 0x01;
    fspi_wait();
    return 0;
}

int fspi_write(uint32_t addr, const uint8_t *buf, uint32_t len) {
    /* 1. Write Enable */
    FSPI_IPCR1 = (LUT_WREN << 24);
    FSPI_IPCMD = 0x01;
    fspi_wait();

    /* 2. Clear & Fill TX FIFO */
    FSPI_IPTXFCR = 0x01;
    for (uint32_t i = 0; i < (len + 3) / 4; i++) {
        uint32_t word = 0;
        memcpy(&word, buf + (i * 4), (len - i*4 > 4) ? 4 : len - i*4);
        REG32(FSPI_TFDR + (i * 4)) = word;
    }

    /* 3. Trigger Program */
    FSPI_IPCR0 = addr;
    FSPI_IPCR1 = (LUT_PP << 24) | len;
    FSPI_IPCMD = 0x01;
    fspi_wait();
    return 0;
}

int fspi_read(uint32_t addr, uint8_t *buf, uint32_t len) {
    FSPI_IPRXFCR = 0x01;
    FSPI_IPCR0 = addr;
    FSPI_IPCR1 = (LUT_READ << 24) | len;
    FSPI_IPCMD = 0x01;
    fspi_wait();

    for (uint32_t i = 0; i < (len + 3) / 4; i++) {
        uint32_t val = REG32(FSPI_RFDR + (i * 4));
        memcpy(buf + (i * 4), &val, (len - i*4 > 4) ? 4 : len - i*4);
    }
    return 0;
}
