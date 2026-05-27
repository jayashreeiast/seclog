/*===================================================================================================*/
/*
*   @file              ComDal.c
*   project            AUTOSAR 4.4 GOLDVIP
*   platform           CORTEXM
*
*   @brief             Integration Framework - ComDal Abstraction Layer component
*   @details           This file implements the ComDal interface to application layer
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
#include "Mcal.h"
#include "Compiler.h"

#include "ComDal_Types.h"
#include "ComDal_Cfg.h"
#include "ComDal.h"
#include "ComDal_Can.h"
#include "ComDal_Nfc.h"
#include "ComDal_Uart.h"

#ifdef IFWK_USE_IPC
#include "ComDal_Ipc.h"
#endif /* IFWK_USE_IPC */

#ifdef IFWK_USE_ETH
#include "ComDal_Eth.h"
#endif /* IFWK_USE_ETH */

#ifdef IFWK_USE_LLCE
#include "ComDal_Llce.h"
#endif /* IFWK_USE_LLCE */

#if (COMDAL_DEV_ERROR_DETECT == STD_ON)
#include "Det.h"
#endif /* COMDAL_DEV_ERROR_DETECT == STD_ON */

#include "Console.h"

/*===================================================================================================*/
/*                              SOURCE FILE VERSION INFORMATION                                      */
/*===================================================================================================*/
#define COMDAL_VENDOR_ID_C                       43

#define COMDAL_AR_RELEASE_MAJOR_VERSION_C        4
#define COMDAL_AR_RELEASE_MINOR_VERSION_C        4
#define COMDAL_AR_RELEASE_REVISION_VERSION_C     0

#define COMDAL_SW_MAJOR_VERSION_C                24
#define COMDAL_SW_MINOR_VERSION_C                12
#define COMDAL_SW_PATCH_VERSION_C                0

/*===================================================================================================*/
/*                                      FILE VERSION CHECKS                                          */
/*===================================================================================================*/

/* Check if source file and ComDal header file are of the same vendor */
#if (COMDAL_VENDOR_ID_C != COMDAL_VENDOR_ID)
    #error "ComDal.c and ComDal.h have different vendor ids"
#endif

/* Check if source file and ComDal header file are of the same Autosar version */
#if ((COMDAL_AR_RELEASE_MAJOR_VERSION_C != COMDAL_AR_RELEASE_MAJOR_VERSION) || \
     (COMDAL_AR_RELEASE_MINOR_VERSION_C != COMDAL_AR_RELEASE_MINOR_VERSION) || \
     (COMDAL_AR_RELEASE_REVISION_VERSION_C != COMDAL_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of ComDal.c and ComDal.h are different"
#endif

/* Check if source file and ComDal header file are of the same Software version */
#if ((COMDAL_SW_MAJOR_VERSION_C != COMDAL_SW_MAJOR_VERSION) || \
     (COMDAL_SW_MINOR_VERSION_C != COMDAL_SW_MINOR_VERSION) || \
     (COMDAL_SW_PATCH_VERSION_C != COMDAL_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of ComDal.c and ComDal.h are different"
#endif

/* Check if source file and ComDal Types header file are from the same vendor */
#if (COMDAL_VENDOR_ID_C != COMDAL_TYPES_VENDOR_ID)
    #error "ComDal.c and ComDal_Types.h have different vendor ids"
#endif

/* Check if source file and ComDal Types header file are of the same Autosar version */
#if ((COMDAL_AR_RELEASE_MAJOR_VERSION_C != COMDAL_TYPES_AR_RELEASE_MAJOR_VERSION) || \
     (COMDAL_AR_RELEASE_MINOR_VERSION_C != COMDAL_TYPES_AR_RELEASE_MINOR_VERSION) || \
     (COMDAL_AR_RELEASE_REVISION_VERSION_C != COMDAL_TYPES_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of ComDal.c and ComDal_Types.h are different"
#endif

/* Check if source file and ComDal Types headerr file are of the same Software version */
#if ((COMDAL_SW_MAJOR_VERSION_C != COMDAL_TYPES_SW_MAJOR_VERSION) || \
     (COMDAL_SW_MINOR_VERSION_C != COMDAL_TYPES_SW_MINOR_VERSION) || \
     (COMDAL_SW_PATCH_VERSION_C != COMDAL_TYPES_SW_PATCH_VERSION) \
    )
  #error "Software Version Numbers of ComDal.c and ComDal_Types.h are different"
#endif

 /* Check if source file and DET header file are of the same version */
#if (COMDAL_DEV_ERROR_DETECT == STD_ON)
    #if ((COMDAL_AR_RELEASE_MAJOR_VERSION_C != DET_AR_RELEASE_MAJOR_VERSION) || \
         (COMDAL_AR_RELEASE_MINOR_VERSION_C != DET_AR_RELEASE_MINOR_VERSION) \
        )
    #error "AutoSar Version Numbers of ComDal.c and Det.h are different"
    #endif
#endif

/* Check if source file and ComDal configuration header file are of the same vendor */
#if (COMDAL_VENDOR_ID_C != COMDAL_CFG_VENDOR_ID)
    #error "ComDal.c and ComDal_Cfg.h have different vendor ids"
#endif

/* Check if source file and ComDal configuration header file are of the same Autosar version */
#if ((COMDAL_AR_RELEASE_MAJOR_VERSION_C != COMDAL_CFG_AR_RELEASE_MAJOR_VERSION) || \
     (COMDAL_AR_RELEASE_MINOR_VERSION_C != COMDAL_CFG_AR_RELEASE_MINOR_VERSION) || \
     (COMDAL_AR_RELEASE_REVISION_VERSION_C != COMDAL_CFG_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of ComDal.c and ComDal_Cfg.h are different"
#endif

/* Check if source file and ComDal configuration header file are of the same software version */
#if ((COMDAL_SW_MAJOR_VERSION_C != COMDAL_CFG_SW_MAJOR_VERSION) || \
     (COMDAL_SW_MINOR_VERSION_C != COMDAL_CFG_SW_MINOR_VERSION) || \
     (COMDAL_SW_PATCH_VERSION_C != COMDAL_CFG_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of ComDal.c and ComDal_Cfg.h are different"
#endif

/* Check if source file and ComDal Can header file are of the same vendor */
#if (COMDAL_VENDOR_ID_C != COMDAL_CAN_VENDOR_ID)
    #error "ComDal.c and ComDal_Can.h have different vendor ids"
#endif

/* Check if source file and ComDal Can header file are of the same Autosar version */
#if ((COMDAL_AR_RELEASE_MAJOR_VERSION_C != COMDAL_CAN_AR_RELEASE_MAJOR_VERSION) || \
     (COMDAL_AR_RELEASE_MINOR_VERSION_C != COMDAL_CAN_AR_RELEASE_MINOR_VERSION) || \
     (COMDAL_AR_RELEASE_REVISION_VERSION_C != COMDAL_CAN_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of ComDal.c and ComDal_Can.h are different"
#endif

/* Check if source file and ComDal Can header file are of the same software version */
#if ((COMDAL_SW_MAJOR_VERSION_C != COMDAL_CAN_SW_MAJOR_VERSION) || \
     (COMDAL_SW_MINOR_VERSION_C != COMDAL_CAN_SW_MINOR_VERSION) || \
     (COMDAL_SW_PATCH_VERSION_C != COMDAL_CAN_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of ComDal.c and ComDal_Can.h are different"
#endif

/* Check if source file and ComDal Uart header file are of the same vendor */
#if (COMDAL_VENDOR_ID_C != COMDAL_UART_VENDOR_ID)
    #error "ComDal.c and ComDal_Uart.h have different vendor ids"
#endif

/* Check if source file and ComDal Uart header file are of the same Autosar version */
#if ((COMDAL_AR_RELEASE_MAJOR_VERSION_C != COMDAL_UART_AR_RELEASE_MAJOR_VERSION) || \
     (COMDAL_AR_RELEASE_MINOR_VERSION_C != COMDAL_UART_AR_RELEASE_MINOR_VERSION) || \
     (COMDAL_AR_RELEASE_REVISION_VERSION_C != COMDAL_UART_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of ComDal.c and ComDal_Uart.h are different"
#endif

/* Check if source file and ComDal Uart header file are of the same software version */
#if ((COMDAL_SW_MAJOR_VERSION_C != COMDAL_UART_SW_MAJOR_VERSION) || \
     (COMDAL_SW_MINOR_VERSION_C != COMDAL_UART_SW_MINOR_VERSION) || \
     (COMDAL_SW_PATCH_VERSION_C != COMDAL_UART_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of ComDal.c and ComDal_Uart.h are different"
#endif

/*===================================================================================================*/
/*                               LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)                          */
/*===================================================================================================*/
typedef enum
{
    COMDAL_INIT     = 0U,     /**< @brief mark No error detected for analog descriptor. */
    COMDAL_DEINIT,            /**< @brief mark conversion on-going for analog descriptor. */
    COMDAL_UNINIT             /**< @brief mark conversion error detected for analog descriptor. */
}ComDal_aeModuleStateType;

/*===================================================================================================*/
/*                                            GLOBAL CONSTANTS                                       */
/*===================================================================================================*/
/**
* @brief          Global Configuration Pointer.
* @details        Data structure containing the set of configuration parameters required for
*                 initializing the COMDAL components.
*
*/
P2CONST(ComDal_ConfigType, COMDAL_VAR, COMDAL_APP_CONST) ComDal_pGlobalConfigPtr = NULL_PTR;


/**
* @brief          Variable storing ComDal module state
* @details        Possible values: UNIT, INIT, DEINIT
*/
static VAR(ComDal_aeModuleStateType,  AUTOMATIC) ComDal_aeModuleState = COMDAL_UNINIT;
/*===================================================================================================*/
/*                                    LOCAL FUNCTION PROTOTYPES                                      */
/*===================================================================================================*/

static INLINE FUNC(Std_ReturnType, COMDAL_APP_CODE) ComDal_ValidateComChannel
(
    VAR(ComDal_ChannelType, AUTOMATIC) ComChannel,
    VAR(uint8, AUTOMATIC) ApiId
);

/*===================================================================================================*/
/*                                      LOCAL FUNCTIONS                                              */
/*===================================================================================================*/

/*===================================================================================================*/
/**
@brief      ComDal_Init - Initialization function for Communications Driver abstraction module
@details    Shall be used to initialize all subcomponents

@param[in]  ConfigPtr    Pointer to configuration data (this data is shared between all CAMDAL components)

@return     None
*/
/*===================================================================================================*/
FUNC(void, COMDAL_APP_CODE) ComDal_Init(P2CONST(ComDal_ConfigType, AUTOMATIC, COMDAL_APP_CONST) ConfigPtr)
{
    if(COMDAL_INIT != ComDal_aeModuleState)
    {
        if(NULL_PTR != ConfigPtr)
        {
            ComDal_pGlobalConfigPtr = ConfigPtr;

#if (COMDAL_MAX_CAN_DESCRIPTORS > 0U)
            /* Initialize all CAN channels. */
            if(0U != ComDal_pGlobalConfigPtr->CanChannelCount)
            {
                ComDal_Can_Init(ComDal_pGlobalConfigPtr);
            }
#endif /* COMDAL_MAX_CAN_DESCRIPTORS > 0U */

#if (COMDAL_MAX_LLCE_DESCRIPTORS > 0U)
            /* Initialize all LLCE channels. */
            if(0U != ComDal_pGlobalConfigPtr->LlceChannelCount)
            {
                ComDal_Llce_Init(ComDal_pGlobalConfigPtr);
            }
#endif /* COMDAL_MAX_LLCE_DESCRIPTORS > 0U */

#if (COMDAL_MAX_LIN_DESCRIPTORS > 0U)
            /* Initialize all LIN channels. */
            if(0U != ComDal_pGlobalConfigPtr->LinChannelCount)
            {
                ComDal_Lin_Init(ComDal_pGlobalConfigPtr);
            }
#endif /* COMDAL_MAX_LIN_DESCRIPTORS > 0U */

#if (COMDAL_MAX_ETH_INTERFACES > 0U)
            /* Initialize the ETH interfaces separately from the channels. */
            if(0U != ComDal_pGlobalConfigPtr->u8EthInterfaceCount)
            {
                ComDal_Eth_Init(ComDal_pGlobalConfigPtr);
            }
#endif /* COMDAL_MAX_ETH_INTERFACES > 0U */

#if (COMDAL_MAX_UART_DESCRIPTORS > 0U)
            /* Initialize all UART channels. */
            if(0U != ComDal_pGlobalConfigPtr->UartChannelCount)
            {
                ComDal_Uart_Init(ComDal_pGlobalConfigPtr);
            }
#endif /* COMDAL_MAX_UART_DESCRIPTORS > 0U */

#if (COMDAL_MAX_NFC_DESCRIPTORS > 0U)
            /* Initialize the NFC component. */
            if(0U != ComDal_pGlobalConfigPtr->NfcChannelCount)
            {
                ComDal_Nfc_Init(ComDal_pGlobalConfigPtr);
            }
#endif /* COMDAL_MAX_NFC_DESCRIPTORS > 0U */

#if (COMDAL_MAX_IPC_DESCRIPTORS > 0U)
            /* Initialize the IPC component. */
            if(0U != ComDal_pGlobalConfigPtr->IpcChannelCount)
            {
                ComDal_Ipc_Init(ComDal_pGlobalConfigPtr);
            }
#endif /* COMDAL_MAX_IPC_DESCRIPTORS > 0U */

            ComDal_aeModuleState = COMDAL_INIT;
            CONSOLE_LOG(CONSOLE_INFO, "ComDal_Init() done.\r\n");
        }
        else
        {
#if (COMDAL_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError((uint16)COMDAL_MODULE_ID, (uint8)0U, COMDAL_INIT_API_ID, COMDAL_E_PARAM_CONFIG);
#endif /* COMDAL_DEV_ERROR_DETECT == STD_ON */

        }
    }
    else
    {
#if (COMDAL_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError((uint16)COMDAL_MODULE_ID, (uint8)0U, COMDAL_INIT_API_ID, COMDAL_E_ALREADY_INITIALIZED);
#endif /* COMDAL_DEV_ERROR_DETECT == STD_ON */

    }
}

/*===================================================================================================*/
/**
@brief      ComDal_DeInit - De-Initialization function for Communications Driver abstraction module
@details    Shall be used to deinitialize all subcomponents

@return     None
*/
/*===================================================================================================*/
FUNC(void, COMDAL_APP_CODE) ComDal_DeInit(void)
{
    if(COMDAL_DEINIT != ComDal_aeModuleState)
    {
#if (COMDAL_MAX_CAN_DESCRIPTORS > 0U)
        /* Deinitialize all CAN channels. */
        if (0U != ComDal_pGlobalConfigPtr->CanChannelCount)
        {
            ComDal_Can_DeInit();
        }
#endif /* COMDAL_MAX_CAN_DESCRIPTORS > 0U */

#if (COMDAL_MAX_LLCE_DESCRIPTORS > 0U)
        /* Initialize all LLCE channels. */
        if(0U != ComDal_pGlobalConfigPtr->LlceChannelCount)
        {
            ComDal_Llce_DeInit();
        }
#endif /* COMDAL_MAX_LLCE_DESCRIPTORS > 0U */

#if (COMDAL_MAX_LIN_DESCRIPTORS > 0U)
        /* Deinitialize all LIN channels. */
        if (ComDal_pGlobalConfigPtr->LinChannelCount)
        {
            ComDal_Lin_DeInit();
        }
#endif /* COMDAL_MAX_LIN_DESCRIPTORS > 0U */

#if (COMDAL_MAX_ETH_INTERFACES > 0U)
        /* Deinitialize all ETH interfaces. */
        if (0U != ComDal_pGlobalConfigPtr->u8EthInterfaceCount)
        {
            ComDal_Eth_DeInit();
        }
#endif /* COMDAL_MAX_ETH_INTERFACES > 0U */

#if(COMDAL_MAX_UART_DESCRIPTORS > 0U)
        /* Deinitialize all UART channels. */
        if (0U != ComDal_pGlobalConfigPtr->UartChannelCount)
        {
            ComDal_Uart_DeInit();
        }
#endif /* COMDAL_MAX_UART_DESCRIPTORS > 0U */

#if(COMDAL_MAX_NFC_DESCRIPTORS > 0U)
        /* Deinitialize the NFC component. */
        if (0U != ComDal_pGlobalConfigPtr->NfcChannelCount)
        {
            ComDal_Nfc_DeInit();
        }
#endif /* COMDAL_MAX_NFC_DESCRIPTORS > 0U */

#if(COMDAL_MAX_IPC_DESCRIPTORS > 0U)
        /* Deinitialize the IPC component. */
        if (0U != ComDal_pGlobalConfigPtr->IpcChannelCount)
        {
            ComDal_Ipc_DeInit();
        }
#endif /* COMDAL_MAX_IPC_DESCRIPTORS > 0U */

        ComDal_pGlobalConfigPtr = NULL_PTR;
        ComDal_aeModuleState = COMDAL_DEINIT;
    }
    else
    {
#if(COMDAL_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError((uint16)COMDAL_MODULE_ID, (uint8)0U, COMDAL_DEINIT_API_ID, COMDAL_E_UNINIT);
#endif /* COMDAL_DEV_ERROR_DETECT == STD_ON */
    }
}

/*===================================================================================================*/
/**
@brief      ComDal_Main - Main function for COMDAL
@details    Shall be used to handle cyclic functions for each COMDAL channel

@return     None
*/
/*===================================================================================================*/
FUNC(void, COMDAL_APP_CODE) ComDal_Main(void)
{
    VAR(uint8, AUTOMATIC)  ComDalChannel;
    VAR(ComDal_BswChannelType, AUTOMATIC)  ChannelType;
    VAR(uint8, AUTOMATIC)  MaxComChannels = ComDal_pGlobalConfigPtr->CanChannelCount + ComDal_pGlobalConfigPtr->LinChannelCount +
                                            ComDal_pGlobalConfigPtr->UartChannelCount + ComDal_pGlobalConfigPtr->NfcChannelCount + ComDal_pGlobalConfigPtr->IpcChannelCount + ComDal_pGlobalConfigPtr->LlceChannelCount;

    if(COMDAL_INIT == ComDal_aeModuleState)
    {
#if (COMDAL_MAX_ETH_INTERFACES > 0U)
        ComDal_Eth_Main();
#endif /* COMDAL_MAX_ETH_INTERFACES > 0U */

        for(ComDalChannel = 0U; ComDalChannel < MaxComChannels; ComDalChannel++)
        {
            ChannelType = ((ComDal_pGlobalConfigPtr->pComChannelMap))[ComDalChannel].ComChannelType;

            switch(ChannelType)
            {
#if(COMDAL_MAX_UART_DESCRIPTORS > 0U)
                case COMDAL_UART_CHANNEL:
                {
                    ComDal_Uart_Main(ComDalChannel);
                }
                break;
#endif /* COMDAL_MAX_UART_DESCRIPTORS > 0U */

#if(COMDAL_MAX_CAN_DESCRIPTORS > 0U)
                case COMDAL_CAN_CHANNEL:
                {
                    ComDal_Can_Main(ComDalChannel);
                }
                break;
#endif /* COMDAL_MAX_CAN_DESCRIPTORS > 0U */

#if(COMDAL_MAX_LLCE_DESCRIPTORS > 0U)
                case COMDAL_LLCE_CHANNEL:
                {
                    ComDal_Llce_Main(ComDalChannel);
                }
                break;
#endif /* COMDAL_MAX_LLCE_DESCRIPTORS > 0U */

#if(COMDAL_MAX_NFC_DESCRIPTORS > 0U)
                case COMDAL_NFC_CHANNEL:
                {
                    ComDal_Nfc_Main(ComDalChannel);
                }
                break;
#endif /* COMDAL_MAX_NFC_DESCRIPTORS > 0U */

                default:
                {
#if(COMDAL_DEV_ERROR_DETECT == STD_ON)
                    Det_ReportError((uint16)COMDAL_MODULE_ID, (uint8)0U, COMDAL_MAINFUNCTION_API_ID, COMDAL_E_INVALID_CHANNEL_TYPE);
#endif /* COMDAL_DEV_ERROR_DETECT == STD_ON */
                }
                break;
            }
        }
    }
    else
    {
        /*call Det: module not initialized*/
#if(COMDAL_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError((uint16)COMDAL_MODULE_ID, (uint8)0U, COMDAL_MAINFUNCTION_API_ID, COMDAL_E_UNINIT);
#endif /* COMDAL_DEV_ERROR_DETECT == STD_ON */
    }
}

/*===================================================================================================*/
/**
@brief      ComDal_ControllerReset - controller reset
@details    Shall be used to reset the NFC controller, overcoming hardware limitations
            NFC controller power line is kept on GND for 20 milliseconds.

@return     None

@pre None
@post None
*/
/*===================================================================================================*/
FUNC(void, COMDAL_APP_CODE) ComDal_ControllerReset(void)
{
#if(COMDAL_MAX_NFC_DESCRIPTORS > 0U)
    ComDal_Nfc_Controller_Reset();
#endif /* COMDAL_MAX_NFC_DESCRIPTORS > 0U */
}

/*===================================================================================================*/
/**
@brief      ComDal_WriteData - API by application to request transmision of new data
            on the give of the COMDAL channel

@param[in]  ComChannel     Logical BSW ComDal channel id.
@param[in]  RxDataLen      Maximum data length provided for that channel
@param[out] WriteBuffer     Data to be sent on the COMDAL channel.
*/
/*===================================================================================================*/
FUNC(void, COMDAL_APP_CODE) ComDal_WriteData
(
    VAR(ComDal_ChannelType, AUTOMATIC) ComChannel,
    P2VAR(char, AUTOMATIC, COMDAL_APPL_DATA) WriteBuffer,
    VAR(uint8, AUTOMATIC) TxDataLen
)
{
    VAR(ComDal_BswChannelType, AUTOMATIC) ComChannelType;

    if(COMDAL_INIT == ComDal_aeModuleState)
    {
        if(NULL_PTR != WriteBuffer)
        {
            if(E_OK == ComDal_ValidateComChannel(ComChannel, COMDAL_WRITEDATA_API_ID))
            {
                ComChannelType = (ComDal_pGlobalConfigPtr->pComChannelMap)[ComChannel].ComChannelType;

                switch(ComChannelType)
                {
#if(COMDAL_MAX_CAN_DESCRIPTORS > 0U)
                    case COMDAL_CAN_CHANNEL:
                    {
                        ComDal_Can_WriteData(ComChannel, WriteBuffer, TxDataLen);
                    }break;
#endif /* COMDAL_MAX_CAN_DESCRIPTORS > 0U */

#if(COMDAL_MAX_LLCE_DESCRIPTORS > 0U)
                    case COMDAL_LLCE_CHANNEL:
                    {
                        ComDal_Llce_WriteData(ComChannel, WriteBuffer, TxDataLen);
                    }break;
#endif /* COMDAL_MAX_LLCE_DESCRIPTORS > 0U */

#if((COMDAL_MAX_UART_DESCRIPTORS > 0U) && (COMDAL_UART_MAX_TX_CHANNELS > 0))
                    case COMDAL_UART_CHANNEL:
                    {
                        ComDal_Uart_WriteData(ComChannel, WriteBuffer, TxDataLen);
                    }break;
#endif /* (COMDAL_MAX_UART_DESCRIPTORS > 0U) && (COMDAL_UART_MAX_TX_CHANNELS > 0) */

#if(COMDAL_MAX_NFC_DESCRIPTORS > 0U)
                    case COMDAL_NFC_CHANNEL:
                    {
                        ComDal_Nfc_WriteData(ComChannel, WriteBuffer, TxDataLen);
                    }break;
#endif /* COMDAL_MAX_NFC_DESCRIPTORS > 0U */

#if(COMDAL_MAX_IPC_DESCRIPTORS > 0U)
                    case COMDAL_IPC_CHANNEL:
                    {
                        ComDal_Ipc_WriteData(ComChannel, WriteBuffer, TxDataLen);
                    }break;
#endif /* COMDAL_MAX_IPC_DESCRIPTORS > 0U */

#if(COMDAL_MAX_LIN_DESCRIPTORS > 0U)
                    case COMDAL_LIN_CHANNEL:
                    break;
#endif /* COMDAL_MAX_LIN_DESCRIPTORS > 0U */
#if(COMDAL_MAX_ETH_DESCRIPTORS)
                    case COMDAL_ETH_CHANNEL:
                    {
                        ComDal_Eth_WriteData(ComChannel, WriteBuffer, TxDataLen);
                    }break;
#endif /* COMDAL_MAX_ETH_DESCRIPTORS > 0U */
                    default :
                    {
#if(COMDAL_DEV_ERROR_DETECT == STD_ON)
                        Det_ReportError((uint16)COMDAL_MODULE_ID, (uint8)0U, COMDAL_WRITEDATA_API_ID, COMDAL_E_INVALID_CHANNEL_TYPE);
#endif /* COMDAL_DEV_ERROR_DETECT == STD_ON */
                    }
                }
            }
            else
            {
#if(COMDAL_DEV_ERROR_DETECT == STD_ON)
            Det_ReportError((uint16)COMDAL_MODULE_ID, (uint8)0U, COMDAL_WRITEDATA_API_ID, COMDAL_E_INVALID_CHANNEL_ID);
#endif /* COMDAL_DEV_ERROR_DETECT == STD_ON */
            }
        }
        else
        {
#if(COMDAL_DEV_ERROR_DETECT == STD_ON)
            Det_ReportError((uint16)COMDAL_MODULE_ID, (uint8)0U, COMDAL_WRITEDATA_API_ID, COMDAL_E_PARAM_POINTER);
#endif /* COMDAL_DEV_ERROR_DETECT == STD_ON */
        }
    }
    else
    {
        /*call Det: module not initialized*/
#if(COMDAL_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError((uint16)COMDAL_MODULE_ID, (uint8)0U, COMDAL_WRITEDATA_API_ID, COMDAL_E_UNINIT);
#endif /* COMDAL_DEV_ERROR_DETECT == STD_ON */
    }
}

/*===================================================================================================*/
/**
@brief      ComDal_ReadData - API used to provide application with data read from
            any of the comunication lines

@details      If the content of the provided pReadBuffer is equal to current content of the channel buffer
            this function shall return  COMDAL_OBSOLETE; if COMDAL was not properly initialized it shall
            return COMDAL_INVALID.
            This function shall return COMDAL_NO_ERROR whenever the data received has be updated.

@param[in]      ComChannel     Logical BSW ComDal channel id.
@param[in/out]  pRxDataLen     Used as input parameter to specify the pReadBuffer size
                               Used as output parameter for amount of read data
@param[out]     pReadBuffer    Data read from the COMDAL channel.


@return     ComDal_ReturnType: COMDAL_NO_ERROR, COMDAL_OBSOLETE, COMDAL_INVALID
*/
/*===================================================================================================*/
FUNC(ComDal_ReturnType, COMDAL_APP_CODE) ComDal_ReadData
(
    VAR(ComDal_ChannelType, AUTOMATIC) ComChannel,
    P2VAR(char, AUTOMATIC, COMDAL_APPL_DATA) pReadBuffer,
    P2VAR(uint16, AUTOMATIC, COMDAL_APPL_DATA) pRxDataLen
)
{
    VAR(ComDal_BswChannelType, AUTOMATIC) ComChannelType;
    VAR(ComDal_ReturnType, AUTOMATIC) ReturnValue = COMDAL_BUSY;

    if(COMDAL_INIT == ComDal_aeModuleState)
    {
        if(NULL_PTR != pReadBuffer)
        {
            if(E_OK == ComDal_ValidateComChannel(ComChannel, COMDAL_READDATA_API_ID))
            {
                ComChannelType = (ComDal_pGlobalConfigPtr->pComChannelMap)[ComChannel].ComChannelType;

                /*CONSOLE_LOG(CONSOLE_INFO,
                            "ComDal_ReadData: ComChannel = %d, ComChannelType = %d, pRxDataLen = %d",
                            ComChannel, ComChannelType, *pRxDataLen
                           );*/

                switch(ComChannelType)
                {
#if(COMDAL_MAX_CAN_DESCRIPTORS > 0U)
                    case COMDAL_CAN_CHANNEL:
                    {
                        ReturnValue = ComDal_Can_ReadData(ComChannel, pReadBuffer, *pRxDataLen);
                    }break;
#endif /* COMDAL_MAX_CAN_DESCRIPTORS > 0U */

#if(COMDAL_MAX_LLCE_DESCRIPTORS > 0U)
                    case COMDAL_LLCE_CHANNEL:
                    {
                        ReturnValue = ComDal_Llce_ReadData(ComChannel, pReadBuffer, *pRxDataLen);
                    }break;
#endif /* COMDAL_MAX_LLCE_DESCRIPTORS > 0U */

#if(COMDAL_MAX_UART_DESCRIPTORS > 0U)
                    case COMDAL_UART_CHANNEL:
                    {
                        ReturnValue = ComDal_Uart_ReadData(ComChannel, pReadBuffer, pRxDataLen);
                    }break;
#endif /* COMDAL_MAX_UART_DESCRIPTORS > 0U */

#if(COMDAL_MAX_NFC_DESCRIPTORS > 0U)
                    case COMDAL_NFC_CHANNEL:
                    {
                        ReturnValue = ComDal_Nfc_ReadData(ComChannel, pReadBuffer, *pRxDataLen);
                    }
                    break;
#endif /* COMDAL_MAX_NFC_DESCRIPTORS > 0U */

#if(COMDAL_MAX_IPC_DESCRIPTORS > 0U)
                    case COMDAL_IPC_CHANNEL:
                    {
                        ReturnValue = ComDal_Ipc_ReadData(ComChannel, pReadBuffer, *pRxDataLen);
                    }
                    break;
#endif /* COMDAL_MAX_IPC_DESCRIPTORS > 0U */

#if(COMDAL_MAX_LIN_DESCRIPTORS > 0U)
                    case COMDAL_LIN_CHANNEL:
                    break;
#endif /* COMDAL_MAX_LIN_DESCRIPTORS > 0U */

#if(COMDAL_MAX_ETH_DESCRIPTORS > 0U)
                    case COMDAL_ETH_CHANNEL:
                        ReturnValue = ComDal_Eth_ReadData(ComChannel, pReadBuffer, pRxDataLen);
                    break;
#endif /* COMDAL_MAX_ETH_DESCRIPTORS > 0U */
                    default :
                    {
#if(COMDAL_DEV_ERROR_DETECT == STD_ON)
                        Det_ReportError((uint16)COMDAL_MODULE_ID,
                                        (uint8)0U,
                                        COMDAL_READDATA_API_ID,
                                        COMDAL_E_INVALID_CHANNEL_TYPE
                                       );
#endif  /* COMDAL_DEV_ERROR_DETECT == STD_ON */
                    }break;
                }
            }
            else
            {
#if(COMDAL_DEV_ERROR_DETECT == STD_ON)
            Det_ReportError((uint16)COMDAL_MODULE_ID,
                            (uint8)0U,
                            COMDAL_READDATA_API_ID,
                            COMDAL_E_INVALID_CHANNEL_ID
                           );
#endif /* COMDAL_DEV_ERROR_DETECT == STD_ON */
            }
        }
        else
        {
#if(COMDAL_DEV_ERROR_DETECT == STD_ON)
            Det_ReportError((uint16)COMDAL_MODULE_ID,
                            (uint8)0U,
                            COMDAL_READDATA_API_ID,
                            COMDAL_E_PARAM_POINTER
                           );
#endif /* COMDAL_DEV_ERROR_DETECT == STD_ON */
        }
    }
    else
    {
        /*call Det: module not initialized */
#if(COMDAL_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError((uint16)COMDAL_MODULE_ID,
                        (uint8)0U,
                        COMDAL_READDATA_API_ID,
                        COMDAL_E_UNINIT
                       );
#endif /* COMDAL_DEV_ERROR_DETECT == STD_ON */
    }

    return ReturnValue;
}

/*===================================================================================================*/
/**
@brief      ComDal_GetMessageHandlerId - API used to provide application with specific channel
            information, e.g: Can ID of last message
@param[in]  ComChannel     Logical BSW ComDal channel id.
@return     uint16: Specific channel ID.
*/
/*===================================================================================================*/
FUNC(uint16, COMDAL_APP_CODE) ComDal_GetMessageHandlerId(VAR(ComDal_ChannelType, AUTOMATIC) ComChannel)
{
    VAR(uint16, AUTOMATIC) ComDal_HandlerId = 0;
    VAR(ComDal_BswChannelType, AUTOMATIC) ComChannelType = COMDAL_CHANNEL_INVALID;

    if(COMDAL_INIT == ComDal_aeModuleState)
    {
        if(E_OK == ComDal_ValidateComChannel(ComChannel, COMDAL_READDATA_API_ID))
        {
            ComChannelType = (ComDal_pGlobalConfigPtr->pComChannelMap)[ComChannel].ComChannelType;

            switch(ComChannelType)
            {
#if(COMDAL_MAX_CAN_DESCRIPTORS > 0U)
                case COMDAL_CAN_CHANNEL:
                {
                    ComDal_HandlerId = ComDal_Can_GetMessageHandlerId(ComChannel);
                }break;
#endif /* COMDAL_MAX_CAN_DESCRIPTORS > 0U */

#if(COMDAL_MAX_LLCE_DESCRIPTORS > 0U)
                case COMDAL_LLCE_CHANNEL:
                {
                    ComDal_HandlerId = ComDal_Llce_GetMessageHandlerId(ComChannel);
                }break;
#endif /* COMDAL_MAX_LLCE_DESCRIPTORS > 0U */

#if((COMDAL_MAX_UART_DESCRIPTORS > 0U) && (COMDAL_UART_MAX_RX_CHANNELS > 0))
                case COMDAL_UART_CHANNEL:
                {
                    /* Not Supported */
                }break;
#endif /* (COMDAL_MAX_UART_DESCRIPTORS > 0U) && (COMDAL_UART_MAX_RX_CHANNELS > 0) */

#if(COMDAL_MAX_NFC_DESCRIPTORS > 0U)
                case COMDAL_NFC_CHANNEL:
                {
                    /* Not Supported */
                }
                break;
#endif /* COMDAL_MAX_NFC_DESCRIPTORS > 0U */

#if(COMDAL_MAX_IPC_DESCRIPTORS > 0U)
                case COMDAL_IPC_CHANNEL:
                {
                    /* Not Supported */
                }
                break;
#endif /* COMDAL_MAX_IPC_DESCRIPTORS > 0U */

                default :
                {
#if(COMDAL_DEV_ERROR_DETECT == STD_ON)
                    Det_ReportError((uint16)COMDAL_MODULE_ID,
                                    (uint8)0U,
                                    COMDAL_GET_CHANNEL_API_ID,
                                    COMDAL_E_INVALID_CHANNEL_TYPE
                                   );
#endif  /* COMDAL_DEV_ERROR_DETECT == STD_ON */
                }break;
            }
        }
        else
        {
#if(COMDAL_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError((uint16)COMDAL_MODULE_ID,
                        (uint8)0U,
                        COMDAL_GET_CHANNEL_API_ID,
                        COMDAL_E_INVALID_CHANNEL_ID
                       );
#endif /* COMDAL_DEV_ERROR_DETECT == STD_ON */
        }
    }
    else
    {
#if(COMDAL_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError((uint16)COMDAL_MODULE_ID,
                        (uint8)0U,
                        COMDAL_GET_CHANNEL_API_ID,
                        COMDAL_E_UNINIT
                       );
#endif /* COMDAL_DEV_ERROR_DETECT == STD_ON */
    }

    return ComDal_HandlerId;
}

/*===================================================================================================*/
/**
@brief      ComDal_GetChannelStatus - API used to provide status for a specific channel
@param[in]  ComChannel     Logical BSW ComDal channel id.
@return     ComDal_State: COMDAL_STATE_IDLE, COMDAL_STATE_RX_ONGOING, COMDAL_STATE_TX_ONGOING, COMDAL_STATE_ERROR.
*/
/*===================================================================================================*/
FUNC(ComDal_State, COMDAL_APP_CODE) ComDal_GetChannelStatus(VAR(ComDal_ChannelType, AUTOMATIC) ComChannel)
{
    VAR(ComDal_State, AUTOMATIC) ComStatus = COMDAL_STATE_ERROR;
    VAR(ComDal_BswChannelType, AUTOMATIC) ComChannelType = COMDAL_CHANNEL_INVALID;

    if (COMDAL_INIT == ComDal_aeModuleState)
    {
        ComChannelType = (ComDal_pGlobalConfigPtr->pComChannelMap)[ComChannel].ComChannelType;

        switch (ComChannelType)
        {
#if (COMDAL_MAX_CAN_DESCRIPTORS > 0U)
            case COMDAL_CAN_CHANNEL:
            {
                ComStatus = ComDal_Can_GetChannelStatus(ComChannel);
            }break;
#endif /* COMDAL_MAX_CAN_DESCRIPTORS > 0U */

#if (COMDAL_MAX_LLCE_DESCRIPTORS > 0U)
            case COMDAL_LLCE_CHANNEL:
            {
                /* Not Supported */
            }break;
#endif /* COMDAL_MAX_LLCE_DESCRIPTORS > 0U */

#if (COMDAL_MAX_UART_DESCRIPTORS > 0U)
            case COMDAL_UART_CHANNEL:
            {
                ComStatus = ComDal_Uart_GetChannelStatus(ComChannel);
            }break;
#endif /* (COMDAL_MAX_UART_DESCRIPTORS > 0U)  */

#if (COMDAL_MAX_NFC_DESCRIPTORS > 0U)
            case COMDAL_NFC_CHANNEL:
            {
                /* Not Supported */
            }break;
#endif /* COMDAL_MAX_NFC_DESCRIPTORS > 0U */

#if (COMDAL_MAX_IPC_DESCRIPTORS > 0U)
            case COMDAL_IPC_CHANNEL:
            {
                /* Not Supported */
            }break;
#endif /* COMDAL_MAX_IPC_DESCRIPTORS > 0U */

#if (COMDAL_MAX_LIN_DESCRIPTORS > 0U)
            case COMDAL_LIN_CHANNEL:
            {
                /* Not Supported */
            }
            break;
#endif /* COMDAL_MAX_LIN_DESCRIPTORS > 0U */

            default :
            {
#if (COMDAL_DEV_ERROR_DETECT == STD_ON)
                Det_ReportError((uint16)COMDAL_MODULE_ID, (uint8)0U, COMDAL_GET_CHANNEL_STATUS_API_ID, COMDAL_E_INVALID_CHANNEL_TYPE);
#endif /* COMDAL_DEV_ERROR_DETECT == STD_ON */
            }
        }
    }
    else
    {
        /*call Det: module not initialized*/
#if (COMDAL_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError((uint16)COMDAL_MODULE_ID, (uint8)0U, COMDAL_GET_CHANNEL_STATUS_API_ID, COMDAL_E_UNINIT);
#endif /* COMDAL_DEV_ERROR_DETECT == STD_ON */
    }

    return ComStatus;
}

/*===================================================================================================*/
/**
@brief      ComDal_ValidateComChannel - Validate com channel
@details    shall check if the given channel index is less than the maximum number of channels
            and also check if the channel is configured to be used for read or write.

@param[in]  ComChannel     Logical BSW ComDal channel id.
@param[in]  ApiId          API id for which this function is called.

@return     Std_ReturnType: E_OK or E_NOT_OK
*/
/*===================================================================================================*/
static INLINE FUNC(Std_ReturnType, COMDAL_APP_CODE) ComDal_ValidateComChannel
(
    VAR(ComDal_ChannelType, AUTOMATIC) ComChannel,
    VAR(uint8, AUTOMATIC) ApiId
)
{
    Std_ReturnType ReturnValue = E_OK;
    VAR(ComDal_AccessType, AUTOMATIC) AccessType;

    AccessType = (ComDal_pGlobalConfigPtr->pComChannelMap)[ComChannel].AccessType;

    if(ComChannel > COMDAL_MAX_CHANNELS_HW_VAR_0 )
    {
        ReturnValue = E_NOT_OK;
    }
    else
    {
        if ((COMDAL_READDATA_API_ID == ApiId && AccessType != COMDAL_RXCHANNEL) ||
            (COMDAL_WRITEDATA_API_ID == ApiId && AccessType != COMDAL_TXCHANNEL))
        {
            ReturnValue = E_NOT_OK;
        }
    }

    return ReturnValue;
}

#ifdef __cplusplus
}
#endif
/** @} */
