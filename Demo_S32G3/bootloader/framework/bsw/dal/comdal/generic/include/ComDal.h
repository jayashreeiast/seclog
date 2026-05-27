/**
*   @file           ComDal.h
*   @version        24.12.0
*
*   @brief          Integration Framework - ComDal high level interface implementation.
*   @details        ComDal high level interface implementation.
*
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*   All Rights Reserved.
*   This file contains sample code only. It is not part of the production code deliverables.
*
*   @addtogroup COMDAL
*   @{
*/
/*==================================================================================================
*   Platform             : CORTEXM
*
*   Autosar Version      : 4.4.0
*   Autosar Revision     : ASR_REL_4_4_REV_0000
*
*   SW Version           : 24.12.0
*   Build Version        : S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
============================================================================================================================*/


#ifndef COMDAL_H
#define COMDAL_H

#ifdef __cplusplus
extern "C"{
#endif
/*===================================================================================================================
                                         INCLUDE FILES
 1) system and project includes
 2) needed interfaces from external units
 3) internal and external interfaces from this unit
====================================================================================================================*/
#include "ComDal_Cfg.h"
/*===================================================================================================================
*                              SOURCE FILE VERSION INFORMATION
====================================================================================================================*/
/**
* @file          ComDal.h
*/
#define COMDAL_VENDOR_ID                           43
#define COMDAL_MODULE_ID                           100

#define COMDAL_AR_RELEASE_MAJOR_VERSION            4
#define COMDAL_AR_RELEASE_MINOR_VERSION            4
#define COMDAL_AR_RELEASE_REVISION_VERSION         0

#define COMDAL_SW_MAJOR_VERSION                    24
#define COMDAL_SW_MINOR_VERSION                    12
#define COMDAL_SW_PATCH_VERSION                    0
/*===================================================================================================================
*                                       FILE VERSION CHECKS
====================================================================================================================*/
/**
* @file           ComDal.h
*/
/* Check if source file and COMDAL header file are of the same vendor */
#if (COMDAL_VENDOR_ID != COMDAL_CFG_VENDOR_ID)
    #error "ComDal.h and ComDal_Cfg.h have different vendor ids"
#endif

/* Check if source file and COMDAL header file are of the same Autosar version */
#if ((COMDAL_AR_RELEASE_MAJOR_VERSION != COMDAL_CFG_AR_RELEASE_MAJOR_VERSION) || \
     (COMDAL_AR_RELEASE_MINOR_VERSION != COMDAL_CFG_AR_RELEASE_MINOR_VERSION) || \
     (COMDAL_AR_RELEASE_REVISION_VERSION != COMDAL_CFG_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of ComDal.h and ComDal_Cfg.h are different"
#endif

/* Check if source file and COMDAL header file are of the same Software version */
#if ((COMDAL_SW_MAJOR_VERSION != COMDAL_CFG_SW_MAJOR_VERSION) || \
     (COMDAL_SW_MINOR_VERSION != COMDAL_CFG_SW_MINOR_VERSION) || \
     (COMDAL_SW_PATCH_VERSION != COMDAL_CFG_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of ComDal.h and ComDal_Cfg.h are different"
#endif

/*===================================================================================================================
*                                          DEFINES AND MACROS
====================================================================================================================*/
/**
*       Development errors.
*       The following errors shall be detectable by the COMDAL module depending on its 
*       configuration (development / production mode).
*
*       All error codes
*
*/

/**
* @brief   API service used without COMDAL module initialization
* */
#define COMDAL_E_UNINIT                                 ((uint8)0xE0U)


/**
* @brief   The COMDAL module is already initilized 
* */
#define COMDAL_E_ALREADY_INITIALIZED                    ((uint8)0xE1U)

/**
* @brief   The COMDAL  module is not properly configured 
* */
#define COMDAL_E_PARAM_CONFIG                           ((uint8)0xE2U)

/**
* @brief   API service is called using an invalid pointer (e.g. the pointer should not be NULL).
* */
#define COMDAL_E_PARAM_POINTER                          ((uint8)0xE3U)

/**
* @brief   API service used with an invalid COMDAL channel type.
* */
#define COMDAL_E_INVALID_CHANNEL_TYPE                    ((uint8)0xE4U)

/**
* @brief   API service used with an invalid COMDAL channel id.
* */
#define COMDAL_E_INVALID_CHANNEL_ID                       ((uint8)0xE5U)



/**
          All AUTOSAR API's service IDs
*/

/**
* @brief API service ID for ComDal_Init function
* */
#define COMDAL_INIT_API_ID                                0x00U

/**
* @brief API service ID for ComDal_DeInit function
* */
#define COMDAL_DEINIT_API_ID                              0x01U

/**
* @brief API service ID for ComDal_Main function
* */
#define COMDAL_MAINFUNCTION_API_ID                        0x02U

/**
* @brief API service ID for ComDal_ReadData function
* */
#define COMDAL_READDATA_API_ID                            0x03U

/**
* @brief API service ID for ComDal_WriteData function
* */
#define COMDAL_WRITEDATA_API_ID                           0x04U


/**
* @brief API service ID for ComDal_GetChannelID function
* */
#define COMDAL_GET_CHANNEL_API_ID                         0x05U

/**
* @brief API service ID for ComDal_GetChannelStatus function
* */
#define COMDAL_GET_CHANNEL_STATUS_API_ID                  0x06U

FUNC(void, COMDAL_APP_CODE) ComDal_Init(P2CONST(ComDal_ConfigType, AUTOMATIC, COMDAL_APP_CONST) ConfigPtr);
FUNC(void, COMDAL_APP_CODE) ComDal_DeInit(void);
FUNC(void, COMDAL_APP_CODE) ComDal_Main(void);
FUNC(void, COMDAL_APP_CODE) ComDal_WriteData
(
    VAR(ComDal_ChannelType, AUTOMATIC) ComChannel,
    P2VAR(char, AUTOMATIC, COMDAL_APPL_DATA) WriteBuffer,
    VAR(uint8, AUTOMATIC) TxDataLen
);
FUNC(ComDal_ReturnType, COMDAL_APP_CODE) ComDal_ReadData
(
    VAR(ComDal_ChannelType, AUTOMATIC) ComChannel,
    P2VAR(char, AUTOMATIC, COMDAL_APPL_DATA) pReadBuffer,
    P2VAR(uint16, AUTOMATIC, COMDAL_APPL_DATA) pRxDataLen
);
FUNC(void, COMDAL_APP_CODE) ComDal_ControllerReset(void);
FUNC(uint16, COMDAL_APP_CODE) ComDal_GetMessageHandlerId(VAR(ComDal_ChannelType, AUTOMATIC) ComChannel);
FUNC(ComDal_State, COMDAL_APP_CODE) ComDal_GetChannelStatus(VAR(ComDal_ChannelType, AUTOMATIC) ComChannel);
#ifdef __cplusplus
}
#endif

#endif /*COMDAL_H*/
/** @} */


