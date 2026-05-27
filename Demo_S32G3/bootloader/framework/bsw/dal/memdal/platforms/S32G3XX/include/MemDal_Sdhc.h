/*===================================================================================================*/
/*
*   @file              MemDal_Sdhc.h
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

#ifndef MEMDAL_SDHC_H
#define MEMDAL_SDHC_H

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
#define MEMDAL_SDHC_VENDOR_ID                           43
#define MEMDAL_SDHC_MODULE_ID                           106

#define MEMDAL_SDHC_AR_RELEASE_MAJOR_VERSION            4
#define MEMDAL_SDHC_AR_RELEASE_MINOR_VERSION            4
#define MEMDAL_SDHC_AR_RELEASE_REVISION_VERSION         0

#define MEMDAL_SDHC_SW_MAJOR_VERSION                    24
#define MEMDAL_SDHC_SW_MINOR_VERSION                    12
#define MEMDAL_SDHC_SW_PATCH_VERSION                    0

/*===================================================================================================*/
/*                                      FILE VERSION CHECKS                                          */
/*===================================================================================================*/

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
* @brief      MemDal Sdhc driver initialization function
* @details    
* 
* @param[in]  pConfig - Pointer to MemDal module configuration structure
* 
* @return     E_OK -  initialization successful
*             E_NOT_OK -  initialization failed
*/
/*===================================================================================================*/
FUNC(Std_ReturnType, AUTOMATIC) MemDal_Sdhc_Init(P2CONST(MemDal_ConfigType, AUTOMATIC, MEMDAL_APPL_CONST ) pConfig);

/*===================================================================================================*/
/**
* @brief      MemDal Sdhc driver deinitialization function
* @details    
* 
* @param[in]  None
* 
* @return     None
*/
/*===================================================================================================*/
FUNC(void, AUTOMATIC) MemDal_Sdhc_Deinit(void);

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
FUNC(Std_ReturnType, AUTOMATIC) MemDal_Sdhc_Read(P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock);

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
FUNC(Std_ReturnType, AUTOMATIC) MemDal_Sdhc_Write(P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock);

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
FUNC(Std_ReturnType, AUTOMATIC) MemDal_Sdhc_Erase(P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock);

/*===================================================================================================*/
/**
* @brief      Check if last SDHC job is done
* @details    
* 
* @param[in]  None
* 
* @return     TRUE - job completed
*             FALSE - job not completed
*/
/*===================================================================================================*/
FUNC(boolean, AUTOMATIC) MemDal_Sdhc_IsJobDone(void);

/*===================================================================================================*/
/**
* @brief      Get the result of a MemDal SDHC read/write/erase operation
* @details    This function will mainly check if the memory operation was successful or not
* 
* @param[in]  None
* 
* @return     MEMDAL_BLOCK_STATE_ERROR - block operation error
*             MEMDAL_BLOCK_STATE_READY - block operation succesful
*             MEMDAL_BLOCK_STATE_BUSY - operation on going
*/
/*===================================================================================================*/
FUNC(MemDal_BlockStatusType, AUTOMATIC) MemDal_Sdhc_GetJobResult(void);


#ifdef __cplusplus
}
#endif

#endif /* MEMDAL_SDHC_H */
