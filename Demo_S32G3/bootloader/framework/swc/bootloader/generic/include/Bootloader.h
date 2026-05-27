/**
 *   @file      Bootloader.h
 *   @version   24.12.0
 *
 *   @brief     Bootloader application public API.
 *
 *   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
 *    All Rights Reserved.
 *
 *    This file contains sample code only. It is not part of the production code
 * deliverables.
 */

#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include "Std_Types.h"
#include "Bootloader_Specific.h"
#include "Bootloader_Types.h"

#include "Bootloader_Cfg.h"

/*==============================================================================
* GLOBAL SYMBOLIC CONSTANTS
==============================================================================*/

/* Define current platform architecture. */
#ifdef CORE_A53
#define PLATFORM_ARCHITECTURE PLATFORM_ARM_AARCH64
#else
#define PLATFORM_ARCHITECTURE PLATFORM_ARM_AARCH32
#endif /* CORE_M7 */

/* Platform-specific wait for interrupt fallback. */
#ifndef BL_WAIT_FOR_INTERRUPT
#define BL_WAIT_FOR_INTERRUPT() __asm("wfi")
#endif /* BL_WAIT_FOR_INTERRUPT */

/* Timeout counter initial value. */
#define TIMEOUT_CNT_INIT_VALUE 0xffff

/* Timeout counter end value. */
#define TIMEOUT_CNT_END_VALUE 0x0

/* Parameter used to indicate an enabled partition. */
#define PARTITION_ENABLED 1
#define PARTITION_DISABLED 0

/* Macro used to align the size of a DMA data chunk to a */
/* given byte (through y) size. */
#define BL_ALIGN_IMAGE_B(x, y)                         \
    ((x) % (1 << (y)) == 0 ? (ALIGN_BYTE_OP((x), (y))) \
                           : (ALIGN_BYTE_OP((x) + (0x1 << (y)), (y))))

/* Macro used to align the size of a data chunk to 8 bytes. */
#define BL_ALIGN_8B(x) BL_ALIGN_IMAGE_B(x, BL_DMA_SIZE_8B)

/* Macro used to align the size of a data chunk to 4 bytes. */
#define BL_ALIGN_4B(x) BL_ALIGN_IMAGE_B(x, BL_DMA_SIZE_4B)

/* Macro used to align the size of a data chunk to 64 bytes. */
#define BL_ALIGN_64B(x) BL_ALIGN_IMAGE_B(x, BL_DMA_SIZE_64B)

/* Macro used to align the size of a data chunk to 1024 bytes. */
#define BL_ALIGN_1024B(x) BL_ALIGN_IMAGE_B(x, 10)

/* Size of DMA data chunk, after splitting into 8 chunks the image. */
/* The image is divided by 8, the number of transfers. */
#define BL_GET_CHUNK_SIZE(dataChunk, chunkNumber)                        \
    ((chunkNumber) > 0x1 ? (((dataChunk) + (chunkNumber)) / chunkNumber) \
                         : (dataChunk))

/* Reset reason masks corresponding to all possible reset reasons. */
#define BL_POR_RESET_REASON_MASK 			((uint8) (1 << 4))
#define BL_DESTRUCTIVE_RESET_REASON_MASK	((uint8) (1 << 3))
#define BL_FUNCTIONAL_RESET_REASON_MASK 	((uint8) (1 << 2))
#define BL_STANDBY_RESET_REASON_MASK 		((uint8) (1 << 1))
#define BL_UNKNOWN_RESET_REASON_MASK 		((uint8) (1 << 0))
#define BL_RESET_REASON_ALLOW_ALL                                              \
    ((uint8) (BL_POR_RESET_REASON_MASK | BL_DESTRUCTIVE_RESET_REASON_MASK |    \
              BL_FUNCTIONAL_RESET_REASON_MASK | BL_STANDBY_RESET_REASON_MASK | \
              BL_UNKNOWN_RESET_REASON_MASK))
#define BL_RESET_REASON_DISALLOW_ALL ((uint8) 0x00U)

/*===============================================================================================*/
/*                                       GLOBAL VARIABLES                                        */
/*===============================================================================================*/
/* Global core information table. */
extern Bl_PlatformCoreInfoType Bl_PlatformCoresInformation[];

/*===============================================================================================*/
/*                                       GLOBAL FUNCTIONS                                        */
/*===============================================================================================*/
/**
 * @brief      Sets logical state of the partition given by user to the
 *             provided state value (enabled or disabled).
 * @details    When called, this function will flag a given partition
 *             as either enabled or disabled depending on the value of
 *             the state parameter.
 * 
 * @param      partition   Partition Id
 * @param      state       PARTITION_ENABLED or PARTITION_DISABLED
 */
void Bl_SetPartitionState(Bl_PlatformPartitionType partition, uint8 state);

/**
 * @brief      Return the partition state for the given partition index.
 * @details    This function returns PARTITION_ENABLED, if given partition
 *             was correctly provided and previously enabled, and
 *             PARTITION_DISABLED otherwise.
 * 
 * @param      uint8              Partition Id
 * 
 * @return     Bl_GetPartitionState PARTITION_ENABLED or PARTITION_DISABLED
 */
uint8 Bl_GetPartitionState(Bl_PlatformPartitionType partition);

/**
 * @brief      Gets the container partition for a target core.
 * @param      Core                     Target core ID
 * 
 * @return     Bl_PlatformPartitionType The target core partition
 */
Bl_PlatformPartitionType BL_GetPartition(Bl_PlatformCoreType Core);

#if (STD_ON == BL_BOOT_TRIGGER_ENABLED)
void Bl_ComDalNotification(void);
#endif /* BL_BOOT_TRIGGER_ENABLED == STD_ON */

/**
 * @brief      Enable peripheral from specific partition.
 * @details    This function enables a specific peripheral
 *             from a given partition
 * 
 * @param      partitionId  Target partition ID
 * @param      peripheralId Target peripheral ID in the current partition
 */
void Bl_EnablePartitionPeripheral(Bl_PlatformPartitionType partitionId,
                                  uint32 peripheralId);

#if (BL_CONFIGURE_DDR == STD_ON)
/**
 * @brief      Initialize DDR memory.
 * @details    This function prepares the DDR memory for future data
 *             copying, using the generated code from the DDR S32DS tool.
 *             This function shall be used only after the DDR clock
 *             has been configured.
 * 
 * @return     StatusType  E_OK or E_NOT_OK
 */
StatusType Bl_ConfigureDDR(void);
#endif /* (BL_CONFIGURE_DDR == STD_ON) */

/**
 * @brief      Loads all applications into memory and starts them.
 * @details    This function iterates through all applications and loads
 *             them into memory and starts them synchronously or asynchronously.
 * 
 * @return     StatusType  E_OK or E_NOT_OK
 */
StatusType Bl_BootApplications(void);

/**

 * @brief      Activate current partition, wait for trigger message
 *             and start the application boot flow.
 * 
 * @return     StatusType  E_OK or E_NOT_OK
 */
StatusType Bl_Run(void);

#endif /* BOOTLOADER_H */
