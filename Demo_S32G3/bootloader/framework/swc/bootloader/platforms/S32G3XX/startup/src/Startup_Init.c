/**
 * @file      Startup_Init.c
 * @version   24.12.0
 *
 * @brief     Additional startup initialization routines
 *
 *            Platform     : CORTEXM
 *            Build Version: S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
 *
 * @copyright (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
 *                All Rights Reserved.
 *
 *            This file contains sample code only.
 *            It is not part of the production code deliverables.
 */

#include "Std_Types.h"
#include "S32G399A.h"
#include "Soc_Ips.h"
#include "Mcal.h"

/* ARM System control block registers. */
#define ARM_SCB_CCR                         (0xE000ED14U)
#define ARM_SCB_CPACR                       (0xE000ED88U)
#define ARM_SCB_CPACR_CP10_MASK             (0x00300000U)
#define ARM_SCB_CPACR_CP11_MASK             (0x00C00000U)
#define ARM_SCB_CCR_NONBASETHRDENA_EN_MASK  (0x00000001U)

#define SCB_CCR_IC_SHIFT                    (17U)
#define SCB_CCR_IC_MASK                     (1UL << 17U)

#define SIUL2_MINOR_MASK                    (0x0FU)
#define SIUL2_MINOR_SHIFT                   (0x0U)
#define SIUL2_MAJOR_MASK                    (0xF0U)
#define SIUL2_MAJOR_SHIFT                   (0x4U)

#define SCB_CCSIDR_NUMSETS_SHIFT            (13U)
#define SCB_CCSIDR_NUMSETS_MASK             (0x7FFFUL << 13U)

#define SCB_CCSIDR_ASSOCIATIVITY_SHIFT      (3U)        
#define SCB_CCSIDR_ASSOCIATIVITY_MASK       (0x3FFUL << 3U)

#define SCB_DCISW_SET_SHIFT                 (5U)
#define SCB_DCISW_SET_MASK                  (0x1FFUL << 5U)

#define SCB_DCISW_WAY_SHIFT                 (30U)
#define SCB_DCISW_WAY_MASK                  (3UL << 30U)
   
#define SCB_CCR_DC_SHIFT                    (16U)
#define SCB_CCR_DC_MASK                     (1UL << 16U)

/* Cache Size ID Register Macros */
#define CCSIDR_WAYS(x)     (((x) & SCB_CCSIDR_ASSOCIATIVITY_MASK) >> SCB_CCSIDR_ASSOCIATIVITY_SHIFT)
#define CCSIDR_SETS(x)     (((x) & SCB_CCSIDR_NUMSETS_MASK) >> SCB_CCSIDR_NUMSETS_SHIFT)

/**
 * @brief The copy table layout, which is used for doing
 *        the ROM-to-RAM initialization via copy.
 */
struct Startup_Init_CopyTable
{
    const uint8 *rom_start;
    const uint8 *rom_end;
    uint8       *ram_start;
};

/**
 * @brief The RAM table layout, which is zero initialized.
 */
struct Startup_Init_ZeroTable
{
    uint8 *ram_start;
    uint8 *ram_end;
};

/**
 * @brief The linker-defined symbols for the init tables.
 *        This will contain the first element of the table, i.e. the size.
 */
extern uint32 __INIT_TABLE_SIZE;
extern uint32 __ZERO_TABLE_SIZE;

static void Startup_m7_cache_init(void)
{
#ifdef D_CACHE_ENABLE
    uint32 ccsidr = 0U;
    uint32 sets = 0U;
    uint32 ways = 0U;

    /*init Data caches*/
    S32_SCB->CSSELR = 0U;                       /* select Level 1 data cache */

    MCAL_DATA_SYNC_BARRIER();
    ccsidr = S32_SCB->CCSIDR;
    sets = (uint32)(CCSIDR_SETS(ccsidr));
    do {
      ways = (uint32)(CCSIDR_WAYS(ccsidr));
      do {
        S32_SCB->DCISW = (((sets << SCB_DCISW_SET_SHIFT) & SCB_DCISW_SET_MASK) |
                      ((ways << SCB_DCISW_WAY_SHIFT) & SCB_DCISW_WAY_MASK)  );

    MCAL_DATA_SYNC_BARRIER();
      } while (ways-- != 0U);
    } while(sets-- != 0U);
    MCAL_DATA_SYNC_BARRIER();
    S32_SCB->CCR |=  (uint32)SCB_CCR_DC_MASK;  /* enable D-Cache */
    MCAL_DATA_SYNC_BARRIER();
    MCAL_INSTRUCTION_SYNC_BARRIER();
#endif /*D_CACHE_ENABLE*/

#ifdef I_CACHE_ENABLE
    /*init Code caches*/
    S32_SCB->ICIALLU = 0UL;                    /* invalidate I-Cache */
    MCAL_DATA_SYNC_BARRIER();
    MCAL_INSTRUCTION_SYNC_BARRIER();
    S32_SCB->CCR |=  (uint32)SCB_CCR_IC_MASK;  /* enable I-Cache */
    MCAL_DATA_SYNC_BARRIER();
    MCAL_INSTRUCTION_SYNC_BARRIER();
#endif /*I_CACHE_ENABLE*/
}

/**
 * @brief Run the copy initialization by using the input address
 *        for the table.
 *        This will linearly copy all ROM memory to RAM, taking into
 *        account the ROM region size.
 *
 * @param copy_table Pre-determined address for the copy table
 * @param len        The number of tables to iterate through
 */
static void Startup_Init_Copy(const struct Startup_Init_CopyTable *copy_table,
                              sint8 len)
{
    for (sint8 i = 0; i < len; i++)
    {
        const uint8 *rom = copy_table[i].rom_start;
        uint8 *ram = copy_table[i].ram_start;
        /* Avoid overflow by using signed values. */
        sint64 size = (sint64) copy_table[i].rom_end -
                      (sint64) copy_table[i].rom_start;

        for (sint64 j = 0; j < size; j++)
        {
            ram[j] = rom[j];
        }
    }
}

/**
 * @brief Run the zero initialization by using the input address
 *        for the table.
 *        This will zero out the whole RAM memory region, from start to end.
 *
 * @param zero_table Pre-determined address for the zero table
 * @param len        The number of tables to iterate through
 */
static void Startup_Init_Zero(const struct Startup_Init_ZeroTable *zero_table,
                              sint8 len)
{
    for (sint8 i = 0; i < len; i++)
    {
        uint8 *ram = zero_table[i].ram_start;
        /* Avoid overflow by using signed values. */
        sint64 size = (sint64) zero_table[i].ram_end -
                      (sint64) zero_table[i].ram_start;

        for(sint64 j = 0; j < size; j++)
        {
            ram[j] = 0U;
        }
    }
}

/**
 * @brief Run the initialization for both the copy and zero table.
 *        If either table size is zero, then the init is skipped.
 *
 */
void Startup_Init_Memory(void)
{
    sint8 len;

    /* Run the ROM-to-RAM copy. */
    const struct Startup_Init_CopyTable *copy_table;
    len = __INIT_TABLE_SIZE;
    /* Skip the first element, which is the size. */
    copy_table = (const struct Startup_Init_CopyTable *) (&__INIT_TABLE_SIZE
                                                          + 1);
    Startup_Init_Copy(copy_table, len);

    /* Run the zero initialization. */
    const struct Startup_Init_ZeroTable *zero_table;
    len = __ZERO_TABLE_SIZE;
    /* Skip the first element, which is the size. */
    zero_table = (const struct Startup_Init_ZeroTable *) (&__ZERO_TABLE_SIZE
                                                          + 1);
    Startup_Init_Zero(zero_table, len);
}

/*
 * SystemWfiConfig : Implement errata ERR051149
 * SystemWfiConfig : Implement errata ERR051166
 */
#if defined(ERR_CORTEX_M7_E051149) || defined(ERR_CORTEX_M7_E051166)
void Startup_Wfi_Config(void)
{
    uint32 u32EdbStatus = 0U;
/*  Major Mask Revision
    0000b - Rev 1.x
    0001b - Rev 2.x
    Minor Mask Revision
    0000b - Rev x.0
    0001b - Rev x.1 */
    uint32 U32MajorMinor = (uint32_t)(IP_SIUL2_0->MIDR1);
    uint8 Minor = (uint8)((U32MajorMinor & SIUL2_MINOR_MASK) >> SIUL2_MINOR_SHIFT);
    uint8 Major = (uint8)((U32MajorMinor & SIUL2_MAJOR_MASK) >> SIUL2_MAJOR_SHIFT);
    if (Major == 0U && Minor == 0U)
    {
        u32EdbStatus = (uint32_t)(IP_SECURITY_CC->EXT_DBGSTAT & SECURITY_EXT_DBGSTAT_EDB_MASK);
        if (u32EdbStatus == 0U)
        {
            IP_MDM_AP->DAP_EN_CTRL &= ~MDM_AP_DAP_EN_CTRL_CSPNIDEN_MASK;
            IP_MDM_AP->DAP_EN_CTRL &= ~MDM_AP_DAP_EN_CTRL_CSPIDEN_MASK;
            IP_MDM_AP->DAP_EN_CTRL &= ~MDM_AP_DAP_EN_CTRL_CNIDEN_MASK;
            IP_MDM_AP->DAP_EN_CTRL &= ~MDM_AP_DAP_EN_CTRL_CDBGEN_MASK;
            IP_MDM_AP->CONTROL |= MDM_AP_CONTROL_CM7_0_EDBGREQ_MASK;
            IP_MDM_AP->CONTROL |= MDM_AP_CONTROL_CM7_1_EDBGREQ_MASK;
            IP_MDM_AP->CONTROL |= MDM_AP_CONTROL_CM7_2_EDBGREQ_MASK;
#if defined(ERR_CORTEX_M7_E051166)
            IP_MDM_AP->CORTEX_A53_A0_IP_DBG |= MDM_AP_CORTEX_A53_A0_IP_DBG_CM7_3_EDBGREQ_MASK;
#endif
        }
    }
}
#endif /* defined(ERR_CORTEX_M7_E051149) || defined(ERR_CORTEX_M7_E051166) */

/**
 * @brief Enable FPU co-processors and NONBASETHRDENA mode.
 *
 */
void Startup_Init_System(void)
{
    /* Make sure the processor can return to Thread mode via EXC_RETURN, */
    /* from Handler mode, regardless of any other active exceptions. */
    REG_WRITE32(ARM_SCB_CCR, REG_READ32(ARM_SCB_CCR) |
                             ARM_SCB_CCR_NONBASETHRDENA_EN_MASK);

#ifdef ENABLE_FPU
    /* Enable CP10 and CP11 coprocessors. */
    REG_WRITE32(ARM_SCB_CPACR,
                REG_READ32(ARM_SCB_CPACR) | ARM_SCB_CPACR_CP10_MASK | ARM_SCB_CPACR_CP11_MASK);
#endif /* ENABLE_FPU */

#if defined(D_CACHE_ENABLE) || defined(I_CACHE_ENABLE)
    Startup_m7_cache_init();
#endif /*defined(D_CACHE_ENABLE) || defined(I_CACHE_ENABLE)*/

/*
 * Startup_Wfi_Config : Implement errata ERR051149
 */
#if defined(ERR_CORTEX_M7_E051149) || defined(ERR_CORTEX_M7_E051166)
    Startup_Wfi_Config();
#endif /* defined(ERR_CORTEX_M7_E051149) || defined(ERR_CORTEX_M7_E051166) */
}
