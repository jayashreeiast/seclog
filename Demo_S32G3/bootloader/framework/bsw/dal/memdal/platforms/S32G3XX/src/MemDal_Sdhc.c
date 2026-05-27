/*===================================================================================================*/
/*
*   @file              MemDal_Sdhc.c
*   project            AUTOSAR 4.4 GOLDVIP
*   platform           CORTEXM
*
*   @brief             Integration Framework - MemDal Sdhc driver
*   @details           This file contains dedicated functions for SD card operations
*
*   Autosar Version    4.4.0
*   Build Version      S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*   All Rights Reserved.
*/
/*===================================================================================================*/

#ifdef __cplusplus
extern "C"{
#endif

/*===================================================================================================*/
/*                                         INCLUDE FILES
   1) system and project includes
   2) needed interfaces from external units
   3) internal and external interfaces from this unit
*/
/*===================================================================================================*/
#include "MemDal_Sdhc.h"

#if (MEMDAL_SDHC_ENABLED == STD_ON)

#if (MEMDAL_DEV_ERROR_DETECT == STD_ON)
    #include "Det.h"
#endif /* MEMDAL_DEV_ERROR_DETECT == STD_ON */

#include "usdhc_fatfs_impl.h"
#include "usdhc_impl.h"
#include "sd/card.h"
#include "sys_init.h"

#include "SysDal_IntSch.h"

/*===================================================================================================*/
/*                              SOURCE FILE VERSION INFORMATION                                      */
/*===================================================================================================*/
#define MEMDAL_SDHC_VENDOR_ID_C                             43

#define MEMDAL_SDHC_AR_RELEASE_MAJOR_VERSION_C              4
#define MEMDAL_SDHC_AR_RELEASE_MINOR_VERSION_C              4
#define MEMDAL_SDHC_AR_RELEASE_REVISION_VERSION_C           0

#define MEMDAL_SDHC_SW_MAJOR_VERSION_C                      24
#define MEMDAL_SDHC_SW_MINOR_VERSION_C                      12
#define MEMDAL_SDHC_SW_PATCH_VERSION_C                      0

/*===================================================================================================*/
/*                                      FILE VERSION CHECKS                                          */
/*===================================================================================================*/

/* Check if MemDal Sdhc driver source file and header file are of the same vendor */
#if (MEMDAL_SDHC_VENDOR_ID_C != MEMDAL_SDHC_VENDOR_ID)
    #error "MemDal_Sdhc.c and MemDal_Sdhc.h have different vendor ids"
#endif

/* Check if MemDal Sdhc driver source file and header file are of the same Autosar version */
#if ((MEMDAL_SDHC_AR_RELEASE_MAJOR_VERSION_C != MEMDAL_SDHC_AR_RELEASE_MAJOR_VERSION) || \
     (MEMDAL_SDHC_AR_RELEASE_MINOR_VERSION_C != MEMDAL_SDHC_AR_RELEASE_MINOR_VERSION) || \
     (MEMDAL_SDHC_AR_RELEASE_REVISION_VERSION_C != MEMDAL_SDHC_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of MemDal_Sdhc.c and MemDal_Sdhc.h are different"
#endif

/* Check if MemDal Sdhc driver source file and header file are of the same Software version */
#if ((MEMDAL_SDHC_SW_MAJOR_VERSION_C != MEMDAL_SDHC_SW_MAJOR_VERSION) || \
     (MEMDAL_SDHC_SW_MINOR_VERSION_C != MEMDAL_SDHC_SW_MINOR_VERSION) || \
     (MEMDAL_SDHC_SW_PATCH_VERSION_C != MEMDAL_SDHC_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of MemDal_Sdhc.c and MemDal_Sdhc.h are different"
#endif

/* Check if MemDal Sdhc driver source file and configuration header file are of the same vendor */
#if (MEMDAL_SDHC_VENDOR_ID_C != MEMDAL_CFG_VENDOR_ID)
    #error "MemDal_Sdhc.c and MemDal_Cfg.h have different vendor ids"
#endif

/* Check if MemDal Sdhc driver source file and configuration header file are of the same Autosar version */
#if ((MEMDAL_SDHC_AR_RELEASE_MAJOR_VERSION_C != MEMDAL_CFG_AR_RELEASE_MAJOR_VERSION) || \
     (MEMDAL_SDHC_AR_RELEASE_MINOR_VERSION_C != MEMDAL_CFG_AR_RELEASE_MINOR_VERSION) || \
     (MEMDAL_SDHC_AR_RELEASE_REVISION_VERSION_C != MEMDAL_CFG_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of MemDal_Sdhc.c and MemDal_Cfg.h are different"
#endif

/* Check if MemDal Sdhc driver source file and configuration header file are of the same software version */
#if ((MEMDAL_SDHC_SW_MAJOR_VERSION_C != MEMDAL_CFG_SW_MAJOR_VERSION) || \
     (MEMDAL_SDHC_SW_MINOR_VERSION_C != MEMDAL_CFG_SW_MINOR_VERSION) || \
     (MEMDAL_SDHC_SW_PATCH_VERSION_C != MEMDAL_CFG_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of MemDal_Sdhc.c and MemDal_Cfg.h are different"
#endif

/*===================================================================================================*/
/*                               LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)                          */
/*===================================================================================================*/

/*===================================================================================================*/
/*                                       DEFINES AND MACROS                                          */
/*===================================================================================================*/

/* uSDHC nvic interrupt id */
#define MEMDAL_SDHC_INTERRUPT_ID                36U
/* PIT nvic interrupt id, used for OSIF timeout transfer in SDHC stack */
#define MEMDAL_PIT_0_INTERRUPT_ID               53U

/* Main drive of the card file system */
#define MEMDAL_SDHC_CARD_DRIVE                  "0:"

/* ISR handler for PIT timer */
extern ISR(PIT_0_ISR);

/*===================================================================================================*/
/*                                             LOCAL CONSTANTS                                       */
/*===================================================================================================*/

/*===================================================================================================*/
/*                                             LOCAL VARIABLES                                       */
/*===================================================================================================*/

/*===================================================================================================*/
/*                                            GLOBAL CONSTANTS                                       */
/*===================================================================================================*/

/*===================================================================================================*/
/*                                            GLOBAL VARIABLES                                       */
/*===================================================================================================*/

/**
* @brief The SDHC status
*/
static VAR(MemDal_ModuleState, AUTOMATIC) MemDal_eSdhcStatus = MEMDAL_UNINITIALIZED;

/**
* @brief Pointer to global MemDal configuration structure
*/
static P2CONST(MemDal_ConfigType, AUTOMATIC, MEMDAL_APPL_DATA) MemDal_Sdhc_pConfig = NULL_PTR;

/**
* @brief Result of last SDHC memory operation
*/
VAR(MemDal_BlockStatusType, AUTOMATIC) MemDal_Sdhc_eJobResult;

/**
* @brief The SDHC file system object
*/
static VAR(FATFS, AUTOMATIC) MemDal_Sdhc_FatfsObj;

/*===================================================================================================*/
/*                                    LOCAL FUNCTION PROTOTYPES                                      */
/*===================================================================================================*/

/*===================================================================================================*/
/*                                      LOCAL FUNCTIONS                                              */
/*===================================================================================================*/

static FUNC(Std_ReturnType, AUTOMATIC) MemDal_Sdhc_InitFatfs(void);
static FUNC(Std_ReturnType, AUTOMATIC) MemDal_Sdhc_FormatCard(void);
#if (STD_ON == MEMDAL_SDHC_FATFS_ENABLED)
static FUNC(Std_ReturnType, AUTOMATIC) MemDal_Sdhc_ReadFile(P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock);
static FUNC(Std_ReturnType, AUTOMATIC) MemDal_Sdhc_WriteFile(P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock);
static FUNC(Std_ReturnType, AUTOMATIC) MemDal_Sdhc_EraseFile(P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock);
#else /* (STD_ON == MEMDAL_SDHC_FATFS_ENABLED) */
static FUNC(Std_ReturnType, AUTOMATIC) MemDal_Sdhc_ReadBlock(P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock);
static FUNC(Std_ReturnType, AUTOMATIC) MemDal_Sdhc_WriteBlock(P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock);
static FUNC(Std_ReturnType, AUTOMATIC) MemDal_Sdhc_EraseBlock(P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock);
#endif /* (STD_ON == MEMDAL_SDHC_FATFS_ENABLED) */

/*===================================================================================================*/
/**
* @brief      MemDal SDHC module initialization
* @details
* @param[in]  pConfig - Pointer to MemDal module configuration structure
*
* @return     E_OK -  initialization successful
*             E_NOT_OK -  initialization failed
*/
/*===================================================================================================*/
FUNC(Std_ReturnType, AUTOMATIC) MemDal_Sdhc_Init(P2CONST( MemDal_ConfigType, AUTOMATIC, MEMDAL_APPL_CONST ) pConfig)
{
    VAR(Std_ReturnType, AUTOMATIC) status = E_NOT_OK;

    if (MEMDAL_INITIALIZED != MemDal_eSdhcStatus)
    {
        MemDal_Sdhc_pConfig = pConfig;

        /* MemDal will self activate SDHC and PIT interrupts because they are needed for driver initialization
        and SysDal interrupts activation occurs later
        */
        sys_disableAllInterrupts();
        sys_registerIsrHandler(MEMDAL_SDHC_INTERRUPT_ID, &uSDHC0_Isr);
        sys_enableIsrSource(MEMDAL_SDHC_INTERRUPT_ID, 0x1);
        sys_registerIsrHandler(MEMDAL_PIT_0_INTERRUPT_ID, &PIT_0_ISR);
        sys_enableIsrSource(MEMDAL_PIT_0_INTERRUPT_ID, 0x2);
        sys_enableAllInterrupts();

        /* Start timer */
        SysDal_IntSch_StartScheduler();

        /* Initialize Os Interface */
        OsIf_Init(NULL_PTR);

        if (STD_ON == MEMDAL_SDHC_FATFS_ENABLED)
        {
            status = MemDal_Sdhc_InitFatfs();
        }
        else
        {
            status = uSDHC_Init(true);
        }

        if (E_OK == status)
        {
            MemDal_eSdhcStatus = MEMDAL_INITIALIZED;
        }
    }
    else
    {
#if (MEMDAL_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(MEMDAL_SDHC_MODULE_ID, 0U, MEMDAL_SDHC_INIT_API_ID, MEMDAL_E_ALREADY_INIT);
#endif /* MEMDAL_DEV_ERROR_DETECT == STD_ON */
    }

    return status;
}

/*===================================================================================================*/
/**
* @brief      Initialize SDHc driver in fatfs mode
* @details
*
* @param[in]  None
*
* @return     E_OK - initialization successful
*             E_NOT_OK - initialization failed
*/
/*===================================================================================================*/
static FUNC(Std_ReturnType, AUTOMATIC) MemDal_Sdhc_InitFatfs(void)
{
    VAR(Std_ReturnType, AUTOMATIC) status = E_NOT_OK;
    FRESULT fstatus = FR_DISK_ERR;

    if (0U == uSDHC_Init(true))
    {
        uSDHC_DeInit();
    }

    uSDHC_fatfs_initialize();

    /* Try to mount root at root directory */
    fstatus = f_mount(&MemDal_Sdhc_FatfsObj, "/", 1);

    if ((FR_NO_FILESYSTEM == fstatus) || (TRUE == MEMDAL_SDHC_FORMATCARD))
    {
        status = MemDal_Sdhc_FormatCard();
    }

    fstatus = f_unmount("/");

    if (FR_OK == fstatus)
    {
        status = E_OK;
    }

    return status;
}

/*===================================================================================================*/
/**
* @brief      Format SD/EMMC cards
* @details    This function will creates partitions on the physical drive and will create FAT/exFAT
*             volumes on these partitions
*
* @param[in]  None
*
* @return     E_OK - format successful
*             E_NOT_OK - format failed
*/
/*===================================================================================================*/
static FUNC(Std_ReturnType, AUTOMATIC) MemDal_Sdhc_FormatCard(void)
{
    VAR(Std_ReturnType, AUTOMATIC) status = E_NOT_OK;
    FRESULT fstatus = FR_DISK_ERR;
    MKFS_PARM format_opt;
    DWORD partitionList[] = {100, 0};   /* one partition from entire space */
    UINT au = 128U;                     /* allocation unit */
    BYTE workBuff[FF_MAX_SS];           /* f_fdisk work area */

    /* Creates partitions on the physical drive */
    fstatus = f_fdisk(0U, partitionList, workBuff);

    /* Creates an FAT volume on the logical drive. */
    do
    {
        format_opt.fmt = FM_FAT32 | FM_FAT;
        format_opt.au_size = FF_MAX_SS * au;

        fstatus = f_mkfs(MEMDAL_SDHC_CARD_DRIVE, &format_opt, workBuff, sizeof(workBuff));
        if (FR_MKFS_ABORTED == fstatus)
        {
            if (au > 1U)
            {
                /* Allocation unit is power of 2; divide by 2 to get next value. */
                au >>= 1U;
            }
            else
            {
                break;
            }
        }
    } while (FR_MKFS_ABORTED == fstatus);

    /* Save fatfs status */
    if (FR_OK == fstatus)
    {
        status = E_OK;
    }

    return status;
}

/*===================================================================================================*/
/**
* @brief      MemDal driver deinitialization function
* @details
*
* @param[in]  None
*
* @return     None
*/
/*===================================================================================================*/
FUNC(void, AUTOMATIC) MemDal_Sdhc_Deinit(void)
{
    if (MEMDAL_INITIALIZED == MemDal_eSdhcStatus)
    {
        uSDHC_DeInit();

        /* Stop timer */
        SysDal_IntSch_StopScheduler();

        sys_disableAllInterrupts();
        sys_registerIsrHandler(MEMDAL_SDHC_INTERRUPT_ID, NULL_PTR);
        sys_disableIsrSource(MEMDAL_SDHC_INTERRUPT_ID);
        sys_registerIsrHandler(MEMDAL_PIT_0_INTERRUPT_ID, NULL_PTR);
        sys_disableIsrSource(MEMDAL_PIT_0_INTERRUPT_ID);
        sys_enableAllInterrupts();

        MemDal_eSdhcStatus = MEMDAL_DEINITIALIZED;
    }
    else
    {
#if (MEMDAL_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(MEMDAL_SDHC_MODULE_ID, 0U, MEMDAL_SDHC_DEINIT_API_ID, MEMDAL_E_UNINIT);
#endif /* MEMDAL_DEV_ERROR_DETECT == STD_ON */
    }
}

/*===================================================================================================*/
/**
* @brief      MemDal Sdhc read function
* @details    Starts an asynchronously job to read from SD card file
*
* @param[in]  pBlock - block descriptor
*
* @return     E_OK - Read job start successfully
*             E_NOT_OK - Read job failed
*/
/*===================================================================================================*/
FUNC(Std_ReturnType, AUTOMATIC) MemDal_Sdhc_Read(P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock)
{
    VAR(Std_ReturnType, AUTOMATIC) status = E_NOT_OK;

    if (MEMDAL_INITIALIZED == MemDal_eSdhcStatus)
    {
#if (STD_ON == MEMDAL_SDHC_FATFS_ENABLED)
        status = MemDal_Sdhc_ReadFile(pBlock);
#else /* (STD_ON == MEMDAL_SDHC_FATFS_ENABLED) */
        status = MemDal_Sdhc_ReadBlock(pBlock);
#endif /* (STD_ON == MEMDAL_SDHC_FATFS_ENABLED) */
    }
    else
    {
#if (MEMDAL_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(MEMDAL_SDHC_MODULE_ID, 0U, MEMDAL_SDHC_READ_API_ID, MEMDAL_E_UNINIT);
#endif /* MEMDAL_DEV_ERROR_DETECT == STD_ON */
    }

    return status;
}

/*===================================================================================================*/
/**
* @brief      MemDal Sdhc write function
* @details    Starts an asynchronously job to write to SD card file
*
* @param[in]  pBlock - block descriptor
*
* @return     E_OK - Write job start successfully
*             E_NOT_OK - Write job failed
*/
/*===================================================================================================*/
FUNC(Std_ReturnType, AUTOMATIC) MemDal_Sdhc_Write(P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock)
{
    VAR(Std_ReturnType, AUTOMATIC) status = E_NOT_OK;

    if (MEMDAL_INITIALIZED == MemDal_eSdhcStatus)
    {
#if (STD_ON == MEMDAL_SDHC_FATFS_ENABLED)
        status = MemDal_Sdhc_WriteFile(pBlock);
#else /* (STD_ON == MEMDAL_SDHC_FATFS_ENABLED) */
        status = MemDal_Sdhc_WriteBlock(pBlock);
#endif /* (STD_ON == MEMDAL_SDHC_FATFS_ENABLED) */
    }
    else
    {
#if (MEMDAL_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(MEMDAL_SDHC_MODULE_ID, 0U, MEMDAL_SDHC_WRITE_API_ID, MEMDAL_E_UNINIT);
#endif /* MEMDAL_DEV_ERROR_DETECT == STD_ON */
    }

    return status;
}

/*===================================================================================================*/
/**
* @brief      MemDal Sdhc erase function
* @details    Starts an asynchronously job to erase SD card file
*
* @param[in]  pBlock - block descriptor
*
* @return     E_OK - Erase job start successfully
*             E_NOT_OK - Erase job failed
*/
/*===================================================================================================*/
FUNC(Std_ReturnType, AUTOMATIC) MemDal_Sdhc_Erase(P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock)
{
    VAR(Std_ReturnType, AUTOMATIC) status = E_NOT_OK;

    if (MEMDAL_INITIALIZED == MemDal_eSdhcStatus)
    {
#if (STD_ON == MEMDAL_SDHC_FATFS_ENABLED)
        status = MemDal_Sdhc_EraseFile(pBlock);
#else /* (STD_ON == MEMDAL_SDHC_FATFS_ENABLED) */
        status = MemDal_Sdhc_EraseBlock(pBlock);
#endif /* (STD_ON == MEMDAL_SDHC_FATFS_ENABLED) */
    }
    else
    {
#if (MEMDAL_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(MEMDAL_SDHC_MODULE_ID, 0U, MEMDAL_SDHC_ERASE_API_ID, MEMDAL_E_UNINIT);
#endif /* MEMDAL_DEV_ERROR_DETECT == STD_ON */
    }

    return status;
}

#if (STD_ON == MEMDAL_SDHC_FATFS_ENABLED)
/*===================================================================================================*/
/**
* @brief      MemDal Sdhc read file function
* @details    Read from memory card in file system mode
*
* @param[in]  pBlock - block descriptor
*
* @return     E_OK - Read successfully
*             E_NOT_OK - Read failed
*/
/*===================================================================================================*/
static FUNC(Std_ReturnType, AUTOMATIC) MemDal_Sdhc_ReadFile(P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock)
{
    P2VAR(MemDal_Sdhc_FileDescriptorType, AUTOMATIC, MEMDAL_APPL_DATA) pFile = NULL_PTR;
    P2VAR(char, AUTOMATIC, MEMDAL_APPL_DATA) pBuffer = NULL_PTR;
    VAR(Std_ReturnType, AUTOMATIC) status = E_NOT_OK;
    VAR(uint8, AUTOMATIC) u8AccessOptions;
    VAR(FRESULT, AUTOMATIC) result;
    VAR(UINT, AUTOMATIC) bytesRead = 0U;
    VAR(UINT, AUTOMATIC) bytesToRead = 0U;
    VAR(FIL, AUTOMATIC) filePtr;

    pFile = &MemDal_Sdhc_pConfig->pSdhcFiles[pBlock->u16DrvBlockId];
    pBuffer = (char*)pBlock->u32RamBuffer;
    bytesToRead = pBlock->u32BlockSize;

    MemDal_Sdhc_eJobResult = MEMDAL_BLOCK_STATE_ERROR;

    /* Immediate mount */
    result = f_mount(&MemDal_Sdhc_FatfsObj, MEMDAL_SDHC_CARD_DRIVE, 1);

    if (FR_OK == result)
    {
        u8AccessOptions = FA_READ;
        u8AccessOptions |= (pFile->u8Options & FA_OPEN_ALWAYS);

        result = f_open(&filePtr, pFile->pFilePath, u8AccessOptions);

        if (FR_OK == result)
        {
            result = f_read(&filePtr, pBuffer, bytesToRead, &bytesRead);

            if (FR_OK == result)
            {
                f_close(&filePtr);
                status = E_OK;
            }
        }

        /* Un-mount drive */
        f_unmount(MEMDAL_SDHC_CARD_DRIVE);
    }

    if (E_OK == status)
    {
        MemDal_Sdhc_eJobResult = MEMDAL_BLOCK_STATE_READY;
    }

    return status;
}

/*===================================================================================================*/
/**
* @brief      MemDal Sdhc write file function
* @details    Write to memory card in file system mode
*
* @param[in]  pBlock - block descriptor
*
* @return     E_OK - Write successfully
*             E_NOT_OK - Write failed
*/
/*===================================================================================================*/
static FUNC(Std_ReturnType, AUTOMATIC) MemDal_Sdhc_WriteFile(P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock)
{
    P2VAR(MemDal_Sdhc_FileDescriptorType, AUTOMATIC, MEMDAL_APPL_DATA) pFile = NULL_PTR;
    P2VAR(char, AUTOMATIC, MEMDAL_APPL_DATA) pBuffer = NULL_PTR;
    VAR(Std_ReturnType, AUTOMATIC) status = E_NOT_OK;
    VAR(uint8, AUTOMATIC) u8AccessOptions;
    VAR(FRESULT, AUTOMATIC) result;
    VAR(UINT, AUTOMATIC) bytesWritten = 0U;
    VAR(UINT, AUTOMATIC) bytesToWrite = 0U;
    VAR(FIL, AUTOMATIC) filePtr;

    pFile = &MemDal_Sdhc_pConfig->pSdhcFiles[pBlock->u16DrvBlockId];
    pBuffer = (char*)pBlock->u32RamBuffer;
    bytesToWrite = pBlock->u32BlockSize;

    MemDal_Sdhc_eJobResult = MEMDAL_BLOCK_STATE_ERROR;

    /* Immediate mount */
    result = f_mount(&MemDal_Sdhc_FatfsObj, MEMDAL_SDHC_CARD_DRIVE, 1);

    if (FR_OK == result)
    {
        u8AccessOptions = FA_WRITE;
        u8AccessOptions |= (pFile->u8Options & FA_OPEN_ALWAYS);

        result = f_open(&filePtr, pFile->pFilePath, u8AccessOptions);

        if (FR_OK == result)
        {
            result = f_write(&filePtr, pBuffer, bytesToWrite, &bytesWritten);

            if (FR_OK == result)
            {
                f_close(&filePtr);
                status = E_OK;
            }
        }

        /* Un-mount drive */
        f_unmount(MEMDAL_SDHC_CARD_DRIVE);
    }

    if (E_OK == status)
    {
        MemDal_Sdhc_eJobResult = MEMDAL_BLOCK_STATE_READY;
    }

    return status;
}

/*===================================================================================================*/
/**
* @brief      MemDal Sdhc erase file function
* @details    Delete the file content on the memory card in file system mode.
*             The file will still exist but it will be empty.
*
* @param[in]  pBlock - block descriptor
*
* @return     E_OK - Erase successfully
*             E_NOT_OK - Erase failed
*/
/*===================================================================================================*/
static FUNC(Std_ReturnType, AUTOMATIC) MemDal_Sdhc_EraseFile(P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock)
{
    P2VAR(MemDal_Sdhc_FileDescriptorType, AUTOMATIC, MEMDAL_APPL_DATA) pFile = NULL_PTR;
    VAR(Std_ReturnType, AUTOMATIC) status = E_NOT_OK;
    VAR(FRESULT, AUTOMATIC) result;
    VAR(FIL, AUTOMATIC) filePtr;

    MemDal_Sdhc_eJobResult = MEMDAL_BLOCK_STATE_ERROR;

    pFile = &MemDal_Sdhc_pConfig->pSdhcFiles[pBlock->u16DrvBlockId];

    /* File erase operation supposed to remove the file content, and the fastest way
       to achieve that is to delete the entire file and create it back.
    */

    /* Immediate mount */
    result = f_mount(&MemDal_Sdhc_FatfsObj, MEMDAL_SDHC_CARD_DRIVE, 1);

    if (FR_OK == result)
    {
        /* First delete the file */
        result = f_unlink(pFile->pFilePath);

        /* And then create the file back */
        if (FR_OK == result)
        {
            result = f_open(&filePtr, pFile->pFilePath, FA_WRITE | FA_OPEN_ALWAYS);

            if (FR_OK == result)
            {
                result = f_close(&filePtr);
                status = E_OK;
            }
        }

        /* Un-mount drive */
        f_unmount(MEMDAL_SDHC_CARD_DRIVE);
    }

    if (E_OK == status)
    {
        MemDal_Sdhc_eJobResult = MEMDAL_BLOCK_STATE_READY;
    }

    return status;
}

#else /* (STD_ON == MEMDAL_SDHC_FATFS_ENABLED) */

/*===================================================================================================*/
/**
* @brief      MemDal Sdhc read block function
* @details    Read from memory card in raw data mode
*
* @param[in]  pBlock - block descriptor
*
* @return     E_OK - Read successfully
*             E_NOT_OK - Read failed
*/
/*===================================================================================================*/
static FUNC(Std_ReturnType, AUTOMATIC) MemDal_Sdhc_ReadBlock(P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock)
{
    VAR(uint32, AUTOMATIC) sectorAddress = 0;
    VAR(uint32, AUTOMATIC) sectorCount = 0;
    P2VAR(memory_card_t, AUTOMATIC, MEMDAL_APPL_DATA) card;

    MemDal_Sdhc_eJobResult = MEMDAL_BLOCK_STATE_ERROR;

    card = uSDHC_GetSDCard();

    if (NULL != card)
    {
        sectorAddress = MemDal_Sdhc_pConfig->pSdhcBlockAddress[pBlock->u16DrvBlockId];
        sectorCount = pBlock->u32BlockSize;

        if((common_status_t)SDHC_STATUS_OK == SD_ReadBlocks(card, (uint8_t*)pBlock->u32RamBuffer, sectorAddress, sectorCount))
        {
            MemDal_Sdhc_eJobResult = MEMDAL_BLOCK_STATE_READY;
        }
    }

    return E_OK;
}

/*===================================================================================================*/
/**
* @brief      MemDal Sdhc write block function
* @details    Write to memory card in raw data mode
*
* @param[in]  pBlock - block descriptor
*
* @return     E_OK - Write successfully
*             E_NOT_OK - Write failed
*/
/*===================================================================================================*/
static FUNC(Std_ReturnType, AUTOMATIC) MemDal_Sdhc_WriteBlock(P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock)
{
    VAR(uint32, AUTOMATIC) sectorAddress = 0;
    VAR(uint32, AUTOMATIC) sectorCount = 0;
    P2VAR(memory_card_t, AUTOMATIC, MEMDAL_APPL_DATA) card;

    MemDal_Sdhc_eJobResult = MEMDAL_BLOCK_STATE_ERROR;

    card = uSDHC_GetSDCard();

    if (NULL != card)
    {
        sectorAddress = MemDal_Sdhc_pConfig->pSdhcBlockAddress[pBlock->u16DrvBlockId];
        sectorCount = pBlock->u32BlockSize;

        if((common_status_t)SDHC_STATUS_OK == SD_WriteBlocks(card, (uint8_t*)pBlock->u32RamBuffer, sectorAddress, sectorCount))
        {
            MemDal_Sdhc_eJobResult = MEMDAL_BLOCK_STATE_READY;
        }
    }

    return E_OK;
}

/*===================================================================================================*/
/**
* @brief      MemDal Sdhc erase block function
* @details    Erase a block on the memory card in raw data mode
*
* @param[in]  pBlock - block descriptor
*
* @return     E_OK - Erase successfully
*             E_NOT_OK - Erase failed
*/
/*===================================================================================================*/
static FUNC(Std_ReturnType, AUTOMATIC) MemDal_Sdhc_EraseBlock(P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock)
{
    VAR(uint32, AUTOMATIC) sectorAddress = 0;
    VAR(uint32, AUTOMATIC) sectorCount = 0;
    P2VAR(memory_card_t, AUTOMATIC, MEMDAL_APPL_DATA) card;

    MemDal_Sdhc_eJobResult = MEMDAL_BLOCK_STATE_ERROR;

    card = uSDHC_GetSDCard();

    if (NULL != card)
    {
        sectorAddress = MemDal_Sdhc_pConfig->pSdhcBlockAddress[pBlock->u16DrvBlockId];
        sectorCount = pBlock->u32BlockSize;

        if((common_status_t)SDHC_STATUS_OK == SD_EraseBlocks(card, sectorAddress, sectorCount))
        {
            MemDal_Sdhc_eJobResult = MEMDAL_BLOCK_STATE_READY;
        }
    }

    return E_OK;
}

#endif /* (STD_ON == MEMDAL_SDHC_FATFS_ENABLED) */

/*===================================================================================================*/
/**
* @brief      Check if last SDHC job is done
* @details    SDHC operations are executed synchronously so the function will return always true
*
* @param[in]  None
*
* @return     TRUE - job completed
*             FALSE - job not completed
*/
/*===================================================================================================*/
FUNC(boolean, AUTOMATIC) MemDal_Sdhc_IsJobDone(void)
{
    return TRUE;
}

/*===================================================================================================*/
/**
* @brief      Get the result of a MemDal SDHC read/write/erase operation
* @details    This function will mainly check if the memory operation was successful or not
*
* @param[in]  None
*
* @return     MEMDAL_BLOCK_STATE_ERROR - block operation error
*             MEMDAL_BLOCK_STATE_READY - block operation successful
*             MEMDAL_BLOCK_STATE_BUSY - operation on going
*/
/*===================================================================================================*/
FUNC(MemDal_BlockStatusType, AUTOMATIC) MemDal_Sdhc_GetJobResult(void)
{
    return MemDal_Sdhc_eJobResult;
}

#endif /* (MEMDAL_SDHC_ENABLED == STD_ON) */

/*===================================================================================================*/
#ifdef __cplusplus
}
#endif
