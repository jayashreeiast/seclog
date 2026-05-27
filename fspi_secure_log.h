#ifndef FSPI_H
#define FSPI_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>

/* S32G3 FlexSPI & MC_ME Base Addresses */
#define FSPI_BASE           0x40310000
#define FSPI_AHB_BASE       0x00000000
#define MC_ME_BASE          0x40088000

/* Register Access Macro */
#define REG32(x) (*(volatile uint32_t *)(uintptr_t)(x))

/* LUT Instruction Macros */
#define LUT_CMD             0x01
#define LUT_ADDR            0x02
#define LUT_DUMMY           0x0C
#define LUT_NXP_WRITE       0x08
#define LUT_NXP_READ        0x09
#define LUT_PAD(x)          (x == 1 ? 0 : (x == 2 ? 1 : (x == 4 ? 2 : 3)))
#define LUT_DEF(ins, pad, opr) (((ins) << 10) | ((pad) << 8) | (opr))

/* Sequence IDs */
#define LUT_READ_ID         0
#define LUT_READ            1
#define LUT_WREN            2
#define LUT_PP              3
#define LUT_SE              4

/* Function Prototypes */
void fspi_init(void);
int fspi_read_id(uint8_t id[3]);
int fspi_write(uint32_t addr, const uint8_t *buf, uint32_t len);
int fspi_read(uint32_t addr, uint8_t *buf, uint32_t len);
int fspi_erase_sector(uint32_t addr);

#endif
