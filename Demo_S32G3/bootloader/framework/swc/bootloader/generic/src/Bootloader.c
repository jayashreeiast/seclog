/**
 * @file      Bootloader.c
 * @version   24.12.0
 *
 * @brief     Bootloader implementation file.
 * @details   The Bootloader is responsible of copying the user configured
 *            boot source images to their respective destination using a certain
 *            transfer method (either DMA, SD transfer or COM).
 *            Afterwards, based on a given configuration, it boots the
 *            application synchronously or asynchronously with respect to the
 *            load-start sequence.
 *
 *            Platform     : CORTEXM
 *            Build Version: S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
 *
 * @copyright (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
 *                All Rights Reserved.
 *
 *            This file contains sample code only.
 *            It is not part of the production code deliverables.
 */

#include "Bootloader.h"

#include "Bootloader_Cfg.h"
#include "CDD_Mcl.h"
#include "Mcal.h"
#include "MemLib.h"
#include "SysDal.h"
#include "Mcu_Cfg.h"

#ifdef ENABLE_RUNTIME_MEASUREMENTS
#include "SysDal_Rtm.h"
#include "SysDal_Rtm_Cfg.h"
#endif /* ENABLE_RUNTIME_MEASUREMENTS */

#if (BL_CRYPTO_USED == STD_ON)
#include "CryptoDal.h"
#include "Crypto.h"
#include "MemLib.h"
#include "Mcu.h"

#if (BL_ENABLE_SECURE_BOOT == STD_ON)
#include "Schm_Fls.h"
#include "Fls.h"
#endif /* BL_ENABLE_SECURE_BOOT == STD_ON */
#endif /* BL_CRYPTO_USED == STD_ON */

#if (STD_ON == BL_BOOT_TRIGGER_ENABLED)
#include "ComDal.h"
#include "Gpt.h"
#endif /* STD_ON == BL_BOOT_TRIGGER_ENABLED */

#if (STD_ON == BL_SDHC_ENABLED)
#include "MemDal.h"
#endif /* STD_ON == BL_SDHC_ENABLED */

#if (STD_ON == BL_COM_BOOT_USED)
#include "ComDal.h"
#include "ComManager.h"

#ifdef IFWK_USE_ETH
#include "ComManager_Http.h"
#endif /* IFWK_USE_ETH */

#endif /* STD_ON == BL_COM_BOOT_USED */

#if (BL_USE_SAFETY == STD_ON)
#include "Saf_Stubs.h"
#include "mSel.h"
#endif /* BL_USE_SAFETY == STD_ON */

#include "Console.h"

#ifdef BL_ENABLE_ECC_CHECK
#include "Qspi_Ip.h"
#include "Qspi_Ip_PBcfg.h"
#endif /* BL_ENABLE_ECC_CHECK */

/*==============================================================================
* LOCAL SYMBOLIC CONSTANTS
==============================================================================*/

/* Maximum hash byte size, available, in case SHA512 authentication is used. */
#define MAX_HASH_SIZE 0x40

/* Authentication header byte size. */
/* This header contains authentication info */
/* (hashes of images and hashes of header). */
#define AUTHENTICATION_HEADER_SIZE 0x50

/* Application signature byte size. */
#define APPLICATION_SIGNATURE_SIZE 0x20

/* Header-specific byte size. */
#define AUTHENTICATION_HEADER_INFO_SIZE 0x30

/* Image hash byte offset from the start address. */
#define IMAGE_HASH_OFFSET 0x10

/* Negative byte offset of flash address where the image is stored */
/* and from where the signature can be located. */
#define BL_IMAGE_SIGNATURE_OFFSET 0x40

/* Negative byte offset of the 32-bit image size. */
#define BL_IMAGE_SIZE_OFFSET 0x44

/* Marker defining that there is no application configured for current core. */
#define BL_APP_NOT_CONFIGURED 0xFF

/* Timeout for SDHC load operation. */
#define SDHC_LOAD_TIMEOUT 1000U

/* The minimum data size (in bytes) for a DMA transfer. */
#define BL_MIN_DMA_TRANSFER_SIZE 4096U

#if (BL_ENABLE_SECURE_BOOT == STD_ON)
    /* Image Vector Table configuration address */
    #define IVT_ADDR 0x0

    /* SMR entry index for the Bootloader application and Boot Table */
    #define BL_BOOTLOADER_SMR_ID 0U
    #define BL_BOOT_TABLE_SMR_ID 1U

    /* Byte assigned for secure boot mode enable/disable */
    #define IVT_BCW_SEC_BOOT_CONFIG_MASK (1 << 3)
    /* Application header size */
    #define BOOT_IMAGE_HEADER_SIZE 0x40
    /* External flash sector size */
    #define QSPI_SECTOR_SIZE 0x1000
    /* HSE System Image contains the Keys, SMR/CR entries */
    #define HSE_SYS_IMAGE_SIZE 0x5000

    #ifdef ENABLE_RUNTIME_MEASUREMENTS
        #ifndef CONFIGURESECUREBOOT
            #error \
                "No measurement named ConfigureSecureBoot is defined. " \
                "Please review RTM configuration"
        #endif /* CONFIGURESECUREBOOT */
    #endif /* ENABLE_RUNTIME_MEASUREMENTS */
#endif /* (BL_ENABLE_SECURE_BOOT == STD_ON) */

#if (STD_ON == BL_COM_BOOT_USED)
/* The timeout for the session busy state. */
#define BL_COM_SESSION_TIMEOUT_MS 5000U
#endif /* STD_ON == BL_COM_BOOT_USED */

#ifdef ENABLE_RUNTIME_MEASUREMENTS
#ifndef LOADANDAUTHAPPLICATION
#error \
    "No measurement named LoadAndAuthApplication is defined. " \
       "Please review RTM configuration"
#endif /* LOADANDAUTHAPPLICATION */
#endif /* ENABLE_RUNTIME_MEASUREMENTS */

#if (BL_ENABLE_DYNAMIC_BOOT == STD_ON)
    /* Macro used to get the number of configured applications. */
    #define BL_GET_CORE_CONFIGS_NMB (dynamicConfig->u8AppsNumber)

    /* Macro used to get the current application configuration pointer. */
    #define BL_GET_CORE_CONFIG_PTR(applicationId) (&dynamicConfig->aApps[applicationId])

    /* Macro used to get the current image source pointer. */
    #define BL_GET_IMG_FRAGMENT_PTR(applicationId, fragmentIdx) \
            (&dynamicConfig->aImages[applicationId][fragmentIdx])
#else
    /* Macro used to get the number of configured applications. */
    #define BL_GET_CORE_CONFIGS_NMB (CONFIGURED_CORES_COUNT)

    /* Macro used to get the current application configuration pointer. */
    #define BL_GET_CORE_CONFIG_PTR(applicationId) (&bootApplications[applicationId])

    /* Macro used to get the current image source pointer. */
    #define BL_GET_IMG_FRAGMENT_PTR(applicationId, fragmentIdx) \
            (&bootApplications[applicationId].pImageFragments[fragmentIdx])
#endif /* BL_ENABLE_DYNAMIC_BOOT == STD_ON */

/*==============================================================================
* LOCAL VARIABLES
==============================================================================*/

/* Store the enabled partitions information
   - 0: partition not enabled
   - 1: partition enabled
*/
static uint8 Bl_isPartitionEnabled[MCU_MAX_NUMBER_OF_PARTITIONS] = {PARTITION_DISABLED};

/* Identifier of the application id that is going to run
   on the same core as the Bootloader. */
static uint8 currentCoreApplicationId = BL_APP_NOT_CONFIGURED;

/* Independent status for all defined application.
   - E_OK: transfer and integrity checks are OK.
   - E_NOT_OK: a problem occurred in either transfer or integrity check */
static StatusType Bl_AppStatus[CONFIGURED_CORES_COUNT];

#if (BL_ENABLE_SECURE_BOOT == STD_ON)
/* Image Vector table configuration */
static Bl_ImageVectorTableType pIVTConfig;

/* The following RAM variables need to be put in non-cacheable memory
   in order to be correctly seen by HSE */
#define CRYPTO_START_SEC_VAR_INIT_8_NO_CACHEABLE
#include "Crypto_MemMap.h"
static uint8 Bl_FlashMirror[QSPI_SECTOR_SIZE];
#define CRYPTO_STOP_SEC_VAR_INIT_8_NO_CACHEABLE
#include "Crypto_MemMap.h"
#endif /* (BL_ENABLE_SECURE_BOOT == STD_ON) */

#if (BL_USE_BREAKPOINT == STD_ON)
/* Debugging macro used for stopping in the main function during debug. */
static volatile uint32 ENABLE_BREAKPOINT_AT_MAIN = 0U;
#endif /* BL_USE_BREAKPOINT == STD_ON */

#if (BL_ENABLE_SECURE_BOOT == STD_ON)
/* CMAC authentication scheme used for signing the applications images */
static const hseAuthScheme_t Bl_CmacAuthScheme = {
    .macScheme.macAlgo = HSE_MAC_ALGO_CMAC,
    .macScheme.sch.cmac.cipherAlgo = HSE_CIPHER_ALGO_AES
};

/* RSA authentication scheme used for signing the boot table */
static const hseAuthScheme_t Bl_RsaAuthScheme = {
    .sigScheme.signSch = HSE_SIGN_RSASSA_PKCS1_V15,
    .sigScheme.sch.rsaPkcs1v15.hashAlgo = HSE_HASH_ALGO_SHA2_256
};
#endif /* BL_ENABLE_SECURE_BOOT == STD_ON */

/*==============================================================================
* GLOBAL VARIABLES
==============================================================================*/
#if (BL_ENABLE_SECURE_BOOT == STD_ON)
#define CRYPTO_START_SEC_VAR_INIT_8_NO_CACHEABLE
#include "Crypto_MemMap.h"

#if (STD_ON == BL_SUPPORT_AUTHENTICATION)
    /* Buffer used to calculate sha signatures*/
    uint8 hashResult[MAX_HASH_SIZE];
#endif /* STD_ON == BL_SUPPORT_AUTHENTICATION */

/* Buffer used to update the HSE System Image */
uint8 Bl_HseSysImage[HSE_SYS_IMAGE_SIZE];

#define CRYPTO_STOP_SEC_VAR_INIT_8_NO_CACHEABLE
#include "Crypto_MemMap.h"
#endif /* BL_ENABLE_SECURE_BOOT == STD_ON */

/* Address of the application that is going to run */
/* on the same core as the Bootloader. */
uint32 currentCoreBootImageAddress;

#if (BL_BOOT_TRIGGER_ENABLED == STD_ON)
/* Flag used to signal a message was received, */
/* before communication timeout during boot sequence. */
volatile boolean Bl_TriggerReceived = FALSE;
#endif /* BL_BOOT_TRIGGER_ENABLED == STD_ON */

#ifdef ENABLE_RUNTIME_MEASUREMENTS
/* Engineering value for load and authenticate operations. */
SysDal_Rtm_OneshotResultType Bl_LoadAndAuthTime;
#endif /* ENABLE_RUNTIME_MEASUREMENTS */

/* The accepted reset reason mask to allow the boot flow. */
extern Bl_ResetReasonType Bl_AcceptedResetReason;

#if (BL_ENABLE_DYNAMIC_BOOT == STD_OFF)
/* Global application configuration table. */
extern Bl_ApplicationDetails bootApplications[];
#endif /* BL_ENABLE_DYNAMIC_BOOT == STD_ON */

#if (BL_ENABLE_DYNAMIC_BOOT == STD_ON)
/* Address of a 2 KiB SRAM memory section for boot table data */
extern char BOOT_TABLE_ADDRESS;

/* Dynamic boot configuration data stored in a special allocated SRAM section */
Bl_DynamicBootConfig *dynamicConfig = (Bl_DynamicBootConfig*)&BOOT_TABLE_ADDRESS;
#endif /* BL_ENABLE_DYNAMIC_BOOT == STD_ON */

/*==============================================================================
* LOCAL FUNCTION PROTOTYPES
==============================================================================*/

#if (BL_USE_SAFETY == STD_ON)
static void Bl_SafetySelectMode(void);
#endif /* (BL_USE_SAFETY == STD_ON) */

#if (BL_SYNCHRONIZED_BOOT == STD_ON)
static void Bl_StartAllApplications(void);
#endif /* (BL_SYNCHRONIZED_BOOT == STD_ON) */

#if (BL_BOOT_TRIGGER_ENABLED == STD_ON)
static boolean Bl_WaitTriggerMessage(void);
#endif /* (BL_BOOT_TRIGGER_ENABLED == STD_ON) */

#if (STD_ON == BL_COM_BOOT_USED)
static StatusType Bl_TransferImageViaCom(uint8 u8ApplicationId,
                                         uint8 u8FragmentIdx);
#endif /* (STD_ON == BL_COM_BOOT_USED) */

#if (BL_ENABLE_CRC_CHECK == STD_ON)
static StatusType Bl_LoadWithCRCAuth(uint8 applicationId, uint8 u8FragmentIdx);
#endif /* (BL_ENABLE_CRC_CHECK == STD_ON) */

#if (STD_ON == BL_SUPPORT_AUTHENTICATION)
static void Bl_ConfigureSHAPipelineMethod(
    uint8 applicationId, uint8 u8FragmentIdx,
    Bl_TransferDescriptorType *transferDescriptors);

static StatusType Bl_LoadWithSHAAuth(uint8 applicationId, uint8 u8FragmentIdx);
#endif /* (STD_ON == BL_SUPPORT_AUTHENTICATION) */

#if (BL_IMAGE_DECRYPTION_ENABLED == STD_ON)
static StatusType Bl_DecryptApplication(uint8 applicationId, uint8 u8FragmentIdx);
#endif /* (BL_IMAGE_DECRYPTION_ENABLED == STD_ON) */

#if (BL_SUPPORT_AUTHENTICATION == STD_ON)
    static StatusType
    Bl_ValidateImageSignature(uint32 startAddress, uint32 size,
                              uint8 authAlgorithm, uint8 *signature,
                              Crypto_OperationModeType algorithmMode);

static StatusType Bl_AuthenticateImageHeader(uint8 applicationId,
                                             uint8 u8FragmentIdx);

static StatusType Bl_AuthenticateImage(uint8 applicationId,
                                       uint8 u8FragmentIdx);
#endif /* (BL_SUPPORT_AUTHENTICATION == STD_ON) */

#if (BL_ENABLE_CRC_CHECK == STD_ON)
static StatusType Bl_CheckApplicationCrc(uint8 applicationId,
                                         uint8 u8FragmentIdx);

static void Bl_DmaCrcChannelEnable(uint32 ChannelNumber);
#endif /* (BL_ENABLE_CRC_CHECK == STD_ON) */

#if (STD_ON == BL_SDHC_ENABLED)
static StatusType Bl_LoadFromSdhc(uint8 u8ApplicationId, uint8 u8FragmentIdx);
#endif /* (STD_ON == BL_SDHC_ENABLED) */

static Bl_ResetReasonType Bl_GetResetReason(void);

static void Bl_DetermineCurrentCoreAppId(void);

static void Bl_ActivateCurrentPartition(void);

static void Bl_ConfigureDmaTransfer(uint32 sourceAddress,
                                    uint32 destinationAddress,
                                    Bl_DmaTransferSize DmaAtomicTransfer,
                                    uint32 transferredSize);

static void Bl_StartDmaTransfer(uint32 flashAddress, uint32 destinationAddress,
                                Bl_DmaTransferSize DmaAtomicTransfer,
                                uint32 transferredSize);

static StatusType Bl_TransferImageFromQspi(uint8 u8ApplicationId,
                                           uint8 u8FragmentIdx);

static StatusType Bl_LoadAndAuthFromQspi(uint8 u8ApplicationId,
                                         uint8 u8FragmentIdx);

static StatusType Bl_WaitApplicationFetch(void);

static StatusType Bl_FetchApplication(uint8 u8ApplicationId,
                                      uint8 u8FragmentIdx);

static StatusType Bl_LoadApplication(uint8 applicationId);

static void Bl_StartApplicationOnCurrentCore(
    Bl_ApplicationDetails *applicationConfig);

/*==============================================================================
* FUNCTION DEFINITIONS
==============================================================================*/

/**
 * @brief      Generate the TCD (transfer configuration descriptor) parameter.
 * @details    This function create the TCD configuration parameter
 *             and ensures that the TCD configuration transfer
 *             size attribute is multiple of the block size transferred by
 *             DMA in one (atomic) operation.
 *
 * @param      sourceAddress       Address from which data will be copied
 *                                 (data peripheral source)
 * @param      destinationAddress  Address at which data will be copied
 *                                 (data peripheral destination)
 * @param      DmaAtomicTransfer   DMA transfer size (in bytes) in one
 *                                 operation (atomic transfer size)
 * @param      transferredSize     Transfer data size
 */
static void Bl_ConfigureDmaTransfer(uint32 sourceAddress,
                                    uint32 destinationAddress,
                                    Bl_DmaTransferSize DmaAtomicTransfer,
                                    uint32 transferredSize)
{
    Mcl_DmaChannelTransferListType DmaCommands[] = {
        {MCL_DMA_CH_SET_SOURCE_ADDRESS, sourceAddress},
        {MCL_DMA_CH_SET_DESTINATION_ADDRESS, destinationAddress},
        {MCL_DMA_CH_SET_SOURCE_TRANSFER_SIZE, DmaAtomicTransfer},
        {MCL_DMA_CH_SET_DESTINATION_TRANSFER_SIZE, DmaAtomicTransfer},
        {MCL_DMA_CH_SET_SOURCE_SIGNED_OFFSET, (0x1 << DmaAtomicTransfer)},
        {MCL_DMA_CH_SET_DESTINATION_SIGNED_OFFSET, (0x1 << DmaAtomicTransfer)},
        {MCL_DMA_CH_SET_MINORLOOP_SIZE, transferredSize},
        {MCL_DMA_CH_SET_MAJORLOOP_COUNT, 1} /* Trigger only once. */
    };

    Mcl_SetDmaChannelTransferList(
        DMA_LOGIC_CH_0, DmaCommands,
        (sizeof(DmaCommands) / sizeof(Mcl_DmaChannelTransferListType)));
}

/**
 * @brief      Initiate a DMA data transfer between specific hardware
 *             peripherals (QuadSPI Flash and RAM).
 * @details    This function configure the transfer control descriptor register
 *             and initiate a DMA data transfer (given data chunk size)
 *             from QuadSPI Flash (given start address) to RAM (given
 *             destination address) on the DMA logic channel 0.
 *
 * @param      flashAddress        QuadSPI flash start address
 *                                 from which the data chunk will be copy
 * @param      destinationAddress  Destination RAM start address, where the
 *                                 data chunk will be copy.
 * @param      DmaAtomicTransfer   DMA transfer size (in bytes) in one
 *                                 operation (atomic transfer size)
 * @param      transferredSize     Number of bytes to be transferred from
 *                                 source hardware peripheral.
 */
static void Bl_StartDmaTransfer(uint32 flashAddress, uint32 destinationAddress,
                                Bl_DmaTransferSize DmaAtomicTransfer,
                                uint32 transferredSize)
{
    Bl_ConfigureDmaTransfer(flashAddress, destinationAddress, DmaAtomicTransfer,
                            transferredSize);

    Mcl_SetDmaChannelCommand(DMA_LOGIC_CH_0, MCL_DMA_CH_START_SERVICE);
}

#ifdef BL_ENABLE_ECC_CHECK
/**
 * @brief      Vendor specific: Function called at the end of each read operation.
 * @details    The intended purpose of this function is to provide to the application the possibility
 *             of interrogating the ECC status of the memory after each read operation.
 *             Return values: E_OK(0) E_NOT_OK(1).
 *             If E_OK(0) is received, the job is considered successful.
 *             If E_NOT_OK(1) is received, the job is considered unsuccessful and marked as failed.
 * @return     StatusType  E_OK or E_NOT_OK
 */
static StatusType Bl_QspiEccCheck(void){
    StatusType Status = E_NOT_OK;
    uint8 Data;
    Qspi_Ip_StatusType Qspi_Ip_Status;

    Qspi_Ip_Status = Qspi_Ip_RunReadCommand(0U, MEMCFG_0_LUT_SEQUENCE_READRD2_DOPI,
                                            BL_ENABLE_ECC_CHECK_ADDRESS,
                                            &Data, NULL_PTR, 1);
    if (STATUS_QSPI_IP_SUCCESS == Qspi_Ip_Status)
    {
        if ( (BL_ENABLE_ECC_CHECK_BITMASK) & Data)
        {
            CONSOLE_LOG(CONSOLE_ERROR, "Qspi ECC error detected %d\r\n", Data);
            Status = E_NOT_OK;
        }
        else
        {
            Status = E_OK;
        }
    }

    return Status;
}
#endif /* BL_ENABLE_ECC_CHECK */

/**
 * @brief   Wait until the DMA data chunk is transferred.
 * @details This function wait until DMA engine terminate to
 *          transfer the data chunk or until the timeout elapses.
 *
 * @return  StatusType             E_OK or E_NOT_OK
 */
static StatusType Bl_WaitApplicationFetch(void)
{
    StatusType Status = E_OK;

    Mcl_DmaChannelStatusType TransferStatus;
    do
    {
        Mcl_GetDmaChannelStatus(DMA_LOGIC_CH_0, &TransferStatus);
        if (MCL_DMA_CH_ERROR_STATE == TransferStatus.ChannelState ||
            0 != TransferStatus.Errors)
        {
            Status = E_NOT_OK;
            break;
        }
    } while (TRUE != TransferStatus.Done);

    #ifdef BL_ENABLE_ECC_CHECK
    if (E_OK == Status)
    {
        Status = Bl_QspiEccCheck();
    }
    #endif /* BL_ENABLE_ECC_CHECK */

    return Status;
}

/**
 * @brief      Transfer an application image from Flash into destination memory.
 * @details    The application image is split into image fragments.
 *
 * @param      u8ApplicationId  Index in the bootloader application array
 * @param      u8FragmentIdx    Index in the boot image fragments array
 *
 * @return     StatusType       E_OK or E_NOT_OK
 */
static StatusType Bl_TransferImageFromQspi(uint8 u8ApplicationId,
                                           uint8 u8FragmentIdx)
{
    StatusType Status = E_NOT_OK;
    Bl_BootImageFragmentType *pImage = BL_GET_IMG_FRAGMENT_PTR(u8ApplicationId, u8FragmentIdx);
    uint32 u32StorageAddress = pImage->u32FlashAddress;
    uint32 u32DestinationAddress = pImage->u32DestinationAddress;
    uint32 u32ChunkSize = BL_ALIGN_64B(pImage->u32Size);

#if (BL_ENABLE_SECURE_BOOT == STD_ON)
    uint8 u8SMRIndex = pImage->u8SMRIndex;

    if (u8SMRIndex) /* if non zero SMRIndex use HSE to load images to RAM */
    {
        Status = CryptoDal_VerifySmr(u8SMRIndex);
    }
    else
    {
        /* Start the DMA transfer with the 64-byte transfer size configuration. */
        Bl_StartDmaTransfer(u32StorageAddress, u32DestinationAddress,
                            BL_DMA_SIZE_64B, u32ChunkSize);

        /* Wait for DMA engine to transfer the application image */
        if (E_OK == Bl_WaitApplicationFetch())
        {
            Status = E_OK;
        }
    }
#else
    /* Start the DMA transfer with the 64-byte transfer size configuration. */
    Bl_StartDmaTransfer(u32StorageAddress, u32DestinationAddress,
                        BL_DMA_SIZE_64B, u32ChunkSize);

    /* Wait for DMA engine to transfer the application image */
    if (E_OK == Bl_WaitApplicationFetch())
    {
        Status = E_OK;
    }
#endif /* (BL_ENABLE_SECURE_BOOT == STD_ON) */

    return Status;
}

#if (BL_USE_SAFETY == STD_ON)
/**
 * @brief      Select the operating mode for the Bootloader, in terms of safety.
 * @details    Either normal or degraded mode, depending on the startup tests.
 */
static void Bl_SafetySelectMode(void)
{
    mSel_InitModeSelectionData();

    /* Initialize array with NCF faults set by user operational mode */
    /* initialization function and injected when demo timeout is exceeded. */
    NcfInjectionInit();

    /* Operational mode selection by the mSel module. */
    if (E_NOT_OK == mSel_SelectMode())
    {
        /* TODO: Degraded mode. */
    }
    CONSOLE_LOG(CONSOLE_INFO, "Bootloader safety checks finished\r\n");
}
#endif /* BL_USE_SAFETY == STD_ON */

#if (STD_ON == BL_SUPPORT_AUTHENTICATION)
/**
 * @brief      Validate the image signature upon DMA transfer completion.
 * @details    After a chunk has been copied, its signature is verified
 *             via hashing, allowing for parallel transfer-authenticate
 *             operations for a certain image fragment.
 *
 * @param      applicationId  Index in the Bootloader application array
 * @param      u8FragmentIdx  Index in the boot image fragments array
 *
 * @return     StatusType     E_OK or E_NOT_OK
 */
static StatusType Bl_LoadWithSHAAuth(uint8 applicationId, uint8 u8FragmentIdx)
{
    uint8 index;
    StatusType Status = E_NOT_OK;
    Bl_ApplicationDetails *pAppDetail = BL_GET_CORE_CONFIG_PTR(applicationId);
    Bl_SignatureAlgorithm signAlgorithm = pAppDetail->signAlgorithm;
    Bl_BootImageFragmentType *pImageFragment = NULL_PTR;
    uint8 *pFlashAddress = NULL_PTR;
    uint8 *pImageSize = NULL_PTR;
    Bl_TransferDescriptorType transferDescriptors[BL_NO_OF_DATA_TRANSFERS] = {0};

    pImageFragment = BL_GET_IMG_FRAGMENT_PTR(applicationId, u8FragmentIdx);
    pFlashAddress = (uint8 *) (pImageFragment->u32FlashAddress - BL_IMAGE_SIGNATURE_OFFSET);
    pImageSize = (uint8 *) (pImageFragment->u32FlashAddress - BL_IMAGE_SIZE_OFFSET);

    /* QSPI images authenticated with SHA1 or SHA256 contains the size
       in the signature header in little endian order. */
    pImageFragment->u32Size = (pImageSize[0] << 24) | (pImageSize[1] << 16) |
                              (pImageSize[2] << 8) | pImageSize[3];

    Bl_ConfigureSHAPipelineMethod(applicationId, u8FragmentIdx, transferDescriptors);

    Bl_StartDmaTransfer(transferDescriptors[0].sourceAddress,
                        transferDescriptors[0].destinationAddress,
                        BL_DMA_SIZE_64B, transferDescriptors[0].blockSize);

    /* Wait for DMA engine to transfer the application image */
    Bl_WaitApplicationFetch();

    for (index = 1; index < BL_NO_OF_DATA_TRANSFERS; index++)
    {
        /* Images with size less than min DMA transfer transfer size */
        /* are copied in one shot. */
        if (CRYPTO_OPERATIONMODE_SINGLECALL == transferDescriptors[0].algorithmMode)
            break;

        /* Copy the next application chunk */
        Bl_StartDmaTransfer(transferDescriptors[index].sourceAddress,
                            transferDescriptors[index].destinationAddress,
                            BL_DMA_SIZE_64B,
                            transferDescriptors[index].blockSize);

        /* Set the crypto engine to hash the image fragment, */
        /* which has been transferred. */
        Status = Bl_ValidateImageSignature(
            transferDescriptors[index - 1].destinationAddress,
            transferDescriptors[index - 1].authBlockSize, signAlgorithm,
            NULL_PTR, transferDescriptors[index - 1].algorithmMode);

        /* Blocking function called after authentication, */
        /* allowing for parallel transfer-authenticate operations. */
        Bl_WaitApplicationFetch();
    }

    /* Set the crypto engine to hash the last image chunk, */
    /* which has been transferred. */
    Status = Bl_ValidateImageSignature(
        transferDescriptors[index - 1].destinationAddress,
        transferDescriptors[index - 1].authBlockSize, signAlgorithm,
        pFlashAddress, transferDescriptors[index - 1].algorithmMode);

    if (E_NOT_OK == Status)
    {
        CONSOLE_LOG(CONSOLE_ERROR,
                    "The signature of boot image, fragment %d"
                    ", for core %s, does not match the configured value.\r\n",
                    u8FragmentIdx, Bl_PlatformCoreNames[pAppDetail->core]);
    }
    else
    {
        CONSOLE_LOG(CONSOLE_INFO,
                    "The signature of boot image fragment %d core %s, is valid.\r\n",
                    u8FragmentIdx, Bl_PlatformCoreNames[pAppDetail->core]);
    }

    return Status;
}

/**
 * @brief      Populate the DMA transfer descriptors with their
 *             corresponding chunks.
 * @details    Create configuration parameter for each transfer chunk
 *             allowing for parallel transfer-authenticate operations.
 *
 * @param      applicationId       Index in the Bootloader application array.
 * @param      u8FragmentIdx       Index in the boot image fragments array
 * @param      transferDescriptors Reference to the SHA configuration
 *                                 parameters array.
 */
static void Bl_ConfigureSHAPipelineMethod(
    uint8 applicationId, uint8 u8FragmentIdx,
    Bl_TransferDescriptorType *transferDescriptors)
{
    uint8 index;
    Bl_BootImageFragmentType *pImage = BL_GET_IMG_FRAGMENT_PTR(applicationId, u8FragmentIdx);
    uint32 chunkSize;
    uint32 lastChunkSize;

    /* Set configuration parameter for the first iteration of */
    /* DMA data transfer with SHA method. */
    transferDescriptors[0].sourceAddress = pImage->u32FlashAddress;
    transferDescriptors[0].destinationAddress = pImage->u32DestinationAddress;

    /* Images with size less than min DMA transfer transfer */
    /* size are copied in one shot. */
    if (pImage->u32Size <= BL_MIN_DMA_TRANSFER_SIZE)
    {
        transferDescriptors[0].algorithmMode = CRYPTO_OPERATIONMODE_SINGLECALL;
        transferDescriptors[0].blockSize = BL_ALIGN_64B(pImage->u32Size);
        transferDescriptors[0].authBlockSize = BL_ALIGN_64B(pImage->u32Size);
    }
    else
    {
        /* Compute the chunk of the DMA transfers and align it */
        /* to the DMA transfer size. */
        chunkSize = BL_ALIGN_64B(
            BL_GET_CHUNK_SIZE(pImage->u32Size, BL_NO_OF_DATA_TRANSFERS));

        /* Compute the last chunk of the DMA transfers and align it */
        /* to the DMA transfer size. */
        lastChunkSize = BL_ALIGN_64B(
            pImage->u32Size - ((BL_NO_OF_DATA_TRANSFERS - 1) * chunkSize));

        transferDescriptors[0].algorithmMode = CRYPTO_OPERATIONMODE_START;
        transferDescriptors[0].blockSize = chunkSize;
        transferDescriptors[0].authBlockSize = chunkSize;

        /* Populate the rest of the transfer chunks. */
        for (index = 1; index < (BL_NO_OF_DATA_TRANSFERS - 1); index++)
        {
            /* Set configuration parameter for next iteration of */
            /*  DMA data transfer with SHA method */
            transferDescriptors[index].sourceAddress =
                transferDescriptors[index - 1].sourceAddress + chunkSize;
            transferDescriptors[index].destinationAddress =
                transferDescriptors[index - 1].destinationAddress + chunkSize;
            transferDescriptors[index].blockSize = chunkSize;
            transferDescriptors[index].authBlockSize = chunkSize;
            transferDescriptors[index].algorithmMode =
                CRYPTO_OPERATIONMODE_UPDATE;
        }

        /* Set configuration parameter for the last iteration of */
        /* DMA data transfers with SHA method. */
        transferDescriptors[BL_NO_OF_DATA_TRANSFERS - 1].sourceAddress =
            transferDescriptors[index - 1].sourceAddress + chunkSize;
        transferDescriptors[BL_NO_OF_DATA_TRANSFERS - 1].destinationAddress =
            transferDescriptors[index - 1].destinationAddress + chunkSize;
        transferDescriptors[BL_NO_OF_DATA_TRANSFERS - 1].blockSize =
            lastChunkSize;
        transferDescriptors[BL_NO_OF_DATA_TRANSFERS - 1].authBlockSize =
            pImage->u32Size - ((BL_NO_OF_DATA_TRANSFERS - 1) * chunkSize);
        transferDescriptors[BL_NO_OF_DATA_TRANSFERS - 1].algorithmMode =
            CRYPTO_OPERATIONMODE_FINISH;
    }
}

/**
 * @brief        Validate the application image using the known and configured
 *               authentication algorithms.
 * @details      Used to sign the application image and validate its
 *               signature. After the application image is signed using
 *               encryption engine the resulting signature key is compared with
 *               the signature key stored as application image header.
 *
 * @param        startAddress     Start address for running the hash operation
 * @param        size             Byte size of the hashed chunk
 * @param        authAlgorithm    Authentication algorithm used for
 *                                the signature key
 * @param        signature        The expected signature key reference
 * @param        algorithmMode    The Crypto access mode
 *
 * @return       StatusType       E_OK or E_NOT_OK
 */
static StatusType Bl_ValidateImageSignature(
    uint32 startAddress, uint32 size, uint8 authAlgorithm, uint8 *signature,
    Crypto_OperationModeType algorithmMode)
{
    uint8 u8HashSize = 0;
    uint8 u8Status = E_OK;
    
    for (uint8 index = 0; index < MAX_HASH_SIZE; index++)
    {
        hashResult[index] = 0;
    }

    u8HashSize = CryptoDal_Hash(hashResult,
                              (uint8*) startAddress,
                              size,
                              (CryptoDal_HashAlgorithmType) authAlgorithm,
                              algorithmMode);
    if (0 != u8HashSize)
    {
        u8Status = MemLib_MemCmp(hashResult, signature, u8HashSize);
    }
    else
    {
        u8Status = E_NOT_OK;
    }

    return u8Status;
}

/**
 * @brief      Authenticate the application image and header.
 *
 * @param      applicationId     Index in the Bootloader application array
 * @param      u8FragmentIdx     Index in the boot image fragments array
 *
 * @return     StatusType        E_OK or E_NOT_OK
 */
static StatusType Bl_AuthenticateImage(uint8 applicationId, uint8 u8FragmentIdx)
{
    StatusType Status = E_OK;
    Bl_ApplicationDetails *pAppDetail = BL_GET_CORE_CONFIG_PTR(applicationId);

    Status = Bl_AuthenticateImageHeader(applicationId, u8FragmentIdx);

    if (E_OK == Status)
    {
        Bl_BootImageFragmentType *pImage = BL_GET_IMG_FRAGMENT_PTR(applicationId, u8FragmentIdx);
        Status = Bl_ValidateImageSignature(
            pImage->u32DestinationAddress + AUTHENTICATION_HEADER_SIZE,
            pImage->u32Size - AUTHENTICATION_HEADER_SIZE,
            pAppDetail->signAlgorithm,
            (uint8 *) (pImage->u32DestinationAddress + IMAGE_HASH_OFFSET),
            CRYPTO_OPERATIONMODE_SINGLECALL);

        MemLib_MemCpy((uint8 *) pImage->u32DestinationAddress,
                      (uint8 *) pImage->u32DestinationAddress +
                          AUTHENTICATION_HEADER_SIZE,
                      pImage->u32Size - AUTHENTICATION_HEADER_SIZE);
    }
    else
    {
        CONSOLE_LOG(CONSOLE_ERROR,
                    "The signature of header for boot image, fragment %d"
                    ", for core %s, does not match the configured value.\r\n",
                    u8FragmentIdx, Bl_PlatformCoreNames[pAppDetail->core]);
    }

    return Status;
}

/**
 * @brief        Authenticate the application image header.
 * @details      Image header has the following structure:
 *               RANDOM (8 Bytes) number, Header Size (4 Bytes),
 *               Image size (4 Bytes), Hash of image (32 bytes).
 *               This image header is compared to the 32-byte hash located
 *               before the image.
 *
 * @param        applicationId  Index in the Bootloader application array
 * @param        u8FragmentIdx  Index in the boot image fragments array
 *
 * @return       StatusType     E_OK or E_NOT_OK
 */
static StatusType Bl_AuthenticateImageHeader(uint8 applicationId,
                                             uint8 u8FragmentIdx)
{
    StatusType Status = E_OK;
    uint8 *signatureAddress;
    uint8 hashSize = 0;
    uint32 headerAddress = 0;
        
    Bl_ApplicationDetails *pAppDetail = BL_GET_CORE_CONFIG_PTR(applicationId);
    Bl_BootImageFragmentType *pImage = BL_GET_IMG_FRAGMENT_PTR(applicationId, u8FragmentIdx);
    
    for (uint8 index = 0; index < MAX_HASH_SIZE; index++)
    {
        hashResult[index] = 0;
    }

    switch (pAppDetail->bootSource)
    {
    case BS_QSPI:
    {
        signatureAddress =
            (uint8 *) pImage->u32FlashAddress - APPLICATION_SIGNATURE_SIZE;
        headerAddress = pImage->u32FlashAddress - AUTHENTICATION_HEADER_SIZE;
    }
    break;
    case BS_SDMMC:
    case BS_COM:
    {
        signatureAddress = (uint8 *) pImage->u32DestinationAddress +
                           AUTHENTICATION_HEADER_INFO_SIZE;
        headerAddress = pImage->u32DestinationAddress;
    }
    break;
    default:
        break;
    }

    hashSize = CryptoDal_Hash(
        (uint8 *) hashResult, (uint8 *) headerAddress,
        AUTHENTICATION_HEADER_INFO_SIZE,
        (CryptoDal_HashAlgorithmType) pAppDetail->signAlgorithm,
        CRYPTO_OPERATIONMODE_SINGLECALL);
    if (0U != hashSize)
    {
        Status = (uint8) MemLib_MemCmp(hashResult, signatureAddress, hashSize);
    }
    else
    {
        Status = E_NOT_OK;
    }

    return Status;
}

#endif /* STD_ON == BL_SUPPORT_AUTHENTICATION */

#if (BL_IMAGE_DECRYPTION_ENABLED == STD_ON)
/**
 * @brief      Decrypt the application image.
 * @details    Images can be stored encrypted in the memory,
 *             hence decryption is needed using specific crypto algorithms.
 *             This is done via the Cryptodal abstraction layer.
 *
 * @param      application     Index in the Bootloader application array
 * @param      u8FragmentIdx   Index in the boot image fragments array
 *
 * @return     StatusType      E_OK or E_NOT_OK
 */
static StatusType Bl_DecryptApplication(uint8 applicationId, uint8 u8FragmentIdx)
{
    StatusType Status = E_OK;
    Bl_ApplicationDetails *pAppDetail = BL_GET_CORE_CONFIG_PTR(applicationId);
    Bl_BootImageFragmentType *pImage = BL_GET_IMG_FRAGMENT_PTR(applicationId, u8FragmentIdx);
    uint32 u32ImageSizeAligned = BL_ALIGN_64B(pImage->u32Size);
    uint8 *pDestinationAddress = (uint8 *) pImage->u32DestinationAddress;

    if (BL_NO_ENCRYPTION_ID != pAppDetail->u8CryptoIndex)
    {
        Status = CryptoDal_Decrypt(
            pDestinationAddress, pDestinationAddress,
            pAppDetail->u8CryptoIndex, u32ImageSizeAligned,
            (uint32 *) &u32ImageSizeAligned);

        if (E_OK != Status)
        {
            CONSOLE_LOG(CONSOLE_ERROR,
                        "Decryption failed for fragment %d, core %s.\r\n",
                        u8FragmentIdx, Bl_PlatformCoreNames[pAppDetail->core]);
        }
        else
        {
            CONSOLE_LOG(CONSOLE_INFO,
                        "Decryption successful for fragment %d, core %s.\r\n",
                        u8FragmentIdx, Bl_PlatformCoreNames[pAppDetail->core]);
        }
    }

    return Status;
}
#endif /* BL_IMAGE_DECRYPTION_ENABLED == STD_ON */

#if (BL_ENABLE_CRC_CHECK == STD_ON)
/**
 * @brief      Check the runtime CRC value against the configured one
 *             for a certain application image.
 * @details    Checks the DMA computed CRC value with the configured CRC
 *             value for the given application image.
 *
 * @param      applicationId  Index in the Bootloader application array
 * @param      u8FragmentIdx  Index in the boot image fragments array
 *
 * @return     StatusType     E_OK or E_NOT_OK
 */
static StatusType Bl_CheckApplicationCrc(uint8 applicationId,
                                         uint8 u8FragmentIdx)
{
    Bl_BootImageFragmentType *pImage = BL_GET_IMG_FRAGMENT_PTR(applicationId, u8FragmentIdx);
    uint32 u32ApplicationCRC = 0;
    StatusType Status = E_NOT_OK;

    Mcl_GetDmaChannelParam(DMA_LOGIC_CH_0, MCL_DMA_CH_GET_FINAL_CRC,
                           &u32ApplicationCRC);

    if (u32ApplicationCRC == pImage->u32CrcValue)
    {
        Status = E_OK;
    }
    else
    {
        Bl_ApplicationDetails *pAppDetail = BL_GET_CORE_CONFIG_PTR(applicationId);
        CONSOLE_LOG(CONSOLE_ERROR,
                    "The CRC value for boot image, fragment %d, for core %s, "
                    "does not match the configured value.\r\n"
                    "Expected: %d\r\n"
                    "Computed: %d\r\n",
                    u8FragmentIdx, Bl_PlatformCoreNames[pAppDetail->core],
                    pImage->u32CrcValue, u32ApplicationCRC);
    }

    return Status;
}

/**
 * @brief      Enable the DMA_CRC feature.
 * @details    Sets and link the DMA_CRC channel with the DMA channel.
 *
 * @param      ChannelNumber  DMA logic channel identifier
 */
static void Bl_DmaCrcChannelEnable(uint32 ChannelNumber)
{
    Mcl_DmaChannelCrcListType DmaCRCConfig[] = {
        {MCL_DMA_CH_SET_CRC_EN_LOGIC, TRUE},
        {MCL_DMA_CH_SET_CRC_INITIAL_VALUE, 0xFFFFFFFF}};

    Mcl_SetDmaChannelCrcList(ChannelNumber, DmaCRCConfig, 2);
}

/**
 * @brief      Transfer an application image from Flash into destination memory
 *             with CRC computation on the fly.
 * @details    The application image is transferred by the DMA engine,
 *             which is configured to compute a CRC over the transferred data.
 *
 * @param      applicationId  Index in the Bootloader application array
 * @param      u8FragmentIdx  Index in the boot image fragments array
 *
 * @return     StatusType     E_OK or E_NOT_OK
 */
static StatusType Bl_LoadWithCRCAuth(uint8 applicationId, uint8 u8FragmentIdx)
{
    StatusType Status = E_NOT_OK;
    Bl_BootImageFragmentType *pImage = BL_GET_IMG_FRAGMENT_PTR(applicationId, u8FragmentIdx);
    uint32 storageAddress = pImage->u32FlashAddress;
    uint32 u32DestinationAddress = pImage->u32DestinationAddress;
    uint32 chunkSize = BL_ALIGN_8B(pImage->u32Size);

    /* Enable and link the CRC channel to DMA engine. */
    Bl_DmaCrcChannelEnable(DMA_LOGIC_CH_0);

    /* Start the DMA transfer using 8-byte transfer size. */
    Bl_StartDmaTransfer(storageAddress, u32DestinationAddress, BL_DMA_SIZE_8B,
                        chunkSize);

    /* Wait for DMA engine to transfer the application image. */
    if (E_OK == Bl_WaitApplicationFetch())
    {
        Status = E_OK;
    }

    return Status;
}
#endif /* BL_ENABLE_CRC_CHECK == STD_ON */

#if (BL_COM_BOOT_USED == STD_ON)
/**
 * @brief      Transfer an application image into destination memory,
 *             through an already configured COM session.
 * @details    The session is started and the application start address
 *             is given as input for the COM RX buffer.
 *             Once the session is complete, the number of bytes received
 *             is compared against the configured image size to validate
 *             the image.
 *
 * @param      u8ApplicationId  Index in the Bootloader application array
 * @param      u8FragmentIdx    Index in the boot image fragments array
 *
 * @return     StatusType       E_OK or E_NOT_OK
 */
static StatusType Bl_TransferImageViaCom(uint8 u8ApplicationId,
                                         uint8 u8FragmentIdx)
{
    Bl_BootImageFragmentType *pImage = BL_GET_IMG_FRAGMENT_PTR(u8ApplicationId, u8FragmentIdx);
    uint32 u32SessionId = pImage->u32ComSessionId;
    uint32 u32ImageSize = pImage->u32Size;
    uint32 u32DestinationAddress = pImage->u32DestinationAddress;
    uint32 u32RxImageSize = 0U;
    uint32 u32InitialTimeMs = 0U;
    uint32 u32DeltaMs = 0U;
    StatusType Status = E_NOT_OK;
    enum ComManager_SessionStatus SessionStatus;

    ComManager_SetRxBuffer(u32SessionId, (uint8 *) u32DestinationAddress);

    /* Move the session into the ready state. */
    ComManager_Main();
    if (COMMANAGER_OK == ComManager_StartSession(u32SessionId))
    {
        u32InitialTimeMs = SysDal_GetSystemTimeMs();
        SessionStatus = ComManager_GetSessionStatus(u32SessionId);
        do
        {
            ComDal_Main();
            SessionStatus = ComManager_GetSessionStatus(u32SessionId);
            u32DeltaMs = SysDal_GetSystemTimeMs() - u32InitialTimeMs;
        } while ((COMMANAGER_SESSION_BUSY == SessionStatus) &&
                 (u32DeltaMs < BL_COM_SESSION_TIMEOUT_MS));

#ifdef IFWK_USE_ETH
        /* Wait for the ACK to be received after sending the last SYN-ACK, once
         * the transfer is finished. */
        u32InitialTimeMs = SysDal_GetSystemTimeMs();
        do
        {
            /* Keeping the RX active until the PCB list is synced to avoid a
             * runtime error. */
            ComDal_Main();
            u32DeltaMs = SysDal_GetSystemTimeMs() - u32InitialTimeMs;
        } while (u32DeltaMs < COMMANAGER_TCP_TIMEOUT_MS);
#endif /* IFWK_USE_ETH */

        if (COMMANAGER_SESSION_COMPLETE == SessionStatus)
        {
            u32RxImageSize = ComManager_GetRxCount(u32SessionId);

            if (u32ImageSize == u32RxImageSize)
            {
                Status = E_OK;
            }
        }
    }

    if (E_OK == Status)
    {
        CONSOLE_LOG(CONSOLE_INFO,
                    "Application %d image fragment %d loaded from COM.\r\n",
                    u8ApplicationId, u8FragmentIdx);
    }
    else
    {
        CONSOLE_LOG(CONSOLE_ERROR,
                    "Application %d image fragment %d failed to load from COM.\r\n",
                    u8ApplicationId, u8FragmentIdx);
    }

    return Status;
}
#endif /* STD_ON == BL_COM_BOOT_USED */

#if (BL_BOOT_TRIGGER_ENABLED == STD_ON)
/**
 * @brief      Callback function for ComDal notification.
 *
 */
void Bl_ComDalNotification(void)
{
    Bl_TriggerReceived = TRUE;
}

/**
 * @brief Whether the boot trigger was received within the timeout window.
 *
 * @return boolean TRUE or FALSE
 */
static boolean Bl_WaitTriggerMessage(void)
{
    boolean is_received = FALSE;

    Gpt_StartTimer(GPT_TIMEOUT_CH, GPT_TIMEOUT_PERIOD);

    Gpt_ValueType ElapsedTime = 0U;
    while (GPT_TIMEOUT_PERIOD > ElapsedTime)
    {
        ElapsedTime = Gpt_GetTimeElapsed(GPT_TIMEOUT_CH);

        /* Calling the main function manually, due to the */
        /* absence of a scheduler in the Bootloader.  */
        ComDal_Main();

        if (TRUE == Bl_TriggerReceived)
        {
            is_received = TRUE;
            break;
        }
    }

    return is_received;
}
#endif /* BL_BOOT_TRIGGER_ENABLED == STD_ON */

/**
 * @brief      Gets the container partition for a target core.
 * @param      Core                     Target core ID
 *
 * @return     Bl_PlatformPartitionType The target core partition
 */
Bl_PlatformPartitionType BL_GetPartition(Bl_PlatformCoreType Core)
{
    return Bl_PlatformCoresInformation[Core].partition;
}

/**
 * @brief      Marks the partition used by the Bootloader as enabled.
 * @details    This function is called once,
 *             before enabling all the other cores.
 */
static void Bl_ActivateCurrentPartition(void)
{
    Bl_PlatformPartitionType partition;

    partition = BL_GetPartition(Platform_GetCoreIdentity());

    Bl_isPartitionEnabled[(uint8) partition] = PARTITION_ENABLED;
}

/**
 * @brief      Starts the application that will run on the Bootloader core.
 * @details    Changes the state of the exception level to hypervisor
 *             (only for A53) and starts an application by setting the PC
 *             to the reset exception handler address.
 *
 * @param      applicationConfig The application configuration
 */
static void Bl_StartApplicationOnCurrentCore(
    Bl_ApplicationDetails *applicationConfig)
{
    currentCoreBootImageAddress = applicationConfig->u32ResetHandler;

    Platform_GoToElevatedExecutionLevel(elevated_mode);
}

/**
 * @brief      Sets logical state of the partition given by user to the
 *             provided state value (enabled or disabled).
 * @details    When called, this function will flag a given partition
 *             as either enabled or disabled depending on the value of
 *             the state parameter.
 *
 * @param      partition   Partition Id
 * @param      state       PARTITION_ENABLED or PARTITION_DISABLED
 */
void Bl_SetPartitionState(Bl_PlatformPartitionType partition, uint8 state)
{
    if (MCU_MAX_NUMBER_OF_PARTITIONS > partition)
    {
        Bl_isPartitionEnabled[(uint8) partition] = state;
    }
}

/**
 * @brief      Return the partition state for the given partition index.
 * @details    This function returns PARTITION_ENABLED, if given partition
 *             was correctly provided and previously enabled, and
 *             PARTITION_DISABLED otherwise.
 *
 * @param      uint8              Partition Id
 *
 * @return     Bl_GetPartitionState PARTITION_ENABLED or PARTITION_DISABLED
 */
uint8 Bl_GetPartitionState(Bl_PlatformPartitionType partition)
{
    uint8 PartitionState = PARTITION_DISABLED;

    if (MCU_MAX_NUMBER_OF_PARTITIONS > partition)
    {
        PartitionState = Bl_isPartitionEnabled[(uint8) partition];
    }

    return PartitionState;
}

#if (STD_ON == BL_SDHC_ENABLED)
/**
 * @brief      Transfer an application image from SDHC into destination memory.
 *
 * @param      u8ApplicationId  Index in the Bootloader application array
 * @param      u8FragmentIdx    Index in the boot image fragments array
 *
 * @return     StatusType       E_OK or E_NOT_OK
 */
static StatusType Bl_LoadFromSdhc(uint8 u8ApplicationId, uint8 u8FragmentIdx)
{
    StatusType Status = E_NOT_OK;
    uint16 u16SdhcReadTimeout = 0U;
#if (STD_ON == BL_SUPPORT_AUTHENTICATION)
    Bl_ApplicationDetails *pAppDetail = BL_GET_CORE_CONFIG_PTR(u8ApplicationId);
#endif /* STD_ON == BL_SUPPORT_AUTHENTICATION */
    Bl_BootImageFragmentType *pImage = BL_GET_IMG_FRAGMENT_PTR(u8ApplicationId, u8FragmentIdx);
    uint32 u32DestinationAddress = pImage->u32DestinationAddress;
    MemDal_BlockIdType blockId = pImage->u16MemBlockId;

    Status = MemDal_ReadBlock(blockId, u32DestinationAddress);
    if (E_OK == Status)
    {
        do
        {
            MemDal_Main();

            u16SdhcReadTimeout++;
        } while ((MEMDAL_BLOCK_STATE_READY != MemDal_GetBlockStatus(blockId)) &&
                 (u16SdhcReadTimeout < SDHC_LOAD_TIMEOUT));

        /* Check if SDHC read timeout expired */
        if (u16SdhcReadTimeout >= SDHC_LOAD_TIMEOUT)
        {
            Status = E_NOT_OK;
        }
    }

#if (STD_ON == BL_SUPPORT_AUTHENTICATION)
    if ((E_OK == Status) &&
        ((SHA1 == pAppDetail->signAlgorithm) ||
         (SHA256 == pAppDetail->signAlgorithm)))
    {
        Status = Bl_AuthenticateImage(u8ApplicationId, u8FragmentIdx);
    }
#endif /* STD_ON == BL_SUPPORT_AUTHENTICATION */

    if (E_OK == Status)
    {
        CONSOLE_LOG(CONSOLE_INFO,
                    "Application %d image fragment %d loaded from SD card.\r\n",
                    u8ApplicationId, u8FragmentIdx);
    }
    else
    {
        CONSOLE_LOG(CONSOLE_ERROR,
                    "Application %d image fragment %d failed to load from SD card.\r\n",
                    u8ApplicationId, u8FragmentIdx);
    }

    return Status;
}
#endif /* STD_ON == BL_SDHC_ENABLED */

/**
 * @brief      Transfer an application image from Flash into destination memory.
 * @details    The user may select between SHA1, SHA256 or CRC authentication
 *             method or just upload the application image without
 *             any authentication.
 *
 * @param      u8ApplicationId  Index in the Bootloader application array
 * @param      u8FragmentIdx    Index in the boot image fragments array
 *
 * @return     StatusType       E_OK or E_NOT_OK
 */
static StatusType Bl_LoadAndAuthFromQspi(uint8 u8ApplicationId,
                                         uint8 u8FragmentIdx)
{
    StatusType Status = E_NOT_OK;
    Bl_ApplicationDetails *pAppDetail = BL_GET_CORE_CONFIG_PTR(u8ApplicationId);
    Bl_BootImageFragmentType *pImage = BL_GET_IMG_FRAGMENT_PTR(u8ApplicationId, u8FragmentIdx);

    switch (pAppDetail->signAlgorithm)
    {
    case NONE:
    {
        /* Fetch application image without authentication method */
        /* and copy to RAM. */
        Status = Bl_TransferImageFromQspi(u8ApplicationId, u8FragmentIdx);
    }
    break;
#if (STD_ON == BL_SUPPORT_AUTHENTICATION)
    case SHA1:
    case SHA256:
    {
        Status = Bl_AuthenticateImageHeader(u8ApplicationId, u8FragmentIdx);
        if (E_OK == Status)
        {
            /* Fetch application image using SHA1 or SHA256 */
            /* authentication method and copy to RAM. */
            Status = Bl_LoadWithSHAAuth(u8ApplicationId, u8FragmentIdx);
        }
    }
    break;
#endif /* STD_ON == BL_SUPPORT_AUTHENTICATION */

#if (BL_ENABLE_CRC_CHECK == STD_ON)
    case CRC32:
    {
        /* Fetch application image using CRC computation method */
        /* and copy to RAM. */
        Status = Bl_LoadWithCRCAuth(u8ApplicationId, u8FragmentIdx);
    }
    break;
#endif /* BL_ENABLE_CRC_CHECK == STD_ON */
    default:
        break;
    }

    if (E_OK == Status)
    {
        CONSOLE_LOG(CONSOLE_INFO,
                    "Application %d image fragment %d of size %d was loaded from QSPI, address 0x%h.\r\n",
                    u8ApplicationId, u8FragmentIdx, BL_ALIGN_64B(pImage->u32Size), pImage->u32FlashAddress);
    }
    else
    {
        CONSOLE_LOG(CONSOLE_ERROR,
                    "Application %d image fragment %d of size %d failed to load from QSPI, address 0x%h.\r\n",
                    u8ApplicationId, u8FragmentIdx, BL_ALIGN_64B(pImage->u32Size), pImage->u32FlashAddress);
    }

    return Status;
}

/**
 * @brief      Transfer an application image from a boot source
 *             (i.e Flash, SD or COM via ComManager) into destination memory.
 * @details    Initiate the image transfer depending on the boot source type.
 *             The final transfer address is also used for
 *             authenticating the image.
 *
 * @param      u8ApplicationId  Index in the Bootloader application array
 * @param      u8FragmentIdx    Index in the boot image fragments array
 *
 * @return     StatusType       E_OK or E_NOT_OK
 */
static StatusType Bl_FetchApplication(uint8 u8ApplicationId,
                                      uint8 u8FragmentIdx)
{
    StatusType Status = E_NOT_OK;
    Bl_ApplicationDetails *pAppDetail = BL_GET_CORE_CONFIG_PTR(u8ApplicationId);

#ifdef ENABLE_RUNTIME_MEASUREMENTS
    SysDal_Rtm_StartMeasurement(LOADANDAUTHAPPLICATION);
#endif /* ENABLE_RUNTIME_MEASUREMENTS */

    switch (pAppDetail->bootSource)
    {
    case BS_QSPI:
    {
        Status = Bl_LoadAndAuthFromQspi(u8ApplicationId, u8FragmentIdx);
        break;
    }
#if (STD_ON == BL_SDHC_ENABLED)
    case BS_SDMMC:
    {
        Status = Bl_LoadFromSdhc(u8ApplicationId, u8FragmentIdx);
        break;
    }
#endif /* STD_ON == BL_SDHC_ENABLED */

#if (STD_ON == BL_COM_BOOT_USED)
    case BS_COM:
    {
        Status = Bl_TransferImageViaCom(u8ApplicationId, u8FragmentIdx);

#if (STD_ON == BL_SUPPORT_AUTHENTICATION)
        if ((E_OK == Status) &&
            ((SHA1 == pAppDetail->signAlgorithm) ||
             (SHA256 == pAppDetail->signAlgorithm)))
        {
            Status = Bl_AuthenticateImage(u8ApplicationId, u8FragmentIdx);
        }
#endif /* STD_ON == BL_SUPPORT_AUTHENTICATION */

        break;
    }
#endif /* STD_ON == BL_COM_BOOT_USED */

    default:
        break;
    }

#ifdef ENABLE_RUNTIME_MEASUREMENTS
    SysDal_Rtm_StopMeasurement(LOADANDAUTHAPPLICATION);
    SysDal_Rtm_GetOneshotMeasResult(LOADANDAUTHAPPLICATION, &Bl_LoadAndAuthTime);
    CONSOLE_LOG(CONSOLE_INFO,
                "Application %d image fragment %d was loaded in %d.%d milliseconds.\r\n",
                 u8ApplicationId, u8FragmentIdx,
                 Bl_LoadAndAuthTime.Value.u16Milliseconds,
                 Bl_LoadAndAuthTime.Value.u16Microseconds);
#endif /* ENABLE_RUNTIME_MEASUREMENTS */

    return Status;
}

/**
 * @brief      Loads an application into destination memory.
 * @details    Encapsulates the steps needed for loading and checking
 *             a bootable application.
 *
 * @param      applicationId  Identifier for the configured application
 *                            to be loaded
 *
 * @return     StatusType     E_OK or E_NOT_OK
 */
static StatusType Bl_LoadApplication(uint8 applicationId)
{
    StatusType Status = E_NOT_OK;
    Bl_ApplicationDetails *pAppDetail = BL_GET_CORE_CONFIG_PTR(applicationId);
    uint8 u8ImgNumber = 0U;

    for (u8ImgNumber = 0;
         u8ImgNumber < pAppDetail->u8ImagesNumber;
         u8ImgNumber++)
    {
        Status = Bl_FetchApplication(applicationId, u8ImgNumber);
        if (E_OK == Status)
        {
#if (BL_ENABLE_CRC_CHECK == STD_ON)
            if (CRC32 == pAppDetail->signAlgorithm)
            {
                Status = Bl_CheckApplicationCrc(applicationId, u8ImgNumber);
            }
#endif /* BL_ENABLE_CRC_CHECK == STD_ON */

#if (BL_IMAGE_DECRYPTION_ENABLED == STD_ON)
            if (E_OK == Status)
            {
                Status = Bl_DecryptApplication(applicationId, u8ImgNumber);
            }
#endif /* BL_IMAGE_DECRYPTION_ENABLED == STD_ON */
        }

        if (E_OK != Status)
        {
            break;
        }
    }

    return Status;
}

/**
 * @brief      Determine the reset reason of the MCU.
 * @details    The reset reason is obtained by reading the RGM_DES, RGM_FES
 *             and MC_ME_MODE_STAT registers.
 *
 * @return     uint8  The reset reason given as a power of 2
 */
static Bl_ResetReasonType Bl_GetResetReason(void)
{
    uint32 rgm_des_val = REG_READ32(RGM_DES);
    uint32 rgm_fes_val = REG_READ32(RGM_FES);
    uint32 mc_me_mode_stat_val = REG_READ32(MC_ME_MODE_STAT);
    Bl_ResetReasonType reset_reason = BL_UNKNOWN_RESET_REASON_MASK;

    if (0x0 == (rgm_des_val ^ RGM_DES_POR))
    {
        if (0x0U == rgm_fes_val)
        {
            /* Only F_POR is set, then it's a POR. */
            reset_reason = BL_POR_RESET_REASON_MASK;
        }
    }
    else if (rgm_des_val & RGM_DES_DESTRUCTIVE_RESET_MASK)
    {
        if ((0x0U == (rgm_des_val & RGM_DES_POR)) &&
            (0x0U == (rgm_fes_val & RGM_FES_FUNCTIONAL_RESET_MASK)))
        {
            /* If any F_DR flag is set, then it's a destructive reset. 
               This requires having the F_POR bit not set and the F_FR flags not set */
            reset_reason = BL_DESTRUCTIVE_RESET_REASON_MASK;
        }
    }
    else if (rgm_fes_val & RGM_FES_FUNCTIONAL_RESET_MASK)
    {
        if (0x0U == rgm_des_val)
        {
            /* If any F_FR flag is set, then it's a functional reset.
               This requires having the F_POR bit not set and the F_DR flags not set */
            reset_reason = BL_FUNCTIONAL_RESET_REASON_MASK;
        }
    }
    else if (mc_me_mode_stat_val & MC_ME_MODE_STAT_PREVMODE)
    {
        /* If PREVIOUS_MODE flag is set, then it's a standby reset. */
        reset_reason = BL_STANDBY_RESET_REASON_MASK;
    }

    return reset_reason;
}

/**
 * @brief      Get current core application ID.
 * @details    This function iterates through all applications and
 *             sets the global variable currentCoreApplicationId with
 *             the value of the application identifier running on the
 *             current core.
 */
static void Bl_DetermineCurrentCoreAppId(void)
{
    uint8 u8Index;
    Bl_PlatformCoreType currentCoreId = Platform_GetCoreIdentity();

    for (u8Index = 0; u8Index < BL_GET_CORE_CONFIGS_NMB; u8Index++)
    {
        Bl_ApplicationDetails *pAppDetail = BL_GET_CORE_CONFIG_PTR(u8Index);
        if (currentCoreId == pAppDetail->core)
        {
            currentCoreApplicationId = u8Index;
            break;
        }
    }
}

#if (BL_SYNCHRONIZED_BOOT == STD_ON)
/**
 * @brief      Starts all applications at the same time.
 * @details    This function iterates through all applications and starts them
 *             one by one.
 *             The applications must have been loaded to their destination
 *             memory beforehand.
 */
static void Bl_StartAllApplications(void)
{
    uint8 u8Index;

    for (u8Index = 0; u8Index < BL_GET_CORE_CONFIGS_NMB; u8Index++)
    {
        Bl_ApplicationDetails *pAppDetail = BL_GET_CORE_CONFIG_PTR(u8Index);
        if ((currentCoreApplicationId != u8Index) &&
            (E_OK == Bl_AppStatus[u8Index]) &&
            (TRUE == pAppDetail->bStartCore))
        {
            Bl_StartApplication(pAppDetail);
        }
    }

    if (BL_APP_NOT_CONFIGURED != currentCoreApplicationId)
    {
        Bl_ApplicationDetails *pAppDetail = BL_GET_CORE_CONFIG_PTR(currentCoreApplicationId);
        if ((E_OK == Bl_AppStatus[currentCoreApplicationId]) &&
            (TRUE == pAppDetail->bStartCore))
        {
            /* In case the application runs on the same core as the */
            /* Bootloader the application is started last, */
            /* so it does not interfere with the Bootloader. */
            Bl_StartApplicationOnCurrentCore(pAppDetail);
        }
    }
}
#endif /* BL_SYNCHRONIZED_BOOT == STD_ON */

/**
 * @brief      Loads all applications into memory and starts them.
 * @details    This function iterates through all applications and loads
 *             them into memory and starts them synchronously or asynchronously.
 *
 * @return     StatusType E_OK or E_NOT_OK
 */
StatusType Bl_BootApplications(void)
{
    StatusType Status = E_OK;
    uint8 u8Index;

    /* Check if current core has any application configured */
    Bl_DetermineCurrentCoreAppId();

    for (u8Index = 0; u8Index < BL_GET_CORE_CONFIGS_NMB; u8Index++)
    {
        Bl_ApplicationDetails *pAppDetail = BL_GET_CORE_CONFIG_PTR(u8Index);
#ifdef BL_PRE_SETUP_REQUIRED
        /* Platform specific pre load setups */
        Bl_PreSetup(pAppDetail);
#endif /* BL_PRE_SETUP_REQUIRED */

        /* Load the application from the storage memory */
        /* and check its integrity. */
        Bl_AppStatus[u8Index] = Bl_LoadApplication(u8Index);

        if (E_OK != Bl_AppStatus[u8Index])
        {
            if (TRUE == pAppDetail->bIsCriticalApp)
            {
                CONSOLE_LOG(
                    CONSOLE_ERROR,
                    "Critical application %s failed to load."
                    "The boot process is stopped.\r\n",
                    Bl_PlatformCoreNames[pAppDetail->core]);
                Status = E_NOT_OK;
                break;
            }
            else
            {
                CONSOLE_LOG(
                    CONSOLE_ERROR,
                    "Non-critical application %s failed to load.\r\n",
                    Bl_PlatformCoreNames[pAppDetail->core]);
            }
        }
        else
        {
            CONSOLE_LOG(CONSOLE_INFO, "Application %s was loaded.\r\n",
                Bl_PlatformCoreNames[pAppDetail->core]);
#if (BL_SYNCHRONIZED_BOOT == STD_OFF)
            /* Start the application on the configured core, */
            /* immediately after it has been loaded. */
            if ((u8Index != currentCoreApplicationId) &&
                (pAppDetail->bStartCore == TRUE))
            {
                Bl_StartApplication(pAppDetail);
            }
#endif /* BL_SYNCHRONIZED_BOOT == STD_OFF */
        }
    }

    /* De-init the peripherals and disable all interrupts, */
    /* after all the applications have been loaded. */
    SysDal_DeInit();
    SysDal_DisableAllInterruptSources();

    if (E_OK == Status)
    {
#if (BL_SYNCHRONIZED_BOOT == STD_ON)
        Bl_StartAllApplications();
#else  /* BL_SYNCHRONIZED_BOOT == STD_ON */
        /* Start the current core application last. */
        if (BL_APP_NOT_CONFIGURED != currentCoreApplicationId)
        {
            Bl_ApplicationDetails *pAppDetail = BL_GET_CORE_CONFIG_PTR(currentCoreApplicationId);
            if ((E_OK == Bl_AppStatus[currentCoreApplicationId]) &&
                (TRUE == pAppDetail->bStartCore))
            {
                Bl_StartApplicationOnCurrentCore(pAppDetail);
            }
        }
#endif /* BL_SYNCHRONIZED_BOOT == STD_ON */
    }

    return Status;
}

#if (BL_ENABLE_DYNAMIC_BOOT == STD_ON)
/**
 * @brief      Copy the BootTable from the storage memory to the SRAM memory
 *             using the DMA driver
 * @return     StatusType  E_OK or E_NOT_OK
 */
static StatusType Bl_Fetch_BootTable(void)
{
    uint32 u32TransferSize = BL_ALIGN_64B(sizeof(Bl_DynamicBootConfig));

    Bl_StartDmaTransfer(BL_BOOT_CONFIG_ADDR, (uint32)dynamicConfig,
                        BL_DMA_SIZE_64B, u32TransferSize);

    /* Wait for DMA engine to transfer the application image */
    if (E_OK == Bl_WaitApplicationFetch())
    {
        return E_OK;
    }

    return E_NOT_OK;
}
#endif /* BL_ENABLE_DYNAMIC_BOOT == STD_ON */

/**
 * @brief      Activate current partition, wait for trigger message
 *             and start the application boot flow.
 *
 * @return     StatusType  E_OK or E_NOT_OK
 */
StatusType Bl_Run(void)
{
    StatusType Status = E_NOT_OK;

    Bl_ActivateCurrentPartition();

    Bl_ResetReasonType reset_reason = Bl_GetResetReason();
    if (reset_reason & Bl_AcceptedResetReason)
    {
        boolean trigger_received = TRUE;

#if (BL_BOOT_TRIGGER_ENABLED == STD_ON)
        trigger_received = Bl_WaitTriggerMessage();
#endif /* BL_BOOT_TRIGGER_ENABLED == STD_ON */

        if (TRUE == trigger_received)
        {
            Status = Bl_BootApplications();
        }

#if (BL_USE_BREAKPOINT == STD_ON)
        /* Prevent compiler optimization, due to the variable being unused. */
        ENABLE_BREAKPOINT_AT_MAIN = 0;
#endif /* BL_USE_BREAKPOINT == STD_ON */
    }

    return Status;
}

#if (BL_ENABLE_SECURE_BOOT == STD_ON)
/**
 * @brief      Check secure boot configuration.
 * @details    Check whether secure boot was configured previously.
 * @return     StatusType  E_OK or E_NOT_OK
 */
static StatusType Bl_IsSecureBootActive(void)
{
    StatusType Status = E_OK;

    if (IVT_BCW_SEC_BOOT_CONFIG_MASK !=
        (pIVTConfig.u32BootCfgWord & IVT_BCW_SEC_BOOT_CONFIG_MASK))
    {
        /* Secure boot is not active */
        Status = E_NOT_OK;
    }

    return Status;
}

/**
 * @brief      Saves the updated Secure Boot configuration to flash.
 * @details    After a set of keys, Secure Memory Regions and Core Reset
 *             entries have been registered in HSE, the configuration must
 *             be stored and indicated to HSE via IVT configuration.
 *             The pSysImage shall contain all the Key Catalogs
 *             and SMR/CR entries stored using a device encryption key.
 * @param      pSysImage              Pointer to the System image, saved in RAM
 * @param      u32SysImageStorageAddr System Image storage address
 * @param      u32SysImageOffset      System Image offset, in case an update
 *                                    is performed
 * @param      u32SysImageSize        System image size
 */
static Std_ReturnType Bl_SaveConfiguration(
    uint8 *pSysImage, uint32 u32SysImageStorageAddr,
    uint32 u32SysImageOffset, uint32 u32SysImageSize)
{
    Std_ReturnType FlsStatus = E_NOT_OK;
    Bl_ImageVectorTableType *pIvtUpdated;
    uint32 u32SysImageSizeAligned = BL_ALIGN_1024B(u32SysImageSize);

    /* SYS_IMAGE erase */
    FlsStatus = Fls_Erase(u32SysImageStorageAddr + u32SysImageOffset, u32SysImageSizeAligned);
    while (MEMIF_IDLE != Fls_GetStatus())
    {
        Fls_MainFunction();
    }
    if ((E_OK != FlsStatus) || (MEMIF_JOB_OK != Fls_GetJobResult()))
    {
        return E_NOT_OK;
    }

    /* SYS_IMAGE write */
    FlsStatus = Fls_Write(u32SysImageStorageAddr + u32SysImageOffset,
                            (const uint8 *) pSysImage, u32SysImageSizeAligned);
    while (MEMIF_IDLE != Fls_GetStatus())
    {
        Fls_MainFunction();
    }
    if ((E_OK != FlsStatus) || (MEMIF_JOB_OK != Fls_GetJobResult()))
    {
        return E_NOT_OK;
    }

    /* Update IVT configuration */
    MemLib_MemCpy(&Bl_FlashMirror[0], IVT_ADDR, QSPI_SECTOR_SIZE);
    pIvtUpdated = (Bl_ImageVectorTableType *) &Bl_FlashMirror[0];
    pIvtUpdated->u32BootCfgWord = IVT_BCW_SEC_BOOT_CONFIG_MASK;
    pIvtUpdated->u32SysImageAddr = u32SysImageStorageAddr;
    pIvtUpdated->u32ExtFlashType = IVT_ADDR;
    pIvtUpdated->u32FlashPageSize = QSPI_SECTOR_SIZE;

    /* IVT HEADER erase */
    FlsStatus = Fls_Erase(IVT_ADDR, QSPI_SECTOR_SIZE);
    while (MEMIF_IDLE != Fls_GetStatus())
    {
        Fls_MainFunction();
    }
    if ((E_OK != FlsStatus) || (MEMIF_JOB_OK != Fls_GetJobResult()))
    {
        return E_NOT_OK;
    }

    FlsStatus = Fls_Write(IVT_ADDR, (const uint8 *) &Bl_FlashMirror[0], QSPI_SECTOR_SIZE);
    while (MEMIF_IDLE != Fls_GetStatus())
    {
        Fls_MainFunction();
    }

    if ((E_OK != FlsStatus) || (MEMIF_JOB_OK != Fls_GetJobResult()))
    {
        return E_NOT_OK;
    }
    else
    {
        return E_OK;
    }
}

/**
 * @brief      Configure secure-boot for the Bootloader application
 * @details    This function configures the secure boot for the Bootloader application image
 *             using the CMAC method and the secure boot for the Boot Table using a RSA method
 * @return     StatusType  E_OK or E_NOT_OK
 */
static StatusType Bl_Configure_Bootloader_SecureBoot(void)
{
    StatusType status = E_OK;
    CryptoDal_SMR_CR_EntryType SMR_CR_Config ={0};
    CryptoDal_SMR_EntryType SMR_Config[2] = {0};
    CryptoDal_CR_EntryType CR_Config = {0};
    uint32 u32AppTagSize;
    uint8 aAppTag[CRYPTODAL_CMAC_TAG_SIZE] = {0};
    Bl_AppImageCfgType *Bl_ImageCfg = (Bl_AppImageCfgType*)pIVTConfig.u32AppAddr;
    uint32 u32BootAppFlashAddr = pIVTConfig.u32AppAddr + BOOT_IMAGE_HEADER_SIZE;
    uint32 u32BootAppRamAddr = Bl_ImageCfg->u32RamAddr;
    uint32 u32BootAppSize = Bl_ImageCfg->u32Size;
    uint32 u32KeyHandle;

    /* Generate Bootloader image signature */
    u32AppTagSize = CRYPTODAL_CMAC_TAG_SIZE;
    status = CryptoDal_GenerateCmac((uint8*)u32BootAppFlashAddr, aAppTag,
        BL_APP_SEC_BOOT_KEY_INDEX, u32BootAppSize, &u32AppTagSize);
    if (status == E_NOT_OK)
    {
        return E_NOT_OK;
    }

    /* Configure Bootloader SMR data */
    u32KeyHandle = CryptoDal_GetKeyHandle(BL_APP_SEC_BOOT_KEY_INDEX, CRYPTODAL_SECONDARY_KEY);
    SMR_Config[0].u8ConfigFlags = HSE_SMR_CFG_FLAG_QSPI_FLASH;
    SMR_Config[0].u32KeyHandle = u32KeyHandle;
    SMR_Config[0].u32SourceAddr = u32BootAppFlashAddr;
    SMR_Config[0].u32DestinationAddr = u32BootAppRamAddr;
    SMR_Config[0].u32AppSize = u32BootAppSize;
    SMR_Config[0].u8EntryIndex = BL_BOOTLOADER_SMR_ID;
    SMR_Config[0].u32Signature = (uint32)aAppTag;
    SMR_Config[0].u16SignatureLength = (uint16)u32AppTagSize;
    SMR_Config[0].authScheme = Bl_CmacAuthScheme;

    CR_Config.u32PreSmrMap |= (1 << BL_BOOTLOADER_SMR_ID);
    SMR_CR_Config.u8SmrNumber++;

#if (BL_ENABLE_DYNAMIC_BOOT == STD_ON)
    /* Configure Boot Table SMR data */
    u32KeyHandle = CryptoDal_GetKeyHandle(BL_CFG_SEC_BOOT_KEY_INDEX, CRYPTODAL_PRIMARY_KEY);
    SMR_Config[1].u8ConfigFlags = (HSE_SMR_CFG_FLAG_QSPI_FLASH|HSE_SMR_CFG_FLAG_INSTALL_AUTH);
    SMR_Config[1].u32KeyHandle = u32KeyHandle;
    SMR_Config[1].u32SourceAddr = BL_BOOT_CONFIG_ADDR;
    SMR_Config[1].u32DestinationAddr = (uint32)dynamicConfig;
    SMR_Config[1].u32AppSize = sizeof(Bl_DynamicBootConfig);
    SMR_Config[1].u8EntryIndex = BL_BOOT_TABLE_SMR_ID;
    SMR_Config[1].u32Signature = BL_BOOT_CONFIG_ADDR + sizeof(Bl_DynamicBootConfig);
    SMR_Config[1].u16SignatureLength = CRYPTODAL_RSA_2048_KEY_LENGTH;
    SMR_Config[1].authScheme = Bl_RsaAuthScheme;

    CR_Config.u32PreSmrMap |= (1 << BL_BOOT_TABLE_SMR_ID);
    SMR_CR_Config.u8SmrNumber++;
#endif /* BL_ENABLE_DYNAMIC_BOOT == STD_ON */

    CR_Config.u32ResetAddr = u32BootAppRamAddr;
    CR_Config.u8CrEntryIndex = 0U;
    CR_Config.u8CoreID = HSE_APP_CORE0;

    SMR_CR_Config.pSmrCfg = &SMR_Config[0];
    SMR_CR_Config.pCrCfg = &CR_Config;

    status = CryptoDal_SMR_CR_Install(&SMR_CR_Config);
    if (status == E_NOT_OK)
    {
        return E_NOT_OK;
    }

#if (BL_ENABLE_DYNAMIC_BOOT == STD_ON)
    /* The Boot Table SMR must be triggered because the information
       contained in this table is used to install applications SMRs */
    return CryptoDal_VerifySmr(BL_BOOT_TABLE_SMR_ID);
#endif /* BL_ENABLE_DYNAMIC_BOOT == STD_ON */
}

/**
 * @brief      Configure secure-boot for all application images
 * @details    This function configures the secure boot for each application
 *             image using the CMAC method
 * @return     StatusType  E_OK or E_NOT_OK
 */
static StatusType Bl_Configure_Applications_SecureBoot(void)
{
    StatusType status = E_OK;
    Bl_ApplicationDetails * pAppDetail = NULL_PTR;
    Bl_BootImageFragmentType * pImage = NULL_PTR;
    CryptoDal_SMR_CR_EntryType SMR_CR_Config ={0};
    CryptoDal_SMR_EntryType SMR_Config[BL_SMR_IMAGES_NUMBER] = {0};
    uint8 aAppTag[BL_SMR_IMAGES_NUMBER][CRYPTODAL_CMAC_TAG_SIZE] = {0};
    uint32 u32KeyHandle;
    uint32 u32AppTagSize;
    uint8 u8IndexFragment = 0;
    uint8 u8IndexApp = 0;
    uint8 u8SmrIndex = 0;

    for (u8IndexApp = 0; u8IndexApp < CONFIGURED_CORES_COUNT ; u8IndexApp++)
    {
        pAppDetail = BL_GET_CORE_CONFIG_PTR(u8IndexApp);
        for (u8IndexFragment = 0; u8IndexFragment < pAppDetail->u8ImagesNumber; u8IndexFragment++)
        {
            /* Get image fragment */
            pImage = BL_GET_IMG_FRAGMENT_PTR(u8IndexApp, u8IndexFragment);

            if (pImage->u8SMRIndex) /* if non zero SMR index add to SMR table*/
            {
                /* Generate application image signature */
                u32AppTagSize = CRYPTODAL_CMAC_TAG_SIZE;
                status = CryptoDal_GenerateCmac((uint8*)pImage->u32FlashAddress, aAppTag[u8SmrIndex],
                    BL_APP_SEC_BOOT_KEY_INDEX, pImage->u32Size, &u32AppTagSize);
                if (status == E_NOT_OK)
                {
                    return E_NOT_OK;
                }

                /* Configure SMR data for the current application fragment */
                u32KeyHandle = CryptoDal_GetKeyHandle(BL_APP_SEC_BOOT_KEY_INDEX, CRYPTODAL_SECONDARY_KEY);
                SMR_Config[u8SmrIndex].u8ConfigFlags = HSE_SMR_CFG_FLAG_QSPI_FLASH;
                SMR_Config[u8SmrIndex].u32KeyHandle = u32KeyHandle;
                SMR_Config[u8SmrIndex].u32SourceAddr = pImage->u32FlashAddress;
                SMR_Config[u8SmrIndex].u32DestinationAddr = pImage->u32DestinationAddress;
                SMR_Config[u8SmrIndex].u32AppSize = pImage->u32Size;
                SMR_Config[u8SmrIndex].u8EntryIndex = pImage->u8SMRIndex;
                SMR_Config[u8SmrIndex].u32Signature = (uint32)aAppTag[u8SmrIndex];
                SMR_Config[u8SmrIndex].u16SignatureLength = (uint16)u32AppTagSize;
                SMR_Config[u8SmrIndex].authScheme = Bl_CmacAuthScheme;

                u8SmrIndex++;
            }
        }
    }

    SMR_CR_Config.pSmrCfg = &SMR_Config[0];
    SMR_CR_Config.pCrCfg = NULL_PTR;
    SMR_CR_Config.u8SmrNumber = u8SmrIndex;

    return CryptoDal_SMR_CR_Install(&SMR_CR_Config);
}

/**
 * @brief      Update HSE System Image
 * @details    The new SysImg contains configuration of Crypto keys and SMR-CR entries
 * @return     StatusType  E_OK or E_NOT_OK
 */
static StatusType Bl_Update_HSE_SysImg(void)
{
    uint32 u32SysImageOffSet;
    uint32 u32SysImageSize = HSE_SYS_IMAGE_SIZE;

    if (E_OK == CryptoDal_GetSysImage(&Bl_HseSysImage[0], &u32SysImageOffSet, &u32SysImageSize))
    {
        return Bl_SaveConfiguration(&Bl_HseSysImage[0], pIVTConfig.u32SysImageAddr,
                                    u32SysImageOffSet, u32SysImageSize);
    }

    return E_NOT_OK;
}

/**
 * @brief      Configures secure boot for the Bootloader image.
 * @details    This function generates a signature for the Bootloader image,
 *             registers the Bootloader flash image as a Secure Memory Region
 *             and ties it to a specific core (i.e. M7_0).
 *             After the SMR has been registered, the Bootloader saves
 *             the configuration containing the SMR and Keys to flash,
 *             in order to be fetched by HSE at the next power on reset.
 *             If the procedure has been successful, the function issues
 *             a power-on-reset.
 * @return     StatusType  E_OK or E_NOT_OK
 */
static StatusType Bl_ConfigureSecureBoot(void)
{
    StatusType status = E_OK;

#ifdef ENABLE_RUNTIME_MEASUREMENTS
    SysDal_Rtm_OneshotResultType Bl_ConfigureSecureBootTime;
#endif /* ENABLE_RUNTIME_MEASUREMENTS */

    MemLib_MemCpy(&pIVTConfig, IVT_ADDR, sizeof(Bl_ImageVectorTableType));

    if (E_NOT_OK == Bl_IsSecureBootActive())
    {
        CONSOLE_LOG(CONSOLE_INFO, "Secure boot activation in progress.\r\n");

#ifdef ENABLE_RUNTIME_MEASUREMENTS
        SysDal_Rtm_StartMeasurement(CONFIGURESECUREBOOT);
#endif /* ENABLE_RUNTIME_MEASUREMENTS */

        status = Bl_Configure_Bootloader_SecureBoot();
        if (status == E_NOT_OK)
        {
            return E_NOT_OK;
        }

        status = Bl_Configure_Applications_SecureBoot();
        if (status == E_NOT_OK)
        {
            return E_NOT_OK;
        }

        status = Bl_Update_HSE_SysImg();
        if (status == E_NOT_OK)
        {
            return E_NOT_OK;
        }

#ifdef ENABLE_RUNTIME_MEASUREMENTS
        SysDal_Rtm_StopMeasurement(CONFIGURESECUREBOOT);
        SysDal_Rtm_GetOneshotMeasResult(CONFIGURESECUREBOOT,
                                        &Bl_ConfigureSecureBootTime);

        CONSOLE_LOG(CONSOLE_INFO,
                    "Secure boot activation finished in %d.%d milliseconds.\r\n\r\n",
                    Bl_ConfigureSecureBootTime.Value.u16Milliseconds,
                    Bl_ConfigureSecureBootTime.Value.u16Microseconds);
#else /* ENABLE_RUNTIME_MEASUREMENTS */
        CONSOLE_LOG(CONSOLE_INFO, "Secure boot activation finished.\r\n");
#endif /* ENABLE_RUNTIME_MEASUREMENTS */

        Mcu_PerformReset();
    }

    return status;
}
#endif /* BL_ENABLE_SECURE_BOOT == STD_ON */

#ifndef UNIT_TESTING
/**
 * @brief      The entry-point for the Bootloader.
 *
 */
int main(void)
{
#if (BL_USE_BREAKPOINT == STD_ON)
    while (0U == ENABLE_BREAKPOINT_AT_MAIN) continue;
#endif /* BL_USE_BREAKPOINT == STD_ON */

    SysDal_Init();

    Bl_ConfigureDDR();

#if (BL_USE_SAFETY == STD_ON)
    Bl_SafetySelectMode();
#endif /* BL_USE_SAFETY == STD_ON */

#if (BL_ENABLE_SECURE_BOOT == STD_ON)
    if (E_OK != Bl_ConfigureSecureBoot())
    {
        CONSOLE_LOG(CONSOLE_INFO, "Bootloader secure boot initialization failed.\r\n");
        while(1){}
    }
    else
    {
        CONSOLE_LOG(CONSOLE_INFO, "Bootloader secure boot is enabled.\r\n");
    }
#elif (BL_ENABLE_DYNAMIC_BOOT == STD_ON)
    if (E_OK != Bl_Fetch_BootTable())
    {
        while(1){}
    }
#endif /* BL_ENABLE_SECURE_BOOT == STD_ON */

    CONSOLE_LOG(CONSOLE_INFO, "Bootloader initialization complete.\r\n");

    Bl_Run();

    SysDal_DisableAllInterruptSources();
    while (1)
    {
        /* Allow the current running core to be waked up. */
        BL_WAIT_FOR_INTERRUPT();
    }
}
#endif /* UNIT_TESTING */
