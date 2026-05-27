/*===================================================================================================*/
/*
*   @file              MemDal_Fee.c
*   project            AUTOSAR 4.4 GOLDVIP
*   platform           CORTEXM
*
*   @brief             Integration Framework - MemDal Fee driver
*   @details           This file initialize the Fee memory driver for MemDal
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
#include "MemDal_Fee.h"

#if (MEMDAL_FEE_ENABLED == STD_ON)

#include "Fee.h"
#include "Fee_Cbk.h"
/*===================================================================================================*/
/*                              SOURCE FILE VERSION INFORMATION                                      */
/*===================================================================================================*/
#define MEMDAL_FEE_VENDOR_ID_C                              43

#define MEMDAL_FEE_AR_RELEASE_MAJOR_VERSION_C               4
#define MEMDAL_FEE_AR_RELEASE_MINOR_VERSION_C               4
#define MEMDAL_FEE_AR_RELEASE_REVISION_VERSION_C            0

#define MEMDAL_FEE_SW_MAJOR_VERSION_C                       24
#define MEMDAL_FEE_SW_MINOR_VERSION_C                       12
#define MEMDAL_FEE_SW_PATCH_VERSION_C                       0

/*===================================================================================================*/
/*                                      FILE VERSION CHECKS                                          */
/*===================================================================================================*/

/* Check if MemDal Fee driver source file and header file are of the same vendor */
#if (MEMDAL_FEE_VENDOR_ID_C != MEMDAL_FEE_VENDOR_ID)
    #error "MemDal_Fee.c and MemDal_Fee.h have different vendor ids"
#endif

/* Check if MemDal Fee driver source file and header file are of the same Autosar version */
#if ((MEMDAL_FEE_AR_RELEASE_MAJOR_VERSION_C != MEMDAL_FEE_AR_RELEASE_MAJOR_VERSION) || \
     (MEMDAL_FEE_AR_RELEASE_MINOR_VERSION_C != MEMDAL_FEE_AR_RELEASE_MINOR_VERSION) || \
     (MEMDAL_FEE_AR_RELEASE_REVISION_VERSION_C != MEMDAL_FEE_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of MemDal_Fee.c and MemDal_Fee.h are different"
#endif

/* Check if MemDal Fee driver source file and header file are of the same Software version */
#if ((MEMDAL_FEE_SW_MAJOR_VERSION_C != MEMDAL_FEE_SW_MAJOR_VERSION) || \
     (MEMDAL_FEE_SW_MINOR_VERSION_C != MEMDAL_FEE_SW_MINOR_VERSION) || \
     (MEMDAL_FEE_SW_PATCH_VERSION_C != MEMDAL_FEE_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of MemDal_Fee.c and MemDal_Fee.h are different"
#endif

/* Check if MemDal Fee driver source file and configuration header file are of the same vendor */
#if (MEMDAL_FEE_VENDOR_ID_C != MEMDAL_CFG_VENDOR_ID)
    #error "MemDal_Fee.c and MemDal_Cfg.h have different vendor ids"
#endif

/* Check if MemDal Fee driver source file and configuration header file are of the same Autosar version */
#if ((MEMDAL_FEE_AR_RELEASE_MAJOR_VERSION_C != MEMDAL_CFG_AR_RELEASE_MAJOR_VERSION) || \
     (MEMDAL_FEE_AR_RELEASE_MINOR_VERSION_C != MEMDAL_CFG_AR_RELEASE_MINOR_VERSION) || \
     (MEMDAL_FEE_AR_RELEASE_REVISION_VERSION_C != MEMDAL_CFG_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of MemDal_Fee.c and MemDal_Cfg.h are different"
#endif

/* Check if MemDal Fee driver source file and configuration header file are of the same software version */
#if ((MEMDAL_FEE_SW_MAJOR_VERSION_C != MEMDAL_CFG_SW_MAJOR_VERSION) || \
     (MEMDAL_FEE_SW_MINOR_VERSION_C != MEMDAL_CFG_SW_MINOR_VERSION) || \
     (MEMDAL_FEE_SW_PATCH_VERSION_C != MEMDAL_CFG_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of MemDal_Fee.c and MemDal_Cfg.h are different"
#endif

/* Check if MemDal_Fee source file and Fee header file are of the same Autosar version */
#if ((MEMDAL_FEE_AR_RELEASE_MAJOR_VERSION_C != FEE_AR_RELEASE_MAJOR_VERSION) || \
     (MEMDAL_FEE_AR_RELEASE_MINOR_VERSION_C != FEE_AR_RELEASE_MINOR_VERSION) || \
     (MEMDAL_FEE_AR_RELEASE_REVISION_VERSION_C != FEE_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of MemDal_Fee.c and Fee.h are different"
#endif

/*===================================================================================================*/
/*                               LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)                          */
/*===================================================================================================*/

/*===================================================================================================*/
/*                                       DEFINES AND MACROS                                          */
/*===================================================================================================*/

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

/*===================================================================================================*/
/*                                    LOCAL FUNCTION PROTOTYPES                                      */
/*===================================================================================================*/

/*===================================================================================================*/
/*                                      LOCAL FUNCTIONS                                              */
/*===================================================================================================*/

/*===================================================================================================*/
/**
* @brief      MemDal Fee read function
* @details    Starts an asynchronously job to read from flash memory
* 
* @param[in]  pBlock - block descriptor
* 
* @return     E_OK - Read job start successfully
*             E_NOT_OK - Read job failed
*/
/*===================================================================================================*/
FUNC(Std_ReturnType, AUTOMATIC) MemDal_Fee_Read(P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock)
{
    VAR(Std_ReturnType, AUTOMATIC) status = E_NOT_OK;
    
    if (MEMIF_IDLE == Fee_GetStatus())
    {
        status = Fee_Read(pBlock->u16DrvBlockId, 0, (uint8*)pBlock->u32RamBuffer, pBlock->u32BlockSize);

        pBlock->eBlockStatus = MEMDAL_BLOCK_STATE_BUSY;
    }

    return status;
}

/*===================================================================================================*/
/**
* @brief      MemDal Fee write function
* @details    Starts an asynchronously job to write to flash memory
* 
* @param[in]  pBlock - block descriptor
* 
* @return     E_OK - Write job start successfully
*             E_NOT_OK - Write job failed
*/
/*===================================================================================================*/
FUNC(Std_ReturnType, AUTOMATIC) MemDal_Fee_Write(P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock)
{
    VAR(Std_ReturnType, AUTOMATIC) status = E_NOT_OK;

    if (MEMIF_IDLE == Fee_GetStatus())
    {
        status = Fee_Write(pBlock->u16DrvBlockId, (const uint8*)pBlock->u32RamBuffer);

        pBlock->eBlockStatus = MEMDAL_BLOCK_STATE_BUSY;
    }

    return status;
}

/*===================================================================================================*/
/**
* @brief      MemDal Fee erase function
* @details    Starts an asynchronously job to erase block flash memory
* 
* @param[in]  pBlock - block descriptor
* 
* @return     E_OK - Erase job start successfully
*             E_NOT_OK - Erase job failed
*/
/*===================================================================================================*/
FUNC(Std_ReturnType, AUTOMATIC) MemDal_Fee_Erase(P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock)
{
    VAR(Std_ReturnType, AUTOMATIC) status = E_NOT_OK;

    if (MEMIF_IDLE == Fee_GetStatus())
    {
        status = Fee_EraseImmediateBlock(pBlock->u16DrvBlockId);

        pBlock->eBlockStatus = MEMDAL_BLOCK_STATE_BUSY;
    }

    return status;
}

/*===================================================================================================*/
/**
* @brief      Check if last FEE job is done
* @details    This will be done by checking the idle state of FEE module
* 
* @param[in]  None
* 
* @return     TRUE - job completed
*             FALSE - job not completed
*/
/*===================================================================================================*/
FUNC(boolean, AUTOMATIC) MemDal_Fee_IsJobDone(void)
{
    return (MEMIF_IDLE == Fee_GetStatus());
}

/*===================================================================================================*/
/**
* @brief      Get the result of a MemDal FEE read/write/erase operation
* @details    This function will mainly check if the memory operation was successful or not
* 
* @param[in]  None
* 
* @return     MEMDAL_BLOCK_STATE_ERROR - block operation error
*             MEMDAL_BLOCK_STATE_READY - block operation succesful
*             MEMDAL_BLOCK_STATE_BUSY - operation on going
*/
/*===================================================================================================*/
FUNC(MemDal_BlockStatusType, AUTOMATIC) MemDal_Fee_GetJobResult(void)
{
    VAR(MemDal_BlockStatusType, AUTOMATIC) jobResult = MEMDAL_BLOCK_STATE_ERROR;

    switch (Fee_GetJobResult())
    {
    case MEMIF_JOB_OK:
    case MEMIF_JOB_CANCELED:
        jobResult = MEMDAL_BLOCK_STATE_READY;
        break;
    
    case MEMIF_JOB_PENDING:
        jobResult = MEMDAL_BLOCK_STATE_BUSY;
        break;
    
    default:
        /* any other statuses are considered a failed operation */
        break;
    }

    return jobResult;
}

/*===================================================================================================*/
/**
* @brief      FLS job end callout
* @details    
* 
* @param[in]  None
* 
* @return     None
*/
/*===================================================================================================*/
FUNC (void, MEMDAL_APP_CODE) Fls_JobEndNotif(void)
{
    Fee_JobEndNotification();
}

/*===================================================================================================*/
/**
* @brief      FLS job error callout
* @details    
* 
* @param[in]  None
* 
* @return     None
*/
/*===================================================================================================*/
FUNC (void, MEMDAL_APP_CODE) Fls_JobErrorNotif(void)
{
    Fee_JobErrorNotification();
}

/*===================================================================================================*/
/**
* @brief      FEE job end callout
* @details    
* 
* @param[in]  None
* 
* @return     None
*/
/*===================================================================================================*/
FUNC (void, MEMDAL_APP_CODE) NvM_JobEndNotification(void)
{

}

/*===================================================================================================*/
/**
* @brief      FEE job error callout
* @details    
* 
* @param[in]  None
* 
* @return     None
*/
/*===================================================================================================*/
FUNC (void, MEMDAL_APP_CODE) NvM_JobErrorNotification(void)
{

}

#endif /* (MEMDAL_FEE_ENABLED == STD_ON) */

/*===================================================================================================*/
#ifdef __cplusplus
}
#endif
