/*
 * Copyright 2025 NXP.
 */

#ifndef HW_SYSTEM_H
#define HW_SYSTEM_H

#ifdef __cplusplus
extern "C"
{
#endif

/*==================================================================================================
*                                       DEFINES AND MACROS
==================================================================================================*/
/* register access */
#define SYS_REG32(address)      ( *(volatile int   *)(address) ) /**<  32-bit register */
#define SYS_REG16(address)      ( *(volatile short *)(address) ) /**<  16-bit register */
#define SYS_REG8(address)       ( *(volatile char  *)(address) ) /**<   8-bit register */

/** STM */
#define ENABLE                              (1 << 0)
#define FREEZE                              (1 << 1) /* Allows the timer counter to be stopped when the device enters Debug mode. */

#define STM_BASE(id)                        (0x4011C000 + (id) * 0x4000)
#define STM_ENABLE(id, div)                 (SYS_REG32(STM_BASE(id)) = (ENABLE | FREEZE) + ((0xff & (div)) << 8))
#define STM_ENABLE_CHAN(id, chan, cmp)      {                                                                       \
                                                SYS_REG32(STM_BASE(id) + 0x10 * (chan) + 0x14) = 1;                 \
                                                SYS_REG32(STM_BASE(id) + 0x10 * (chan) + 0x18) = (cmp);             \
                                                SYS_REG32(STM_BASE(id) + 0x10 * (chan) + 0x10) = 1;                 \
                                            }
#define STM_UPDATE_CHAN(id, chan, new)      {                                                                       \
                                                uint32_t oldVal = SYS_REG32(STM_BASE(id) + 0x10 * (chan) + 0x18);   \
                                                oldVal += (new);                                                    \
                                                SYS_REG32(STM_BASE(id) + 0x10 * (chan) + 0x14) = 1;                 \
                                                SYS_REG32(STM_BASE(id) + 0x10 * (chan) + 0x18) = oldVal;            \
                                            }
#define STM_INT_CLEAR(id, chan)      {                                                                       \
                                                SYS_REG32(STM_BASE(id) + 0x10 * (chan) + 0x14) = 1;            \
                                            }

#define STM_CR_VAL(id, chan)      ( SYS_REG32(STM_BASE(id) + 0x10 * (chan) + 0x4) )
#define STM_CNT_VAL(id)           ( SYS_REG32(STM_BASE(id) + 0x4 ) )
#define STM_CMP_VAL(id, chan)     ( SYS_REG32(STM_BASE(id) + 0x10 * (chan) + 0x18) )


#define BASE_ADDRESS_MC_ME      0x40088000u
#define BASE_ADDRESS_FXOSC      0x40050000u
#define MC_ME_CTL_KEY           *(volatile unsigned int*)(BASE_ADDRESS_MC_ME + 0x000u)
#define MC_ME_PRTN0_PCONF       *(volatile unsigned int*)(BASE_ADDRESS_MC_ME + 0x100u)
#define MC_ME_PRTN0_PUPD        *(volatile unsigned int*)(BASE_ADDRESS_MC_ME + 0x104u)
#define FXOSC_CTRL              *(volatile unsigned int*)(BASE_ADDRESS_FXOSC + 0x000u)


#ifdef __cplusplus
}
#endif

#endif /** HW_SYSTEM_H */
