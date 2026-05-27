/*===================================================================================================*/
/*
*   @file              MemDal.h
*   project            AUTOSAR 4.4 GOLDVIP
*   platform           CORTEXM
*
*   @brief             Integration Framework - Memory Driver Abstraction Layer component
*   @details           This header file implements the MemDAL interface to application layer
*
*   Autosar Version    4.4.0
*   Build Version      S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*   All Rights Reserved.
*/
/*===================================================================================================*/

#ifndef MEMDAL_H
#define MEMDAL_H

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
#define MEMDAL_VENDOR_ID                                43
#define MEMDAL_MODULE_ID                                106

#define MEMDAL_AR_RELEASE_MAJOR_VERSION                 4
#define MEMDAL_AR_RELEASE_MINOR_VERSION                 4
#define MEMDAL_AR_RELEASE_REVISION_VERSION              0

#define MEMDAL_SW_MAJOR_VERSION                         24
#define MEMDAL_SW_MINOR_VERSION                         12
#define MEMDAL_SW_PATCH_VERSION                         0

/*===================================================================================================*/
/*                                      FILE VERSION CHECKS                                          */
/*===================================================================================================*/
/* Check if MemDal configuration header file and cfg header file are of the same vendor */
#if (MEMDAL_VENDOR_ID != MEMDAL_CFG_VENDOR_ID)
    #error "MemDal.h and MemDal_Cfg.h have different vendor ids"
#endif

/* Check if MemDal configuration header file and cfg header file are of the same Autosar version */
#if ((MEMDAL_AR_RELEASE_MAJOR_VERSION != MEMDAL_CFG_AR_RELEASE_MAJOR_VERSION) || \
     (MEMDAL_AR_RELEASE_MINOR_VERSION != MEMDAL_CFG_AR_RELEASE_MINOR_VERSION) || \
     (MEMDAL_AR_RELEASE_REVISION_VERSION != MEMDAL_CFG_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of MemDal.h and MemDal_Cfg.h are different"
#endif

/* Check if MemDal configuration header file and cfg header file are of the same software version */
#if ((MEMDAL_SW_MAJOR_VERSION != MEMDAL_CFG_SW_MAJOR_VERSION) || \
     (MEMDAL_SW_MINOR_VERSION != MEMDAL_CFG_SW_MINOR_VERSION) || \
     (MEMDAL_SW_PATCH_VERSION != MEMDAL_CFG_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of MemDal.h and MemDal_Cfg.h are different"
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
* @brief      MemDal component initialization function
* @details    
* 
* @param[in]  pConfig - Pointer to MemDal module configuration structure
* 
* @return     None
*/
/*===================================================================================================*/
FUNC(void, AUTOMATIC) MemDal_Init(P2CONST( MemDal_ConfigType, AUTOMATIC, MEMDAL_APPL_CONST ) pConfig);

/*===================================================================================================*/
/**
* @brief      MemDal component deinitialization function
* @details    
* 
* @param[in]  None
* 
* @return     None
*/
/*===================================================================================================*/
FUNC(void, AUTOMATIC) MemDal_DeInit(void);

/*===================================================================================================*/
/**
* @brief      MemDal component main function
* @details    This function must be called cyclically
* 
* @param[in]  None
* 
* @return     None
*/
/*===================================================================================================*/
FUNC(void, AUTOMATIC) MemDal_Main(void);

/*===================================================================================================*/
/**
* @brief      MemDal read block
* @details    This function will set the destination address of the RAM buffer and start the read operation.
*             The caller has to ensure that size of the RAM buffer is large enough to fit the entire MemDal block.
*
* @param[in]  BlockId - ID of the block to be read
* @param[in]  u32DestinationAddress - the address of the destination RAM buffer
* 
* @return     E_OK - read operation started
*             E_NOT_OK - read operation rejected
*/
/*===================================================================================================*/
FUNC(Std_ReturnType, AUTOMATIC) MemDal_ReadBlock
(
    CONST(MemDal_BlockIdType, AUTOMATIC) BlockId,
    VAR(uint32, AUTOMATIC) u32DestinationAddress
);

/*===================================================================================================*/
/**
* @brief      MemDal write block
* @details    This function will set the source address of the RAM buffer and start the write operation.
*             The caller has to ensure data integrity of RAM buffer while data is copyed to external memory.
*             If block write type is ONREQUEST the transfer will be done in the next MemDal_Main() call.
*             If block write type is ONCLOSING the transfer will be done on ECU transition to sleep mode.
*
* @param[in]  BlockId - ID of the block to be written
* @param[in]  u32SourceAddress - the address of the source RAM buffer
* 
* @return     E_OK - write operation started
*             E_NOT_OK - write operation rejected
*/
/*===================================================================================================*/
FUNC(Std_ReturnType, AUTOMATIC) MemDal_WriteBlock
(
    CONST(MemDal_BlockIdType, AUTOMATIC) BlockId,
    VAR(uint32, AUTOMATIC) u32SourceAddress
);

/*===================================================================================================*/
/**
* @brief      MemDal erase block
* @details    This function will trigger an erase operation of the external memory block. MemDal will not
*             affect the content of RAM buffer during this operation, this being application responsibility.
*
* @param[in]  BlockId - ID of the block to be erased
* 
* @return     E_OK - erase operation started
*             E_NOT_OK - erase operation rejected
*/
/*===================================================================================================*/
FUNC(Std_ReturnType, AUTOMATIC) MemDal_EraseBlock
(
    CONST(MemDal_BlockIdType, AUTOMATIC) BlockId
);

/*===================================================================================================*/
/**
* @brief      Get the block status
* @details    The block will start in the UNINIT state and will switch between BUSY and READY status
*             during read/write operations.
*
* @param[in]  BlockId - ID of the block
* 
* @return     MEMDAL_BLOCK_STATE_ERROR - block operations error
*             MEMDAL_BLOCK_STATE_READY - block ready for operations
*             MEMDAL_BLOCK_STATE_BUSY - operations on going
*             MEMDAL_BLOCK_STATE_UNINIT -  block not initialized
*/
/*===================================================================================================*/
FUNC(MemDal_BlockStatusType, AUTOMATIC) MemDal_GetBlockStatus
(
    CONST(MemDal_BlockIdType, AUTOMATIC) BlockId
);

#ifdef __cplusplus
}
#endif

#endif /* MEMDAL_H */
