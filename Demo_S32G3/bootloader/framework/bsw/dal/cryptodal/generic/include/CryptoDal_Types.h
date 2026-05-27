/**
*   @file        CryptoDal_Types.h
*   @version     24.12.0
*
*   @brief       AUTOSAR - Integration Framework - Types file for CRYPTO Driver Abstraction Layer
*   @details     This file implements the CRYPTODAL Types
*
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*   All Rights Reserved.
*   This file contains sample code only. It is not part of the production code deliverables.
*
*   @addtogroup CRYPTODAL
*   @{
*/
/*=========================================================================================================================
*   Project              : AUTOSAR 4.4 GOLDVIP
*   Platform             : CORTEXM
*
*   Autosar Version      : 4.4.0
*   Autosar Revision     : ASR_REL_4_4_REV_0000
*
*   SW Version           : 24.12.0
*   Build Version        : S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
============================================================================================================================*/

#ifndef CRYPTODAL_TYPES_H
#define CRYPTODAL_TYPES_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
 1) system and project includes
 2) needed interfaces from external units
 3) internal and external interfaces from this unit
==================================================================================================*/
#include "Std_Types.h"
#include "Mcal.h"
#include "hse_common_types.h"

/*==================================================================================================
                                       SOURCE FILE VERSION INFORMATION
====================================================================================================*/
#define CRYPTODAL_TYPES_VENDOR_ID                           43
#define CRYPTODAL_TYPES_MODULE_ID                           105

#define CRYPTODAL_TYPES_AR_RELEASE_MAJOR_VERSION            4
#define CRYPTODAL_TYPES_AR_RELEASE_MINOR_VERSION            4
#define CRYPTODAL_TYPES_AR_RELEASE_REVISION_VERSION         0

#define CRYPTODAL_TYPES_SW_MAJOR_VERSION                    24
#define CRYPTODAL_TYPES_SW_MINOR_VERSION                    12
#define CRYPTODAL_TYPES_SW_PATCH_VERSION                    0

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/


/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/


/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/

/* Flag for invalid CryptoDal key number */
#define CRYPTODAL_INVALID_KEY 0xFFU

/* Byte size of a AES_256_CMAC authentication tag */
#define CRYPTODAL_CMAC_TAG_SIZE 32U

/* Byte size of a RSA_2048 authentication tag */
#define CRYPTODAL_RSA_2048_KEY_LENGTH 256U

/*==================================================================================================
*                                             ENUMS
==================================================================================================*/
/**
* @brief          Enumeration containing crypto algorithms
* @details        This enumeration is differentiating between different crypto algorithms
*/
typedef enum
{
    CRYPTODAL_AES_128_CBC  = 0U,      /**< @brief AES simetric encryption using 128 bits-key in CBC mode. */
    CRYPTODAL_AES_128_ECB  = 1U,      /**< @brief AES simetric encryption using 128 bits-key in ECB mode. */
    CRYPTODAL_AES_256_CMAC = 2U,      /**< @brief AES simetric message signing using 256 bits-key in CMAC mode. */
    CRYPTODAL_RSA_2048_PKCS = 3u      /**< @brief Asimetric message signing using 2048 bits-key in RSA PKCS mode. */
} CryptoDal_AlgorithmType;

/**
* @brief          Enumeration containing crypto hash algorithms
* @details        This enumeration is differentiating between different crypto hashing primitives
*/
typedef enum
{
    CRYPTODAL_SHA1    = 1U,    /**< @brief SHA algorithm with 128 bits result size */
    CRYPTODAL_SHA256  = 2U     /**< @brief SHA algorithm with 256 bits result size */

} CryptoDal_HashAlgorithmType;

/**
* @brief          Enumeration containing crypto operation tipes
* @details        This enumeration is differentiating between different crypto operation modes
*/

typedef enum
{
   CRYPTODAL_ONE_SHOT    = 0U,    /**< @brief Algorithm result is expected after first call. */
   CRYPTODAL_CONTINUOUS  = 1U     /**< @brief Algorithm result is expected after several calls. */
} CryptoDal_AlgorithmModeType;

/**
* @brief          Enumeration containing crypto accelerators
* @details        This enumeration is differentiating between different crypto accelerators
*/
typedef enum
{
    CRYPTODAL_CSEC    = 0U,    /**< @brief Csec Hardware acceleration module. */
    CRYPTODAL_CRYPTO  = 1U     /**< @brief Crypto Hardware acceleration module. */
} CryptoDal_EngineType;

/**
* @brief          Structure used for CRYPTODAL channel mapping.
* @details
*/
typedef struct
{
  /**< @brief Bsw channel id*/
  P2CONST(char,  AUTOMATIC, CRYPTODAL_APPL_CONST)           pCryptoKey;
} CryptoDal_BswChannelMapType;

/**
* @brief          Structure used for CRYPTODAL Secure Memory Region (SMR) configuration
*/
typedef struct
{
    hseAuthScheme_t authScheme;                 /**< @brief The authentication scheme used to verify the SMR */
    uint32          u32SourceAddr;              /**< @brief Flash storage address of SMR */
    uint32          u32DestinationAddr;         /**< @brief RAM storage address of SMR */
    uint32          u32AppSize;                 /**< @brief Size of the SMR */
    uint32          u32Signature;               /**< @brief The location of authenticity signature over SMR */
    uint32          u32KeyHandle;               /**< @brief Crypto key handle used for authentication */
    uint16          u16SignatureLength;         /**< @brief The length of the SMR authentication proof */
    uint8           u8EntryIndex;               /**< @brief Unique SMR entry id */
    uint8           u8ConfigFlags;              /**< @brief Configuration flags of SMR entry */
} CryptoDal_SMR_EntryType;

/**
* @brief          Structure used for CRYPTODAL Core Reset (CR) configuration
*/
typedef struct
{
    uint32  u32ResetAddr;            /**< @brief The primary address of the first instruction after a regular reset */
    uint32  u32PreSmrMap;            /**< @brief The SMR(s) which need to be verified before reset */
    uint8   u8CoreID;                /**< @brief What core will be started upon successful verification */
    uint8   u8CrEntryIndex;          /**< @brief Index in the Core Reset table */
} CryptoDal_CR_EntryType;

/**
* @brief          Structure used for CRYPTODAL Secure Memory Region (SMR) / Core Reset (CR) configuration
*/
typedef struct
{
    CryptoDal_SMR_EntryType* pSmrCfg;      /**< @brief Pointer to a list of SMR's */
    CryptoDal_CR_EntryType*  pCrCfg;       /**< @brief Core Reset configuration */
    uint8                    u8SmrNumber;  /**< @brief Number of SMR entries */
} CryptoDal_SMR_CR_EntryType;

/**
* @brief          Structure containing configuration data for the initialization of CRYPTODAL.
* @details        Data structure containing the set of configuration parameters required
*                 for initializing the CRYPTODAL.
*/
typedef struct
{
    /**< @brief key size in bytes */
    CONST(uint16  , AUTOMATIC)                                        u16EncryptionKeyLen;

    /**< @brief crypto algorithm used for current key */
    CONST(CryptoDal_AlgorithmType, AUTOMATIC)                         eEncryptionAlgorithm;

    /**< @brief crypto engine type */
    CONST(CryptoDal_EngineType, AUTOMATIC)                            eEncryptionEngine;

    /**< @brief pointer to crypto channel map*/
    P2CONST(uint8,  AUTOMATIC, AUTOMATIC)                             u8CryptoKey;

    /**< @brief encryption engine internal key reference */
    CONST(uint8, AUTOMATIC)                                           u8EncryptionKeyInternalRef;

    /**< @brief encryption engine key alternate reference. Shall be used if the  same key needs
	            different flags for crypto operations */
    CONST(uint8, AUTOMATIC)                                           u8EncryptionKeyAlternateRef;

} CryptoDal_ConfigType;



/*================================================================================================*/
#ifdef __cplusplus
}
#endif

#endif  /* CRYPTODAL_TYPES_H*/
/** @} */

