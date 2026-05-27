/*
 * Copyright 2025 NXP.
 */
#include "CanIf_Cbk.h"

/* RX Specific variables used by stub functions */
volatile uint8 Can_RxData[CAN_FD_FRAME_LEN];
volatile uint8  Can_RxHandle;
volatile Can_IdType Can_RxId;
volatile uint8  Can_RxDlc;
volatile uint8  Can_ControllerId;

/* TX  Ok Transmission Specific variables */
volatile PduIdType Can_TxConfirmation_CanTxPduId;
volatile uint32   Can_Tx_No[16];

/* RX and TX specific counters */
#if CAN_43_LLCE_MAX_PARTITIONS == 1U
    volatile uint32 Can_RxIndication;
    volatile uint32 Can_TxConfirmation;
    volatile uint32 last_RxIndication;
    volatile uint32 last_TxConfirmation;
#else
    volatile uint32 Can_RxIndication[MAX_NUM_OF_CORES];
    volatile uint32 Can_TxConfirmation[MAX_NUM_OF_CORES];
    volatile uint32 last_RxIndication[MAX_NUM_OF_CORES];
    volatile uint32 last_TxConfirmation[MAX_NUM_OF_CORES];
#endif

/* Bus Off Specific variables */
volatile uint8  Can_BusOffConfirmation;
volatile uint32 u32CustomCallbackExecutions = 0;

/*==================================================================================================
*                                      CAN Call Back functions implementation
==================================================================================================*/

void Can_CallBackSetUp(void)
{

#if CAN_43_LLCE_MAX_PARTITIONS != 1U
    uint8 u8Idx = 0U;
#endif
    
    /* Reset global flags. */
    Can_BusOffConfirmation = 0U;    
    Can_RxDlc = 0U;

#if CAN_43_LLCE_MAX_PARTITIONS == 1U
    Can_TxConfirmation = 0U;
    Can_RxIndication = 0U;
#else
    for (u8Idx = 0U; u8Idx < MAX_NUM_OF_CORES; u8Idx++)
    {
        Can_TxConfirmation[u8Idx] = 0U;
        Can_RxIndication[u8Idx] = 0U; 
    }
#endif

}

void CanIf_RxIndication( const Can_HwType* Mailbox, const PduInfoType* PduInfoPtr )
{   
    /* Local variable */
    uint8 i = 0U;
#if CAN_43_LLCE_MAX_PARTITIONS != 1U
    uint32 u32CoreId = OsIf_GetCoreID();
#endif

    for (i = 0; i < PduInfoPtr->SduLength; i++)
    {
        /* Pointer to received L-SDU (payload) */
        Can_RxData[i] = PduInfoPtr->SduDataPtr[i];
    }

     for (i = PduInfoPtr->SduLength; i < CAN_FD_FRAME_LEN; i++)
    {
        /* Clean the rest of the payload if it's not a CAN FD frame*/
        Can_RxData[i] = 0;
    }

    /* Get the id of the corresponding hardware object: Range is 0 .. (total number of HRH-1) */
    Can_RxHandle = Mailbox->Hoh;
    /* standard/Extended Can ID of L-PDU  that has been successfully received */
    Can_RxId = Mailbox->CanId;
    /* Data length code (length of L0PDU payload) */
    Can_RxDlc = PduInfoPtr->SduLength;
    /*Get the id of Controller*/
    Can_ControllerId = Mailbox->ControllerId;

    /* Increment counter to record reception of a message */
#if CAN_43_LLCE_MAX_PARTITIONS == 1U
    Can_RxIndication++;
#else
    Can_RxIndication[u32CoreId]++;
#endif
}

void CanIf_ControllerBusOff( uint8 Controller)
{   
    /* Increment counter to record bus off events */
    Can_BusOffConfirmation++;
    (void)Controller;
}

void CanIf_ControllerModeIndication( uint8 Controller, Can_ControllerStateType ControllerMode )
{
    (void)ControllerMode;
    (void)Controller;
}

void CanIf_TxConfirmation( PduIdType CanTxPduId)
{
#if CAN_43_LLCE_MAX_PARTITIONS != 1U
    uint32 u32CoreId = OsIf_GetCoreID();
#endif
    /* Increment counter to record transmission of a message */
#if CAN_43_LLCE_MAX_PARTITIONS == 1U
    Can_TxConfirmation++;
#else
    u32CoreId = OsIf_GetCoreID();
    Can_TxConfirmation[u32CoreId]++;
#endif
    /* Store the PDU ID for debug purposes */
    Can_TxConfirmation_CanTxPduId = CanTxPduId;
    Can_Tx_No[CanTxPduId]++;
}

void RxTimestampNotification(Can_HwHandleType Hoh, uint32 u32TimestampVal)
{
	(void)u32TimestampVal; /* timestamp for current received frame identified through Hoh */
	(void)Hoh;
}

void TxTimestampNotification(Can_HwHandleType Hoh, PduIdType CanTxPduId, uint32 u32TimestampVal)
{
	(void)u32TimestampVal; /* timestamp for current frame confirmation identified through Hoh and CanTxPduId (frameTag1 and frameTag2) */
	(void)Hoh;
	(void)CanTxPduId;
}

/* Callback for errors. This is triggered for errors configured in INTERRUPT or by calling MainFunction_Error */
void CanErrorNotification(uint8 u8CanIfCtrlId, Llce_Can_NotificationIdType eNotifId, volatile Llce_Can_ErrorNotifType* ErrorInfo)
{
	(void)u8CanIfCtrlId; 
	(void)eNotifId;
	(void)ErrorInfo;
}

void CanWriteCustomCallback(Can_HwHandleType Hoh, PduIdType CanTxPduId, uint32 u32TxMb)
{
	(void)Hoh; 
	(void)CanTxPduId;
	(void)u32TxMb;
}

void CanTxConfirmationCustomCallback(Can_HwHandleType Hoh, PduIdType CanTxPduId)
{
	(void)Hoh; 
	(void)CanTxPduId;
}


boolean Can_43_LLCE_RxCustomCallback(uint8 Hrh, Can_IdType CanId, uint8 CanDataLength, const uint8* CanSduPtr, uint32 u32RxMb)
{
    /* Suppress warnings for unused arguments */
    (void)Hrh; (void)CanId; (void)CanDataLength; (void)CanSduPtr; (void)u32RxMb;

    u32CustomCallbackExecutions++;

    /* Returns FALSE for even IDs */
    return (CanId & 0x01);
}

/* Circular permutation on data buffer. The reason of using this function is for checking data sent is the same with data received every time.  */
void Circular_Permutation(volatile uint8 *Payload, uint8 n)
{
    uint8 temp = Payload[0];

    for (uint8 Idx = 0; Idx < n - 1; Idx++)
    {
        Payload[Idx] = Payload[Idx + 1];
    }

    Payload[n - 1] = temp;
}

StatusType Check_Status(Can_PduType *CanMessage)
{
    uint32 can_counter = 0;
    uint8 u8Idx = 0;
    StatusType transmission_status = E_OK;

#if CAN_43_LLCE_MAX_PARTITIONS != 1U    
    uint32 u32CoreId = OsIf_GetCoreID();
#endif

    /* Wait for transmission and reception */
#if CAN_43_LLCE_MAX_PARTITIONS == 1U    
    while ( (( (Can_TxConfirmation - last_TxConfirmation) < 1) || ( (Can_RxIndication - last_RxIndication) < 1))  && (can_counter < TP_TIMEOUT) )
#else
    while ( (( (Can_TxConfirmation[u32CoreId] - last_TxConfirmation[u32CoreId]) < 1) || ( (Can_RxIndication[u32CoreId] - last_RxIndication[u32CoreId]) < 1))  && (can_counter < TP_TIMEOUT) )
#endif
    {
        can_counter++;
    }

    /* Check received Data length */
    if (Can_RxDlc != CanMessage->length)
    {
        transmission_status = E_NOT_OK;
    }

    /* Check integrity of received Data content */
    for (u8Idx = 0; u8Idx < CanMessage->length; u8Idx++)
    {
        if (Can_RxData[u8Idx] != CanMessage->sdu[u8Idx])
        {
            transmission_status = E_NOT_OK;
            break;
        }
    }

#if CAN_43_LLCE_MAX_PARTITIONS == 1U    
    last_RxIndication++;
    last_TxConfirmation++;
#else
    last_RxIndication[u32CoreId]++;
    last_TxConfirmation[u32CoreId]++;
#endif

    /* Shuffle the data in order to check sending/receiving of another frame */
    Circular_Permutation(CanMessage->sdu, CAN_FD_FRAME_LEN);

    return transmission_status;
}