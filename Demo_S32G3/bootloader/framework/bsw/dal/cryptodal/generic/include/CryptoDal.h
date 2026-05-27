/**
*   @file           CryptoDal.h
*   @version        24.12.0
*
*   @brief          Integration Framework - CryptoDal high level interface implementation.
*   @details        CryptoDal high level interface implementation.
*
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*   All Rights Reserved.
*   This file contains sample code only. It is not part of the production code deliverables.
*
*   @addtogroup CRYPTODAL
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

#ifndef CRYPTODAL_H
#define CRYPTODAL_H

#ifdef __cplusplus
extern "C"{
#endif
/*===================================================================================================================
                                         INCLUDE FILES
 1) system and project includes
 2) needed interfaces from external units
 3) internal and external interfaces from this unit
====================================================================================================================*/
#include "CryptoDal_Cfg.h"
#if CRYPTODAL_USE_CRYPTO
#include "Csm_Types.h"
#include "Hse_Ip.h"
#endif /* CRYPTODAL_USE_CRYPTO */
/*===================================================================================================================
*                              SOURCE FILE VERSION INFORMATION
====================================================================================================================*/
/**
* @file          CryptoDal.h
*/
#define CRYPTODAL_VENDOR_ID                           43
#define CRYPTODAL_MODULE_ID                           105

#define CRYPTODAL_AR_RELEASE_MAJOR_VERSION            4
#define CRYPTODAL_AR_RELEASE_MINOR_VERSION            4
#define CRYPTODAL_AR_RELEASE_REVISION_VERSION         0

#define CRYPTODAL_SW_MAJOR_VERSION                    24
#define CRYPTODAL_SW_MINOR_VERSION                    12
#define CRYPTODAL_SW_PATCH_VERSION                    0
/*===================================================================================================================
*                                       FILE VERSION CHECKS
====================================================================================================================*/


/**
* @file           CryptoDal.h
*/
/* Check if source file and CRYPTODAL header file are of the same vendor */
#if (CRYPTODAL_VENDOR_ID != CRYPTODAL_CFG_VENDOR_ID)
    #error "CryptoDal.h and CryptoDal_Cfg.h have different vendor ids"
#endif

/* Check if source file and CRYPTODAL header file are of the same Autosar version */
#if ((CRYPTODAL_AR_RELEASE_MAJOR_VERSION != CRYPTODAL_CFG_AR_RELEASE_MAJOR_VERSION) || \
     (CRYPTODAL_AR_RELEASE_MINOR_VERSION != CRYPTODAL_CFG_AR_RELEASE_MINOR_VERSION) || \
     (CRYPTODAL_AR_RELEASE_REVISION_VERSION != CRYPTODAL_CFG_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of CryptoDal.h and CryptoDal_Cfg.h are different"
#endif

/* Check if source file and CRYPTODAL header file are of the same Software version */
#if ((CRYPTODAL_SW_MAJOR_VERSION != CRYPTODAL_CFG_SW_MAJOR_VERSION) || \
     (CRYPTODAL_SW_MINOR_VERSION != CRYPTODAL_CFG_SW_MINOR_VERSION) || \
     (CRYPTODAL_SW_PATCH_VERSION != CRYPTODAL_CFG_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of CryptoDal.h and CryptoDal_Cfg.h are different"
#endif



/*===================================================================================================================
*                                          DEFINES AND MACROS
====================================================================================================================*/
/**
*       Development errors.
*       The following errors shall be detectable by the CRYPTODAL module depending on its
*       configuration (development / production mode).
*
*       All error codes
*
*/

/**
* @brief   API service used without CRYPTODAL module initialization
* */
#define CRYPTODAL_E_UNINIT                                 ((uint8)0xE0U)


/**
* @brief   The CRYPTODAL module is already initilized
* */
#define CRYPTODAL_E_ALREADY_INITIALIZED                    ((uint8)0xE1U)

/**
* @brief   The CRYPTODAL  module is not properly configured
* */
#define CRYPTODAL_E_PARAM_CONFIG                           ((uint8)0xE2U)

/**
* @brief   API service is called using an invalid pointer (e.g. the pointer should not be NULL).
* */
#define CRYPTODAL_E_PARAM_POINTER                          ((uint8)0xE3U)

/**
* @brief   API service used with an invalid CRYPTODAL channel type.
* */
#define CRYPTODAL_E_INVALID_CHANNEL_TYPE                    ((uint8)0xE4U)

/**
* @brief   API service used with an invalid CRYPTODAL channel id.
* */
#define CRYPTODAL_E_INVALID_CHANNEL_ID                       ((uint8)0xE5U)

/**
* @brief   The CRYPTODAL  module returned an invalid result
* */
#define CRYPTODAL_E_INVALID_RESULT                          ((uint8)0xE6U)

/**
          All AUTOSAR API's service IDs
*/

/**
* @brief API service ID for CryptoDal_Init function
* */
#define CRYPTODAL_INIT_API_ID                                  0x00U

/**
* @brief API service ID for CryptoDal_Encrypt function
* */
#define CRYPTODAL_ENCRYPT_API_ID                               0x01U

/**
* @brief API service ID for CryptoDal_Decrypt function
* */
#define CRYPTODAL_DECRYPT_API_ID                               0x02U

/**
* @brief API service ID for CryptoDal_DeInit function
* */
#define CRYPTODAL_DEINIT_API_ID                                0x03U

/**
* @brief API service ID for CryptoDal_Hash function
* */
#define CRYPTODAL_HASH_API_ID                                   0x04U

/**
* @brief Macro used to fetch primary Crypto key
* */
#define CRYPTODAL_PRIMARY_KEY                                   0x1U

/**
* @brief Macro used to fetch secondary Crypto key
* */
#define CRYPTODAL_SECONDARY_KEY                                 0x2U

FUNC(void, CRYPTODAL_APP_CODE) CryptoDal_Init(
    P2CONST(CryptoDal_ConfigType, AUTOMATIC, CRYPTODAL_APP_CONST) ConfigPtr);

FUNC(Std_ReturnType, CRYPTODAL_APP_CODE) CryptoDal_Encrypt(
    P2VAR(uint8, AUTOMATIC, CRYPTODAL_APPL_CONST) pText,
    P2VAR(uint8, AUTOMATIC, CRYPTODAL_APPL_CONST) pCipherText,
    VAR(uint8, AUTOMATIC) u8KeyIndex,
    VAR(uint32, AUTOMATIC) u32MessageSize,
    P2VAR(uint32, AUTOMATIC, CRYPTODAL_APPL_CONST) pResultSize);

FUNC(Std_ReturnType, CRYPTODAL_APP_CODE) CryptoDal_Decrypt(
    P2VAR(uint8, AUTOMATIC, CRYPTODAL_APPL_CONST) pCipherText,
    P2VAR(uint8, AUTOMATIC, CRYPTODAL_APPL_CONST) pText,
    VAR(uint8, AUTOMATIC) u8KeyIndex,
    VAR(uint32, AUTOMATIC) u32MessageSize,
    P2VAR(uint32, AUTOMATIC, CRYPTODAL_APPL_CONST) pResultSize);

#if (CRYPTODAL_SUPPORT_HASHING == STD_ON)
FUNC(Std_ReturnType, CRYPTODAL_APP_CODE) CryptoDal_Hash(
    P2VAR(uint8, AUTOMATIC, CRYPTODAL_APPL_CONST) pHashData,
    P2VAR(uint8, AUTOMATIC, CRYPTODAL_APPL_CONST) pInputData,
    VAR(uint32, AUTOMATIC) u32InputDataSize,
    VAR(CryptoDal_HashAlgorithmType, AUTOMATIC) eHashAlgorithm,
    VAR(Crypto_OperationModeType, AUTOMATIC) eAlgorithmMode);
#endif /* (CRYPTODAL_SUPPORT_HASHING == STD_ON)*/

#if (CRYPTODAL_SUPPORT_CMAC == STD_ON)
FUNC(Std_ReturnType, CRYPTODAL_APP_CODE) CryptoDal_GenerateCmac(
    P2VAR(uint8, AUTOMATIC, CRYPTODAL_CONST) pInputData,
    P2VAR(uint8, AUTOMATIC, CRYPTODAL_CONST) pTag,
    VAR(uint8, AUTOMATIC)  u8KeyIndex,
    VAR(uint32, AUTOMATIC) u32InputDataSize,
    P2VAR(uint32, AUTOMATIC, CRYPTODAL_CONST) pTagSize);
#endif /* (CRYPTODAL_SUPPORT_CMAC == STD_ON) */

FUNC(hseKeyHandle_t, CRYPTODAL_APP_CODE) CryptoDal_GetKeyHandle(
    VAR(uint8, AUTOMATIC) u8CryptoDalIdx,
    VAR(uint8, AUTOMATIC) u8Type);

#if (CRYPTODAL_SUPPORT_SMR == STD_ON)
FUNC(Std_ReturnType, CRYPTODAL_APP_CODE) CryptoDal_VerifySmr(
    VAR(uint8, AUTOMATIC) u8EntryIndex);
FUNC(Std_ReturnType, CRYPTODAL_APP_CODE) CryptoDal_SMR_CR_Install(
    P2VAR(CryptoDal_SMR_CR_EntryType, AUTOMATIC, CRYPTODAL_CONST) pSMR_CR_Config);
FUNC(Std_ReturnType, CRYPTODAL_APP_CODE) CryptoDal_GetSysImage(
    P2VAR(uint8, AUTOMATIC, CRYPTODAL_CONST) pImage,
    P2VAR(uint32, AUTOMATIC, CRYPTODAL_CONST) pSysImageOffset,
    P2VAR(uint32, AUTOMATIC, CRYPTODAL_CONST) pHseImageSize);
#endif /* (CRYPTODAL_SUPPORT_SMR == STD_ON) */

FUNC(void, CRYPTODAL_APP_CODE) CryptoDal_GetHseFwVersion(void);

#ifdef __cplusplus
}
#endif

#endif /*CRYPTODAL_H*/
/** @} */


