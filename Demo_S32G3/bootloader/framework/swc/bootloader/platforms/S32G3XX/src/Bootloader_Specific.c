/**
 * @file      Bootloader_Specific.c
 * @version
 * 24.12.0
 *
 * @brief     Bootloader platform-specific functionality
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

#include "PlatformTypes.h"
#include "Bootloader.h"
#include "Bootloader_Cfg.h"
#include "Bootloader_Specific.h"
#include "S32G399A_RESET.h"
#include "Power_Ip_MC_ME.h"

#ifdef ENABLE_RUNTIME_MEASUREMENTS
#include "SysDal_Rtm.h"
#include "SysDal_Rtm_Cfg.h"
#endif /* ENABLE_RUNTIME_MEASUREMENTS */

#if  (BL_ENABLE_FLEX_NOC == STD_ON)
#include "FlexNoC_Init.h"
#endif /* (BL_ENABLE_FLEX_NOC == STD_ON) */

#if (BL_CONFIGURE_DDR == STD_ON)
#include "ddr_init.h"
#endif /* BL_CONFIGURE_DDR == STD_ON */

#include "Console.h"

/*==============================================================================
* LOCAL SYMBOLIC CONSTANTS
==============================================================================*/

#if (BL_CONFIGURE_DDR == STD_ON)
/* DDR peripheral slot in partition 0 */
#define DDR_PARTITION_SLOT 0x1
#endif /* BL_CONFIGURE_DDR == STD_ON */

/* FlexNoc related masks */
#define RD_INTERCONNECT_STAT_MASK 0x10U
#define RD_INTERCONNECT_DISABLE_MASK 0x8U
#define RD_CTRL_REG_UNLOCK_MASK 0x80000000U

/*===================================================================================================*/
/*                                       GLOBAL CONSTANTS                                            */
/*===================================================================================================*/

/* Configuration for enabling the platform partitions */
const Power_Ip_MC_ME_PartitionConfigType partitionConfig[4] = {
    {TRUE, TRUE, 0U, 1U, 1U, 0U, NULL_PTR, 0U, NULL_PTR },
    {TRUE, TRUE, 1U, 1U, 1U, 0U, NULL_PTR, 0U, NULL_PTR },
    {TRUE, TRUE, 2U, 1U, 1U, 0U, NULL_PTR, 0U, NULL_PTR },
    {TRUE, TRUE, 3U, 1U, 1U, 0U, NULL_PTR, 0U, NULL_PTR }
};

const Power_Ip_MC_ME_ModeConfigType modeConfig = {
    &partitionConfig
};

/*==============================================================================
* EXTERNAL DECLARATIONS
==============================================================================*/

/* Address of the application that is going to run on the same core */
/* as the bootloader */
extern volatile uint32 currentCoreBootImageAddress;

/*==============================================================================
* GLOBAL VARIABLES
==============================================================================*/

/* Platform cores table, containing information about each core on */
/* the current platform. */
/* This array follows the same ordering as the MSCM_CPXNUM register, */
/* in terms of the logical indexes (i.e. the logical processor number */
/* matches the array index corresponding to a certain core). */
Bl_PlatformCoreInfoType Bl_PlatformCoresInformation[] = {
    {A53_0, PARTITION_1, 0}, {A53_1, PARTITION_1, 1}, {A53_2, PARTITION_1, 2},
    {A53_3, PARTITION_1, 3}, {M7_0, PARTITION_0, 0},  {M7_1, PARTITION_0, 1},
    {M7_2, PARTITION_0, 2},  {M7_3, PARTITION_0, 3},  {A53_4, PARTITION_1, 4},
    {A53_5, PARTITION_1, 5}, {A53_6, PARTITION_1, 6}, {A53_7, PARTITION_1, 7},
};

/* The names of the cores. */
/* The array order must match the cores indexing from Bl_PlatformCoreType. */
const char *Bl_PlatformCoreNames[] = {
    "A53_0", "A53_1", "A53_2", "A53_3", "M7_0",  "M7_1",
    "M7_2",  "M7_3",  "A53_4", "A53_5", "A53_6", "A53_7",
};

/*==============================================================================
* LOCAL FUNCTION PROTOTYPES
==============================================================================*/

static StatusType Bl_EnablePartition(Bl_PlatformPartitionType);
static StatusType Bl_EnableRdInterconnectInterface(Bl_PlatformPartitionType);
static uint8 Bl_GetCoreInternalId(Bl_PlatformCoreType);

/*==============================================================================
* FUNCTION DEFINITIONS
==============================================================================*/

/**
 * @brief      Get current core identity.
 * @details    Convert processor logical number to core identity
 *             logical Bootloader identification.
 *             The processor numbering is platform dependent. For more details,
 *             check in the user manual the chapter related to CPXNUM register.
 * @return     Bl_PlatformCoreType  Current core ID
 */
Bl_PlatformCoreType Platform_GetCoreIdentity(void)
{
    return Bl_PlatformCoresInformation[REG_READ32(MSCM_CPXNUM_REG)].core;
}

/**
 * @brief      Handler to set the program counter to the start address
 *             of the user application loaded using the Bootloader.
 * @details    The branching to the application entry-point is
 *             done depending on the current ARM architecture (AARCH32/AARCH64).
 *             Only AARCH32 is supported.
 */
void Platform_ElevatedExecHandler(void)
{
    ASM_KEYWORD("ldr r0, =currentCoreBootImageAddress");
    ASM_KEYWORD("ldr r1, [r0]");
    ASM_KEYWORD("mov pc, r1");
}

/**
 * @brief      Go to a certain execution level mode.
 * @details    Increases exception level mode in order to change
 *             the protected registers.
 *             AARCH64 requires an elevated system call in order to branch
 *             to the user-supplied application entry-point using a handler.
 *             The EL2 exception handler has been updated with a
 *             user-defined handler.
 * @param      executionLevel The targeted execution level
 */
void Platform_GoToElevatedExecutionLevel(execLvlMode executionLevel)
{
    switch (executionLevel)
    {
    case user_mode:
        /* Not supported yet */

    case supervisor_mode:
        /* Not supported yet */

    case elevated_mode:
    {
        /* Go directly to elevated handler. */
        Platform_ElevatedExecHandler();
    }
        break;

    default:
        break;
    }
}

/**
 * @brief      Enables a core partition.
 * @param      partition The targeted partition
 * @return     StatusType   E_OK or E_NOT_OK
 */
static StatusType Bl_EnablePartition(Bl_PlatformPartitionType partition)
{
    /* Keep track of the time needed to start the partition. */
    /* If it takes too much time, consider the partition disabled. */
    uint16 timeout = TIMEOUT_CNT_INIT_VALUE;

    /* Auxiliary variable used for R/W access to registers. */
    uint32 regValue = 0;
    StatusType status = E_NOT_OK;

    if (PARTITION_DISABLED == Bl_GetPartitionState(partition))
    {
#if (BL_ENABLE_A53_LOCKSTEP == STD_ON)
        /* Check for partition number to enable lockstep. */
        if (PARTITION_1 == partition)
        {
            REG_WRITE32(A53_CLUSTER_GPR06,
                        REG_READ32(A53_CLUSTER_GPR06) | A53_CLUSTER_CA53_LOCKSTEP_ENABLE));
        }
#endif /* (BL_ENABLE_A53_LOCKSTEP == STD_ON) */

        /* Enable clock partition. */
        Power_Ip_MC_ME_EnablePartitionClock(&modeConfig, partition);

        /* Unlock software reset domain control register. */
        Bl_EnableRdInterconnectInterface(partition);

        /* Cluster reset. */
        regValue = REG_READ32(RGM_PRST(partition)) & RGM_PRST_CLUSTER;
        REG_WRITE32(RGM_PRST(partition), regValue);

        regValue =
            REG_READ32(MC_ME_PRTN_N_PCONF(partition)) & MC_ME_OUTPUT_PARTITION;
        REG_WRITE32(MC_ME_PRTN_N_PCONF(partition), regValue);

        regValue =
            REG_READ32(MC_ME_PRTN_N_PUPD(partition)) | MC_ME_OUTPUT_STATUS;
        REG_WRITE32(MC_ME_PRTN_N_PUPD(partition), regValue);

        /* Write the valid key sequence. */
        REG_WRITE32(MC_ME_CTL_KEY_ADDR, MC_ME_CTL_KEY_VALUE);
        REG_WRITE32(MC_ME_CTL_KEY_ADDR, MC_ME_CTL_KEY_INVERTED_VALUE);

        /* Wait until cluster is not in reset. */
        while ((REG_READ32(RGM_PSTAT(partition)) & RGM_PSTAT_RESET_STATE) !=
               E_OK)
            continue;
        while (((REG_READ32(MC_ME_PRTN_N_STAT(partition)) &
                 MC_ME_OUTPUT_STATUS) != 0x0) &&
               (timeout--))
            continue;

        if (TIMEOUT_CNT_END_VALUE != timeout)
        {
            Bl_SetPartitionState(partition, PARTITION_ENABLED);
            status = E_OK;
        }
    }
    else
    {
        /* The partition is already enabled. */
        status = E_OK;
    }

    return status;
}

/**
 * @brief      Gets the core ID in the container partition.
 * @param      core     Target core
 * @return     uint8  The core index in the parent partition
 */
static uint8 Bl_GetCoreInternalId(Bl_PlatformCoreType core)
{
    return Bl_PlatformCoresInformation[core].coreIdPartitionOffset;
}

#if  (BL_ENABLE_FLEX_NOC == STD_ON)
/**
 * @brief      Enables the FlexNoC access
 * @details    This function will unlock the SRD registers, enables the interconnect interface
 *             wait for acknowledge and then lock back the registers

 * @param      partition: partition for which the interconnect interface is enabled
 *
 * @return     StatusType   E_OK or E_NOT_OK
 */
static StatusType Bl_EnableRdInterconnectInterface(Bl_PlatformPartitionType partition)
{
    StatusType status = E_OK;
    uint16 timeout =  TIMEOUT_CNT_INIT_VALUE;
    __IO uint32 *ctrlReg;
    __I uint32 *statReg;

    switch (partition)
    {
    case PARTITION_0:
        /* The RD0 is released from reset by default after POR, destructive, or functional reset*/
        /* Nothing to be done */
        return E_OK;

    case PARTITION_1:
        ctrlReg = &IP_RDC->RD1_CTRL_REG;
        statReg = &IP_RDC->RD1_STAT_REG;
        break;

    case PARTITION_2:
        ctrlReg = &IP_RDC->RD2_CTRL_REG;
        statReg = &IP_RDC->RD2_STAT_REG;
        break;

    case PARTITION_3:
        ctrlReg = &IP_RDC->RD3_CTRL_REG;
        statReg = &IP_RDC->RD3_STAT_REG;
        break;

    default:
        return E_NOT_OK;
    }

    if ((*statReg & RD_INTERCONNECT_STAT_MASK) != 0U) /* not already enabled */
    {
        /* Unlock software reset domain control register for writing */
        *ctrlReg |= RD_CTRL_REG_UNLOCK_MASK;

        /* Enable the interconnect interface of software reset domain */
        *ctrlReg &= ~RD_INTERCONNECT_DISABLE_MASK;

        /* Wait for software reset domain status register */
        /* to acknowledge interconnect interface enabled */
        while (((*statReg & RD_INTERCONNECT_STAT_MASK) != 0U) && (timeout--)) continue;

        if (TIMEOUT_CNT_END_VALUE != timeout) /* Reset domain init succes */
        {
            status = E_OK;
        } else
        {
            status = E_NOT_OK;
        }

        /* Relock the reset domain register */
        *ctrlReg &= ~RD_CTRL_REG_UNLOCK_MASK;
    }

    return status;
}
#endif /* (BL_ENABLE_FLEX_NOC == STD_ON) */

/**
 * @brief      Run core-specific setup
 * @details    Implement core specific configuration before loading
 *             the application.
 * @param[in]  applicationConfig  The user application config
 * @return     None
 */
void Bl_PreSetup(Bl_ApplicationDetails *applicationConfig)
{
    (void)applicationConfig;

#if  (BL_ENABLE_FLEX_NOC == STD_ON)
    StatusType status = E_NOT_OK;

    /* init the the flexNoc registers */
    if (A53_0 == applicationConfig->core)
    {
        status = Bl_EnablePartition(PARTITION_1);
    }

    if (M7_0 == applicationConfig->core)
    {
        status = Bl_EnableRdInterconnectInterface(PARTITION_2);
        status |= Bl_EnableRdInterconnectInterface(PARTITION_3);
    }

    if(status == E_OK)
    {
        FlexNOC_InitNoc(applicationConfig->core);
    }
#endif /* (BL_ENABLE_FLEX_NOC == STD_ON) */
}

/**
 * @brief      Starts an application.
 * @details    Starts an application based on the input
 *             application configuration.
 * @param      applicationConfig  The user application config
 * @return     StatusType            E_OK or E_NOT_OK
 */
StatusType Bl_StartApplication(Bl_ApplicationDetails *applicationConfig)
{
    uint8 coreId;
    uint32 resetCore;
    uint32 coreStartAddress;
    Bl_PlatformPartitionType partition;
    StatusType status = E_NOT_OK;
    uint16 timeout = TIMEOUT_CNT_INIT_VALUE;

    coreStartAddress = (applicationConfig->u32ResetHandler) & 0xFFFFFFFC;
    coreId = Bl_GetCoreInternalId(applicationConfig->core);
    partition = BL_GetPartition(applicationConfig->core);

    /* Set the core Vector Table address before enabling the partition. */
    REG_WRITE32(MC_ME_PRTN_N_CORE_M_ADDR(partition, coreId), coreStartAddress);

    if (E_OK == Bl_EnablePartition(partition))
    {
        /* Enable core clock. */
        REG_WRITE32(MC_ME_PRTN_N_CORE_M_PCONF(partition, coreId), 1);

        /* Partition peripherals are always enabled in partition 0. */
        if (PARTITION_0 != partition)
        {
            REG_WRITE32(MC_ME_PRTN_N_PCONF(partition), 1);
        }
        /* Trigger hardware process for clock update. */
        REG_WRITE32(MC_ME_PRTN_N_CORE_M_PUPD(partition, coreId), 1);

        /* Write key sequence. */
        REG_WRITE32(MC_ME_CTL_KEY_ADDR, MC_ME_CTL_KEY_VALUE);
        REG_WRITE32(MC_ME_CTL_KEY_ADDR, MC_ME_CTL_KEY_INVERTED_VALUE);

        /* Wait for clock to be enabled. */
        while (((REG_READ32(MC_ME_PRTN_N_CORE_M_STAT(partition, coreId)) &
                 MC_ME_CORE_CLOCK_STAT_MASK) != 1) &&
               (timeout--))
            continue;
        while (((REG_READ32(MC_ME_PRTN_N_STAT(partition)) &
                 MC_ME_CLOCK_ACTIVE) != 1) &&
               (timeout--))
            continue;

        /* Pull the core out of reset and wait for it. */
        resetCore = REG_READ32(RGM_PRST(partition)) &
                    RGM_PRST_RESET_CORE(partition, coreId);
        REG_WRITE32(RGM_PRST(partition), resetCore);
        while ((REG_READ32(RGM_PSTAT(partition)) &
                RGM_PRST_STATUS_CORE(partition, coreId)) != 0)
            continue;

        if (TIMEOUT_CNT_END_VALUE != timeout)
            status = E_OK;
    }

    return status;
}

/**
 * @brief      Enable peripheral from specific partition.
 * @details    This function enables a specific peripheral
 *             from a given partition
 * @param      partitionId  Target partition ID
 * @param      peripheralId Target peripheral ID in the current partition
 */
void Bl_EnablePartitionPeripheral(Bl_PlatformPartitionType partitionId, uint32 peripheralId)
{
    REG_WRITE32(MC_ME_PRTN_N(partitionId), 0x1);
    REG_WRITE32(MC_ME_PRTN_N_CLK_ENABLE(partitionId),
                (REG_READ32(MC_ME_PRTN_N_CLK_ENABLE(partitionId)) |
                 (1 << peripheralId)));
    REG_WRITE32(MC_ME_PRTN_N_PUPD(partitionId), 0x1);
    REG_WRITE32(MC_ME_CTL_KEY_ADDR, MC_ME_CTL_KEY_VALUE);
    REG_WRITE32(MC_ME_CTL_KEY_ADDR, MC_ME_CTL_KEY_INVERTED_VALUE);

    while ((1UL << peripheralId) !=
           (REG_READ32(MC_ME_PRTN_N_CLK_STATUS(partitionId)) &
            (1 << peripheralId)))
        continue;
}

#if (BL_CONFIGURE_DDR == STD_ON)
/**
 * @brief      Initialize DDR memory.
 * @details    This function prepares the DDR memory for future data
 *             copying, using the generated code from the DDR S32DS tool.
 *             This function shall be used only after the DDR clock
 *             has been configured.
 * @return     StatusType  E_OK or E_NOT_OK
 */
StatusType Bl_ConfigureDDR(void)
{
    Bl_EnablePartitionPeripheral(PARTITION_0, DDR_PARTITION_SLOT);

    return ddr_init();
}
#endif /* (BL_CONFIGURE_DDR == STD_ON) */
