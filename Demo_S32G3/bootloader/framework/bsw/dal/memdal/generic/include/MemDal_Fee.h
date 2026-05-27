/*===================================================================================================*/
/*
*   @file              MemDal_Fee.h
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

#ifndef MEMDAL_FEE_H
#define MEMDAL_FEE_H

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
#include "MemDal_Cfg.h"

/*===================================================================================================*/
/*                              SOURCE FILE VERSION INFORMATION                                      */
/*===================================================================================================*/
#define MEMDAL_FEE_VENDOR_ID                            43

#define MEMDAL_FEE_AR_RELEASE_MAJOR_VERSION             4
#define MEMDAL_FEE_AR_RELEASE_MINOR_VERSION             4
#define MEMDAL_FEE_AR_RELEASE_REVISION_VERSION          0

#define MEMDAL_FEE_SW_MAJOR_VERSION                     24
#define MEMDAL_FEE_SW_MINOR_VERSION                     12
#define MEMDAL_FEE_SW_PATCH_VERSION                     0

/*===================================================================================================*/
/*                                      FILE VERSION CHECKS                                          */
/*===================================================================================================*/
/* Check if MemDal fee configuration header file and cfg header file are of the same vendor */
#if (MEMDAL_FEE_VENDOR_ID != MEMDAL_CFG_VENDOR_ID)
    #error "MemDal_Fee.h and MemDal_Cfg.h have different vendor ids"
#endif

/* Check if MemDal fee configuration header file and cfg header file are of the same Autosar version */
#if ((MEMDAL_FEE_AR_RELEASE_MAJOR_VERSION != MEMDAL_CFG_AR_RELEASE_MAJOR_VERSION) || \
     (MEMDAL_FEE_AR_RELEASE_MINOR_VERSION != MEMDAL_CFG_AR_RELEASE_MINOR_VERSION) || \
     (MEMDAL_FEE_AR_RELEASE_REVISION_VERSION != MEMDAL_CFG_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of MemDal_Fee.h and MemDal_Cfg.h are different"
#endif

/* Check if MemDal fee configuration header file and cfg header file are of the same software version */
#if ((MEMDAL_FEE_SW_MAJOR_VERSION != MEMDAL_CFG_SW_MAJOR_VERSION) || \
     (MEMDAL_FEE_SW_MINOR_VERSION != MEMDAL_CFG_SW_MINOR_VERSION) || \
     (MEMDAL_FEE_SW_PATCH_VERSION != MEMDAL_CFG_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of MemDal_Fee.h and MemDal_Cfg.h are different"
#endif
/*===================================================================================================*/
/*                                 STRUCTURES AND OTHER TYPEDEFS                                     */
/*===================================================================================================*/

/*===================================================================================================*/
/*                                       DEFINES AND MACROS                                          */
/*===================================================================================================*/

/*===================================================================================================*/
/*                                       GLOBAL CONSTANTS                                            */
/*===================================================================================================*/

/*===================================================================================================*/
/*                                       GLOBAL VARIABLES                                            */
/*===================================================================================================*/

/*===================================================================================================*/
/*                                       GLOBAL FUNCTIONS                                            */
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
FUNC(Std_ReturnType, AUTOMATIC) MemDal_Fee_Read(P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock);

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
FUNC(Std_ReturnType, AUTOMATIC) MemDal_Fee_Write(P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock);

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
FUNC(Std_ReturnType, AUTOMATIC) MemDal_Fee_Erase(P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock);

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
FUNC(boolean, AUTOMATIC) MemDal_Fee_IsJobDone(void);

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
FUNC(MemDal_BlockStatusType, AUTOMATIC) MemDal_Fee_GetJobResult(void);


#ifdef __cplusplus
}
#endif

#endif /* MEMDAL_FEE_H */
