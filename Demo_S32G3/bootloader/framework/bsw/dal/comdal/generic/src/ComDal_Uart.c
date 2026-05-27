/*================================================================================================*/
/*
*   @file               ComDal_Uart.c
*   project             AUTOSAR 4.4 GOLDVIP
*   platform            CORTEXM

*   @version            24.12.0
*
*   @brief              Integration Framework - ComDal Abstraction Layer internal Uart driver 
                        component
*   @details            This file implements the Uart driver for the ComDal interface to application 
                        layer.
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

/*==================================================================================================
*                                        INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#include "Std_Types.h"
#include "ComDal.h"
#include "ComDal_Types.h"

#if (COMDAL_DEV_ERROR_DETECT == STD_ON)
#include "Det.h"
#endif /* COMDAL_DEV_ERROR_DETECT == STD_ON */

#if COMDAL_MAX_UART_DESCRIPTORS > 0U

#include "SysDal.h"
#include "ComDal_Uart.h"
#include "StringLib.h"
#include "MathUtils.h"
#include "CDD_Uart.h"

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define COMDAL_UART_VENDOR_ID_C                       43

#define COMDAL_UART_AR_RELEASE_MAJOR_VERSION_C        4
#define COMDAL_UART_AR_RELEASE_MINOR_VERSION_C        4
#define COMDAL_UART_AR_RELEASE_REVISION_VERSION_C     0

#define COMDAL_UART_SW_MAJOR_VERSION_C                24
#define COMDAL_UART_SW_MINOR_VERSION_C                12
#define COMDAL_UART_SW_PATCH_VERSION_C                0

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/
/* Check if source file and Comdal configuration header file are of the same vendor */
#if (COMDAL_UART_VENDOR_ID_C != COMDAL_CFG_VENDOR_ID)
    #error "ComDal_Uart.c and ComDal_Cfg.h have different vendor ids"
#endif

/* Check if source file and Comdal configuration header file are of the same Autosar version */
#if ((COMDAL_UART_AR_RELEASE_MAJOR_VERSION_C != COMDAL_CFG_AR_RELEASE_MAJOR_VERSION) || \
     (COMDAL_UART_AR_RELEASE_MINOR_VERSION_C != COMDAL_CFG_AR_RELEASE_MINOR_VERSION) || \
     (COMDAL_UART_AR_RELEASE_REVISION_VERSION_C != COMDAL_CFG_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of ComDal_Uart.c and ComDal_Cfg.h are different"
#endif

/* Check if source file and Comdal configuration header file are of the same Software version */
#if ((COMDAL_UART_SW_MAJOR_VERSION_C != COMDAL_CFG_SW_MAJOR_VERSION) || \
     (COMDAL_UART_SW_MINOR_VERSION_C != COMDAL_CFG_SW_MINOR_VERSION) || \
     (COMDAL_UART_SW_PATCH_VERSION_C != COMDAL_CFG_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of ComDal_Uart.c and ComDal_Cfg.h are different"
#endif

/* Check if source file and Comdal types header file are of the same vendor */
#if (COMDAL_UART_VENDOR_ID_C != COMDAL_TYPES_VENDOR_ID)
    #error "ComDal_Uart.c and ComDal_Types.h have different vendor ids"
#endif

/* Check if source file and Comdal types header file are of the same Autosar version */
#if ((COMDAL_UART_AR_RELEASE_MAJOR_VERSION_C != COMDAL_TYPES_AR_RELEASE_MAJOR_VERSION) || \
     (COMDAL_UART_AR_RELEASE_MINOR_VERSION_C != COMDAL_TYPES_AR_RELEASE_MINOR_VERSION) || \
     (COMDAL_UART_AR_RELEASE_REVISION_VERSION_C != COMDAL_TYPES_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of ComDal_Uart.c and ComDal_Types.h are different"
#endif

/* Check if source file and Comdal types header file are of the same Software version */
#if ((COMDAL_UART_SW_MAJOR_VERSION_C != COMDAL_TYPES_SW_MAJOR_VERSION) || \
     (COMDAL_UART_SW_MINOR_VERSION_C != COMDAL_TYPES_SW_MINOR_VERSION) || \
     (COMDAL_UART_SW_PATCH_VERSION_C != COMDAL_TYPES_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of ComDal_Uart.c and ComDal_Types.h are different"
#endif

/* Check if source file and Comdal Can header file are from the same vendor */
#if (COMDAL_UART_VENDOR_ID_C != COMDAL_UART_VENDOR_ID)
    #error "ComDal_Uart.c and ComDal_Uart.h have different vendor ids"
#endif

/* Check if source file and Comdal Can header file are of the same Autosar version */
#if ((COMDAL_UART_AR_RELEASE_MAJOR_VERSION_C != COMDAL_UART_AR_RELEASE_MAJOR_VERSION) || \
     (COMDAL_UART_AR_RELEASE_MINOR_VERSION_C != COMDAL_UART_AR_RELEASE_MINOR_VERSION) || \
     (COMDAL_UART_AR_RELEASE_REVISION_VERSION_C != COMDAL_UART_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of ComDal_Uart.c and ComDal_Uart.h are different"
#endif

/* Check if source file and Comdal Can header file are of the same Software version */
#if ((COMDAL_UART_SW_MAJOR_VERSION_C != COMDAL_UART_SW_MAJOR_VERSION) || \
     (COMDAL_UART_SW_MINOR_VERSION_C != COMDAL_UART_SW_MINOR_VERSION) || \
     (COMDAL_UART_SW_PATCH_VERSION_C != COMDAL_UART_SW_PATCH_VERSION) \
    )
  #error "Software Version Numbers of ComDal_Uart.c and ComDal_Uart.h are different"
#endif

#ifndef DISABLE_MCAL_INTERMODULE_ASR_CHECK
 /* Check if source file and Std_Types header file are of the same AUTOSAR version */
 #if ((COMDAL_UART_AR_RELEASE_MAJOR_VERSION_C != STD_AR_RELEASE_MAJOR_VERSION) || \
      (COMDAL_UART_AR_RELEASE_MINOR_VERSION_C != STD_AR_RELEASE_MINOR_VERSION) \
     )
 #error "AutoSar Version Numbers of ComDal_Uart.c and Std_Types.h are different"
 #endif
 /* Check if source file and Mcal header file are of the same AUTOSAR version */
 #if ((COMDAL_UART_AR_RELEASE_MAJOR_VERSION_C != MCAL_AR_RELEASE_MAJOR_VERSION) || \
      (COMDAL_UART_AR_RELEASE_MINOR_VERSION_C != MCAL_AR_RELEASE_MINOR_VERSION) \
     )
 #error "AutoSar Version Numbers of ComDal_Uart.c and Mcal.h are different"
 #endif
#endif

/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
typedef enum
{
    RX_IDLE  = 0U,          /**< @brief UART Rx state IDLE. */
    RX_ONGOING,                /**< @brief UART RX state on going.*/
} ComDal_UartRxStateType;

typedef struct{
    /**< @brief Rx receiving state */
    VAR(ComDal_UartRxStateType, AUTOMATIC) eRxState;
    /**< @brief Rx message timeouted */
    VAR(uint32, AUTOMATIC)                 u32Timeout;
    /**< @brief Rx message size */
    VAR(uint8, AUTOMATIC)                  u8MsgSize;
    /**< @brief Rx receive size */
    VAR(uint8, AUTOMATIC)                  u8RcvSize;

}ComDal_UartMsgRcvStateType;

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/

#define GET_UART_DESCRIPTOR_IDX(ComIdx) ComDal_Uart_pConfig->pComChannelMap[ComIdx].ComDescriptorIndex

#define GET_UART_DESCRIPTOR(ComIdx) ((ComDal_Uart_pConfig == NULL_PTR) ? NULL_PTR :                 \
                                     (&(ComDal_Uart_pConfig->pUartChanDescriptor                    \
                                        [GET_UART_DESCRIPTOR_IDX(ComIdx)]                           \
                                    )))

#define GET_RINGBUFFER_FREE_SPACE(pRingBuffer) (pRingBuffer->u16Size - pRingBuffer->u16ElementsCount)

#define ADVANCE_UART_RINGBUFF_HEAD(pRingBuffer, pos) (pRingBuffer->u16HeadIndex =             \
                                                        MOD((pRingBuffer->u16HeadIndex + pos), \
                                                            pRingBuffer->u16Size))
#define ADVANCE_UART_RINGBUFF_TAIL(pRingBuffer, pos) (pRingBuffer->u16TailIndex =             \
                                                        MOD((pRingBuffer->u16TailIndex + pos), \
                                                            pRingBuffer->u16Size))
/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/
#define UART_TIMEOUT_RX_MSG   100u

/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/


/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/

/**
@brief Pointer to global ComDal configuration structure
*/
static P2CONST(ComDal_ConfigType, AUTOMATIC, COMDAL_APPL_CONST) ComDal_Uart_pConfig;

/**
@brief Pointer to list of Uart descriptors
*/
static P2CONST(ComDal_UartDescriptorType, AUTOMATIC, COMDAL_APPL_CONST) ComDal_Uart_pDescriptors;

/**
@brief Last UART TX transfer size
*/
static volatile VAR(uint16, AUTOMATIC) ComDal_Uart_u16LastTxSize[COMDAL_UART_HW_INSTANCES];

/**
@brief List of UART TX ring buffers
*/
static P2VAR(ComDal_RingBuffer, AUTOMATIC, COMDAL_APPL_DATA) ComDal_Uart_pTxBuffers[COMDAL_UART_HW_INSTANCES];

/**
@brief List of UART RX ring buffers
*/
static P2VAR(ComDal_RingBuffer, AUTOMATIC, COMDAL_APPL_DATA) ComDal_Uart_pRxBuffers[COMDAL_UART_HW_INSTANCES];

/**
@brief Variable used to store the Uart character
*/
static VAR(uint8, AUTOMATIC) ComDal_Uart_u8RxData[COMDAL_UART_HW_INSTANCES];

/**
@brief Represents the state of the ComDal Uart channels
*/
static VAR(ComDal_State, AUTOMATIC) ComDal_Uart_ChannelsState[COMDAL_UART_HW_INSTANCES];
/**
@brief Represents the message receiving state
*/
static VAR(ComDal_UartMsgRcvStateType, AUTOMATIC) ComDal_Uart_RxRecvState[COMDAL_UART_HW_INSTANCES];
/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/
#if (COMDAL_UART_MAX_RX_CHANNELS > 0U)
static FUNC(void, COMDAL_APP_CODE) ComDal_Uart_RxProcess(uint8 u8UartPortId);
FUNC(void, COMDAL_APP_CODE) ComDal_Uart_RxIndication
(
    uint32 u32Channel,
    Uart_EventType UartEvent
);
FUNC(void, COMDAL_APP_CODE) ComDal_Uart_ReadFromRingBuffer
(
    P2VAR(ComDal_RingBuffer, AUTOMATIC, COMDAL_APPL_DATA) pRingBuffer,
    P2VAR(uint8, AUTOMATIC, COMDAL_APPL_DATA) pDestinationBuffer,
    P2VAR(uint16, AUTOMATIC, COMDAL_APPL_DATA) pBufSize
);
static FUNC(void, COMDAL_APP_CODE) ComDal_Uart_ResetRxState
(
    VAR(uint8, AUTOMATIC) u8PortId
);
static FUNC(void, COMDAL_APP_CODE) ComDal_Uart_UpdateRxTimeout
(
    VAR(uint8, AUTOMATIC) u8PortId
);
static FUNC(void, COMDAL_APP_CODE) ComDal_Uart_ProcessRcvData
(
    VAR(uint32, AUTOMATIC) u32Channel
);
#endif /* (COMDAL_UART_MAX_RX_CHANNELS > 0U) */

#if (COMDAL_UART_MAX_TX_CHANNELS > 0U)
static FUNC(void, COMDAL_APP_CODE) ComDal_Uart_TxProcess(uint8 u8UartPortId);
FUNC(void, COMDAL_APP_CODE) ComDal_Uart_TxIndication
(
    uint32 u32Channel,
    Uart_EventType UartEvent
);
static FUNC(void, COMDAL_APP_CODE) ComDal_Uart_GetNextTxBuffer
(
    P2VAR(ComDal_RingBuffer, AUTOMATIC, COMDAL_APPL_DATA) pRingBuffer,
    P2VAR(uint32_t, AUTOMATIC, COMDAL_APPL_DATA) pNextBuffer,
    P2VAR(uint16_t, AUTOMATIC, COMDAL_APPL_DATA) pNextBufferSize
);
FUNC(void, COMDAL_APP_CODE) ComDal_Uart_WriteToRingBuffer
(
    P2VAR(ComDal_RingBuffer, AUTOMATIC, COMDAL_APPL_DATA) pRingBuffer,
    P2VAR(uint8, AUTOMATIC, COMDAL_APPL_DATA) pSourceBuffer,
    VAR(uint16, AUTOMATIC) u16BufSize
);
#endif

FUNC(void, COMDAL_APP_CODE) ComDal_Uart_TxRxIndication
(
    uint32 hwInstance,
    Uart_EventType UartEvent
);
static inline FUNC(void, COMDAL_APP_CODE) ComDal_Uart_RingBufferAppend
(
    P2VAR(ComDal_RingBuffer, AUTOMATIC, COMDAL_APPL_DATA) RingBuffer,
    uint8 u8Data
);
static inline FUNC(ComDal_BswDescriptorIdType, COMDAL_APP_CODE) ComDal_Uart_GetDescriptorIdxByUartChannelId
(
    VAR(uint8, AUTOMATIC) UartChannelId,
    VAR(ComDal_AccessType, AUTOMATIC) AccessType
);

#if(COMDAL_UART_MAX_RX_CHANNELS > 0)
/*================================================================================================*/
/**
@brief      Internal function used to receive messages asynchronously

@param[in]  u8UartPortId  Uart hardware instance id

@return     None
*/
/*================================================================================================*/
static FUNC(void, COMDAL_APP_CODE) ComDal_Uart_RxProcess(uint8 u8UartPortId)
{
    if (E_OK == Uart_AsyncReceive(u8UartPortId, &ComDal_Uart_u8RxData[u8UartPortId], 1))
    {
        ComDal_Uart_ChannelsState[u8UartPortId] = COMDAL_STATE_RX_ONGOING;
    }
}

/*================================================================================================*/
/**
@brief      Internal function used to reset message receive state machine

@param[in]  u8UartPortId  Uart hardware instance id

@return     None
*/
/*================================================================================================*/
static FUNC(void, COMDAL_APP_CODE) ComDal_Uart_ResetRxState
(
    VAR(uint8, AUTOMATIC) u8PortId
)
{
    P2VAR(ComDal_RingBuffer, AUTOMATIC, COMDAL_APPL_DATA) pRingBuffer = ComDal_Uart_pRxBuffers[u8PortId];

    if(RX_ONGOING == ComDal_Uart_RxRecvState[u8PortId].eRxState)
    {
        /* Reset the head to the value before a new message is received */
        pRingBuffer->u16HeadIndex = MOD((pRingBuffer->u16HeadIndex - (ComDal_Uart_RxRecvState[u8PortId].u8RcvSize + 1u)),
                                        pRingBuffer->u16Size);
        pRingBuffer->u16ElementsCount -= (ComDal_Uart_RxRecvState[u8PortId].u8RcvSize + 1u);
        ComDal_Uart_RxRecvState[u8PortId].u32Timeout = 0u;
        ComDal_Uart_RxRecvState[u8PortId].u8MsgSize = 0u;
        ComDal_Uart_RxRecvState[u8PortId].u8RcvSize = 0u;
        ComDal_Uart_RxRecvState[u8PortId].eRxState = RX_IDLE;
    }
}


/*================================================================================================*/
/**
@brief      Internal function used to verify if timeout occured during receiving message

@param[in]  u8UartPortId  Uart hardware instance id

@return     None
*/
/*================================================================================================*/
static FUNC(void, COMDAL_APP_CODE) ComDal_Uart_UpdateRxTimeout
(
    VAR(uint8, AUTOMATIC) u8PortId
)
{
    if(RX_ONGOING == ComDal_Uart_RxRecvState[u8PortId].eRxState)
    {
        ComDal_Uart_RxRecvState[u8PortId].u32Timeout += 1u;
        if(UART_TIMEOUT_RX_MSG <= ComDal_Uart_RxRecvState[u8PortId].u32Timeout)
        {
            ComDal_Uart_ResetRxState(u8PortId);
            (void)Uart_AsyncReceive(u8PortId, &ComDal_Uart_u8RxData[u8PortId], 1);
        }
    }
}
/*================================================================================================*/
/**
@brief      Internal function used to process received data
@param[in]  u32Channel  Uart hardware instance id
@return     None
*/
/*================================================================================================*/
static FUNC(void, COMDAL_APP_CODE) ComDal_Uart_ProcessRcvData
(
    VAR(uint32, AUTOMATIC) u32Channel
)
{
    switch (ComDal_Uart_RxRecvState[u32Channel].eRxState)
    {
    case RX_IDLE:
        ComDal_Uart_RxRecvState[u32Channel].u8MsgSize = ComDal_Uart_u8RxData[u32Channel];
        ComDal_Uart_RxRecvState[u32Channel].eRxState = RX_ONGOING;
        break;
    case RX_ONGOING:
        ComDal_Uart_RxRecvState[u32Channel].u32Timeout  = 0;
        ComDal_Uart_RxRecvState[u32Channel].u8RcvSize += 1u;
        if(ComDal_Uart_RxRecvState[u32Channel].u8RcvSize ==
            ComDal_Uart_RxRecvState[u32Channel].u8MsgSize)
        {
            ComDal_Uart_RxRecvState[u32Channel].u8RcvSize = 0u;
            ComDal_Uart_RxRecvState[u32Channel].u8MsgSize = 0u;
            ComDal_Uart_RxRecvState[u32Channel].eRxState = RX_IDLE;
        }
        break;
    }
    ComDal_Uart_RingBufferAppend(ComDal_Uart_pRxBuffers[u32Channel], ComDal_Uart_u8RxData[u32Channel]);
    Uart_SetBuffer(u32Channel, &ComDal_Uart_u8RxData[u32Channel], 1, UART_RECEIVE);
}


/*================================================================================================*/
/**
@brief      Called by RTD Uart driver to notify application the transfer is complete
@details    Callback can receive as UartEvent parameter:
            - UART_EVENT_RX_FULL if the position in the ring buffer needs to be changed
            - UART_EVENT_END_TRANSFER if the transfer is complete
            - UART_EVENT_ERROR if the transfer encountered an error

@param[in]  u32Channel  Logical BSW Uart channel id
@param[in]  UartEvent   Indicates the state in which the callback was called

@return     None
*/
/*================================================================================================*/
FUNC(void, COMDAL_APP_CODE) ComDal_Uart_RxIndication
(
    uint32 u32Channel,
    Uart_EventType UartEvent
)
{
    if(UART_EVENT_RX_FULL == UartEvent)
    {
        ComDal_Uart_ProcessRcvData(u32Channel);
    }
    else
    {
        ComDal_Uart_ResetRxState(u32Channel);
        (void)Uart_AsyncReceive(u32Channel, &ComDal_Uart_u8RxData[u32Channel], 1);
    }

    if(UART_EVENT_END_TRANSFER == UartEvent)
    {
        if (((ComDal_Uart_pConfig->pUartChanDescriptor)[u32Channel]).ComDal_Notification != NULL_PTR)
        {
            VAR(ComDal_ChannelType, AUTOMATIC) ComDescriptorId = 
                ComDal_Uart_GetDescriptorIdxByUartChannelId(u32Channel, COMDAL_RXCHANNEL);

            /*Call the configured notification function*/
            ((ComDal_Uart_pConfig->pUartChanDescriptor)[u32Channel]).ComDal_Notification(ComDescriptorId);
        }
    }
}

/*================================================================================================*/
/**
@brief      Copy data from a ring buffer to an input buffer.

@details    This function will advance the tail index based on the amount of data copied.
            The pDestinationBuffer will be terminated by COMDAL_EOT_CHAR

@param[in]      pRingBuffer         Pointer to ring buffer instance
@param[in/out]  pDestinationBuffer  Pointer to the destination buffer
@param[in]      u16BufSize          Size of the provided buffer

@return     None
*/
/*================================================================================================*/
FUNC(void, COMDAL_APP_CODE) ComDal_Uart_ReadFromRingBuffer
(
    P2VAR(ComDal_RingBuffer, AUTOMATIC, COMDAL_APPL_DATA) pRingBuffer,
    P2VAR(uint8, AUTOMATIC, COMDAL_APPL_DATA) pDestinationBuffer,
    P2VAR(uint16, AUTOMATIC, COMDAL_APPL_DATA) pBufSize
)
{
    VAR(uint16, AUTOMATIC) u16Idx = 0u;
    VAR(uint16, AUTOMATIC) u16MsgSize = (uint16_t)pRingBuffer->pBuffer[pRingBuffer->u16TailIndex];

    if((u16MsgSize <=  (pRingBuffer->u16ElementsCount - 1)) &&
        ((*pBufSize) >= u16MsgSize))
    {
        ADVANCE_UART_RINGBUFF_TAIL(pRingBuffer, 1);
        for (u16Idx = 0; u16Idx < u16MsgSize; u16Idx++)
        {
            pDestinationBuffer[u16Idx] = pRingBuffer->pBuffer[pRingBuffer->u16TailIndex];
            ADVANCE_UART_RINGBUFF_TAIL(pRingBuffer, 1);
        }
        pRingBuffer->u16ElementsCount -= (u16MsgSize + 1u);
        *pBufSize = u16MsgSize;
    }
    else
    {
        *pBufSize = 0u;
    }
}
#endif /* COMDAL_UART_MAX_RX_CHANNELS > 0 */

#if(COMDAL_UART_MAX_TX_CHANNELS > 0)
/*================================================================================================*/
/**
@brief      Internal function used to transmit the message asynchronously

@param[in]  u8UartPortId  Uart hardware instance id

@return     None
*/
/*================================================================================================*/
static FUNC(void, COMDAL_APP_CODE) ComDal_Uart_TxProcess(uint8 u8UartPortId)
{
    VAR(uint32_t, AUTOMATIC) u32TxData = 0U;
    VAR(uint16_t, AUTOMATIC) u16TxDataSize = 0U;

    ComDal_Uart_GetNextTxBuffer(ComDal_Uart_pTxBuffers[u8UartPortId], &u32TxData, &u16TxDataSize);

    if (u16TxDataSize > 0U)
    {
        ComDal_Uart_u16LastTxSize[u8UartPortId] = u16TxDataSize;
        ComDal_Uart_ChannelsState[u8UartPortId] = COMDAL_STATE_TX_ONGOING;
        (void)Uart_AsyncSend(u8UartPortId, (uint8 *)u32TxData, u16TxDataSize);
    }
}

/*================================================================================================*/
/**
@brief      Called by RTD Uart driver to notify application the transfer is complete
@details    Callback can receive as UartEvent parameter:
            - UART_EVENT_TX_EMPTY if the position in the ring buffer needs to be changed
            - UART_EVENT_END_TRANSFER if the transfer is complete
            - UART_EVENT_ERROR if the transfer encountered an error

@param[in]  u32Channel  Logical BSW Uart channel id
@param[in]  UartEvent   Indicates the state in which the callback was called

@return     None
*/
/*================================================================================================*/
FUNC(void, COMDAL_APP_CODE) ComDal_Uart_TxIndication
(
    uint32 u32Channel,
    Uart_EventType UartEvent
)
{
    P2VAR(ComDal_RingBuffer, AUTOMATIC, COMDAL_APPL_DATA) pRingBuffer = NULL_PTR;
    VAR(uint32_t, AUTOMATIC) u32TxData = 0U;
    VAR(uint16_t, AUTOMATIC) u16TxDataSize = 0U;

    if (UartEvent == UART_EVENT_TX_EMPTY)
    {
        pRingBuffer = ComDal_Uart_pTxBuffers[u32Channel];

        pRingBuffer->u16TailIndex = MOD
        (
            pRingBuffer->u16TailIndex + ComDal_Uart_u16LastTxSize[u32Channel],
            pRingBuffer->u16Size
        );

        pRingBuffer->u16ElementsCount -= ComDal_Uart_u16LastTxSize[u32Channel];

        ComDal_Uart_GetNextTxBuffer(pRingBuffer, &u32TxData, &u16TxDataSize);

        if (u16TxDataSize > 0U)
        {
            (void)Uart_SetBuffer(u32Channel, (uint8 *)u32TxData, u16TxDataSize, UART_SEND);
            ComDal_Uart_u16LastTxSize[u32Channel] = u16TxDataSize;
        }
    }
    else if (UartEvent == UART_EVENT_END_TRANSFER)
    {
        ComDal_Uart_ChannelsState[u32Channel] = COMDAL_STATE_IDLE;

        if (((ComDal_Uart_pConfig->pUartChanDescriptor)[u32Channel]).ComDal_Notification != NULL_PTR)
        {
            VAR(ComDal_ChannelType, AUTOMATIC) ComDescriptorId = 
                ComDal_Uart_GetDescriptorIdxByUartChannelId(u32Channel, COMDAL_TXCHANNEL);

            /*Call the configured notification function*/
            ((ComDal_Uart_pConfig->pUartChanDescriptor)[u32Channel]).ComDal_Notification(ComDescriptorId);
        }
    }
    else if (UartEvent == UART_EVENT_ERROR)
    {
        ComDal_Uart_ChannelsState[u32Channel] = COMDAL_STATE_ERROR;
    }
}

/*================================================================================================*/
/**
@brief      Get the next chunck of bytes from a ring buffer to be transmitted
@details    The TX data is hold on a ring buffer but the underlying drivers needs a pointer to
            a continuous memory buffer. This function will compare Head and Tail indices of the
            ring buffer and return a correct buffer pointer and buffer size.

@param[in]  pRingBuffer     Pointer to the ring buffer structure
@param[out] pNextBuffer     Pointer to the next continuous buffer of data from ring buffer
@param[out] pNextBufferSize Size of the next continuous buffer of data

@return     None
*/
/*================================================================================================*/
static FUNC(void, COMDAL_APP_CODE) ComDal_Uart_GetNextTxBuffer
(
    P2VAR(ComDal_RingBuffer, AUTOMATIC, COMDAL_APPL_DATA) pRingBuffer,
    P2VAR(uint32_t, AUTOMATIC, COMDAL_APPL_DATA) pNextBuffer,
    P2VAR(uint16_t, AUTOMATIC, COMDAL_APPL_DATA) pNextBufferSize
)
{
    if (pRingBuffer->u16TailIndex < pRingBuffer->u16HeadIndex)
    {
        *pNextBufferSize = pRingBuffer->u16HeadIndex - pRingBuffer->u16TailIndex;
        *pNextBuffer = (uint32_t)&pRingBuffer->pBuffer[pRingBuffer->u16TailIndex];
    }
    else if (pRingBuffer->u16TailIndex > pRingBuffer->u16HeadIndex)
    {
        *pNextBufferSize = pRingBuffer->u16Size - pRingBuffer->u16TailIndex;
        *pNextBuffer = (uint32_t)&pRingBuffer->pBuffer[pRingBuffer->u16TailIndex];
    }
    else
    {
        *pNextBufferSize = 0U;
        *pNextBuffer = 0U;
    }
}

/*================================================================================================*/
/**
@brief      Copy data from an input buffer to a ring buffer.

@details    This function will advance the head index based on the amount of data copied.

@param[in]  pRingBuffer     Pointer to ring buffer instance
@param[in]  pSourceBuffer   Pointer to the destination buffer
@param[in]  u16BufSize      Size of the provided buffer

@return     None
*/
/*================================================================================================*/
FUNC(void, COMDAL_APP_CODE) ComDal_Uart_WriteToRingBuffer
(
    P2VAR(ComDal_RingBuffer, AUTOMATIC, COMDAL_APPL_DATA) pRingBuffer,
    P2VAR(uint8, AUTOMATIC, COMDAL_APPL_DATA) pSourceBuffer,
    VAR(uint16, AUTOMATIC) u16BufSize
)
{
    VAR(uint16, AUTOMATIC) u16Idx = 0;

    for (u16Idx = 0; u16Idx < u16BufSize; u16Idx++)
    {
        pRingBuffer->pBuffer[pRingBuffer->u16HeadIndex] = pSourceBuffer[u16Idx];
        ADVANCE_UART_RINGBUFF_HEAD(pRingBuffer, 1);
    }

    pRingBuffer->u16ElementsCount += u16BufSize;
}
#endif /* COMDAL_UART_MAX_TX_CHANNELS > 0*/

/*================================================================================================*/
/**
@brief      Initialization function for ComDal_Uart component.
@details    This function shall be called during System Initialization to setup UART HW and
            initialize all global data

@param[in]  pConfig     Pointer to configuration data (this data is shared between all ComDal
                        components)

@return None
*/
/*================================================================================================*/
FUNC(void, COMDAL_APP_CODE) ComDal_Uart_Init(const ComDal_ConfigType *pConfig)
{
    P2CONST(ComDal_UartDescriptorType, AUTOMATIC, COMDAL_APPL_CONST) pUartChannel = NULL_PTR;
    P2CONST(ComDal_BswChannelMapType, AUTOMATIC, COMDAL_APPL_CONST) pComChannel = NULL_PTR;
    VAR(Std_ReturnType, AUTOMATIC) Status = E_NOT_OK;
    VAR(uint8, AUTOMATIC) u8Idx = 0U;

    ComDal_Uart_pConfig = pConfig;
    ComDal_Uart_pDescriptors = pConfig->pUartChanDescriptor;

    for (u8Idx = 0; u8Idx < COMDAL_MAX_CHANNELS_HW_VAR_0; u8Idx++)
    {
        pComChannel = &pConfig->pComChannelMap[u8Idx];

        if (pComChannel->ComChannelType == COMDAL_UART_CHANNEL)
        {
            pUartChannel = GET_UART_DESCRIPTOR(u8Idx);

            if (pComChannel->AccessType == COMDAL_RXCHANNEL)
            {
                ComDal_Uart_pRxBuffers[pUartChannel->UartChannelId] = pUartChannel->pRingBuffer;
                ComDal_Uart_RxRecvState[pUartChannel->UartChannelId].eRxState = RX_IDLE;
                ComDal_Uart_RxRecvState[pUartChannel->UartChannelId].u8MsgSize = 0u;
                ComDal_Uart_RxRecvState[pUartChannel->UartChannelId].u8RcvSize = 0u;
                ComDal_Uart_RxRecvState[pUartChannel->UartChannelId].u32Timeout = 0u;
                Status = Uart_AsyncReceive(pUartChannel->UartChannelId, 
                                        &ComDal_Uart_u8RxData[pUartChannel->UartChannelId], 
                                        1);
                if (E_OK == Status)
                {
                    ComDal_Uart_ChannelsState[pUartChannel->UartChannelId] = COMDAL_STATE_RX_ONGOING;
                }
            }
            else if (pComChannel->AccessType == COMDAL_TXCHANNEL)
            {
                ComDal_Uart_pTxBuffers[pUartChannel->UartChannelId] = pUartChannel->pRingBuffer;
            }
        }
    }
}

/*================================================================================================*/
/**
@brief      De-Initialization function for ComDal_Uart component.
@details    This function shall be called during System Shut-down to disable UART HW and de-initialize
            all global data

@return     None
*/
/*================================================================================================*/
FUNC(void, COMDAL_APP_CODE) ComDal_Uart_DeInit(void)
{
    VAR(uint8, AUTOMATIC) u8Idx = 0U;

    Uart_Deinit();

    for (u8Idx = 0U; u8Idx < ComDal_Uart_pConfig->UartChannelCount; u8Idx++)
    {
        P2VAR(ComDal_RingBuffer, AUTOMATIC, COMDAL_APPL_DATA) RingBuffer = NULL_PTR;
        RingBuffer = ComDal_Uart_pConfig->pUartChanDescriptor[u8Idx].pRingBuffer;

        RingBuffer->u16ElementsCount = 0;
        RingBuffer->u16HeadIndex = 0;
        RingBuffer->u16TailIndex = 0;
    }

    for (u8Idx = 0U; u8Idx < COMDAL_UART_HW_INSTANCES; u8Idx++)
    {
        ComDal_Uart_ChannelsState[u8Idx] = COMDAL_STATE_IDLE;
    }

    ComDal_Uart_pConfig = NULL_PTR;
}

/*================================================================================================*/
/**
@brief      Main function used to handle state-machine of each UART channel

@param[in]  ComChannel  Logical BSW ComDal channel id.

@return     None
*/
/*================================================================================================*/
FUNC(void, COMDAL_APP_CODE) ComDal_Uart_Main(VAR(ComDal_ChannelType, AUTOMATIC) ComChannel)
{
    P2VAR(ComDal_RingBuffer, AUTOMATIC, COMDAL_APPL_DATA) pRingBuffer = NULL_PTR;
    VAR(ComDal_BswDescriptorIdType, AUTOMATIC) UartDescriptorId = 0;
    VAR(uint8, AUTOMATIC) UartPortId = 0;
    VAR(ComDal_AccessType, AUTOMATIC) AccessType;

    UartDescriptorId = GET_UART_DESCRIPTOR_IDX(ComChannel);
    UartPortId = ComDal_Uart_pDescriptors[UartDescriptorId].UartChannelId;
    pRingBuffer = ComDal_Uart_pDescriptors[UartDescriptorId].pRingBuffer;
    AccessType = ComDal_Uart_pConfig->pComChannelMap[ComChannel].AccessType;

#if (COMDAL_UART_MAX_TX_CHANNELS > 0U)
    if (AccessType == COMDAL_TXCHANNEL)
    {
        /* TX operations have priority over RX operations.
        If there are pending data to be transmitted
        abort RX operation and start data transmission */
        if (pRingBuffer->u16ElementsCount > 0U)
        {
            /* Any RX ongoing operation must be abort */
            if ((ComDal_Uart_ChannelsState[UartPortId] == COMDAL_STATE_RX_ONGOING) &&
                (ComDal_Uart_RxRecvState[UartPortId].eRxState == RX_IDLE))
            {
                Uart_Abort(UartPortId, UART_RECEIVE);
                ComDal_Uart_ChannelsState[UartPortId] = COMDAL_STATE_IDLE;
            }
            /* Trigger first data transmission.
            Any remaining data will be transmitted from the UART TX callback.*/
            if (ComDal_Uart_ChannelsState[UartPortId] == COMDAL_STATE_IDLE)
            {
                ComDal_Uart_TxProcess(UartPortId);
            }
        }
    }
#endif /* COMDAL_UART_MAX_TX_CHANNELS > 0U */

#if (COMDAL_UART_MAX_RX_CHANNELS > 0U)
    if (AccessType == COMDAL_RXCHANNEL)
    {
        if (ComDal_Uart_ChannelsState[UartPortId] == COMDAL_STATE_IDLE)
        {
            /* Trigger the first data reception */
            ComDal_Uart_RxProcess(UartPortId);
        }
        ComDal_Uart_UpdateRxTimeout(UartPortId);
    }
#endif /* COMDAL_UART_MAX_RX_CHANNELS > 0U */
}

/*================================================================================================*/
/**
@brief      API used to provide data that was read from the UART lines.
@details    This function will copy the data from the internal ring buffer used by ComDal Uart
            to the buffer provided by the user.

@param[in]      ComChannel  Logical BSW ComDal channel id.
@param[out]     pReadBuffer Pointer to the destination buffer
@param[in/out]  pBufferSize Used as input parameter to specify the pReadBuffer size
                            Used as output parameter for amount of read data

@return     ComDal_ReturnType: COMDAL_NO_ERROR, COMDAL_OBSOLETE, COMDAL_INVALID,
            COMDAL_OVERFLOW
*/
/*================================================================================================*/
FUNC(ComDal_ReturnType, COMDAL_APP_CODE) ComDal_Uart_ReadData
(
    ComDal_ChannelType ComChannel,
    char *pReadBuffer,
    uint16 *pBufferSize
)
{
#if (COMDAL_UART_MAX_RX_CHANNELS > 0U)
    VAR(ComDal_BswDescriptorIdType, AUTOMATIC) UartDescriptorId = 0;
    P2VAR(ComDal_RingBuffer, AUTOMATIC, COMDAL_APPL_DATA) pRingBuffer = NULL_PTR;
    VAR(ComDal_AccessType, AUTOMATIC) AccessType;

    AccessType = ComDal_Uart_pConfig->pComChannelMap[ComChannel].AccessType;
    if (AccessType != COMDAL_RXCHANNEL)
    {
       #if (COMDAL_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError((uint16)COMDAL_MODULE_ID, (uint8)0U, COMDAL_READDATA_API_ID,
                        COMDAL_E_INVALID_CHANNEL_TYPE);
        #endif /*COMDAL_DEV_ERROR_DETECT == STD_ON*/
        return COMDAL_INVALID;
    }

    UartDescriptorId = GET_UART_DESCRIPTOR_IDX(ComChannel);
    pRingBuffer = ComDal_Uart_pConfig->pUartChanDescriptor[UartDescriptorId].pRingBuffer;

    if (pRingBuffer->u16ElementsCount == 0)
    {
        return COMDAL_OBSOLETE;
    }

    /* The output buffer must be at least 1 bytes*/
    if (*pBufferSize < 1)
    {
        return COMDAL_INVALID;
    }

    ComDal_Uart_ReadFromRingBuffer(pRingBuffer, (uint8*)pReadBuffer, pBufferSize);

    return COMDAL_NO_ERROR;
#else
    (void)ComChannel;
    (void)pReadBuffer;
    (void)pBufferSize;

    return COMDAL_INVALID;
#endif /* COMDAL_UART_MAX_RX_CHANNELS > 0U */
}

/*================================================================================================*/
/**
@brief      Function used to create a UART write request if no UART TX is ongoing
@details    Copy the message from the buffer provided by the user to the internal ring buffer used
            by ComDal Uart.

@param[in]  ComChannel          Logical BSW ComDal channel id.
@param[in]  pWriteBuffer        Data to be written on UART for the assigned input channel.
@param[in]  u8WriteBufferSize   Size of the buffer provided

@return     none
*/
/*================================================================================================*/
FUNC(ComDal_ReturnType, COMDAL_APP_CODE) ComDal_Uart_WriteData
(
    ComDal_ChannelType ComChannel,
    char *pWriteBuffer, uint16 u16WriteBufferSize
)
{
#if (COMDAL_UART_MAX_TX_CHANNELS > 0U)
    P2VAR(ComDal_RingBuffer, AUTOMATIC, COMDAL_APPL_DATA) pRingBuffer = NULL_PTR;
    P2CONST(ComDal_UartDescriptorType, AUTOMATIC, COMDAL_APPL_CONST) pUartChn = NULL_PTR;
    VAR(ComDal_AccessType, AUTOMATIC) AccessType;

    pUartChn = GET_UART_DESCRIPTOR(ComChannel);

    AccessType = ComDal_Uart_pConfig->pComChannelMap[ComChannel].AccessType;
    if (AccessType != COMDAL_TXCHANNEL)
    {
        #if (COMDAL_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError((uint16)COMDAL_MODULE_ID, (uint8)0U, COMDAL_READDATA_API_ID,
                        COMDAL_E_INVALID_CHANNEL_TYPE);
        #endif /*COMDAL_DEV_ERROR_DETECT == STD_ON*/
        return COMDAL_INVALID;
    }

    pRingBuffer = pUartChn->pRingBuffer;

    if (u16WriteBufferSize > GET_RINGBUFFER_FREE_SPACE(pRingBuffer))
    {
        return COMDAL_OVERFLOW;
    }

    ComDal_Uart_WriteToRingBuffer(pRingBuffer, (uint8*)pWriteBuffer, u16WriteBufferSize);

    return COMDAL_NO_ERROR;
#else
    (void)ComChannel;
    (void)pWriteBuffer;
    (void)u16WriteBufferSize;

    return COMDAL_INVALID;
#endif /* COMDAL_UART_MAX_TX_CHANNELS > 0U */
}

/*================================================================================================*/
/**
@brief      Provide status for UART channel

@param[in]  ComChannel  Logical BSW ComDal channel id.

@return     ComDal_State: COMDAL_STATE_IDLE, COMDAL_STATE_RX_ONGOING, COMDAL_STATE_TX_ONGOING, COMDAL_STATE_ERROR.
*/
/*================================================================================================*/
FUNC(ComDal_State, COMDAL_APP_CODE) ComDal_Uart_GetChannelStatus(VAR(ComDal_ChannelType, AUTOMATIC) ComChannel)
{
    VAR(ComDal_State, AUTOMATIC) ChanStatus;
    VAR(ComDal_BswDescriptorIdType, AUTOMATIC) UartDescriptorId = 0;
    VAR(uint8, AUTOMATIC) UartPortId = 0;

    UartDescriptorId = GET_UART_DESCRIPTOR_IDX(ComChannel);
    UartPortId = ComDal_Uart_pDescriptors[UartDescriptorId].UartChannelId;

    ChanStatus = ComDal_Uart_ChannelsState[UartPortId];
    
    return ChanStatus;
}

/*================================================================================================*/
/**
@brief      Called by RTD Uart driver to notify application about the transfer
@details    This function acts like a wrapper that selects the RX and TX callback according to the
            channel type.

@return     None
*/
/*================================================================================================*/
FUNC(void, COMDAL_APP_CODE) ComDal_Uart_TxRxIndication
(
    uint32 hwInstance,
    Uart_EventType UartEvent
)
{
    if (COMDAL_STATE_RX_ONGOING == ComDal_Uart_ChannelsState[hwInstance])
    {
#if(COMDAL_UART_MAX_RX_CHANNELS > 0)
        ComDal_Uart_RxIndication(hwInstance, UartEvent);
#endif
    }

    else if (COMDAL_STATE_TX_ONGOING == ComDal_Uart_ChannelsState[hwInstance])
    {
#if(COMDAL_UART_MAX_TX_CHANNELS > 0)
        ComDal_Uart_TxIndication(hwInstance, UartEvent);
#endif
    }
}

/*================================================================================================*/
/**
@brief      This function will append a byte to the ring buffer and then will advance the head index

@details    In case of overflow there are two options:
                1 - discard new received data
                2 - discard old received data
            In the current function is applied option 2.
            The overflow event must be flagged and provided through a public interface.

@param[in]  pRingBuffer         Pointert to the ring buffer structure
@param[in]  u8Data              Data byte to be added

@return     None
*/
/*================================================================================================*/
static inline FUNC(void, COMDAL_APP_CODE) ComDal_Uart_RingBufferAppend
(
    P2VAR(ComDal_RingBuffer, AUTOMATIC, COMDAL_APPL_DATA) pRingBuffer,
    uint8 u8Data
)
{
    /* In case of overflow old received data will be lost */
    pRingBuffer->pBuffer[pRingBuffer->u16HeadIndex] = u8Data;

    if (GET_RINGBUFFER_FREE_SPACE(pRingBuffer) > 0)
    {
        ADVANCE_UART_RINGBUFF_HEAD (pRingBuffer, 1);
        pRingBuffer->u16ElementsCount++;
    }
    else
    {
        /*
            This is an OVERFLOW in the ring buffer.
            In case of overflow both head and tail indices are advance.
        */
        ADVANCE_UART_RINGBUFF_HEAD (pRingBuffer, 1);
        pRingBuffer->u16TailIndex = pRingBuffer->u16HeadIndex;
        /* ToDo: add OVERFLOW state for this ring buffer */
    }
}

/*================================================================================================*/
/**
@brief     This function will return the descriptor index for a given UART channel id   

@param[in] UartChannelId - UART channel id

@return    ComDal_BswDescriptorIdType - descriptor index
 */
/*================================================================================================*/
static inline FUNC(ComDal_BswDescriptorIdType, COMDAL_APP_CODE) ComDal_Uart_GetDescriptorIdxByUartChannelId
(
    VAR(uint8, AUTOMATIC) UartChannelId,
    VAR(ComDal_AccessType, AUTOMATIC) AccessType
)
{
    VAR(ComDal_BswDescriptorIdType, AUTOMATIC) ComDescriptorId = 0;

    for (ComDescriptorId = 0; ComDescriptorId < COMDAL_MAX_CHANNELS_HW_VAR_0; ComDescriptorId++)
    {
        if (ComDal_Uart_pConfig->pComChannelMap[ComDescriptorId].ComChannelType == COMDAL_UART_CHANNEL &&
            ComDal_Uart_pConfig->pComChannelMap[ComDescriptorId].AccessType == AccessType)
        {
            VAR(ComDal_BswDescriptorIdType, AUTOMATIC) UartDescriptorId = 
                ComDal_Uart_pConfig->pComChannelMap[ComDescriptorId].ComDescriptorIndex;
            if (ComDal_Uart_pDescriptors[UartDescriptorId].UartChannelId == UartChannelId)
            {
                break;
            }
        }
    }

    return ComDescriptorId;
}

#endif /* COMDAL_MAX_UART_DESCRIPTORS > 0U */

#ifdef __cplusplus
}
#endif
/** @} */
