/*================================================================================================*/
/*
*   @file               ComDal_Ipc.c
*   project             AUTOSAR 4.4 GOLDVIP
*   platform            CORTEXM

*   @version            24.12.0
*
*   @brief              Integration Framework - ComDal Abstraction Layer IPC component
*   @details            This file implements the ComDal Inter-processor Communication interface.
*
*   Autosar Version     4.4.0
*   Build Version       S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*   All Rights Reserved.
*/
/*================================================================================================*/

#ifdef __cplusplus
extern "C"{
#endif

#include "Std_Types.h"
#include "Mcal.h"

#include "ComDal_Drv_Cfg.h"

#ifdef IFWK_USE_IPC
#include "ComDal_Types.h"
#include "ComDal_Cfg.h"
#include "ComDal_Ipc.h"
#include "MemLib.h"
#include "typedefs.h"
#include "ipc-shm.h"

#if (defined(S32G2XX) || defined(S32Z27X))
#include "ipcf_Ip_Cfg.h"
#endif

/*====================================================================================================================
*                              SOURCE FILE VERSION INFORMATION
=====================================================================================================================*/
#define COMDAL_IPC_VENDOR_ID_C                       43

#define COMDAL_IPC_AR_RELEASE_MAJOR_VERSION_C        4
#define COMDAL_IPC_AR_RELEASE_MINOR_VERSION_C        4
#define COMDAL_IPC_AR_RELEASE_REVISION_VERSION_C     0

#define COMDAL_IPC_SW_MAJOR_VERSION_C                24
#define COMDAL_IPC_SW_MINOR_VERSION_C                12
#define COMDAL_IPC_SW_PATCH_VERSION_C                0

/*====================================================================================================================
*                                     FILE VERSION CHECKS
=====================================================================================================================*/
#if (COMDAL_MAX_IPC_DESCRIPTORS > 0U)
/* Check if source file and Comdal configuration header file are of the same vendor */
#if (COMDAL_IPC_VENDOR_ID_C != COMDAL_CFG_VENDOR_ID)
    #error "ComDal_Ipc.c and ComDal_Cfg.h have different vendor ids"
#endif

/* Check if source file and Comdal configuration header file are of the same Autosar version */
#if ((COMDAL_IPC_AR_RELEASE_MAJOR_VERSION_C != COMDAL_CFG_AR_RELEASE_MAJOR_VERSION) || \
     (COMDAL_IPC_AR_RELEASE_MINOR_VERSION_C != COMDAL_CFG_AR_RELEASE_MINOR_VERSION) || \
     (COMDAL_IPC_AR_RELEASE_REVISION_VERSION_C != COMDAL_CFG_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of ComDal_Ipc.c and ComDal_Cfg.h are different"
#endif

/* Check if source file and Comdal configuration header file are of the same Software version */
#if ((COMDAL_IPC_SW_MAJOR_VERSION_C != COMDAL_CFG_SW_MAJOR_VERSION) || \
     (COMDAL_IPC_SW_MINOR_VERSION_C != COMDAL_CFG_SW_MINOR_VERSION) || \
     (COMDAL_IPC_SW_PATCH_VERSION_C != COMDAL_CFG_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of ComDal_Ipc.c and ComDal_Cfg.h are different"
#endif

/* Check if source file and Comdal types header file are of the same vendor */
#if (COMDAL_IPC_VENDOR_ID_C != COMDAL_TYPES_VENDOR_ID)
    #error "ComDal_Ipc.c and ComDal_Types.h have different vendor ids"
#endif

/* Check if source file and Comdal types header file are of the same Autosar version */
#if ((COMDAL_IPC_AR_RELEASE_MAJOR_VERSION_C != COMDAL_TYPES_AR_RELEASE_MAJOR_VERSION) || \
     (COMDAL_IPC_AR_RELEASE_MINOR_VERSION_C != COMDAL_TYPES_AR_RELEASE_MINOR_VERSION) || \
     (COMDAL_IPC_AR_RELEASE_REVISION_VERSION_C != COMDAL_TYPES_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of ComDal_Ipc.c and ComDal_Types.h are different"
#endif

/* Check if source file and Comdal types header file are of the same Software version */
#if ((COMDAL_IPC_SW_MAJOR_VERSION_C != COMDAL_TYPES_SW_MAJOR_VERSION) || \
     (COMDAL_IPC_SW_MINOR_VERSION_C != COMDAL_TYPES_SW_MINOR_VERSION) || \
     (COMDAL_IPC_SW_PATCH_VERSION_C != COMDAL_TYPES_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of ComDal_Ipc.c and ComDal_Types.h are different"
#endif

/* Check if source file and Comdal Can header file are of the same Autosar version */
#if ((COMDAL_IPC_AR_RELEASE_MAJOR_VERSION_C != COMDAL_IPC_AR_RELEASE_MAJOR_VERSION) || \
     (COMDAL_IPC_AR_RELEASE_MINOR_VERSION_C != COMDAL_IPC_AR_RELEASE_MINOR_VERSION) || \
     (COMDAL_IPC_AR_RELEASE_REVISION_VERSION_C != COMDAL_IPC_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of ComDal_Ipc.c and ComDal_Ipc.h are different"
#endif

/* Check if source file and Comdal Can header file are of the same Software version */
#if ((COMDAL_IPC_SW_MAJOR_VERSION_C != COMDAL_IPC_SW_MAJOR_VERSION) || \
     (COMDAL_IPC_SW_MINOR_VERSION_C != COMDAL_IPC_SW_MINOR_VERSION) || \
     (COMDAL_IPC_SW_PATCH_VERSION_C != COMDAL_IPC_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of ComDal_Ipc.c and ComDal_Ipc.h are different"
#endif

#ifndef DISABLE_MCAL_INTERMODULE_ASR_CHECK
/* Check if source file and Std_Types header file are of the same AutoSar version */
#if ((COMDAL_IPC_AR_RELEASE_MAJOR_VERSION_C != STD_AR_RELEASE_MAJOR_VERSION) || \
      (COMDAL_IPC_AR_RELEASE_MINOR_VERSION_C != STD_AR_RELEASE_MINOR_VERSION) \
     )
    #error "AutoSar Version Numbers of ComDal_Ipc.c and Std_Types.h are different"
#endif

/* Check if source file and Mcal header file are of the same AutoSar version */
#if ((COMDAL_IPC_AR_RELEASE_MAJOR_VERSION_C != MCAL_AR_RELEASE_MAJOR_VERSION) || \
      (COMDAL_IPC_AR_RELEASE_MINOR_VERSION_C != MCAL_AR_RELEASE_MINOR_VERSION) \
     )
    #error "AutoSar Version Numbers of ComDal_Ipc.c and Mcal.h are different"
#endif
#endif
#endif

/*=================================================================================================================================
                                        LOCAL MACROS
===================================================================================================================================*/

/* ComDal IPC buffer pool count */
#define COMDAL_IPC_POOL_COUNT                1U

/* ComDal IPC buffer size */
#define COMDAL_IPC_BUFFER_SIZE               128U

/* ComDal IPC buffer count */
#define COMDAL_IPC_BUFFER_COUNT              10U

/* ComDal IPC channel count */
#define COMDAL_IPC_CHANNEL_COUNT             2U

/* ComDal IPC instance */
#define COMDAL_IPC_INSTANCE                  0U

/*=================================================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
===================================================================================================================================*/
/* IPC states */
typedef enum
{
    COMDAL_IPC_INITIALIZED  = 0U,    /* IPC is initialized. */
    COMDAL_IPC_ERROR,                /* IPC has encountered an error. */
    COMDAL_IPC_UNINITIALIZED = 255   /* IPC is not initialized. */
} ComDal_IpcStateType;

/* IPC communication states */
typedef enum
{
    COMDAL_IPC_IDLE = 0U,            /* IPC is in IDLE state, waiting for communication */
    COMDAL_IPC_DATA_RECEIVED         /* Data was received */
} ComDal_IpcCommunicationStateType;

/*=================================================================================================================================
                                       GLOBAL CONSTANTS
===================================================================================================================================*/
CONST(void*, AUTOMATIC) rx_cb_arg = NULL_PTR;

/*=================================================================================================================================
                                       GLOBAL VARIABLES
===================================================================================================================================*/
static VAR(ComDal_IpcStateType, AUTOMATIC) ComDal_IpcState = COMDAL_IPC_UNINITIALIZED;
static VAR(ComDal_IpcCommunicationStateType, AUTOMATIC) ComDal_IpcCommunicationState = COMDAL_IPC_IDLE;
static VAR(char, AUTOMATIC) ComDal_IpcRxBuffer[COMDAL_IPC_BUFFER_SIZE];
static VAR(size_t, AUTOMATIC) ComDal_IpcRxBufferSize = 0;
extern FUNC(void, COMDAL_APP_CODE) Sys_Init_SRAM(int ComDal_IpcStartAddress, unsigned int size);
/*=================================================================================================================================
                                        LOCAL FUNCTIONS
===================================================================================================================================*/
#if (defined(S32G2XX) || defined(S32Z27X))
static FUNC(StatusType, COMDAL_APP_CODE) ComDal_Ipc_IpcInit(void);
#else
static FUNC(StatusType, COMDAL_APP_CODE) ComDal_Ipc_CustomInit(void);
#endif

/*=================================================================================================================================*/
/**
@brief      ComDal_Ipc_ComDal_RxCallback() - ComDal_Ipc read callback function
@details    This function is called when there is data available for reading over the Ipc shared memory channel

@param[in] CallbackArguments    Pointer to callback arguments
@param[in] ChannelId            The Ipc shared memory channel id
@param[in] Buffer               Pointer to received buffer
@param[in] BufferSize           The size of the received buffer

@return None
*/
/*=================================================================================================================================*/
FUNC(void, AUTOMATIC) ComDal_Ipc_RxCallback(
    P2VAR(void, AUTOMATIC, COMDAL_APPL_DATA) CallbackArguments,
    CONST(uint8_t, AUTOMATIC) u8Instance,
    VAR(int, AUTOMATIC) ChannelId,
    P2VAR(void, AUTOMATIC, COMDAL_APPL_DATA) Buffer,
    VAR(size_t, AUTOMATIC) BufferSize)
{
    (void)CallbackArguments;
    (void)u8Instance;

    /* Check for buffer overflow */
    if (COMDAL_IPC_BUFFER_SIZE > BufferSize)
    {
        MemLib_MemCpy(ComDal_IpcRxBuffer, Buffer, BufferSize);
        ComDal_IpcRxBufferSize = BufferSize;

        /* Release the buffer */
        if (E_OK != ipc_shm_release_buf(u8Instance, ChannelId, Buffer))
        {
            ComDal_IpcState = COMDAL_IPC_ERROR;
        }
        else
        {
            ComDal_IpcCommunicationState = COMDAL_IPC_DATA_RECEIVED;
        }
    }
}

#if (defined(S32G2XX) || defined(S32Z27X))
/*=================================================================================================================================*/
/**
@brief      ComDal_Ipc_IpcInit() - Initialization of IPC using own configuration structure
@details    This function shall initialise the Ipc shared memory device using Ipc framework configuration structure

@param[in]  none

@return None
*/
/*=================================================================================================================================*/
static FUNC(StatusType, COMDAL_APP_CODE) ComDal_Ipc_IpcInit(void)
{
    #ifdef S32G2XX
    P2VAR(struct ipc_shm_cfg, AUTOMATIC, COMDAL_APPL_DATA) shm_cfg;

    shm_cfg = ipcf_shm_instances_cfg.shm_cfg;

    /* Initialize local and remote SRAM memory */
    Sys_Init_SRAM(shm_cfg->local_shm_addr, shm_cfg->shm_size);
    Sys_Init_SRAM(shm_cfg->remote_shm_addr, shm_cfg->shm_size);

    /* Initialize Ipc shared memory device with the settings from the IPCF plugin */
    return ipc_shm_init(&ipcf_shm_instances_cfg);
    #endif /* S32G2XX */

    #ifdef S32Z27X
    if (E_OK != ipc_shm_init(&ipcf_shm_instances_cfg))
    {
        return E_NOT_OK;
    }
    /* wait for remote core to initialize the shm driver */
    while (ipc_shm_is_remote_ready(COMDAL_IPC_INSTANCE) != 0)
    {
        /* busy wait */
    }
    return E_OK;
    #endif /* S32Z27X */


}
#else
/*=================================================================================================================================*/
/**
@brief      ComDal_Ipc_CustomInit() - Initialization of IPC using custom configuration structure
@details    This function shall initialise the Ipc shared memory device using a custom configuration
            from ComDal->ComExtDevices->IpcExtConfig
@param[in]  none

@return None
*/
/*=================================================================================================================================*/
static FUNC(StatusType, COMDAL_APP_CODE) ComDal_Ipc_CustomInit(void)
{
     /* Shared memory buffer pool parameters */
    struct ipc_shm_pool_cfg IpcShmBufferPools[COMDAL_IPC_POOL_COUNT];
    struct ipc_shm_channel_cfg IpcShmChannels[COMDAL_IPC_CHANNEL_COUNT];
    struct ipc_shm_cfg IpcShmConfiguration;
    struct ipc_shm_instances_cfg ipcf_shm_cfgs;

    ipcf_shm_cfgs.num_instances = 1u;
    ipcf_shm_cfgs.shm_cfg = &IpcShmConfiguration;

    IpcShmBufferPools[0].num_bufs = COMDAL_IPC_BUFFER_COUNT;
    IpcShmBufferPools[0].buf_size = COMDAL_IPC_BUFFER_SIZE;

    /* Shared memory channel parameters */
    IpcShmChannels[0].type                 = IPC_SHM_MANAGED;
    IpcShmChannels[0].ch.managed.num_pools = COMDAL_IPC_POOL_COUNT;
    IpcShmChannels[0].ch.managed.pools     = IpcShmBufferPools;
    IpcShmChannels[0].ch.managed.rx_cb     = ComDal_Ipc_RxCallback;
    IpcShmChannels[0].ch.managed.cb_arg    = NULL_PTR;

    IpcShmChannels[1].type                 = IPC_SHM_MANAGED;
    IpcShmChannels[1].ch.managed.num_pools = COMDAL_IPC_POOL_COUNT;
    IpcShmChannels[1].ch.managed.pools     = IpcShmBufferPools;
    IpcShmChannels[1].ch.managed.rx_cb     = ComDal_Ipc_RxCallback;
    IpcShmChannels[1].ch.managed.cb_arg    = NULL_PTR;

    /* IPC shared memory configuration */
    IpcShmConfiguration.local_shm_addr    = COMDAL_IPC_SHM_LOCAL_ADDRESS;
    IpcShmConfiguration.remote_shm_addr   = COMDAL_IPC_SHM_REMOTE_ADDRESS;
    IpcShmConfiguration.inter_core_rx_irq = COMDAL_IPC_SHM_RX_IRQ;
    IpcShmConfiguration.inter_core_tx_irq = COMDAL_IPC_SHM_TX_IRQ;
    IpcShmConfiguration.remote_core.type  = COMDAL_IPC_SHM_REMOTE_CORE_TYPE;
    IpcShmConfiguration.remote_core.index = COMDAL_IPC_SHM_REMOTE_CORE_ID;
    IpcShmConfiguration.shm_size          = COMDAL_IPC_SHM_SIZE;
    IpcShmConfiguration.num_channels      = COMDAL_IPC_CHANNEL_COUNT;
    IpcShmConfiguration.channels          = IpcShmChannels;

    /* Initialize local and remote SRAM memory */
    Sys_Init_SRAM(COMDAL_IPC_SHM_LOCAL_ADDRESS, COMDAL_IPC_SHM_SIZE);
    Sys_Init_SRAM(COMDAL_IPC_SHM_REMOTE_ADDRESS, COMDAL_IPC_SHM_SIZE);

    /* Initialize Ipc shared memory device */
    return ipc_shm_init(&ipcf_shm_cfgs);
}
#endif /* if (defined(S32G2XX) || defined(S32Z27X)) */

/*=================================================================================================================================*/
/**
@brief      ComDal_Ipc_Init() - Initialization function for ComDal_Ipc component.
@details    This function shall initialise the Ipc shared memory device

@param[in]  ConfigPtr    Pointer to configuration data (this data is shared between all ComDal components)

@return None
*/
/*=================================================================================================================================*/
FUNC(void, COMDAL_APP_CODE) ComDal_Ipc_Init(P2CONST(ComDal_ConfigType, AUTOMATIC, COMDAL_APPL_CONST) ConfigPtr)
{
    (void)ConfigPtr;

    /* Initialize Ipc shared memory device */
    #if (defined(S32G2XX) || defined(S32Z27X))
    if (E_OK != ComDal_Ipc_IpcInit())
    #else
    if (E_OK != ComDal_Ipc_CustomInit())
    #endif /* if (defined(S32G2XX) || defined(S32Z27X)) */
    {
        ComDal_IpcState = COMDAL_IPC_ERROR;
    }
    else
    {
        ComDal_IpcState = COMDAL_IPC_INITIALIZED;
    }
}

/*=================================================================================================================================*/
/**
@brief      ComDal_Ipc_DeInit() - De-Initialization function for ComDal_Ipc component.
@details    This function calls Ipc_shm_free() function to release the Ipc shared
            memory device.

@return None
*/
/*=================================================================================================================================*/
FUNC(void, COMDAL_APP_CODE) ComDal_Ipc_DeInit(void)
{
    ipc_shm_free();
    ComDal_IpcState  = COMDAL_IPC_UNINITIALIZED;
}

/*=================================================================================================================================*/
/**
@brief      ComDal_Ipc_ReadData - API used to provide data that was read from the IPC lines.
@details    This function will provide users with the data received on the Ipc shared memory channel.
            This function shall return:
                COMDAL_BUSY:     if no new Ipc message was received for that channel;
                COMDAL_INVALID:  if COMDAL was not properly initialized;
                COMDAL_OVERFLOW: if RxDataLen > than COMDAL_IPC_BUFFER_SIZE;
                COMDAL_NO_ERROR: if the data received was updated;

@param[in]  ComChannel     Logical BSW ComDal channel id.
@param[in]  RxDataLen      Maximum data length provided for that channel
@param[out] ReadBuffer     Data read from IPC for the assigned input channel.

@return     ComDal_ReturnType: COMDAL_NO_ERROR, COMDAL_BUSY, COMDAL_INVALID, COMDAL_OVERFLOW
*/
/*=================================================================================================================================*/
FUNC(ComDal_ReturnType, COMDAL_APP_CODE) ComDal_Ipc_ReadData(
    VAR(ComDal_ChannelType, AUTOMATIC) ComChannel,
    P2VAR(char, AUTOMATIC, COMDAL_APPL_DATA) ReadBuffer,
    VAR(uint16, AUTOMATIC) RxDataLen)
{
    VAR(uint16, AUTOMATIC) MsgIndex = 0;
    VAR(ComDal_ReturnType, AUTOMATIC) ReturnVal = COMDAL_NO_ERROR;

    (void)ComChannel;

    if (COMDAL_IPC_INITIALIZED == ComDal_IpcState)
    {
        if (NULL_PTR != ReadBuffer)
        {
            if (COMDAL_IPC_DATA_RECEIVED == ComDal_IpcCommunicationState)
            {
                if (COMDAL_IPC_BUFFER_SIZE > RxDataLen)
                {
                    for (MsgIndex = 0; MsgIndex < RxDataLen; MsgIndex++)
                    {
                        ReadBuffer[MsgIndex] = ComDal_IpcRxBuffer[MsgIndex];
                    }
                    ComDal_IpcCommunicationState = COMDAL_IPC_IDLE;
                }
                else
                {
                    ReturnVal = COMDAL_OVERFLOW;
                }
            }
            else
            {
                ReturnVal = COMDAL_BUSY;
            }
        }
        else
        {
            ReturnVal = COMDAL_INVALID;
        }
    }
    else
    {
        ReturnVal = COMDAL_INVALID;
    }

    return ReturnVal;
}

/*=================================================================================================================================*/
/**
@brief      ComDal_Ipc_WriteData - API called from upper layer to send data over the IPC shared memory channels.
@details

@param[in]  ComChannel     Logical BSW ComDal channel id.
@param[in]  TxDataLen      Maximum data length provided for that channel
@param[out] TxData         Data to be written on IPC for the assigned input channel.

@return     none
*/
/*=================================================================================================================================*/
FUNC(void, COMDAL_APP_CODE) ComDal_Ipc_WriteData(
    VAR(ComDal_ChannelType, AUTOMATIC) ComChannel,
    P2VAR(char, AUTOMATIC, COMDAL_APPL_DATA) TxData,
    VAR(uint16, AUTOMATIC) TxDataLen)
{
    VAR(const uint16, AUTOMATIC) ChannelIndex = 0;
    P2VAR(char, AUTOMATIC, FW_APPL_DATA) Buffer = NULL_PTR;

    (void)ComChannel;

    if (COMDAL_IPC_INITIALIZED == ComDal_IpcState)
    {
        Buffer = ipc_shm_acquire_buf(COMDAL_IPC_INSTANCE, ChannelIndex, TxDataLen);

        if (NULL_PTR != Buffer)
        {
            MemLib_MemCpy(Buffer, TxData, TxDataLen);
            if (E_OK != ipc_shm_tx(COMDAL_IPC_INSTANCE, ChannelIndex, Buffer, TxDataLen))
            {
                ComDal_IpcState = COMDAL_IPC_ERROR;
            }
        }
    }
}

#endif /* COMDAL_MAX_IPC_DESCRIPTORS > 0U */

#ifdef __cplusplus
}
#endif
