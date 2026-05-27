/**
 * @file      Bootloader_Specific.h
 * @version   24.12.0
 *
 * @brief     Bootloader platform-specific types and constants.
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

#ifndef BOOTLOADERSPECIFIC_H
#define BOOTLOADERSPECIFIC_H

#include "BasicTypes.h"

/* Available platform partitions */
typedef enum
{                    /*--- Software resettable domains ---*/
    PARTITION_0 = 0, /* Reset domain 0: Cortex-M7         */
    PARTITION_1 = 1, /* Reset domain 1: Cortex-A53        */
    PARTITION_2 = 2, /* Reset domain 2: PFE               */
    PARTITION_3 = 3  /* Reset domain 3: LLCE              */
} Bl_PlatformPartitionType;

/* Platform cores mapping to their core number. */
/* The values must match the MSCM_CPXNUM values. */
typedef enum Bl_PlatformCoreType
{
    A53_0 = 0,
    A53_1 = 1,
    A53_2 = 2,
    A53_3 = 3,
    M7_0 = 4,
    M7_1 = 5,
    M7_2 = 6,
    M7_3 = 7,
    A53_4 = 8,
    A53_5 = 9,
    A53_6 = 10,
    A53_7 = 11,
} Bl_PlatformCoreType;

/**
 * @brief Platform core information, including the core ID,
 * core name and partition ID.
 */
typedef struct
{
    Bl_PlatformCoreType core;           /* Target core */
    Bl_PlatformPartitionType partition; /* Target partition */
    uint8_t coreIdPartitionOffset;      /* Core ID inside the partition */
} Bl_PlatformCoreInfoType;

#include "Bootloader_Types.h"

/* Platform architecture types */
#define PLATFORM_ARM_AARCH32 (32)
#define PLATFORM_ARM_AARCH64 (64)

/* Number of transfers for loading an application image. */
#define BL_NO_OF_DATA_TRANSFERS 0x8

/* The RDC_RD1_STAT_REGISTER offset from the RDC base address. */
#define RDC_RD1_STAT_REGISTER_OFFSET (0x84)

/* A53 CLUSTER GPR registers. */
#define A53_CLUSTER_GPR_BASE_ADDR (0x4007C400UL)
#define A53_CLUSTER_GPR06 (A53_CLUSTER_GPR_BASE_ADDR + 0x18)
#define A53_CLUSTER_CA53_LOCKSTEP_ENABLE (0x00000001U)

/* MC_ME registers. */
#define MC_ME_BASE_ADDR (0x40088000UL)
#define MC_ME_CORE_CLOCK_STAT_MASK 0x1

#define MC_ME_CTL_KEY_ADDR (MC_ME_BASE_ADDR)
#define MC_ME_CTL_KEY_VALUE (0x00005AF0)
#define MC_ME_CTL_KEY_INVERTED_VALUE (0x0000A50F)
#define MC_ME_PARTITION_CLOCK_ENABLE (0x00000001U)
#define MC_ME_TRIGGER_PROCESS (0x00000001U)
#define MC_ME_OUTPUT_PARTITION (0xFFFFFFFB)
#define MC_ME_OUTPUT_STATUS (0x4)
#define MC_ME_CLOCK_ACTIVE (0x1)
#define MC_ME_MODE_STAT (MC_ME_BASE_ADDR + 0xC)
#define MC_ME_MODE_STAT_PREVMODE (0x00000001)

/* MC_ME partition registers. */
#define MC_ME_PRTN_N(n) (MC_ME_BASE_ADDR + 0x100 + (n) *0x200)
#define MC_ME_PRTN_N_PCONF(n) (MC_ME_PRTN_N(n))
#define MC_ME_PRTN_N_PUPD(n) (MC_ME_PRTN_N(n) + 0x4)
#define MC_ME_PRTN_N_STAT(n) (MC_ME_PRTN_N(n) + 0x8)
#define MC_ME_PRTN_N_CLK_STATUS(n) (MC_ME_PRTN_N(n) + 0x10)
#define MC_ME_PRTN_N_CLK_ENABLE(n) (MC_ME_PRTN_N(n) + 0x30)

/* MC_ME partition n core m definitions. */
#define MC_ME_PRTN_N_CORE_M(n, m) \
    (MC_ME_BASE_ADDR + 0x140 + (n) *0x200 + (m) *0x20)
#define MC_ME_PRTN_N_CORE_M_PCONF(n, m) (MC_ME_PRTN_N_CORE_M(n, m))
#define MC_ME_PRTN_N_CORE_M_PUPD(n, m) (MC_ME_PRTN_N_CORE_M(n, m) + 0x4)
#define MC_ME_PRTN_N_CORE_M_STAT(n, m) (MC_ME_PRTN_N_CORE_M(n, m) + 0x8)
#define MC_ME_PRTN_N_CORE_M_ADDR(n, m) (MC_ME_PRTN_N_CORE_M(n, m) + 0xC)

/* MC_RGM registers. */
#define MC_RGM_BASE_ADDR (0x40078000UL)

#define RGM_DES (MC_RGM_BASE_ADDR + 0x0)
#define RGM_DES_POR (0x00000001U)
#define RGM_DES_DESTRUCTIVE_RESET_MASK (0xC0073F5AU)

#define RGM_FES (MC_RGM_BASE_ADDR + 0x8)
#define RGM_FES_FUNCTIONAL_RESET_MASK (0xC0340058U)

#define RGM_PRST(per) (MC_RGM_BASE_ADDR + 0x40 + (per) *0x8)
#define RGM_PSTAT(per) (MC_RGM_BASE_ADDR + 0x140 + (per) *0x8)

#define RGM_PRST_CLUSTER (0xFFFFFFFE)
#define RGM_PSTAT_RESET_STATE (0x1)
#define RGM_PRST_RESET_CORE(n, m) (~(1 << (m + n)))
#define RGM_PRST_STATUS_CORE(n, m) ((1 << (m + n)))

/* RDC registers. */
#define RDC_BASE_ADDR (0x40080000UL)

#define RDC_RD1_CTRL_REGISTER (RDC_BASE_ADDR + 0x4)
#define RDC_RD1_STAT_REGISTER (RDC_BASE_ADDR + RDC_RD1_STAT_REGISTER_OFFSET)

#define RDC_RD1_CTRL_UNLOCK_ENABLE (0x80000000)
#define RDC_RD1_CTRL_UNLOCK_DISABLE (~(0x80000000))
#define RDC_RD1_XBAR_INTERFACE_DISABLE (~(0x8))
#define RDC_RD1_XBAR_INTERFACE_STAT (0x10U)
#define RDC_RD2_XBAR_INTERFACE_STAT (0x10U)

/* MSCM registers. */
#define MSCM_BASEADDRESS (0x40198000UL)
#define MSCM_CPXNUM_REG (MSCM_BASEADDRESS + 0x04UL)
#define MSCM_IRCP4ISR1 (MSCM_BASEADDRESS + 0x288UL)

extern const char *Bl_PlatformCoreNames[];

/**
 * @brief      Get current core identity.
 * @details    Convert processor logical number to core identity
 *             logical Bootloader identification.
 *             The processor numbering is platform dependent. For more details,
 *             check in the user manual the chapter related to CPXNUM register.
 *
 * @return     Bl_PlatformCoreType  Current core ID
 */
Bl_PlatformCoreType Platform_GetCoreIdentity(void);

/**
 * @brief      Go to a certain execution level mode.
 * @details    Increases exception level mode in order to change
 *             the protected registers.
 *             AARCH64 requires an elevated system call in order to branch
 *             to the user-supplied application entry-point using a handler.
 *             The EL2 exception handler has been updated with a
 *             user-defined handler.
 *
 * @param      executionLevel The targeted execution level
 */
void Platform_GoToElevatedExecutionLevel(execLvlMode executionLevel);

/**
 * @brief      Starts an application.
 * @details    Starts an application based on the input
 *             application configuration.
 *
 * @param      applicationConfig  The user application config
 *
 * @return     StatusType            E_OK or E_NOT_OK
 */
StatusType Bl_StartApplication(Bl_ApplicationDetails *applicationConfig);

/**
 * @brief      Run core-specific setup
 * @details    Implement core specific configuration before loading
 *             the application.
 * @param[in]  applicationConfig  The user application config
 * @return     None
 */
void Bl_PreSetup(Bl_ApplicationDetails *applicationConfig);

#endif /* BOOTLOADERSPECIFIC_H */
