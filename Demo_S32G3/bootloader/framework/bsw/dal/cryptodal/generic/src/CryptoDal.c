/**
*   @file           CryptoDal.c
*   @version        24.12.0
*
*   @brief          Integration Framework - CRYPTODAL Abstraction Layer component
*   @details        This file implements the CRYPTODAL interface to application layer.
*
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*   All Rights Reserved.
*   This file contains sample code only. It is not part of the production code deliverables.
*
*   @addtogroup CRYPTODAL
*   @{
*/
/*=================================================================================================
*   Platform             : CORTEXM
*
*   Autosar Version      : 4.4.0
*   Autosar Revision     : ASR_REL_4_4_REV_0000
*   SW Version           : 24.12.0
*   Build Version        : S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
===================================================================================================*/

#ifdef __cplusplus
extern "C"{
#endif

/*=================================================================================================
                                         INCLUDE FILES
 1) system and project includes
 2) needed interfaces from external units
 3) internal and external interfaces from this unit
===================================================================================================*/
#include "Mcal.h"
#include "Compiler.h"

#include "CryptoDal_Types.h"
#include "CryptoDal.h"
#include "CryptoDal_Cfg.h"
#include "Console.h"
#include "Crypto_ASRExtension.h"

#if CRYPTODAL_USE_CSEC
#include "CryptoDal_Csec.h"
#endif /* CRYPTODAL_USE_CSEC */

#if CRYPTODAL_USE_CRYPTO
#include "MemLib.h"
#include "Crypto.h"
#include "Crypto_Private.h"
#endif /* CRYPTODAL_USE_CRYPTO */

#if (CRYPTODAL_DEV_ERROR_DETECT == STD_ON)
#include "Det.h"
#endif /* CRYPTODAL_DEV_ERROR_DETECT == STD_ON */



/*=================================================================================================
*                              SOURCE FILE VERSION INFORMATION
===================================================================================================*/
#define CRYPTODAL_VENDOR_ID_C                       43

#define CRYPTODAL_AR_RELEASE_MAJOR_VERSION_C        4
#define CRYPTODAL_AR_RELEASE_MINOR_VERSION_C        4
#define CRYPTODAL_AR_RELEASE_REVISION_VERSION_C     0

#define CRYPTODAL_SW_MAJOR_VERSION_C                24
#define CRYPTODAL_SW_MINOR_VERSION_C                12
#define CRYPTODAL_SW_PATCH_VERSION_C                0


/*=================================================================================================
*                                     FILE VERSION CHECKS
===================================================================================================*/

/* Check if source file and CRYPTODAL header file are of the same vendor */
#if (CRYPTODAL_VENDOR_ID_C != CRYPTODAL_VENDOR_ID)
    #error "CryptoDal.c and CryptoDal.h have different vendor ids"
#endif

/* Check if source file and CryptoDal header file are of the same Autosar version */
#if ((CRYPTODAL_AR_RELEASE_MAJOR_VERSION_C != CRYPTODAL_AR_RELEASE_MAJOR_VERSION) || \
     (CRYPTODAL_AR_RELEASE_MINOR_VERSION_C != CRYPTODAL_AR_RELEASE_MINOR_VERSION) || \
     (CRYPTODAL_AR_RELEASE_REVISION_VERSION_C != CRYPTODAL_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of CryptoDal.c and CryptoDal.h are different"
#endif

/* Check if source file and CryptoDal header file are of the same Software version */
#if ((CRYPTODAL_SW_MAJOR_VERSION_C != CRYPTODAL_SW_MAJOR_VERSION) || \
     (CRYPTODAL_SW_MINOR_VERSION_C != CRYPTODAL_SW_MINOR_VERSION) || \
     (CRYPTODAL_SW_PATCH_VERSION_C != CRYPTODAL_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of CryptoDal.c and CryptoDal.h are different"
#endif

/* Check if source file and CryptoDal Types header file are from the same vendor */
#if (CRYPTODAL_VENDOR_ID_C != CRYPTODAL_TYPES_VENDOR_ID)
    #error "CryptoDal.c and CryptoDal_Types.h have different vendor ids"
#endif

/* Check if source file and CryptoDal Types header file are of the same Autosar version */
#if ((CRYPTODAL_AR_RELEASE_MAJOR_VERSION_C != CRYPTODAL_TYPES_AR_RELEASE_MAJOR_VERSION) || \
     (CRYPTODAL_AR_RELEASE_MINOR_VERSION_C != CRYPTODAL_TYPES_AR_RELEASE_MINOR_VERSION) || \
     (CRYPTODAL_AR_RELEASE_REVISION_VERSION_C != CRYPTODAL_TYPES_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of CryptoDal.c and CryptoDal_Types.h are different"
#endif

/* Check if source file and CryptoDal Types headerr file are of the same Software version */
#if ((CRYPTODAL_SW_MAJOR_VERSION_C != CRYPTODAL_TYPES_SW_MAJOR_VERSION) || \
     (CRYPTODAL_SW_MINOR_VERSION_C != CRYPTODAL_TYPES_SW_MINOR_VERSION) || \
     (CRYPTODAL_SW_PATCH_VERSION_C != CRYPTODAL_TYPES_SW_PATCH_VERSION) \
    )
  #error "Software Version Numbers of CryptoDal.c and CryptoDal_Types.h are different"
#endif


 /* Check if source file and DET header file are of the same version */
#if (CRYPTODAL_DEV_ERROR_DETECT == STD_ON)
    #if ((CRYPTODAL_AR_RELEASE_MAJOR_VERSION_C != DET_AR_RELEASE_MAJOR_VERSION) || \
         (CRYPTODAL_AR_RELEASE_MINOR_VERSION_C != DET_AR_RELEASE_MINOR_VERSION) \
        )
    #error "AutoSar Version Numbers of CryptoDal.c and Det.h are different"
    #endif
#endif


/* Check if source file and CryptoDal configuration header file are of the same vendor */
#if (CRYPTODAL_VENDOR_ID_C != CRYPTODAL_CFG_VENDOR_ID)
    #error "CryptoDal.c and CryptoDal_Cfg.h have different vendor ids"
#endif

/* Check if source file and CryptoDal configuration header file are of the same Autosar version */
#if ((CRYPTODAL_AR_RELEASE_MAJOR_VERSION_C != CRYPTODAL_CFG_AR_RELEASE_MAJOR_VERSION) || \
     (CRYPTODAL_AR_RELEASE_MINOR_VERSION_C != CRYPTODAL_CFG_AR_RELEASE_MINOR_VERSION) || \
     (CRYPTODAL_AR_RELEASE_REVISION_VERSION_C != CRYPTODAL_CFG_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of CryptoDal.c and CryptoDal_Cfg.h are different"
#endif

/* Check if source file and CryptoDal configuration header file are of the same software version */
#if ((CRYPTODAL_SW_MAJOR_VERSION_C != CRYPTODAL_CFG_SW_MAJOR_VERSION) || \
     (CRYPTODAL_SW_MINOR_VERSION_C != CRYPTODAL_CFG_SW_MINOR_VERSION) || \
     (CRYPTODAL_SW_PATCH_VERSION_C != CRYPTODAL_CFG_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of CryptoDal.c and CryptoDal_Cfg.h are different"
#endif

/*=================================================================================================
*                                       LOCAL TYPES
===================================================================================================*/

/*=================================================================================================
*                                        LOCAL MACROS
===================================================================================================*/

/* Crypto driver object used */
#define CRYPTODAL_DRIVER_ID 0U

/* Output length in bytes for hashing oprations */
#define CRYPTODAL_MAX_DIGEST_SIZE 64U

/* Maximum allowed message size for encryption/decryption */
#define CRYPTODAL_MAX_SIZE 128U

/* Minimum allowed message size for encryption/decryption */
#define CRYPTODAL_MIN_SIZE 16U

/* Digest size for SHA1 operations, in bytes */
#define CRYPTODAL_SHA1_DIGEST_SIZE 20U

/* Digest size for SHA256 operations, in bytes */
#define CRYPTODAL_SHA256_DIGEST_SIZE 32U

/* Check whether a HSE status is set */
#define CHECK_HSE_STATUS(hseStatus) ((hseStatus) == ((hseStatus) & Hse_Ip_GetHseStatus(CRYPTO_MU_0_U8)))

/* NEEDED for non-secure Signal HSE clock and Flash configuration finished */
#define HSE_SEND_EVENT_CONFIG_DONE() { IP_MU0__MUB->GCR = HSE_HOST_PERIPH_CONFIG_DONE; }

/* Convert bytes length to bits length */
#define BYTES_TO_BITS(byteLen) ((byteLen) * 8U)

/*=================================================================================================
                                       GLOBAL CONSTANTS
===================================================================================================*/
/**
* @brief          Global Configuration Pointer.
* @details        Data structure containing the set of configuration parameters required for
*                 initializing the CRYPTODAL components.
*
*/
P2CONST(CryptoDal_ConfigType, CRYPTODAL_VAR, CRYPTODAL_APP_CONST) CryptoDal_pGlobalConfigPtr = NULL_PTR;

/* Structure containing configuration for encrypt/decrypt/hashing operations */
const Crypto_JobInfoType JobInfo =
{
    0U, /* jobId: The family of the algorithm */
    0U  /* jobPriority: Specifies the importance of the job (the higher, the more important). */
};

/* Structure containing configuration for encrypt operations */
const Crypto_PrimitiveInfoType EncryptPrimitiveInfo =
{
    /* resultLength: Contains the result length in bytes */
    CRYPTODAL_MAX_SIZE,

    /* service: Contains the enum of the used service */
    CRYPTO_ENCRYPT,
    {
        CRYPTO_ALGOFAM_AES,             /* family: The family of the algorithm */
        CRYPTO_ALGOFAM_NOT_SET,         /* secondaryFamily: The secondary family of the algorithm  */
        16U,                            /* keyLength: Key length in bits */
        CRYPTO_ALGOMODE_ECB             /* mode: The operation mode to be used with that algorithm */
    }
};

/* Structure containing configuration for decrypt operations */
const Crypto_PrimitiveInfoType DecryptPrimitiveInfo =
{
    /* resultLength: Contains the result length in bytes */
    CRYPTODAL_MAX_SIZE,

    /* service: Contains the enum of the used service */
    CRYPTO_DECRYPT,
    {
        CRYPTO_ALGOFAM_AES,             /* family: The family of the algorithm */
        CRYPTO_ALGOFAM_NOT_SET,         /* secondaryFamily: The secondary family of the algorithm  */
        16U,                            /* keyLength: Key length in bits */
        CRYPTO_ALGOMODE_ECB             /* mode: The operation mode to be used with that algorithm */
    }
};

/* Structure containing configuration for SHA1 hashing operations */
const Crypto_PrimitiveInfoType HashSha1PrimitiveInfo =
{
    /* resultLength: Contains the result length in bytes */
    CRYPTODAL_SHA1_DIGEST_SIZE,

    /* service: Contains the enum of the used service */
    CRYPTO_HASH,
    {
        CRYPTO_ALGOFAM_SHA1,            /* family: The family of the algorithm */
        CRYPTO_ALGOFAM_NOT_SET,         /* secondaryFamily: The secondary family of the algorithm  */
        0,                              /* keyLength: Key length in bits */
        CRYPTO_ALGOMODE_NOT_SET         /* mode: The operation mode to be used with that algorithm */
    }
};

/* Structure containing configuration for SHA256 hashing operations */
const Crypto_PrimitiveInfoType HashSha256PrimitiveInfo =
{
    /* resultLength: Contains the result length in bytes */
    CRYPTODAL_SHA256_DIGEST_SIZE,

    /* service: Contains the enum of the used service */
    CRYPTO_HASH,
    {
        CRYPTO_ALGOFAM_SHA2_256,         /* family: The family of the algorithm */
        CRYPTO_ALGOFAM_NOT_SET,          /* secondaryFamily: The secondary family of the algorithm  */
        0,                               /* keyLength: Key length in bits */
        CRYPTO_ALGOMODE_NOT_SET          /* mode: The operation mode to be used with that algorithm */
    }
};

const Crypto_PrimitiveInfoType CmacPrimitiveInfo =
{
    /* resultLength: Contains the result length in bytes */
    16U,

    /* service: Contains the enum of the used service */
    CRYPTO_MACGENERATE,
    {
        CRYPTO_ALGOFAM_AES,               /* family: The family of the algorithm */
        CRYPTO_ALGOFAM_NOT_SET,           /* secondaryFamily: The secondary family of the algorithm  */
        16U,                              /* keyLength: Key length in bits */
        CRYPTO_ALGOMODE_CMAC              /* mode: The operation mode to be used with that algorithm */
    }
};

/*=================================================================================================
                                       GLOBAL VARIABLES
===================================================================================================*/

/* HSE XRDC configuration structure */
extern CONST(hseAttrMUConfig_t, CRYPTODAL_VAR) CryptoDal_Crypto_XRDC_Config;

#define CRYPTO_START_SEC_VAR_INIT_8_NO_CACHEABLE
#include "Crypto_MemMap.h"
/* public exponent */
static uint8 rsa2048BootKeyE[] = { 0x00, 0x01, 0x00, 0x01};
#define CRYPTO_STOP_SEC_VAR_INIT_8_NO_CACHEABLE
#include "Crypto_MemMap.h"

/* The following RAM variables need to be put in non-cacheable
 * memory in order to be correctly seen by HSE
 */

#define CRYPTO_START_SEC_VAR_INIT_32_NO_CACHEABLE
#include "Crypto_MemMap.h"
	VAR(uint32, AUTOMATIC) u32SysImgSize;
	VAR(uint32, AUTOMATIC) u32SysImgOffset;
    VAR(uint32, AUTOMATIC) u32HashResultSize;
#define CRYPTO_STOP_SEC_VAR_INIT_32_NO_CACHEABLE
#include "Crypto_MemMap.h"

#define CRYPTO_START_SEC_VAR_INIT_UNSPECIFIED_NO_CACHEABLE
#include "Crypto_MemMap.h"
    VAR(hseKeyInfo_t, AUTOMATIC) keyInfo;
	VAR(hseSrvDescriptor_t, AUTOMATIC) HseSrvDesc;
	VAR(hseSmrEntry_t, AUTOMATIC) SmrEntry;
	VAR(hseCrEntry_t, AUTOMATIC) CoreResetEntry;
	VAR(hseSmrEntryInstallSrv_t, AUTOMATIC) SmrInstallRequest;
	VAR(hseFormatKeyCatalogsSrv_t, AUTOMATIC) FormatKeyCatalogsReq;
    VAR(hseAttrFwVersion_t, AUTOMATIC) HseFwVersion;
#define CRYPTO_STOP_SEC_VAR_INIT_UNSPECIFIED_NO_CACHEABLE
#include "Crypto_MemMap.h"

#define CRYPTO_START_SEC_VAR_INIT_32_NO_CACHEABLE
#include "Crypto_MemMap.h"


#define CRYPTO_STOP_SEC_VAR_INIT_32_NO_CACHEABLE
#include "Crypto_MemMap.h"

static VAR(boolean, AUTOMATIC) bIsCatalogFormated = FALSE;

/* Structure containing further information, which depends on the job and the crypto primitive */
Crypto_JobPrimitiveInfoType JobPrimitiveInfo = { 0, };

/* Crypto job descriptor */
Crypto_JobType CryptoDalJob = { 0, };
Crypto_JobType CryptoDalHashJob = { 0, };

/*=================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===================================================================================================*/
static FUNC(hseSrvResponse_t, CRYPTO_CODE) CryptoDal_HseSrv_Request(
    P2VAR(hseSrvDescriptor_t, AUTOMATIC, CRYPTODAL_APPL_DATA) pHseMsg);
static FUNC(void, CRYPTODAL_APP_CODE) CryptoDal_HseStatus(void);
static FUNC(hseSrvResponse_t, CRYPTODAL_APP_CODE) CryptoDal_GetKeyInfo(
    VAR(hseKeyHandle_t, AUTOMATIC) keyHandle);
static FUNC(hseSrvResponse_t, CRYPTODAL_APP_CODE) CryptoDal_EraseKey(
    VAR(hseKeyHandle_t, AUTOMATIC) keyHandle,
    VAR(hseEraseKeyOptions_t, AUTOMATIC) eraseKeyOptions);
static FUNC(Std_ReturnType, CRYPTODAL_APP_CODE) CryptoDal_InstallAllKeys(void);
static FUNC(Std_ReturnType, CRYPTODAL_APP_CODE) CryptoDal_InstallKey(
    VAR(uint8, AUTOMATIC) u8CryptoKey,
    VAR(uint8, AUTOMATIC) u8CryptoDalKey);
static FUNC(void, CRYPTODAL_APP_CODE) CryptoDal_HSE_Config(void);
static FUNC(void, CRYPTODAL_APP_CODE) CryptoDal_Data_Init(void);
static FUNC(Std_ReturnType, CRYPTODAL_APP_CODE) CryptoDal_FormatKeyCatalogs(void);
static FUNC(Std_ReturnType, CRYPTODAL_APP_CODE) CryptoDal_InstallSmr(
    P2VAR(CryptoDal_SMR_CR_EntryType, AUTOMATIC, CRYPTODAL_APPL_DATA) pSMR_CR_Config,
    VAR(uint8, AUTOMATIC) u8SmrIndex);
static FUNC(Std_ReturnType, CRYPTODAL_APP_CODE) CryptoDal_InstallCr(
    P2VAR(CryptoDal_CR_EntryType, AUTOMATIC, CRYPTODAL_APPL_DATA) pCR_Config);
static FUNC(Std_ReturnType, CRYPTODAL_APP_CODE) CryptoDal_Init_MessagingUnits_Protection(
    P2CONST(hseAttrMUConfig_t, AUTOMATIC, CRYPTODAL_CONST) pXRDC_Config);
static FUNC(Std_ReturnType, CRYPTODAL_APP_CODE) CryptoDal_ImportRsaKey(
    VAR(hseKeyHandle_t, AUTOMATIC) handle,
    VAR(hseKeyType_t, AUTOMATIC) type,
    VAR(hseKeyFlags_t, AUTOMATIC) flags,
    P2CONST(uint8, AUTOMATIC, CRYPTODAL_CONST) pN,
    VAR(uint16, AUTOMATIC) modLen,
    P2CONST(uint8, AUTOMATIC, CRYPTODAL_CONST) pE,
    VAR(uint16, AUTOMATIC) eLen,
    P2CONST(uint8, AUTOMATIC, CRYPTODAL_CONST) pD);

/*=================================================================================================
*                                       LOCAL FUNCTIONS
===================================================================================================*/

/*=================================================================================================
*                                       GLOBAL FUNCTIONS
===================================================================================================*/

/*=================================================================================================*/
/**
@brief      CryptoDal_Init - Initialization function for Communications Driver abstraction module
@details    Shall be used to initialize all subcomponents

@param[in]  ConfigPtr    Pointer to configuration data (this data is shared between all CryptoDal components)

@return     None
*/
/*=================================================================================================*/
FUNC(void, CRYPTODAL_APP_CODE) CryptoDal_Init(P2CONST(CryptoDal_ConfigType, AUTOMATIC, CRYPTODAL_APP_CONST) ConfigPtr)
{
    HSE_SEND_EVENT_CONFIG_DONE();

    /* Wait for HSE to initialize (along with RNG module) by reading the status bits in FSR */
    while(!CHECK_HSE_STATUS(HSE_STATUS_INIT_OK | HSE_STATUS_RNG_INIT_OK));

    if(NULL_PTR != ConfigPtr)
    {
        /* Initialize hardware module */
        Crypto_Init(NULL_PTR);

        CryptoDal_pGlobalConfigPtr = ConfigPtr;

        /* Check whether HSE is configured (SYS_IMG is present) */
        if(!CHECK_HSE_STATUS(HSE_STATUS_INSTALL_OK))
        {
            CryptoDal_HSE_Config();
        }

        if(E_NOT_OK == CryptoDal_InstallAllKeys())
        {
            while(1);
        }

        CryptoDal_Data_Init();
    }
#if(CRYPTODAL_DEV_ERROR_DETECT == STD_ON)
    else
    {
        Det_ReportError((uint16)CRYPTODAL_MODULE_ID, (uint8)0U, CRYPTODAL_INIT_API_ID, CRYPTODAL_E_PARAM_POINTER);
    }
#endif /* (CRYPTODAL_DEV_ERROR_DETECT == STD_ON) */
}

/*=================================================================================================*/
/**
@brief      Configuration of HSE services
@details    Configure NVM attributes, key catalogs, install NVM keys

@return     None
*/
/*=================================================================================================*/
static FUNC(void, CRYPTODAL_APP_CODE) CryptoDal_HSE_Config(void)
{
    Std_ReturnType status;

#if (CRYPTODAL_ENABLE_HSE_MEMORY_PROTECTION == STD_ON)
    status = CryptoDal_Init_MessagingUnits_Protection(&CryptoDal_Crypto_XRDC_Config);
    if(status == E_NOT_OK)
    {
        while(1);
    }
#endif /* (CRYPTODAL_ENABLE_HSE_MEMORY_PROTECTION == STD_ON) */

    status = CryptoDal_FormatKeyCatalogs();
    if(status == E_NOT_OK)
    {
        while(1);
    }
}

/*=================================================================================================*/
/**
@brief      CryptoDal data initialization
@details    This function configure the init values for different ComDal internal variables

@return     None
*/
/*=================================================================================================*/
static FUNC(void, CRYPTODAL_APP_CODE) CryptoDal_Data_Init(void)
{
    /* Job used for hasing operations.
       The jobState variable is used as an internal state machine switch */
    MemLib_MemSet(&CryptoDalHashJob, 0, sizeof(Crypto_JobType));
    CryptoDalHashJob.jobId            = 1;
    CryptoDalHashJob.jobState         = CRYPTO_JOBSTATE_IDLE;
    CryptoDalHashJob.jobInfo          = &JobInfo;
    CryptoDalHashJob.jobPrimitiveInfo = &JobPrimitiveInfo;

    /* Job used for encryption and CMAC generation */
    MemLib_MemSet(&CryptoDalJob, 0, sizeof(Crypto_JobType));
    CryptoDalJob.jobId                        = 1;
    CryptoDalJob.jobState                     = CRYPTO_JOBSTATE_IDLE;
    CryptoDalJob.jobPrimitiveInputOutput.mode = CRYPTO_OPERATIONMODE_SINGLECALL;
    CryptoDalJob.jobInfo                      = &JobInfo;
    CryptoDalJob.jobPrimitiveInfo             = &JobPrimitiveInfo;
}

#if (CRYPTODAL_ENABLE_HSE_MEMORY_PROTECTION == STD_ON)
/*=================================================================================================*/
/**
@brief     Initialization function for CryptoDal XRDC memory protection component.
@details   This function uses the generated CryptoDal XRDC configuration and initialize each of the available messaging units.
           The input structure contains the following fields, per each MU: {MU_ENABLED/DISABLED, GLOBAL_XRDC_DOMAIN, MEMORY_SIZE}. 
           The following restrictions must be enforced: HSE MU0 can't be disabled, memory size can't be bigger than 4096, 
           GLOBAL_XRDC_DOMAIN domain referenced must be enabled in the Platform XRDC component. 

@pre       Platform XRDC must be enabled and configured

@param[in] pXRDC_Config pointer to structure used to configure memory protection units.

@return    None
*/
/*=================================================================================================*/
static FUNC(Std_ReturnType, CRYPTODAL_APP_CODE) CryptoDal_Init_MessagingUnits_Protection(
    P2CONST(hseAttrMUConfig_t, AUTOMATIC, CRYPTODAL_CONST) pXRDC_Config)
{
    VAR(Std_ReturnType, CRYPTODAL_VAR) eRetVal = E_NOT_OK;

    MemLib_MemSet(&HseSrvDesc, 0, sizeof(hseSrvDescriptor_t));

    /* Create the request to HSE using service descriptor */
    HseSrvDesc.srvId                     = HSE_SRV_ID_SET_ATTR;
    HseSrvDesc.hseSrv.setAttrReq.attrId  = HSE_MU_CONFIG_ATTR_ID;
    HseSrvDesc.hseSrv.setAttrReq.attrLen = sizeof(hseAttrMUConfig_t);
    HseSrvDesc.hseSrv.setAttrReq.pAttr   = HSE_PTR_TO_HOST_ADDR(pXRDC_Config);

    if(HSE_SRV_RSP_OK == CryptoDal_HseSrv_Request(&HseSrvDesc))
    {
        eRetVal = E_OK;
    }

    return eRetVal;
}
#endif /* (CRYPTODAL_ENABLE_HSE_MEMORY_PROTECTION == STD_ON) */

/*=================================================================================================*/
/**
@brief      Format the HSE key catalogs
@details    First step in configuring the keys

@return     None
*/
/*=================================================================================================*/
static FUNC(Std_ReturnType, CRYPTODAL_APP_CODE) CryptoDal_FormatKeyCatalogs(void)
{
    Std_ReturnType eRetVal = E_OK;

    MemLib_MemSet(&FormatKeyCatalogsReq, 0, sizeof(hseFormatKeyCatalogsSrv_t));
    FormatKeyCatalogsReq.pNvmKeyCatalogCfg = HSE_PTR_TO_HOST_ADDR(aHseNvmKeyCatalog);
    FormatKeyCatalogsReq.pRamKeyCatalogCfg = HSE_PTR_TO_HOST_ADDR(aHseRamKeyCatalog);

    MemLib_MemSet(&HseSrvDesc, 0, sizeof(hseSrvDescriptor_t));
    HseSrvDesc.srvId                       = HSE_SRV_ID_FORMAT_KEY_CATALOGS;
    HseSrvDesc.hseSrv.formatKeyCatalogsReq = FormatKeyCatalogsReq;

    if(HSE_SRV_RSP_OK != CryptoDal_HseSrv_Request(&HseSrvDesc))
    {
        eRetVal = E_NOT_OK;
    }
    else
    {
        bIsCatalogFormated = TRUE;
    }

    return eRetVal;
}

/*=================================================================================================*/
/**
* @brief            Sends a request to the HSE
* @details          Get a free MU channel and send the request to the HSE
*
* @param[in]        pHseMsg    Pointer to the descriptor containing the HSE request
* @return           Result of the operation
*/
/*=================================================================================================*/
static FUNC(hseSrvResponse_t, CRYPTO_CODE) CryptoDal_HseSrv_Request(
    P2VAR(hseSrvDescriptor_t, AUTOMATIC, CRYPTODAL_APPL_DATA) pHseMsg)
{
    VAR  (uint8,            AUTOMATIC) u8MuChannel  = HSE_IP_INVALID_MU_CHANNEL_U8;
    VAR  (uint8,            AUTOMATIC) u8MuInstance = CRYPTO_MU_0_U8;
    VAR  (hseSrvResponse_t, AUTOMATIC) RetVal       = (hseSrvResponse_t)HSE_SRV_RSP_OK;
    VAR  (Hse_Ip_ReqType,   AUTOMATIC) Hse_Ip_Req   = {HSE_IP_REQTYPE_SYNC, NULL_PTR, NULL_PTR, CRYPTO_TIMEOUT_DURATION_U32};

    u8MuChannel = Hse_Ip_GetFreeChannel(u8MuInstance);
    if (HSE_IP_INVALID_MU_CHANNEL_U8 != u8MuChannel)
    {
        RetVal = Hse_Ip_ServiceRequest(u8MuInstance, u8MuChannel, &Hse_Ip_Req, pHseMsg);
    }
    else
    {
        RetVal = (hseSrvResponse_t)HSE_SRV_RSP_NOT_ENOUGH_SPACE;
    }
    Hse_Ip_ReleaseChannel(u8MuInstance, u8MuChannel);
    return RetVal;
}

/*=================================================================================================*/
/**
* @brief            Get the Crypto key handle
* @details          Key handle retrival is done based on the CryptoDal key index
*
* @param[in]        u8CryptoDalKey  ID of the CryptoDal key
* @param[in]        u8Type          CryptoDal key type: primary or secondary
* @return           hseKeyHandle_t  Crypto key handle
*/
/*=================================================================================================*/
FUNC(hseKeyHandle_t, CRYPTODAL_APP_CODE) CryptoDal_GetKeyHandle(
    VAR(uint8, AUTOMATIC) u8CryptoDalIdx,
    VAR(uint8, AUTOMATIC) u8Type)
{
    uint32 u32KeyHandle = HSE_INVALID_KEY_HANDLE;
    uint8 u8CryptoKeyID;
    uint8 u8KeyElemIdx;

    if (u8Type == CRYPTODAL_PRIMARY_KEY)
    {
        u8CryptoKeyID = CryptoDal_Config[u8CryptoDalIdx].u8EncryptionKeyInternalRef;
        u8KeyElemIdx = Crypto_aKeyList[u8CryptoKeyID].pCryptoKeyElementList[0];
        u32KeyHandle = Crypto_aKeyElementList[u8KeyElemIdx].u32HseKeyHandle;
    }
    else if (u8Type == CRYPTODAL_SECONDARY_KEY)
    {
        u8CryptoKeyID = CryptoDal_Config[u8CryptoDalIdx].u8EncryptionKeyAlternateRef;
        u8KeyElemIdx = Crypto_aKeyList[u8CryptoKeyID].pCryptoKeyElementList[0];
        u32KeyHandle = Crypto_aKeyElementList[u8KeyElemIdx].u32HseKeyHandle;
    }

    return u32KeyHandle;
}

/*=================================================================================================*/
/**
@brief      Get info about a Crypto key
@details    This function checks if the key slot is used or empty

@param[in]  keyHandle - Crypto key handle to be checked

@retval     HSE request response
*/
/*=================================================================================================*/
static FUNC(hseSrvResponse_t, CRYPTODAL_APP_CODE) CryptoDal_GetKeyInfo(
    VAR(hseKeyHandle_t, AUTOMATIC) keyHandle)
{
    hseKeyInfo_t reqKeyInfo = {0, };

    /* Clear the service descriptor placed in shared memory */
    MemLib_MemSet(&HseSrvDesc, 0, sizeof(hseSrvDescriptor_t));

    /* Fill the service descriptor */
    HseSrvDesc.srvId = HSE_SRV_ID_GET_KEY_INFO;
    HseSrvDesc.hseSrv.getKeyInfoReq.keyHandle = keyHandle;
    HseSrvDesc.hseSrv.getKeyInfoReq.pKeyInfo = HSE_PTR_TO_HOST_ADDR(&reqKeyInfo);

    /* Send the request synchronously */
    return CryptoDal_HseSrv_Request(&HseSrvDesc);
}

/*=================================================================================================*/
/**
@brief      Erase a Crypto key from HSE catalogs

@param[in]  keyHandle - Crypto key handle to be erased
@param[in]  eraseKeyOptions - Erase options

@retval     HSE request response
*/
/*=================================================================================================*/
static FUNC(hseSrvResponse_t, CRYPTODAL_APP_CODE) CryptoDal_EraseKey(
    VAR(hseKeyHandle_t, AUTOMATIC) keyHandle,
    VAR(hseEraseKeyOptions_t, AUTOMATIC) eraseKeyOptions)
{
    /* Clear the service descriptor placed in shared memory */
    MemLib_MemSet(&HseSrvDesc, 0, sizeof(hseSrvDescriptor_t));

    /* Fill the service descriptor */
    HseSrvDesc.srvId = HSE_SRV_ID_ERASE_KEY;
    HseSrvDesc.hseSrv.eraseKeyReq.keyHandle = keyHandle;
    HseSrvDesc.hseSrv.eraseKeyReq.eraseKeyOptions = eraseKeyOptions;

    /* Send the request synchronously */
    return CryptoDal_HseSrv_Request(&HseSrvDesc);
}

/*=================================================================================================*/
/**
@brief      Install all the keys configured in CryptoDal driver
@details    This oferation has to be done only once when the Crypto driver is initialized

@retval     E_OK - all keys were installed successfully
            E_NOT_OK - an error occurred
*/
/*=================================================================================================*/
static FUNC(Std_ReturnType, CRYPTODAL_APP_CODE) CryptoDal_InstallAllKeys(void)
{
    Std_ReturnType eRetCode = E_OK;
    uint8 u8CryptoKey = 0u;
    uint8 u8Idx = 0u;

    for (u8Idx = 0; u8Idx < CRYPTODAL_MAX_CIPHERS; u8Idx++)
    {
        /* Fetch primary CryptoDal key */
        u8CryptoKey = CryptoDal_pGlobalConfigPtr[u8Idx].u8EncryptionKeyInternalRef;
        if (u8CryptoKey != CRYPTODAL_INVALID_KEY)
        {
            eRetCode = CryptoDal_InstallKey(u8CryptoKey, u8Idx);
            if (eRetCode == E_NOT_OK)
            {
                break;
            }
        }

        /* Fetch secondary CryptoDal key */
        u8CryptoKey = CryptoDal_pGlobalConfigPtr[u8Idx].u8EncryptionKeyAlternateRef;
        if (u8CryptoKey != CRYPTODAL_INVALID_KEY)
        {
            eRetCode = CryptoDal_InstallKey(u8CryptoKey, u8Idx);
            if (eRetCode == E_NOT_OK)
            {
                break;
            }
        }
    }

    return eRetCode;
}

/*=================================================================================================*/
/**
@brief      Install a single Crypto key
@details    The key type and catalog status will be checked ahead.
            The RAM keys will always be installed regardless of their catalog status.
            The NVM keys will be installed only if the catalog was formated in the CryptoDal initialization phase.

@param[in]  u8CryptoKey - id of the Crypto key
@param[in]  u8CryptoDalKey - id of the CryptoDal key

@retval     E_OK - key installed successfully or skipped
            E_NOT_OK - errors during key installation
*/
/*=================================================================================================*/
static FUNC(Std_ReturnType, CRYPTODAL_APP_CODE) CryptoDal_InstallKey(
    VAR(uint8, AUTOMATIC) u8CryptoKey,
    VAR(uint8, AUTOMATIC) u8CryptoDalKey)
{
    Std_ReturnType Status = E_OK;
    uint32 u32KeyElementIdx = Crypto_aKeyList[u8CryptoKey].pCryptoKeyElementList[0];
    uint32 u32KeyHandle = Crypto_aKeyElementList[u32KeyElementIdx].u32HseKeyHandle;
    hseKeyCatalogId_t KeyCatalog = GET_CATALOG_ID(u32KeyHandle);

    if (!bIsCatalogFormated && (KeyCatalog == HSE_KEY_CATALOG_ID_NVM))
    {
        /* Skip NVM keys installation if catalog was not formated */
    }
    else
    {
        if(CRYPTODAL_RSA_2048_PKCS == CryptoDal_pGlobalConfigPtr[u8CryptoDalKey].eEncryptionAlgorithm)
        {
            Status = CryptoDal_ImportRsaKey(u32KeyHandle, HSE_KEY_TYPE_RSA_PUB, HSE_KF_USAGE_VERIFY,
                CryptoDal_pGlobalConfigPtr[u8CryptoDalKey].u8CryptoKey,
                CryptoDal_pGlobalConfigPtr[u8CryptoDalKey].u16EncryptionKeyLen,
                rsa2048BootKeyE, sizeof(rsa2048BootKeyE)/sizeof(rsa2048BootKeyE[0]), NULL_PTR);
        }
        else{
            Status = Crypto_KeyElementSet(
                u8CryptoKey, CRYPTO_KEY_MATERIAL_U32,
                CryptoDal_pGlobalConfigPtr[u8CryptoDalKey].u8CryptoKey,
                CryptoDal_pGlobalConfigPtr[u8CryptoDalKey].u16EncryptionKeyLen);
        }
    }

    return Status;
}

/*=================================================================================================*/
/**
@brief      Install a RSA Crypto key
@details    The RSA key may be compound of public exponent E and modulus N (for public keys)
            or of a private exponent D and modulus N (for private keys)

@param[in]  handle - Crypto key handle where this key is installed
@param[in]  type - Key type as public or private
@param[in]  flags - Key usage flags
@param[in]  pN - Modulus N
@param[in]  modLen - Length of modulus N / exponent D
@param[in]  pE - Public exponent E
@param[in]  eLen - Length of exponent E
@param[in]  pD - Private exponent D

@retval     E_OK - key installed successfully or skipped
            E_NOT_OK - errors during key installation
*/
/*=================================================================================================*/
static FUNC(Std_ReturnType, CRYPTODAL_APP_CODE) CryptoDal_ImportRsaKey(
    VAR(hseKeyHandle_t, AUTOMATIC) handle,
    VAR(hseKeyType_t, AUTOMATIC) type,
    VAR(hseKeyFlags_t, AUTOMATIC) flags,
    P2CONST(uint8, AUTOMATIC, CRYPTODAL_CONST) pN,
    VAR(uint16, AUTOMATIC) modLen,
    P2CONST(uint8, AUTOMATIC, CRYPTODAL_CONST) pE,
    VAR(uint16, AUTOMATIC) eLen,
    P2CONST(uint8, AUTOMATIC, CRYPTODAL_CONST) pD)
{
    Std_ReturnType eRetVal = E_OK;
    hseImportKeySrv_t *pImportKeyReq;
    hseSrvResponse_t srvResponse = HSE_SRV_RSP_GENERAL_ERROR;

    srvResponse = CryptoDal_GetKeyInfo(handle);
    if(HSE_SRV_RSP_OK == srvResponse)
    {
        /* NVM slot is already populated. Erase the key to enable import in plain */
        if(HSE_SRV_RSP_OK != CryptoDal_EraseKey(handle, HSE_ERASE_NOT_USED))
        {
            eRetVal = E_NOT_OK;
        }
    }
    else if(HSE_SRV_RSP_KEY_EMPTY != srvResponse)
    {
        eRetVal = E_NOT_OK;
    }


    /* pD may be NULL (when importing an RSA_PUB type) */
    uint16 dLen = (NULL != pD) ? modLen : 0U;

    /* Declare the information about the key to be imported */
    MemLib_MemSet(&keyInfo, 0, sizeof(hseKeyInfo_t));
    keyInfo.keyType     = type;                    /* Will import a RSA key (public / pair) */
    keyInfo.keyFlags    = flags;                   /* Usage flags for this key (e.g. SIGN/VERIFY/PROVISION/EXCHANGE) */
    keyInfo.keyBitLen   = BYTES_TO_BITS(modLen);  /* Length of modulus in bits */
    keyInfo.keyCounter  = 0UL;                     /* Relevant only for NVM keys - must be > than previous */
    keyInfo.smrFlags    = 0UL;                     /* Not used here - default value */
    keyInfo.specific.pubExponentSize = eLen;       /* Specific for RSA key - size of public exponent */

    /* Clear the service descriptor placed in shared memory */
    MemLib_MemSet(&HseSrvDesc, 0, sizeof(hseSrvDescriptor_t));
    pImportKeyReq = &(HseSrvDesc.hseSrv.importKeyReq);

    /* Fill the service descriptor */
    HseSrvDesc.srvId                = HSE_SRV_ID_IMPORT_KEY;
    pImportKeyReq->targetKeyHandle  = handle;
    pImportKeyReq->pKeyInfo         = HSE_PTR_TO_HOST_ADDR(&keyInfo);
    pImportKeyReq->pKey[0]          = HSE_PTR_TO_HOST_ADDR(pN);
    pImportKeyReq->keyLen[0]        = modLen;
    pImportKeyReq->pKey[1]          = HSE_PTR_TO_HOST_ADDR(pE);
    pImportKeyReq->keyLen[1]        = eLen;
    pImportKeyReq->pKey[2]          = HSE_PTR_TO_HOST_ADDR(pD);
    pImportKeyReq->keyLen[2]        = dLen;

    /* Key imported in plain - not encrypted => cipherKeyHandle = HSE_INVALID_KEY_HANDLE */
    pImportKeyReq->cipher.cipherKeyHandle   = HSE_INVALID_KEY_HANDLE;
    /* Key imported without authentication - not signed => authKeyHandle = HSE_INVALID_KEY_HANDLE */
    pImportKeyReq->keyContainer.authKeyHandle = HSE_INVALID_KEY_HANDLE;

    if (eRetVal == E_OK)
    {
        if(HSE_SRV_RSP_OK != CryptoDal_HseSrv_Request(&HseSrvDesc))
        {
            eRetVal = E_NOT_OK;
        }
    }

    return eRetVal;
}

/*=================================================================================================*/
/**
@brief      Function used for encryption of data received as plaintext.
@details    This function shall be called if user wants to encrypt messages using the Crypto module.
            This function shall pad the message received in plain text if the buffer is not a multiple of 128 bits.

@param[in]  pText              Pointer to encrypted data which needs to be encrypted
@param[in]  pCipherText        Pointer to encrypted data which needs to be decoded
@param[in]  u8KeyIndex         Index of associated crypto channel, containing information about encryption engine and algorithm
@param[in]  u32MessageSize     Size of cypher text received from upper layers
@param[in]  pResultSize        Size of encrypted message

@retval     E_OK if operation is successful, E_NOT_OK otherwise
*/
/*=================================================================================================*/
FUNC(Std_ReturnType, CRYPTODAL_APP_CODE) CryptoDal_Encrypt(
    P2VAR(uint8, AUTOMATIC, CRYPTODAL_APPL_CONST) pText,
    P2VAR(uint8, AUTOMATIC, CRYPTODAL_APPL_CONST) pCipherText,
    VAR(uint8, AUTOMATIC) u8KeyIndex,
    VAR(uint32, AUTOMATIC) u32TextSize,
    P2VAR(uint32, AUTOMATIC, CRYPTODAL_APPL_CONST) pResultSize)
{
    Std_ReturnType RetVal = E_NOT_OK;
    VAR(uint8, AUTOMATIC) u8CryptoKeyIndex;
    VAR(CryptoDal_AlgorithmType, AUTOMATIC) eAlgorithm;

    if(CryptoDal_pGlobalConfigPtr == NULL_PTR)
    {
#if(CRYPTODAL_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError((uint16)CRYPTODAL_MODULE_ID, (uint8)0U, CRYPTODAL_ENCRYPT_API_ID, CRYPTODAL_E_UNINIT);
#endif /* (CRYPTODAL_DEV_ERROR_DETECT == STD_ON) */
    }
    else if (u32TextSize < CRYPTODAL_MIN_SIZE)
    {
#if(CRYPTODAL_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError((uint16)CRYPTODAL_MODULE_ID, (uint8)0U, CRYPTODAL_ENCRYPT_API_ID, CRYPTODAL_E_PARAM_CONFIG);
#endif /* (CRYPTODAL_DEV_ERROR_DETECT == STD_ON) */
    }
    else
    {
        /* Fetch desired encryption algorithm */
        eAlgorithm = CryptoDal_pGlobalConfigPtr[u8KeyIndex].eEncryptionAlgorithm;

        /* Fetch internal encryption engine key reference */
        u8CryptoKeyIndex = CryptoDal_pGlobalConfigPtr[u8KeyIndex].u8EncryptionKeyInternalRef;

        /* Mark the key as valid */
        RetVal = Crypto_KeySetValid(u8CryptoKeyIndex);

        if(E_OK == RetVal)
        {
            MemLib_MemSet(&JobPrimitiveInfo, 0, sizeof(Crypto_JobPrimitiveInfoType));

            switch(eAlgorithm)
            {
                case CRYPTODAL_AES_128_ECB:
                {
                    /* Prepare information in the job to be sent to Crypto driver */
                    JobPrimitiveInfo.primitiveInfo                   = &EncryptPrimitiveInfo;
                    JobPrimitiveInfo.processingType                  = CRYPTO_PROCESSING_SYNC;
                    JobPrimitiveInfo.cryIfKeyId                      = u8CryptoKeyIndex;

                    CryptoDalJob.jobPrimitiveInputOutput.inputPtr    = pText;
                    CryptoDalJob.jobPrimitiveInputOutput.inputLength = u32TextSize;
                    CryptoDalJob.jobPrimitiveInputOutput.outputPtr   = pCipherText;
                    CryptoDalJob.jobPrimitiveInputOutput.outputLengthPtr = pResultSize;

                    RetVal = Crypto_ProcessJob(CRYPTODAL_DRIVER_ID, &CryptoDalJob);

                } break;

                case CRYPTODAL_AES_128_CBC:
                    /* TODO */
                default:
                    break;

            }
        }
    }

    return RetVal;
}


/*=================================================================================================*/
/**
@brief      Function used for decryption of received messages
@details    Shall be used to decrypt messages received via communication instances.
            Any cyphered message should have a crypto channel
            associated, containing a cryptographic key and an encryption engine.

@param[in]  pCipherText      Pointer to message data received from upper layer
@param[in]  u8KeyIndex       Index of associated CryptoDal channel
@param[in]  u32MessageSize   Size of received data
@param[in]  pResultSize      Size of data after decryption

@retval     E_OK if operation is successful, E_NOT_OK otherwise
*/
/*=================================================================================================*/
FUNC(Std_ReturnType, CRYPTODAL_APP_CODE) CryptoDal_Decrypt(
    P2VAR(uint8, AUTOMATIC, CRYPTODAL_APPL_CONST) pCipherText,
    P2VAR(uint8, AUTOMATIC, CRYPTODAL_APPL_CONST) pText,
    VAR(uint8, AUTOMATIC) u8KeyIndex,
    VAR(uint32, AUTOMATIC) u32CipherTextSize,
    P2VAR(uint32, AUTOMATIC, CRYPTODAL_APPL_CONST) pResultSize)
{
    Std_ReturnType RetVal = E_NOT_OK;
    VAR(uint8, AUTOMATIC) u8CryptoKeyIndex;
    VAR(CryptoDal_AlgorithmType, AUTOMATIC) eAlgorithm;

    if(CryptoDal_pGlobalConfigPtr == NULL_PTR)
    {
#if(CRYPTODAL_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError((uint16)CRYPTODAL_MODULE_ID, (uint8)0U, CRYPTODAL_DECRYPT_API_ID, CRYPTODAL_E_UNINIT);
#endif /* (CRYPTODAL_DEV_ERROR_DETECT == STD_ON) */
    }
    else if (u32CipherTextSize < CRYPTODAL_MIN_SIZE)
    {
#if(CRYPTODAL_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError((uint16)CRYPTODAL_MODULE_ID, (uint8)0U, CRYPTODAL_DECRYPT_API_ID, CRYPTODAL_E_PARAM_CONFIG);
#endif /* (CRYPTODAL_DEV_ERROR_DETECT == STD_ON) */
    }
    else
    {
        /* Fetch desired decryption algorithm */
        eAlgorithm = CryptoDal_pGlobalConfigPtr[u8KeyIndex].eEncryptionAlgorithm;

        /* Fetch internal decryption engine key reference */
        u8CryptoKeyIndex = CryptoDal_pGlobalConfigPtr[u8KeyIndex].u8EncryptionKeyInternalRef;

        /* Mark the key as valid */
        RetVal = Crypto_KeySetValid(u8CryptoKeyIndex);

        if(E_OK == RetVal)
        {
            MemLib_MemSet(&JobPrimitiveInfo, 0, sizeof(Crypto_JobPrimitiveInfoType));

            switch(eAlgorithm)
            {
                case CRYPTODAL_AES_128_ECB:
                {
                    /* Prepare information in the job to be sent to Crypto driver */
                    JobPrimitiveInfo.primitiveInfo                   = &DecryptPrimitiveInfo;
                    JobPrimitiveInfo.processingType                  = CRYPTO_PROCESSING_SYNC;
                    JobPrimitiveInfo.cryIfKeyId                      = u8CryptoKeyIndex;

                    CryptoDalJob.jobPrimitiveInputOutput.inputPtr    = pCipherText;
                    CryptoDalJob.jobPrimitiveInputOutput.inputLength = u32CipherTextSize;
                    CryptoDalJob.jobPrimitiveInputOutput.outputPtr   = pText;
                    CryptoDalJob.jobPrimitiveInputOutput.outputLengthPtr = pResultSize;

                    RetVal = Crypto_ProcessJob(CRYPTODAL_DRIVER_ID, &CryptoDalJob);

                } break;

                case CRYPTODAL_AES_128_CBC:
                    /* TODO */
                default:
                    break;

            }
        }
    }

    return RetVal;
}


#if (CRYPTODAL_SUPPORT_HASHING == STD_ON)
/*=================================================================================================*/
/**
@brief      Function used for hashing operations on data inputs
@details    Shall be used to Hash data buffers of any size. If the buffer is split, the function
            shall be called in this exact order: first with START for CryptoDal_AlgorithmMode,
            for the next iterations with UPDATE, and for the last call with FINISH.

@param[in]  pHashData           Pointer where the result shall be stored
@param[in]  pInputData          Pointer to the input data which needs to be hashed
@param[in]  u32InputDataSize    Size of the input buffer
@param[in]  eHashAlgorithm      Used algorithm: e.g: SHA1/SHA256
@param[in]  eAlgorithmMode      Depending on user's implementation, the function can be called in
                                one-shot or streaming mode (START/UPDATE/FINISH)

@return     Size of Hash
*/
/*=================================================================================================*/
FUNC(Std_ReturnType, CRYPTODAL_APP_CODE) CryptoDal_Hash(
    P2VAR(uint8, AUTOMATIC, CRYPTODAL_APPL_CONST) pHashData,
    P2VAR(uint8, AUTOMATIC, CRYPTODAL_APPL_CONST) pInputData,
    VAR(uint32, AUTOMATIC) u32InputDataSize,
    VAR(CryptoDal_HashAlgorithmType, AUTOMATIC) eHashAlgorithm,
    VAR(Crypto_OperationModeType, AUTOMATIC) eAlgorithmMode)
{
    VAR(uint8, AUTOMATIC) u8HashSize = 0;
    VAR(Std_ReturnType, AUTOMATIC) eRetVal = E_OK;

    MemLib_MemSet(&JobPrimitiveInfo, 0, sizeof(Crypto_JobPrimitiveInfoType));

    u32HashResultSize = CRYPTODAL_MAX_DIGEST_SIZE;

    CryptoDalHashJob.jobPrimitiveInputOutput.inputPtr        = pInputData;
    CryptoDalHashJob.jobPrimitiveInputOutput.inputLength     = u32InputDataSize;
    CryptoDalHashJob.jobPrimitiveInputOutput.outputPtr       = pHashData;
    CryptoDalHashJob.jobPrimitiveInputOutput.outputLengthPtr = &u32HashResultSize;
    CryptoDalHashJob.jobPrimitiveInputOutput.mode            = eAlgorithmMode;

    switch(eHashAlgorithm)
    {
        case CRYPTODAL_SHA1:
        {
            /* Prepare information in the job to be sent to Crypto driver */
            u8HashSize = CRYPTODAL_SHA1_DIGEST_SIZE;
            JobPrimitiveInfo.processingType = CRYPTO_PROCESSING_SYNC;
            JobPrimitiveInfo.primitiveInfo = &HashSha1PrimitiveInfo;

            eRetVal = Crypto_ProcessJob(CRYPTODAL_DRIVER_ID, &CryptoDalHashJob);
            if(E_OK != eRetVal)
            {
                u8HashSize = 0;
            }
        } break;

        case CRYPTODAL_SHA256:
        {
            u8HashSize = CRYPTODAL_SHA256_DIGEST_SIZE;
            JobPrimitiveInfo.processingType = CRYPTO_PROCESSING_SYNC;
            JobPrimitiveInfo.primitiveInfo = &HashSha256PrimitiveInfo;

            eRetVal = Crypto_ProcessJob(CRYPTODAL_DRIVER_ID, &CryptoDalHashJob);
            if(E_OK != eRetVal)
            {
                u8HashSize = 0;
            }
        } break;

        default:
        {
            u8HashSize = 0;
            break;
        }
    }

    return u8HashSize;
}
#endif /* CRYPTODAL_SUPPORT_HASHING == STD_ON) */


#if (CRYPTODAL_SUPPORT_CMAC == STD_ON)
/*=================================================================================================*/
/**
@brief      Function used for Message Authentication tags generation
@details    Shall be used to generate Cipher-based Message Authentication Code for data inputs

@param[in]  pInputData        Pointer to the input data which needs to be authenticated
@param[in]  pTag              Pointer to the output result buffer
@param[in]  u8KeyIndex        Index of associated CryptoDal channel
@param[in]  u32InputDataSize  Input data size, in bytes
@param[in]  pTagSize          Pointer where the final size of the tag is stored

@retval     E_OK if operation is successful, E_NOT_OK otherwise
*/
/*=================================================================================================*/
FUNC(Std_ReturnType, CRYPTODAL_APP_CODE) CryptoDal_GenerateCmac(
    P2VAR(uint8, AUTOMATIC, CRYPTODAL_CONST) pInputData,
    P2VAR(uint8, AUTOMATIC, CRYPTODAL_CONST) pTag,
    VAR(uint8, AUTOMATIC)  u8KeyIndex,
    VAR(uint32, AUTOMATIC) u32InputDataSize,
    P2VAR(uint32, AUTOMATIC, CRYPTODAL_CONST) pTagSize)
{
    VAR(uint8, AUTOMATIC) u8CryptoKeyIndex;
    VAR(CryptoDal_AlgorithmType, AUTOMATIC) eAlgorithm;
    VAR(Std_ReturnType, AUTOMATIC) eRetVal = E_NOT_OK;

    if (NULL_PTR != CryptoDal_pGlobalConfigPtr)
    {
        /* Fetch desired decryption algorithm */
        eAlgorithm = CryptoDal_pGlobalConfigPtr[u8KeyIndex].eEncryptionAlgorithm;

        /* Fetch internal decryption engine key reference */
        u8CryptoKeyIndex = CryptoDal_pGlobalConfigPtr[u8KeyIndex].u8EncryptionKeyInternalRef;

        eRetVal = Crypto_KeySetValid(u8CryptoKeyIndex);

        if (E_OK == eRetVal)
        {
            if (CRYPTODAL_AES_256_CMAC == eAlgorithm)
            {
                MemLib_MemSet(&JobPrimitiveInfo, 0, sizeof(Crypto_JobPrimitiveInfoType));

                JobPrimitiveInfo.primitiveInfo                   = &CmacPrimitiveInfo;
                JobPrimitiveInfo.processingType                  = CRYPTO_PROCESSING_SYNC;
                JobPrimitiveInfo.cryIfKeyId                      = u8CryptoKeyIndex;

                CryptoDalJob.jobPrimitiveInputOutput.inputPtr    = pInputData;
                CryptoDalJob.jobPrimitiveInputOutput.inputLength = u32InputDataSize;
                CryptoDalJob.jobPrimitiveInputOutput.outputPtr   = pTag;
                CryptoDalJob.jobPrimitiveInputOutput.outputLengthPtr = pTagSize;
                eRetVal = Crypto_ProcessJob(CRYPTODAL_DRIVER_ID, &CryptoDalJob);
            }
        }
    }

    return eRetVal;
}
#endif /* (CRYPTODAL_SUPPORT_CMAC == STD_ON) */


#if (CRYPTODAL_SUPPORT_SMR == STD_ON)
/*=================================================================================================*/
/**
@brief      Verifies an installed SMR Entry
@details    Loads(copies to RAM) and verifies on demand an already installed SMR entry
            The SMR(s) used in CORE RESET table can be verified on-demand only if they were loaded before in SRAM or the BOOT_SEQ = 0,
            otherwise, an error will be reported (NOT ALLOWED).
            The SMR(s) that are not part of the CORE RESET table configuration can be loaded and verified at run time.
            Note that on the second call of this service, the HSE will only performed the verification in SRAM.
            Using this service, the SMR(s) can not be loaded and verified from SD/MMC memory.
            This function is available only in HSE_H configurations and for QSPI data source.

@param[in]  u8entryIndex The index in the SMR table to verify

@retval     E_OK if operation is successful, E_NOT_OK otherwise
*/
/*=================================================================================================*/
FUNC(Std_ReturnType, CRYPTODAL_APP_CODE) CryptoDal_VerifySmr(VAR(uint8, AUTOMATIC)  u8EntryIndex)
{
    VAR(Std_ReturnType, AUTOMATIC) eRetVal = E_NOT_OK;

    MemLib_MemSet(&HseSrvDesc, 0, sizeof(hseSrvDescriptor_t));

    if (NULL_PTR != CryptoDal_pGlobalConfigPtr)
    {
        HseSrvDesc.srvId = HSE_SRV_ID_SMR_VERIFY ;
        HseSrvDesc.hseSrv.smrVerifyReq.entryIndex = u8EntryIndex;

        if(HSE_SRV_RSP_OK == CryptoDal_HseSrv_Request(&HseSrvDesc))
        {
            eRetVal = E_OK;
        }
    }

    return eRetVal;
}

/*=================================================================================================*/
/**
@brief      Installs a Secure Memory Region entry in the HSE System Configuration image
@details    Shall be used to install SMR entries in the HSE. If the operation is successful, at the next reset HSE will copy
            the specified SMR entries to RAM memory. This function is available only in HSE_H configurations.

@param[in]  pSMR_Config             Pointer to the first element from the SMR configuration data array
@param[in]  u8SmrIndex              Which SMR to intall from the  array

@retval     E_OK if operation is successful, E_NOT_OK otherwise
*/
/*=================================================================================================*/
static FUNC(Std_ReturnType, CRYPTODAL_APP_CODE) CryptoDal_InstallSmr(
    P2VAR(CryptoDal_SMR_CR_EntryType, AUTOMATIC, CRYPTODAL_APPL_DATA) pSMR_CR_Config,
    VAR(uint8, AUTOMATIC) u8SmrIndex
    )
{
    CryptoDal_SMR_EntryType *pSMR_Config = &pSMR_CR_Config->pSmrCfg[u8SmrIndex];
    hseSmrEntryInstallSrv_t *pSmrEntryInstall;
    VAR(Std_ReturnType, AUTOMATIC) eRetVal = E_NOT_OK;

    MemLib_MemSet(&SmrEntry, 0, sizeof(hseSmrEntry_t));
    SmrEntry.configFlags = pSMR_Config->u8ConfigFlags;
    SmrEntry.pSmrDest = pSMR_Config->u32DestinationAddr;

    /* Provide key and authentication scheme used for verification */
    SmrEntry.authKeyHandle = pSMR_Config->u32KeyHandle;
    SmrEntry.authScheme = pSMR_Config->authScheme;

    /* Provide input and signature of the SMR to be verified */
    SmrEntry.pSmrSrc = pSMR_Config->u32SourceAddr;
    SmrEntry.smrSize = pSMR_Config->u32AppSize;
    SmrEntry.pInstAuthTag[0] = pSMR_Config->u32Signature;
    /* Plain SMR, encryption not used. */
    SmrEntry.smrDecrypt.decryptKeyHandle = HSE_SMR_DECRYPT_KEY_HANDLE_NOT_USED;

    MemLib_MemSet(&HseSrvDesc, 0, sizeof(hseSrvDescriptor_t));
    HseSrvDesc.srvId = HSE_SRV_ID_SMR_ENTRY_INSTALL;

    pSmrEntryInstall = &HseSrvDesc.hseSrv.smrEntryInstallReq;
    pSmrEntryInstall->accessMode        = HSE_ACCESS_MODE_ONE_PASS;
    pSmrEntryInstall->entryIndex        = pSMR_Config->u8EntryIndex;
    pSmrEntryInstall->pSmrEntry         = HSE_PTR_TO_HOST_ADDR(&SmrEntry);
    pSmrEntryInstall->pSmrData          = HSE_PTR_TO_HOST_ADDR(pSMR_Config->u32SourceAddr);
    pSmrEntryInstall->smrDataLength     = pSMR_Config->u32AppSize;
    pSmrEntryInstall->pAuthTag[0]       = HSE_PTR_TO_HOST_ADDR(pSMR_Config->u32Signature);
    pSmrEntryInstall->authTagLength[0]  = pSMR_Config->u16SignatureLength;

    if(HSE_SRV_RSP_OK == CryptoDal_HseSrv_Request(&HseSrvDesc))
    {
        eRetVal = E_OK;
    }

    return eRetVal;
}

/*=================================================================================================*/
/**
@brief      Installs a Core Reset entry in the HSE System Configuration image
@details    Shall be used to install CR entries in the HSE. If the operation is successful, at the next reset HSE will start
            the specified core and start execution from the specified ram address. This function is available only in HSE_H configurations.

@param[in]  pCR_Config             Pointer to the CR configuration data

@retval     E_OK if operation is successful, E_NOT_OK otherwise
*/
/*=================================================================================================*/
static FUNC(Std_ReturnType, CRYPTODAL_APP_CODE) CryptoDal_InstallCr(
    P2VAR(CryptoDal_CR_EntryType, AUTOMATIC, CRYPTODAL_APPL_DATA) pCR_Config)
{
    VAR(Std_ReturnType, AUTOMATIC) eRetVal = E_OK;

    MemLib_MemSet(&CoreResetEntry, 0, sizeof(hseCrEntry_t));
    /* Configure a Core Reset Entry for Advanced Secure Boot */
    CoreResetEntry.coreId = pCR_Config->u8CoreID;
    CoreResetEntry.preBootSmrMap = pCR_Config->u32PreSmrMap;
    CoreResetEntry.pPassReset = pCR_Config->u32ResetAddr;
    CoreResetEntry.crSanction = HSE_CR_SANCTION_KEEP_CORE_IN_RESET;
    CoreResetEntry.startOption = HSE_CR_AUTO_START;

    MemLib_MemSet(&HseSrvDesc, 0, sizeof(hseSrvDescriptor_t));
    /* Create the request to HSE using service descriptor */
    HseSrvDesc.srvId                                  = HSE_SRV_ID_CORE_RESET_ENTRY_INSTALL;
    HseSrvDesc.hseSrv.crEntryInstallReq.crEntryIndex  = pCR_Config->u8CrEntryIndex;
    HseSrvDesc.hseSrv.crEntryInstallReq.pCrEntry      = HSE_PTR_TO_HOST_ADDR(&CoreResetEntry);

    if(HSE_SRV_RSP_OK != CryptoDal_HseSrv_Request(&HseSrvDesc))
    {
        eRetVal = E_NOT_OK;
    }

    return eRetVal;
}

/*=================================================================================================*/
/**
@brief      Installs a Secure Memory Region and a Core Reset entry in the HSE System Configuration image
@details    Shall be used to install SMR/CR entries in the HSE. If the operation is successful, at the next reset HSE will copy
            the specified SMR entries to RAM memory. This function is available only in HSE_H configurations.

@param[in]  pSMR_CR_Config - Pointer to the SMR/CR configuration data

@retval     E_OK if operation is successful, E_NOT_OK otherwise
*/
/*=================================================================================================*/
FUNC(Std_ReturnType, CRYPTODAL_APP_CODE) CryptoDal_SMR_CR_Install(
    P2VAR(CryptoDal_SMR_CR_EntryType, AUTOMATIC, CRYPTODAL_CONST) pSMR_CR_Config)
{
    VAR(Std_ReturnType, AUTOMATIC) eRetVal = E_OK;
    VAR(uint8, AUTOMATIC) u8Index;

    if (NULL_PTR != CryptoDal_pGlobalConfigPtr)
    {
        for (u8Index = 0; u8Index < pSMR_CR_Config->u8SmrNumber; u8Index++)
        {
            eRetVal = CryptoDal_InstallSmr(pSMR_CR_Config, u8Index);

            if (E_NOT_OK == eRetVal)
            {
                break;
            }
        }

        if (E_OK == eRetVal && NULL != pSMR_CR_Config->pCrCfg)
        {
            eRetVal = CryptoDal_InstallCr(pSMR_CR_Config->pCrCfg);
        }
    }

    return eRetVal;
}

/*=================================================================================================*/
/**
@brief      Function used for getting the HSE System Image
@details    Shall be used to retrieve the System Image from the HSE subsystem. Whenever a new operation has been registered in the
            HSE (Non volatile key, OTFAD configuration, SMR/CR configuration), a system configuration is being kept in HSE internal RAM.
            Upon this request, the HSE will publish the system image using the device encryption key.
@param[in]  pImage            pointer where the image will be stored in RAM
@param[in]  pSysImageOffset   pointer where system image offset will be stored
@param[in]  pHseImageSize     pointer where system image size will be stored

@retval     E_OK if operation is successful, E_NOT_OK otherwise
*/
/*=================================================================================================*/
FUNC(Std_ReturnType, CRYPTODAL_APP_CODE) CryptoDal_GetSysImage(
    P2VAR(uint8, AUTOMATIC, CRYPTODAL_CONST) pImage,
    P2VAR(uint32, AUTOMATIC, CRYPTODAL_CONST) pSysImageOffset,
    P2VAR(uint32, AUTOMATIC, CRYPTODAL_CONST) pHseImageSize)
{
    VAR(Std_ReturnType, AUTOMATIC) eRetVal = E_OK;
    if (NULL_PTR != CryptoDal_pGlobalConfigPtr)
    {
        /* Clear previous request */
        MemLib_MemSet(&HseSrvDesc, 0, sizeof(hseSrvDescriptor_t));
        HseSrvDesc.srvId                                   = HSE_SRV_ID_GET_SYS_IMAGE_SIZE;
        HseSrvDesc.hseSrv.getSysImageSizeReq.pSysImageSize = HSE_PTR_TO_HOST_ADDR(&u32SysImgSize);

        CryptoDal_HseSrv_Request(&HseSrvDesc);
        if (u32SysImgSize > (*pHseImageSize))
        {
            /* Not enough memory was allocated in the output buffer */
            eRetVal = E_NOT_OK;
        }

        /* Clear previous request */
        MemLib_MemSet(&HseSrvDesc, 0, sizeof(hseSrvDescriptor_t));

        HseSrvDesc.srvId                                    = HSE_SRV_ID_PUBLISH_SYS_IMAGE;
        HseSrvDesc.hseSrv.publishSysImageReq.publishOptions = HSE_PUBLISH_ALL_DATA_SETS;
        HseSrvDesc.hseSrv.publishSysImageReq.pPublishOffset = HSE_PTR_TO_HOST_ADDR(&u32SysImgOffset);
        HseSrvDesc.hseSrv.publishSysImageReq.pBuffLength    = HSE_PTR_TO_HOST_ADDR(&u32SysImgSize);
        HseSrvDesc.hseSrv.publishSysImageReq.pBuff          = HSE_PTR_TO_HOST_ADDR(pImage);

        if (E_OK == eRetVal)
        {
            if (HSE_SRV_RSP_OK != CryptoDal_HseSrv_Request(&HseSrvDesc))
            {
                eRetVal = E_NOT_OK;
            }
            else
            {
                *pSysImageOffset = u32SysImgOffset;
                *pHseImageSize = u32SysImgSize;
            }
        }
    }

    return eRetVal;
}
#endif /* (CRYPTODAL_SUPPORT_SMR == STD_ON) */

/*=================================================================================================*/
/**
@brief      Report HSE firmware status
@details    This function will log all the set bits of the MU.FSR register

@retval     none
*/
/*=================================================================================================*/
static FUNC(void, CRYPTODAL_APP_CODE) CryptoDal_HseStatus(void)
{
    hseStatus_t HseStatus = 0u;
    uint8 u8Idx = 0u;
    char StatusFlag[][40] = {
        "",
        "HSE_SHE_STATUS_SECURE_BOOT",
        "HSE_SHE_STATUS_SECURE_BOOT_INIT",
        "HSE_SHE_STATUS_SECURE_BOOT_FINISHED",
        "HSE_SHE_STATUS_SECURE_BOOT_OK",
        "HSE_STATUS_RNG_INIT_OK",
        "HSE_STATUS_HOST_DEBUGGER_ACTIVE",
        "HSE_STATUS_HSE_DEBUGGER_ACTIVE",
        "HSE_STATUS_INIT_OK",
        "HSE_STATUS_INSTALL_OK",
        "HSE_STATUS_BOOT_OK",
        "HSE_STATUS_CUST_SUPER_USER",
        "HSE_STATUS_OEM_SUPER_USER",
        "HSE_STATUS_PUBLISH_SYS_IMAGE",
        "HSE_STATUS_PRIMARY_SYS_IMAGE",
        "HSE_STATUS_BACKUP_SYS_IMAGE",
    };

    HseStatus = Hse_Ip_GetHseStatus(CRYPTO_MU_0_U8);

    for (u8Idx = 1; u8Idx < 16; u8Idx++)
    {
        if ((1 << u8Idx) == ((1 << u8Idx) & HseStatus))
        {
            CONSOLE_LOG(CONSOLE_INFO, "CryptoDal HSE status %s\r\n", StatusFlag[u8Idx]);
        }
    }
}

/*=================================================================================================================================*/
/**
* @brief            Get the version of the HSE FW
* @details          This function retrieves HSE FW attributes, including the type of SoC,
*                   firmware type (standard or premium), and revision number.
*
* @param[in]        none
* @return           none
*/
/*=================================================================================================================================*/
FUNC(void, CRYPTODAL_APP_CODE) CryptoDal_GetHseFwVersion(void)
{
    hseSrvResponse_t srvResponse = HSE_SRV_RSP_GENERAL_ERROR;

    /* Clear previous request */
    MemLib_MemSet(&HseSrvDesc, 0, sizeof(hseSrvDescriptor_t));
    MemLib_MemSet(&HseFwVersion, 0, sizeof(hseAttrFwVersion_t));

    HseSrvDesc.srvId                      = HSE_SRV_ID_GET_ATTR;
    HseSrvDesc.hseSrv.getAttrReq.attrId   = HSE_FW_VERSION_ATTR_ID;
    HseSrvDesc.hseSrv.getAttrReq.attrLen  = sizeof(hseAttrFwVersion_t);
    HseSrvDesc.hseSrv.getAttrReq.pAttr    = HSE_PTR_TO_HOST_ADDR(&HseFwVersion);

    srvResponse = CryptoDal_HseSrv_Request(&HseSrvDesc);

    if(HSE_SRV_RSP_OK != srvResponse)
    {
        CONSOLE_LOG(CONSOLE_ERROR, "CryptoDal Error getting the HSE FW version\r\n");
    }
	else
	{
        CONSOLE_LOG(CONSOLE_INFO, "HSE FW Version: %d.%d.%d_%d.%d.%d\r\n",
            HseFwVersion.reserved, HseFwVersion.socTypeId, HseFwVersion.fwTypeId,
            HseFwVersion.majorVersion, HseFwVersion.minorVersion, HseFwVersion.patchVersion);
    }
}

#ifdef __cplusplus
}
#endif
/** @} */
