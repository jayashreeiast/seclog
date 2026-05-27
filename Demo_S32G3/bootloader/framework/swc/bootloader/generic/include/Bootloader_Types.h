/**
 *   @file      Bootloader_Types.h
 *   @version   24.12.0
 *
 *   @brief   Bootloader generic types header.
 *
 *   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
 *    All Rights Reserved.
 *
 *    This file contains sample code only. It is not part of the production code
 * deliverables.
 */

#ifndef BOOTLOADER_TYPES_H
#define BOOTLOADER_TYPES_H

#include "Bootloader_Cfg.h"
#include "Std_Types.h"

#if (BL_SUPPORT_AUTHENTICATION == STD_ON)
#include "Csm_Types.h"
#endif /* BL_SUPPORT_AUTHENTICATION == STD_ON */

/**
 * @brief Signature algorithm used for authenticating an application image.
 */
typedef enum
{
    NONE = 0,   /* Image is not signed */
    SHA1 = 1,   /* Image is signed using secure hash algorithm 1 */
    SHA256 = 2, /* Image is signed using secure hash algorithm 256 */
    CRC32 = 3   /* Image is signed using secure CRC32 algorithm */
} Bl_SignatureAlgorithm;

/**
 * @brief Image storage medium.
 */
typedef enum
{
    BS_UNKNOWN = 0, /* Image storage is unknown */
    BS_QSPI = 1,    /* Image is stored in the QSPI memory */
    BS_SDMMC = 2,   /* Image is stored on a SD/eMMC card */
    BS_COM = 3      /* Image is retrieved externally via ComManager */
} Bl_BootSource;

/**
 * @brief Possible DMA transfer sizes for one operation.
 */
typedef enum
{
    BL_DMA_SIZE_4B = 2U,  /* 4 bytes*/
    BL_DMA_SIZE_8B = 3U,  /* 8 bytes*/
    BL_DMA_SIZE_16B = 4U, /* 16 bytes*/
    BL_DMA_SIZE_32B = 5U, /* 32 bytes*/
    BL_DMA_SIZE_64B = 6U  /* 64 bytes*/
} Bl_DmaTransferSize;

/**
 * @brief Boot image fragment details.
 *        This is used for loading one image into SRAM, which
 *        can be part of a multi-fragment binary.
 */
typedef struct
{
    /* The flash address at which the application is stored. */
    uint32 u32FlashAddress;
    /* The ID corresponding to the session which will
      handle the image transfer. */
    uint32 u32ComSessionId;
    /* The destination RAM address of the application. */
    uint32 u32DestinationAddress;
    /* Size of application image. */
    uint32 u32Size;
    /* CRC value computed for application image. */
    uint32 u32CrcValue;
    /* MemDal block ID containing application image. */
    uint16 u16MemBlockId;
    /* The index in the HSE SMR table */
    uint8 u8SMRIndex;
} Bl_BootImageFragmentType;

/**
 * @brief User application details used by the Bootloader
 *        to start a core after loading all the fragments.
 *        These include the image fragments collection and
 *        the number of fragments to be loaded.
 */
typedef struct
{
    /* Identifies the core uniquely on a platform. */
    Bl_PlatformCoreType core;
    /* Image storage. */
    Bl_BootSource bootSource;
    /* Encryption key index - each image fragment must */
    /* be encrypted with the same algorithm and key. */
    uint8 u8CryptoIndex;
    /* The address of the application reset handler. */
    uint32 u32ResetHandler;
    /* Signature algorithm. */
    Bl_SignatureAlgorithm signAlgorithm;
    /* Number of image fragments to be loaded. */
    uint8 u8ImagesNumber;
#if (BL_ENABLE_DYNAMIC_BOOT == STD_OFF)
    /* The fragments the boot image is composed from. */
    Bl_BootImageFragmentType *pImageFragments;
#endif /* BL_ENABLE_DYNAMIC_BOOT == STD_OFF */
    /* Indicates whether the boot flow relies on successful image load. */
    boolean bIsCriticalApp;
    /* Flag to mark if bootloader should start the core. */
    boolean bStartCore;
} Bl_ApplicationDetails;

#if (BL_ENABLE_DYNAMIC_BOOT == STD_ON)
/**
 * @brief Dynamic boot configuration used by the Bootloader
 *        application to load images and start the cores.
 */
typedef struct
{
    /* Number of applications */
    uint8 u8AppsNumber;
    /* List of application configurations */
    Bl_ApplicationDetails aApps[BL_DYNAMIC_APP_SLOTS];
    /* List of application images */
    Bl_BootImageFragmentType aImages[BL_DYNAMIC_APP_SLOTS][BL_DYNAMIC_FRAGMENT_SLOTS];
} Bl_DynamicBootConfig;
#endif /* BL_ENABLE_DYNAMIC_BOOT == STD_ON */

#if (BL_SUPPORT_AUTHENTICATION == STD_ON)
/**
 * @brief Transfer descriptor used for authentication operations
 *        performed over a user image fragment chunk.
 */
typedef struct
{
    /* Source address of chunk transferred by DMA */
    uint32 sourceAddress;
    /* Destination address of the chunk transferred by DMA */
    uint32 destinationAddress;
    /* Size of chunk loaded via DMA engine */
    uint32 blockSize;
    /* Chunk size authenticated after being fetched, */
    /* using one of the supported algorithms (SHA1, SHA256). */
    uint32 authBlockSize;
    /* Algorithm mode, deciding which state of the execution is used. */
    Crypto_OperationModeType algorithmMode;
} Bl_TransferDescriptorType;
#endif /* (BL_SUPPORT_AUTHENTICATION == STD_ON) */

/**
 * @brief The mapping for the user-flashed IVT blob.
 *        This structures holds the members described in the
 *        reference manual Boot chapter.
 *        These can be used to determine, for instance, whether the secure
 *        bit is on or off.
 */
typedef struct
{
    uint32 u32IVT_header;       /**< @brief IVT header marker */
    uint32 u32reserved1;        /**< @brief 4 bytes reserved */
    uint32 u32SelfTestAddr;     /**< @brief Pointer to Self test image */
    uint32 u32SelfTestBckAddr;  /**< @brief Pointer to Self test backup image */
    uint32 u32DCDAddr;          /**< @brief Pointer to DCD image */
    uint32 u32DCDBckAddr;       /**< @brief Pointer to DCD backup image */
    uint32 u32HSEAddr;          /**< @brief Pointer to HSE FW image*/
    uint32 u32HSEBckAddr;       /**< @brief Pointer to HSE FW backup image */
    uint32 u32AppAddr;          /**< @brief Pointer to Application bootloader */
    uint32 u32AppBckAddr;       /**< @brief Pointer to Application bootloader backup */
    uint32 u32BootCfgWord;      /**< @brief Boot Configuration Word */
    uint32 u32LCCfgWord;        /**< @brief Life-Cycle configuration word */
    uint32 u32DebugConfig;      /**< @brief DEBUG Config */
    uint32 u32SysImageAddr;     /**< @brief SYS-IMG Config */
    uint32 u32SysImageBckAddr;  /**< @brief DEBUG Config */
    uint32 u32ExtFlashType;     /**< @brief External flash type where SYS-IMG is flashed */
    uint32 u32FlashPageSize;    /**< @brief Erasable page size of external */
    uint32 u32AppExtFlashType;  /**< @brief External flash type where Application is flashed */
} Bl_ImageVectorTableType;

/**
 * @brief Application bootloader image details.
 *        These details are taken from the IVT application bootloader
 *        parameter.
 */
typedef struct
{
    uint32 u32Header;        /**< @brief Image header marker */
    uint32 u32RamAddr;       /**< @brief Destination RAM address */
    uint32 u32RamEntryPoint; /**< @brief Application reset handler */
    uint32 u32Size;          /**< @brief Application size */
} Bl_AppImageCfgType;

/**
 * @brief Processor execution level.
 */
typedef enum
{
    user_mode = 0U,
    supervisor_mode,
    elevated_mode
} execLvlMode;

typedef uint8 Bl_ResetReasonType;

#endif /* BOOTLOADER_TYPES_H */
