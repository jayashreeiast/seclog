/**

*   @file              ComDal_Llce.c
*   project            AUTOSAR 4.4 GOLDVIP 
*   platform           CORTEXM 
*   @version           24.12.0 
* 
*   @brief             Integration Framework - ComDal Abstraction Layer component 
*   @details           This file implements the ComDal interface to application layer for LLCE interface.
* 
*   Autosar Version    4.4.0 
*   Build Version      S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120 
* 
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP 
*   All Rights Reserved. 
* 
===================================================================================================================================*/

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

/*=================================================================================================================================
                                         INCLUDE FILES
 1) system and project includes
 2) needed interfaces from external units
 3) internal and external interfaces from this unit
===================================================================================================================================*/
#include "Std_Types.h"
#include "Mcal.h"
#include "ComDal_Types.h"
#include "ComDal_Cfg.h"
#include "ComDal_Llce.h"

#ifdef IFWK_USE_LLCE
#include "Can_Llce.h"
#include "Llce_InterfaceCanConfig.h"
#include "MemLib.h"
#endif /* IFWK_USE_LLCE */
/*=================================================================================================================================
*                              SOURCE FILE VERSION INFORMATION
===================================================================================================================================*/
#define COMDAL_LLCE_VENDOR_ID_C                       43

#define COMDAL_LLCE_AR_RELEASE_MAJOR_VERSION_C        4
#define COMDAL_LLCE_AR_RELEASE_MINOR_VERSION_C        4
#define COMDAL_LLCE_AR_RELEASE_REVISION_VERSION_C     0

#define COMDAL_LLCE_SW_MAJOR_VERSION_C                24
#define COMDAL_LLCE_SW_MINOR_VERSION_C                12
#define COMDAL_LLCE_SW_PATCH_VERSION_C                0

/*=================================================================================================================================
*                                     FILE VERSION CHECKS
===================================================================================================================================*/
#if COMDAL_MAX_LLCE_DESCRIPTORS > 0 
/* Check if source file and Comdal configuration header file are of the same vendor */
#if (COMDAL_LLCE_VENDOR_ID_C != COMDAL_CFG_VENDOR_ID)
    #error "ComDal_Llce.c and ComDal_Cfg.h have different vendor ids"
#endif /* File version check */

/* Check if source file and Comdal configuration header file are of the same Autosar version */
#if ((COMDAL_LLCE_AR_RELEASE_MAJOR_VERSION_C != COMDAL_CFG_AR_RELEASE_MAJOR_VERSION) || \
     (COMDAL_LLCE_AR_RELEASE_MINOR_VERSION_C != COMDAL_CFG_AR_RELEASE_MINOR_VERSION) || \
     (COMDAL_LLCE_AR_RELEASE_REVISION_VERSION_C != COMDAL_CFG_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of ComDal_Llce.c and ComDal_Cfg.h are different"
#endif /* File version check */

/* Check if source file and Comdal configuration header file are of the same Software version */
#if ((COMDAL_LLCE_SW_MAJOR_VERSION_C != COMDAL_CFG_SW_MAJOR_VERSION) || \
     (COMDAL_LLCE_SW_MINOR_VERSION_C != COMDAL_CFG_SW_MINOR_VERSION) || \
     (COMDAL_LLCE_SW_PATCH_VERSION_C != COMDAL_CFG_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of ComDal_Llce.c and ComDal_Cfg.h are different"
#endif /* File version check */

/* Check if source file and Comdal types header file are of the same vendor */
#if (COMDAL_LLCE_VENDOR_ID_C != COMDAL_TYPES_VENDOR_ID)
    #error "ComDal_Llce.c and ComDal_Types.h have different vendor ids"
#endif /* File version check */

/* Check if source file and Comdal types header file are of the same Autosar version */
#if ((COMDAL_LLCE_AR_RELEASE_MAJOR_VERSION_C != COMDAL_TYPES_AR_RELEASE_MAJOR_VERSION) || \
     (COMDAL_LLCE_AR_RELEASE_MINOR_VERSION_C != COMDAL_TYPES_AR_RELEASE_MINOR_VERSION) || \
     (COMDAL_LLCE_AR_RELEASE_REVISION_VERSION_C != COMDAL_TYPES_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of ComDal_Llce.c and ComDal_Types.h are different"
#endif /* File version check */

/* Check if source file and Comdal types header file are of the same Software version */
#if ((COMDAL_LLCE_SW_MAJOR_VERSION_C != COMDAL_TYPES_SW_MAJOR_VERSION) || \
     (COMDAL_LLCE_SW_MINOR_VERSION_C != COMDAL_TYPES_SW_MINOR_VERSION) || \
     (COMDAL_LLCE_SW_PATCH_VERSION_C != COMDAL_TYPES_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of ComDal_Llce.c and ComDal_Types.h are different"
#endif /* File version check */

/* Check if source file and Comdal Llce header file are from the same vendor */
#if (COMDAL_LLCE_VENDOR_ID_C != COMDAL_LLCE_VENDOR_ID)
    #error "ComDal_Llce.c and ComDal_Llce.h have different vendor ids"
#endif /* File version check */

/* Check if source file and Comdal Llce header file are of the same Autosar version */
#if ((COMDAL_LLCE_AR_RELEASE_MAJOR_VERSION_C != COMDAL_LLCE_AR_RELEASE_MAJOR_VERSION) || \
     (COMDAL_LLCE_AR_RELEASE_MINOR_VERSION_C != COMDAL_LLCE_AR_RELEASE_MINOR_VERSION) || \
     (COMDAL_LLCE_AR_RELEASE_REVISION_VERSION_C != COMDAL_LLCE_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of ComDal_Llce.c and ComDal_Llce.h are different"
#endif /* File version check */

/* Check if source file and Comdal Llce header file are of the same Software version */
#if ((COMDAL_LLCE_SW_MAJOR_VERSION_C != COMDAL_LLCE_SW_MAJOR_VERSION) || \
     (COMDAL_LLCE_SW_MINOR_VERSION_C != COMDAL_LLCE_SW_MINOR_VERSION) || \
     (COMDAL_LLCE_SW_PATCH_VERSION_C != COMDAL_LLCE_SW_PATCH_VERSION) \
    )
  #error "Software Version Numbers of ComDal_Llce.c and ComDal_Llce.h are different"
#endif /* File version check */
#endif /* COMDAL_MAX_LLCE_DESCRIPTORS > 0 */
/*=================================================================================================================================
*                                   LOCAL MACROS
===================================================================================================================================*/
/* LLCE cores count */
#define COMDAL_LLCE_NUM_CORES              4U

/* LLCE base address */
#define LLCE_0_BASEADDR                    ((uint32)0x43000000UL)

/* LLCE core start register */
#define COMDAL_LLCE_CORE_CONFIG_ADDR       (LLCE_0_BASEADDR + 0xFF8000)

/* LLCE data storage */
#define COMDAL_LLCE_DATA_START_ADDR        (LLCE_0_BASEADDR + 0x800000)

/* LLCE shared RAM length */
#define COMDAL_LLCE_DATA_LENGTH            0x4FFFF

/* LLCE status register */
#define COMDAL_LLCE_STATUS_ADDR            (LLCE_0_BASEADDR + 0x840000)

/* LLCE timeout, if the firmware is not up after this period, code will fail */
#define COMDAL_LLCE_FW_TIMEOUT             1000000

/* LLCE reset cores command */
#define COMDAL_LLCE_RESET_CORES_CMD        0x0

/* LLCE start cores command */
#define COMDAL_LLCE_START_CORES_CMD        0xF

/* LLCE memory sizes, per each core */
#define COMDAL_LLCE_CORE_0_MEM_SIZE        0x2000
#define COMDAL_LLCE_CORE_1_MEM_SIZE        0x8000
#define COMDAL_LLCE_CORE_2_MEM_SIZE        0x8000
#define COMDAL_LLCE_CORE_3_MEM_SIZE        0x10000

/* Macro for calculating the firmware start address for specific cores */
#define COMDAL_LLCE_CORE_START_ADDR(x)    (LLCE_0_BASEADDR + (x) * 0x100000)

/*=================================================================================================================================
*                                   LOCAL TYPES
===================================================================================================================================*/
/**< @brief Transmission status enumerations */
typedef enum
{
    TX_INITIALIZED_STATE  = 0U,       /**< @brief Llce Tx channel is initialized. */
    TX_REQUESTED_STATE,               /**< @brief Llce Tx channel has a requested transmision*/
    TX_IDLE_STATE,                    /**< @brief Llce Tx channel is IDLE*/
    TX_PENDING_STATE,                 /**< @brief Llce Tx channel is Pending transmission */
    TX_ERROR_STATE = 255              /**< @brief Llce Tx channel is in an error state*/
} ComDal_Llce_aeTxStateType;

/**< @brief LLCE core code abstraction */
typedef struct {
    uint8* const pu8FirmwareStartAddr;      /**< @brief Firmware start address */
    const uint32 u32CoreRamSize;            /**< @brief Memory size for core */
    const uint8* const pu8CodeStorageAddr;  /**< @brief Firmware storage address in current executable file */
    const uint32 *u32CodeSize;              /**< @brief Firmware size */
} ComDal_Llce_FirmwareSettingType;

#if COMDAL_MAX_LLCE_DESCRIPTORS > 0 
/*=================================================================================================================================
                                       EXTERNAL DECLARATIONS
===================================================================================================================================*/

/* LLCE firmware for Data Transfer Engine */
extern const uint8 dte_bin[];
/* LLCE firmware size for Data Transfer Engine */
extern const uint32 dte_bin_len;

/* LLCE firmware for Reception Protocol Processing Engine */
extern const uint8 ppe_rx_bin[];
/* LLCE firmware size for Reception Protocol Processing Engine */
extern const uint32 ppe_rx_bin_len;

/* LLCE firmware for Transmission Protocol Processing Engine */
extern const uint8 ppe_tx_bin[];
/* LLCE firmware size for Transmission Protocol Processing Engine */
extern const uint32 ppe_tx_bin_len;

/* LLCE firmware for Advanced features Engine */
extern const uint8 frpe_bin[];
/* LLCE firmware size for Advanced features Engine */
extern const uint32 frpe_bin_len;

/*=================================================================================================================================
                                       LOCAL VARIABLES
===================================================================================================================================*/
/* ComDal configuration pointer */
static P2CONST(ComDal_ConfigType, AUTOMATIC, COMDAL_APPL_CONST) ComDal_Llce_pConfigPtr;

/* Information about firmware boot status */
static __attribute__((section(".llce_boot_end"))) volatile Llce_Mgr_StatusType ComDal_Llce_pFirmwareStatus;

/* Local variable containing information about firmware status */
static VAR(boolean, AUTOMATIC)                   ComDal_Llce_bFirmwareIsUp = FALSE;

/* Ongoing transfer status variable, per each descriptor */
static VAR(boolean, AUTOMATIC)                   ComDal_Llce_abTxFlag[COMDAL_MAX_LLCE_DESCRIPTORS];

/* Transfer confirmation array, per each descriptor */
static VAR(uint8, AUTOMATIC)                     ComDal_Llce_au8TxConfirmation[COMDAL_MAX_LLCE_DESCRIPTORS];

/* Transfer timeout setting */
static VAR(uint8, AUTOMATIC)                     ComDal_Llce_au8TxTimeout[COMDAL_MAX_LLCE_DESCRIPTORS];

/* Message ID sender. This array is storing  information about the sender */
static VAR(uint16, AUTOMATIC)                    ComDal_Llce_u16MessageId[COMDAL_MAX_LLCE_DESCRIPTORS];

/* Receive status */
static VAR(ComDal_ReturnType, AUTOMATIC)         ComDal_Llce_aeRxState[COMDAL_MAX_LLCE_DESCRIPTORS];

/* Transfer Status */
static VAR(ComDal_Llce_aeTxStateType, AUTOMATIC) ComDal_Llce_aeTxState[COMDAL_MAX_LLCE_DESCRIPTORS];
                                                 
/*=================================================================================================================================
                                       LOCAL CONSTANTS
===================================================================================================================================*/
/* LLCE cores data */
static const ComDal_Llce_FirmwareSettingType ComDal_Llce_CoreData[COMDAL_LLCE_NUM_CORES] = 
{
    {(uint8*)COMDAL_LLCE_CORE_START_ADDR(0), COMDAL_LLCE_CORE_0_MEM_SIZE,  dte_bin,    &dte_bin_len},       /* Data transfer engine code*/
    {(uint8*)COMDAL_LLCE_CORE_START_ADDR(1), COMDAL_LLCE_CORE_1_MEM_SIZE,  ppe_rx_bin, &ppe_rx_bin_len},    /* Receive protocol engine code */
    {(uint8*)COMDAL_LLCE_CORE_START_ADDR(2), COMDAL_LLCE_CORE_2_MEM_SIZE,  ppe_tx_bin, &ppe_tx_bin_len},    /* Transmit protocol engine code */
    {(uint8*)COMDAL_LLCE_CORE_START_ADDR(3), COMDAL_LLCE_CORE_3_MEM_SIZE,  frpe_bin,   &frpe_bin_len}       /* Advanced features (CAN2ETH) engine code */
};

/*=================================================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===================================================================================================================================*/

static FUNC(void, COMDAL_APP_CODE) ComDal_Llce_PrepareTxMessage(VAR(ComDal_ChannelType, AUTOMATIC)  ComChannel, P2VAR(Can_PduType, AUTOMATIC, CAN_APPL_VAR) PduInfo);

/*==================================================================================================================================
*                                       GLOBAL FUNCTIONS
===================================================================================================================================*/
/*=================================================================================================================================*/
/**
@brief      ComDal_Llce_LoadFirmware - Initialization function for LLCE cores 
@details    This function shall reset the LLCE cores, load the firmware, start the cores and wait for initialization to be complete. 
            If initialization is not complete ComDal_Llce_bFirmwareIsUp shall be set to false.
            
@return     None
*/
/*=================================================================================================================================*/
FUNC(void, COMDAL_APP_CODE) ComDal_Llce_LoadFirmware(void)
{
    uint8 u8Index;
    uint32 u32Timeout;	

    /* Put all cores in reset*/
    REG_WRITE32(COMDAL_LLCE_CORE_CONFIG_ADDR, COMDAL_LLCE_RESET_CORES_CMD);
 
    for (u8Index = 0U; u8Index < COMDAL_LLCE_NUM_CORES; u8Index++)
    {
        /* Set memory using 8 byte write operation to avoid ECC errors */
        MemLib_MemSet64(ComDal_Llce_CoreData[u8Index].pu8FirmwareStartAddr, 0, ComDal_Llce_CoreData[u8Index].u32CoreRamSize);
        /* Copy firmware */
        MemLib_MemCpy(ComDal_Llce_CoreData[u8Index].pu8FirmwareStartAddr, ComDal_Llce_CoreData[u8Index].pu8CodeStorageAddr, *(ComDal_Llce_CoreData[u8Index].u32CodeSize));
    }
    
    /* Clear SRAM Data section */
    MemLib_MemSet64((uint64*)COMDAL_LLCE_DATA_START_ADDR, 0, COMDAL_LLCE_DATA_LENGTH);
	
    u32Timeout = COMDAL_LLCE_FW_TIMEOUT;
    
    /* Start LLCE cores */
    REG_WRITE32(COMDAL_LLCE_CORE_CONFIG_ADDR, COMDAL_LLCE_START_CORES_CMD);      

    /* Wait for the LLCE firmware cores to start running and boot. */            
    while (( (LLCE_FW_SUCCESS != ComDal_Llce_pFirmwareStatus.eTxBootStatus)  || (LLCE_FW_SUCCESS != ComDal_Llce_pFirmwareStatus.eRxBootStatus) || \
             (LLCE_FW_SUCCESS != ComDal_Llce_pFirmwareStatus.eDteBootStatus) || (LLCE_FW_SUCCESS != ComDal_Llce_pFirmwareStatus.eFrpeBootStatus) ) && 
             (u32Timeout > 0))
    {
		u32Timeout--;
    }
    if (0 != u32Timeout)
    {
        ComDal_Llce_bFirmwareIsUp = TRUE;
    }
}


/*=================================================================================================================================*/
/**
@brief      ComDal_Llce_Init - Initialization function for COMDAL LLCE components
@details    This function shall:
             - initialize all global variables used by LLCE component.
             - enable interrupts for channels NOT using polling mode  
             - Start LLCE controllers. 
            
@return     None
*/
/*=================================================================================================================================*/
FUNC(void, COMDAL_APP_CODE) ComDal_Llce_Init(P2CONST(ComDal_ConfigType, AUTOMATIC, CAN_APPL_CONST) ConfigPtr)
{
    VAR(uint8, AUTOMATIC) u8HwUnitindex;
    VAR(uint8, AUTOMATIC) u8LlceDescriptorId = 0;
    static VAR(boolean, AUTOMATIC) ComDal_Llce_abControllerModeState[COMDAL_MAX_LLCE_HW_CONTROLERS];
    
    ComDal_Llce_pConfigPtr = ConfigPtr;   
    
    if (TRUE == ComDal_Llce_bFirmwareIsUp)
    {
        
        for(u8HwUnitindex = 0; u8HwUnitindex < COMDAL_MAX_LLCE_HW_CONTROLERS; u8HwUnitindex++)
        {
            ComDal_Llce_abControllerModeState[u8HwUnitindex] = FALSE;
        }    
        
        for(u8LlceDescriptorId = 0; u8LlceDescriptorId < ComDal_Llce_pConfigPtr->LlceChannelCount; u8LlceDescriptorId++)
        {
            ComDal_Llce_aeTxState[u8LlceDescriptorId] = TX_INITIALIZED_STATE;
            
            ComDal_Llce_aeRxState[u8LlceDescriptorId] = COMDAL_INVALID;
       
            ComDal_Llce_au8TxConfirmation[u8LlceDescriptorId] = 0;

            ComDal_Llce_au8TxTimeout[u8LlceDescriptorId] = 0;
#ifndef AUTOSAR_OS_USED
            if((FALSE == ((ComDal_Llce_pConfigPtr->pLlceChanDescriptor)[u8LlceDescriptorId]).bPollingEnabled)) 
            {
                Can_Llce_EnableControllerInterrupts(((ComDal_Llce_pConfigPtr->pLlceChanDescriptor)[u8LlceDescriptorId]).CanControllerId);
            }   
            else
            {
                Can_Llce_DisableControllerInterrupts(((ComDal_Llce_pConfigPtr->pLlceChanDescriptor)[u8LlceDescriptorId]).CanControllerId);
            }          
#endif /* AUTOSAR_OS_USED */
            if(FALSE == ComDal_Llce_abControllerModeState[((ComDal_Llce_pConfigPtr->pLlceChanDescriptor)[u8LlceDescriptorId]).CanControllerId])
            {    
                ComDal_Llce_abControllerModeState[((ComDal_Llce_pConfigPtr->pLlceChanDescriptor)[u8LlceDescriptorId]).CanControllerId] = TRUE;

                if(E_OK != Can_43_LLCE_SetControllerMode(((ComDal_Llce_pConfigPtr->pLlceChanDescriptor)[u8LlceDescriptorId]).CanControllerId, CAN_CS_STARTED))
                {
                    ComDal_Llce_aeTxState[u8LlceDescriptorId] = TX_ERROR_STATE;     
                }
            }    
        }
    }
}

/*=================================================================================================================================*/
/**
@brief      ComDal_Llce_DeInit - DeInitialization function for COMDAL CAN LLCE components
@details    This function shall:
             - de-initialize all global variables used by LLCE component.
             - disable all interrupts used by LLCE channels
             - STOP CAN LLCE controller. 
@return     None
*/
/*=================================================================================================================================*/
FUNC(void, COMDAL_APP_CODE) ComDal_Llce_DeInit(void)
{
    VAR(ComDal_BswDescriptorIdType, AUTOMATIC) u8LlceDescriptorId = 0;
    
    for(u8LlceDescriptorId = 0; u8LlceDescriptorId <  ComDal_Llce_pConfigPtr->LlceChannelCount; u8LlceDescriptorId++)
    {
        ComDal_Llce_aeTxState[u8LlceDescriptorId] = TX_INITIALIZED_STATE;
        
        ComDal_Llce_aeRxState[u8LlceDescriptorId] = COMDAL_INVALID;
   
        ComDal_Llce_au8TxConfirmation[u8LlceDescriptorId] = 0;

        ComDal_Llce_au8TxTimeout[u8LlceDescriptorId] = 0;
        
        if((FALSE == ((ComDal_Llce_pConfigPtr->pLlceChanDescriptor)[u8LlceDescriptorId]).bPollingEnabled)) 
        {
            Can_Llce_DisableControllerInterrupts(((ComDal_Llce_pConfigPtr->pLlceChanDescriptor)[u8LlceDescriptorId]).CanControllerId);
        }          

        (void)Can_43_LLCE_SetControllerMode((((ComDal_Llce_pConfigPtr->pLlceChanDescriptor))[u8LlceDescriptorId]).CanControllerId, CAN_CS_STOPPED);
    }   
          
    ComDal_Llce_pConfigPtr = NULL_PTR;
}


/*=================================================================================================================================*/
/**
@brief      ComDal_Llce_Main - Main function used to handle state-machine of each LLCE channel 
@details    For each LLCE channel, this function shall:
            - check if a new TX request was issued and if Yes, prepare TX message
            - when polling mode is used, cyclically check if message was transmitted successfully by calling Can_MainFunction_Write
            - when polling mode is used, cyclically check if a new RX message was received successfully by calling Can_MainFunction_Read
            
@return     None
*/
/*=================================================================================================================================*/
FUNC(void, COMDAL_APP_CODE) ComDal_Llce_Main(VAR(ComDal_ChannelType, AUTOMATIC) ComChannel)
{

    VAR(Can_PduType, AUTOMATIC) CanMessage;
    VAR(Can_HwHandleType, AUTOMATIC) CanHardwareObject;
    VAR(Std_ReturnType, AUTOMATIC) ReturnValue = E_OK;
    VAR(ComDal_BswDescriptorIdType, AUTOMATIC) u8LlceDescriptorId = 0;

    u8LlceDescriptorId = (ComDal_Llce_pConfigPtr->pComChannelMap)[ComChannel].ComDescriptorIndex; 

    switch(ComDal_Llce_aeTxState[u8LlceDescriptorId])
    {
        case TX_REQUESTED_STATE:
        {
            ComDal_Llce_PrepareTxMessage(ComChannel, &CanMessage);
            
            CanHardwareObject = ((ComDal_Llce_pConfigPtr->pLlceChanDescriptor)[u8LlceDescriptorId]).CanHwObject;
            
            ReturnValue = Can_43_LLCE_Write(CanHardwareObject, &CanMessage);

            if(E_OK == ReturnValue)
            {
                ComDal_Llce_au8TxTimeout[u8LlceDescriptorId] = 0;
                
                ComDal_Llce_abTxFlag[u8LlceDescriptorId] = FALSE;
                
                /* the message was prepared for transmission - proceed to transmit state */
                ComDal_Llce_aeTxState[u8LlceDescriptorId] = TX_PENDING_STATE;
            }
            else
            {
                 ComDal_Llce_aeTxState[u8LlceDescriptorId] = TX_ERROR_STATE;
            }
        } break;

        case TX_PENDING_STATE:
        {
            /** Wait for  Tx confirmation */
            if((FALSE == ComDal_Llce_abTxFlag[u8LlceDescriptorId]) && (ComDal_Llce_au8TxTimeout[u8LlceDescriptorId] < ((ComDal_Llce_pConfigPtr->pLlceChanDescriptor)[u8LlceDescriptorId]).CanTxTimeout))
            {
#if (CAN_43_LLCE_TXPOLL_SUPPORTED == STD_ON)
                if(TRUE == ((ComDal_Llce_pConfigPtr->pLlceChanDescriptor)[u8LlceDescriptorId]).bPollingEnabled)
                {
                    Can_Llce_MainFunctionWrite();
                }
#endif
                /*increment the timeout*/
                ComDal_Llce_au8TxTimeout[u8LlceDescriptorId]++;
            }
            else
            {
                if(FALSE == ComDal_Llce_abTxFlag[u8LlceDescriptorId])
                {
                    ComDal_Llce_aeTxState[u8LlceDescriptorId] = TX_ERROR_STATE;                  
                }
            }
        } break;

        default:
        {
            /* if we got here, something went terribly wrong*/
        }
        break;        
    }     
    
#if (CAN_43_LLCE_RXPOLL_SUPPORTED == STD_ON)
    if(TRUE == ((ComDal_Llce_pConfigPtr->pLlceChanDescriptor)[u8LlceDescriptorId]).bPollingEnabled)
    {
        /** Wait for  Rx confirmation */
        Can_Llce_MainFunctionRead();
    }
#endif
}


/*=================================================================================================================================*/
/**
@brief      ComDal_Llce_ReadData - API used to provide data that was read from the LLCE.
@details    This function will provide users with the data received on the CAN LLCE line for the provided
            input channel.
            This function shall return  COMDAL_OBSOLETE if no new LLCE message was received for that channel; 
            if COMDAL was not properly initialized it shall return COMDAL_INVALID. 
            This function shall return COMDAL_NO_ERROR whenever the data received has be updated.
  
@param[in]  ComChannel     Logical BSW ComDal channel id.
@param[in]  LlceDataLen    Maximum data length provided for that channel
@param[out] ReadBuffer     Data read from LLCE for the assigned input channel.

  
@return     ComDal_ReturnType: COMDAL_NO_ERROR, COMDAL_OBSOLETE, COMDAL_INVALID
*/
/*=================================================================================================================================*/
FUNC(ComDal_ReturnType, COMDAL_APP_CODE) ComDal_Llce_ReadData(VAR(ComDal_ChannelType, AUTOMATIC) ComChannel, P2VAR(char, AUTOMATIC, COMDAL_APPL_DATA) ReadBuffer, VAR(uint8, AUTOMATIC) LlceDataLen)
{
    VAR(uint8, AUTOMATIC) MsgIndex;
    VAR(ComDal_BswDescriptorIdType, AUTOMATIC) u8LlceDescriptorId = 0;
    VAR(ComDal_ReturnType, AUTOMATIC) ReturnValue = COMDAL_NO_ERROR;
    
    if(NULL_PTR != ComDal_Llce_pConfigPtr)
    {
        u8LlceDescriptorId   =  (ComDal_Llce_pConfigPtr->pComChannelMap)[ComChannel].ComDescriptorIndex;

        if(COMDAL_NO_ERROR == ComDal_Llce_aeRxState[u8LlceDescriptorId] )
        {
            /*write external buffer */
            for(MsgIndex = 0; MsgIndex < LlceDataLen; MsgIndex++)
            {   
                ReadBuffer[MsgIndex] = (char)(((ComDal_Llce_pConfigPtr->pLlceChanDescriptor)[u8LlceDescriptorId]).pDataBuffer[MsgIndex]);             
            }
            
            ComDal_Llce_aeRxState[u8LlceDescriptorId] = COMDAL_OBSOLETE;
        }
        else
        {
            ReturnValue = ComDal_Llce_aeRxState[u8LlceDescriptorId];            
        }
    }
    
    return ReturnValue;
}


/*=================================================================================================================================*/
/**
@brief      ComDal_Can_WriteData - function used to create a LLCE write request if no CAN LLCE TX is ongoing

@param[in]  ComChannel     Logical BSW ComDal channel id.
@param[in]  LlceDataLen    Maximum data length provided for that channel
@param[out] TxDataString   Data to be written on LLCE for the assigned input channel.


@return     none
*/
/*=================================================================================================================================*/
FUNC(void, COMDAL_APP_CODE) ComDal_Llce_WriteData(VAR(ComDal_ChannelType, AUTOMATIC) ComChannel, P2VAR(char, AUTOMATIC, COMDAL_APPL_DATA) TxDataString, VAR(uint8, AUTOMATIC) LlceDataLen)
{
    VAR(uint8, AUTOMATIC) u8Index;
    VAR(ComDal_BswDescriptorIdType, AUTOMATIC) u8LlceDescriptorId = 0;

    if(NULL_PTR != ComDal_Llce_pConfigPtr)
    {
        u8LlceDescriptorId = (ComDal_Llce_pConfigPtr->pComChannelMap)[ComChannel].ComDescriptorIndex;
        
        /*check if not transmition is ongoing*/
        if((TX_IDLE_STATE == ComDal_Llce_aeTxState[u8LlceDescriptorId]) || (TX_INITIALIZED_STATE == ComDal_Llce_aeTxState[u8LlceDescriptorId]))
        {
            /*update Tx data buffer with new data */ 
            for(u8Index = 0; u8Index < LlceDataLen; u8Index++)
            {   
                (((ComDal_Llce_pConfigPtr->pLlceChanDescriptor)[u8LlceDescriptorId]).pDataBuffer)[u8Index] = (uint8)TxDataString[u8Index];   
            }

           /*enable a new transmition*/
           ComDal_Llce_aeTxState[u8LlceDescriptorId] = TX_REQUESTED_STATE;           
        }
    }
}

/*=================================================================================================================================*/
/**
@brief      CanIf_TxConfirmation - callback function to simulate CanIf
@details    This callback is called from CAN LLCE driver whenever a new TX message was succesfully 
            transmited.
            On TX complete the TX line becomes IDLE and current TX request buffer is cleared. 
            
@return     none
*/
/*=================================================================================================================================*/
FUNC(void, CANIF_CODE) CanIf_TxConfirmation(VAR(PduIdType, AUTOMATIC) CanTxPduId)
{
    volatile VAR(uint8, AUTOMATIC) u8Index;
    volatile VAR(PduIdType, AUTOMATIC) controllerId;
    volatile VAR(ComDal_BswDescriptorIdType, AUTOMATIC) u8LlceDescriptorId = 0;    
    
    for(u8LlceDescriptorId = 0; u8LlceDescriptorId < COMDAL_MAX_LLCE_DESCRIPTORS; u8LlceDescriptorId++)
    {
        controllerId = ((ComDal_Llce_pConfigPtr->pLlceChanDescriptor)[u8LlceDescriptorId]).SwPduHandle;
    
        /*get the descriptor that matches in id and Handle**/ 
        if(CanTxPduId == controllerId)
        {
            /* Increment counter to record transmission of a message */
            ComDal_Llce_au8TxConfirmation[u8LlceDescriptorId]++;
            
            ComDal_Llce_abTxFlag[u8LlceDescriptorId] = TRUE;
    
            ComDal_Llce_aeTxState[u8LlceDescriptorId] = TX_IDLE_STATE;
            
            /*clear internal Tx Data */
            for(u8Index = 0; u8Index < ((ComDal_Llce_pConfigPtr->pLlceChanDescriptor)[u8LlceDescriptorId]).CanDataLen; u8Index++)
            {   
                (((ComDal_Llce_pConfigPtr->pLlceChanDescriptor)[u8LlceDescriptorId]).pDataBuffer)[u8Index] = (char)0U;   
            }

            if(((ComDal_Llce_pConfigPtr->pLlceChanDescriptor)[u8LlceDescriptorId]).ComDal_Notification != NULL_PTR)			
            {
                VAR(ComDal_ChannelType, AUTOMATIC) ComChannelId = 
                    ((ComDal_Llce_pConfigPtr->pLlceChanDescriptor)[u8LlceDescriptorId]).ComChannelId;

                /* Call the configured notification function */
                ((ComDal_Llce_pConfigPtr->pLlceChanDescriptor)[u8LlceDescriptorId]).ComDal_Notification(ComChannelId);    
            }
    
            /*intentional loop break to reduce run-time since only one match is expected*/
            break;
        }
    }
}


/*=================================================================================================================================*/
/**
@brief      CanIf_RxIndication - callback function to simulate CanIf
@details    This callback is called from CAN LLCE driver whenever a new RX message was successfully 
            received.
            This function will identify the channel descriptor for which this message was sent
            and store received data in its allocated buffer.            

@return     none            
*/
/*=================================================================================================================================*/
FUNC(void, CANIF_CODE) CanIf_RxIndication(P2CONST(Can_HwType, AUTOMATIC, COMDAL_APPL_DATA) mailbox, P2CONST(PduInfoType, AUTOMATIC, COMDAL_APPL_DATA) Rx_Pdu)
{
    /** Local variable */
    /*volatile VAR(PduIdType, AUTOMATIC) swPduHandle;*/
    volatile VAR(Can_IdType, AUTOMATIC) id;
    volatile VAR(uint8, AUTOMATIC) u8Index = 0U;
    volatile VAR(ComDal_BswDescriptorIdType, AUTOMATIC) u8LlceDescriptorId = 0;    
    
    for(u8LlceDescriptorId = 0; u8LlceDescriptorId < COMDAL_MAX_LLCE_DESCRIPTORS; u8LlceDescriptorId++)
    {
        id = ((ComDal_Llce_pConfigPtr->pLlceChanDescriptor)[u8LlceDescriptorId]).PduId | CAN_FD_ID_DESCRIPTOR;
    
        if(mailbox->Hoh == id)
        {                      
            for(u8Index = 0; u8Index < Rx_Pdu->SduLength; u8Index++)
            {
                (((ComDal_Llce_pConfigPtr->pLlceChanDescriptor)[u8LlceDescriptorId]).pDataBuffer)[u8Index] = (char)(Rx_Pdu->SduDataPtr[u8Index]);                 
            }
            
            ComDal_Llce_u16MessageId[u8LlceDescriptorId] = mailbox->CanId;
            ComDal_Llce_aeRxState[u8LlceDescriptorId] = COMDAL_NO_ERROR; 
            if(((ComDal_Llce_pConfigPtr->pLlceChanDescriptor)[u8LlceDescriptorId]).ComDal_Notification != NULL_PTR)			
            {
                VAR(ComDal_ChannelType, AUTOMATIC) ComChannelId = 
                    ((ComDal_Llce_pConfigPtr->pLlceChanDescriptor)[u8LlceDescriptorId]).ComChannelId;

                /* Call the configured notification function */
                ((ComDal_Llce_pConfigPtr->pLlceChanDescriptor)[u8LlceDescriptorId]).ComDal_Notification(ComChannelId);    
            }
            
			/*intentional loop break to reduce run-time since only one match is expected*/
			break;
        }
    }
}


/*=================================================================================================================================*/
/**
@brief      ComDal_Llce_PrepareTxMessage internal function used to setup the CAN message with specific parameters

@return     none
*/
/*=================================================================================================================================*/
static FUNC(void, COMDAL_APP_CODE) ComDal_Llce_PrepareTxMessage(VAR(ComDal_ChannelType, AUTOMATIC) ComChannel, P2VAR(Can_PduType, AUTOMATIC, CAN_APPL_VAR) PduInfo)
{
    VAR(ComDal_BswDescriptorIdType, AUTOMATIC) u8LlceDescriptorId = (ComDal_Llce_pConfigPtr->pComChannelMap)[ComChannel].ComDescriptorIndex;
    
    /** ID, length, etc. */
    PduInfo->length = ((ComDal_Llce_pConfigPtr->pLlceChanDescriptor)[u8LlceDescriptorId]).CanDataLen;
    PduInfo->swPduHandle = ((ComDal_Llce_pConfigPtr->pLlceChanDescriptor)[u8LlceDescriptorId]).SwPduHandle;
    PduInfo->sdu = (uint8 *)(((ComDal_Llce_pConfigPtr->pLlceChanDescriptor)[u8LlceDescriptorId]).pDataBuffer);
    PduInfo->id = ((ComDal_Llce_pConfigPtr->pLlceChanDescriptor)[u8LlceDescriptorId]).PduId | CAN_FD_ID_DESCRIPTOR;
    
}


#if  (CAN_43_LLCE_BUSOFFPOLL_SUPPORTED == STD_ON)
/*=================================================================================================================================*/
/**
@brief      CanIf_ControllerBusOff - callback function to simulate CanIf
@details    This callback can be called to handle Bus Off request if BusOff is handled in polling mode
            The definition of this function is required by Can driver

@return     none
*/
/*=================================================================================================================================*/
FUNC(void, CANIF_CODE) CanIf_ControllerBusOff(VAR(uint8, AUTOMATIC) Controller)
{
    /*stub function definition required by Can driver - this function is not currently used*/
    (void)Controller; /*parameter currently not used*/
}
#endif /* CAN_43_LLCE_BUSOFFPOLL_SUPPORTED == STD_ON */

/*=================================================================================================================================*/
/**
@brief      Control of the CAN message transmission and CAN message payload
@details    Within this API, the upper layer module (called module) shall check whether the available data fits into the buffer size
            reported by PduInfoPtr->SduLength. If it fits, it shall copy its data into the buffer provided by PduInfoPtr->SduDataPtr
            and update the length of the actual copied data in PduInfoPtr->SduLength. If not, it returns E_NOT_OK without changing
            
TxPduId     ID of the SDU that is requested to be transmitted
PduInfoPtr  Pointer to a buffer (SduDataPtr) to where the SDU data shall be copied, and the available buffer size in SduLengh

@return     E_OK - to trigger CAN message transmission
            E_NOT_OK - do not trigger CAN message transmission
*/
/*=================================================================================================================================*/
FUNC (Std_ReturnType, CANIF_CODE) CanIf_TriggerTransmit( VAR(PduIdType, AUTOMATIC) TxPduId, P2VAR(PduInfoType, AUTOMATIC, CANIF_VAR) PduInfoPtr)
{
    VAR(Std_ReturnType, AUTOMATIC) TxStatus = E_NOT_OK;
    /* No check required. ComDal accepts only valid PDU with a valid data buffer allocated */
    (void)TxPduId;
    (void)PduInfoPtr;
    return TxStatus;
}

/*=================================================================================================================================*/
/**
@brief      CanIf_RxIndication - callback function to simulate CanIf
@details    This callback is called from Can Driver: Can_SetControllerMode to inform higher levels about the controller state.
            The definition of this function is required by Can driver

@return     none 
*/        
/*=================================================================================================================================*/    
FUNC(void, CANIF_CODE) CanIf_ControllerModeIndication(VAR(uint8, AUTOMATIC) Controller, VAR(Can_ControllerStateType, AUTOMATIC) ControllerMode)
{
    /*stub function definition required by Can driver  - this function is not currently used*/
    (void)Controller;      /*parameter currently not used*/
    (void)ControllerMode;  /*parameter currently not used*/
}


/*=================================================================================================================================*/
/**
@brief      ComDal_Can_GetMessageHandlerId - API used to provide application with specific channel information, Can ID of last message
@param[in]  ComChannel     Logical BSW ComDal channel id.
@return     uint16: Specific channel ID.
*/
/*=================================================================================================================================*/
FUNC(uint16, COMDAL_APP_CODE) ComDal_Llce_GetMessageHandlerId(VAR(ComDal_ChannelType, AUTOMATIC) ComChannel)
{   
    uint16 u16MessageHandlerID = 0;
    if (ComChannel < COMDAL_MAX_LLCE_DESCRIPTORS)
    {
        u16MessageHandlerID = ComDal_Llce_u16MessageId[ComChannel];
    }
    return u16MessageHandlerID;
}

#endif /*COMDAL_MAX_LLCE_DESCRIPTORS*/
/*=================================================================================================================================*/
#ifdef __cplusplus
}
#endif /* __cplusplus */
/** @} */
