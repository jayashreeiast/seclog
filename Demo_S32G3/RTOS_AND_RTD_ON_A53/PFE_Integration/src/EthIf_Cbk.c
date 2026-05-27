/*
 *  Copyright 2017, 2019-2025 NXP
 *
 *  NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only
 *  be used strictly in accordance with the applicable license terms. By
 *  expressly accepting such terms or by downloading, installing, activating
 *  and/or otherwise using the software, you are agreeing that you have read,
 *  and that you agree to comply with and are bound by, such license terms. If
 *  you do not agree to be bound by the applicable license terms, then you may
 *  not retain, install, activate or otherwise use the software.
 *
 *  This file contains sample code only. It is not part of the production code deliverables.
 */

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
*                                        INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#include "ComStackTypes.h"
#include "EthIf_Cbk.h"
#ifdef LWIP
#include "lwip_demo.h"
#endif
/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/

#define ETHIF_VENDOR_ID_C                      43
#define ETHIF_AR_RELEASE_MAJOR_VERSION_C       4
#define ETHIF_AR_RELEASE_MINOR_VERSION_C       4
#define ETHIF_AR_RELEASE_REVISION_VERSION_C    0
#define ETHIF_SW_MAJOR_VERSION_C               0
#define ETHIF_SW_MINOR_VERSION_C               8
#define ETHIF_SW_PATCH_VERSION_C               0

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/
/* Check if current file and ETHIF header file are of the same vendor */
#if (ETHIF_VENDOR_ID_C != ETHIF_VENDOR_ID)
    #error "EthIf_Cbk.c and EthIf_Cbk.h have different vendor ids"
#endif
/* Check if current file and ETHIF header file are of the same Autosar version */
#if ((ETHIF_AR_RELEASE_MAJOR_VERSION_C    != ETHIF_AR_RELEASE_MAJOR_VERSION) || \
     (ETHIF_AR_RELEASE_MINOR_VERSION_C    != ETHIF_AR_RELEASE_MINOR_VERSION) || \
     (ETHIF_AR_RELEASE_REVISION_VERSION_C != ETHIF_AR_RELEASE_REVISION_VERSION))
    #error "AutoSar Version Numbers of EthIf_Cbk.c and EthIf_Cbk.h are different"
#endif
/* Check if current file and ETHIF header file are of the same Software version */
#if ((ETHIF_SW_MAJOR_VERSION_C != ETHIF_SW_MAJOR_VERSION) || \
     (ETHIF_SW_MINOR_VERSION_C != ETHIF_SW_MINOR_VERSION) || \
     (ETHIF_SW_PATCH_VERSION_C != ETHIF_SW_PATCH_VERSION))
    #error "Software Version Numbers of EthIf_Cbk.c and EthIf_Cbk.h are different"
#endif

/*==================================================================================================
*                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/


/*==================================================================================================
*                                       LOCAL MACROS
==================================================================================================*/


/*==================================================================================================
*                                      LOCAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
*                                      LOCAL VARIABLES
==================================================================================================*/


/*==================================================================================================
*                                      GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
*                                      GLOBAL VARIABLES
==================================================================================================*/

#define ETH_43_PFE_START_SEC_VAR_INIT_32
#include "Eth_43_PFE_MemMap.h"

#define ETH_43_PFE_STOP_SEC_VAR_INIT_32
#include "Eth_43_PFE_MemMap.h"

#define ETH_43_PFE_START_SEC_VAR_INIT_8_NO_CACHEABLE
#include "Eth_43_PFE_MemMap.h"

volatile uint8                  IngressNotifications = 0U;
volatile uint8                  EgressNotifications = 0U;
volatile uint8                  MgmtInfoNotifications = 0U;

#define ETH_43_PFE_STOP_SEC_VAR_INIT_8_NO_CACHEABLE
#include "Eth_43_PFE_MemMap.h"

#define ETH_43_PFE_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Eth_43_PFE_MemMap.h"

EthSwt_MgmtObjectType           txGlobalMgmtObject;

#define ETH_43_PFE_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Eth_43_PFE_MemMap.h"

#define ETH_43_PFE_START_SEC_VAR_INIT_32
#include "Eth_43_PFE_MemMap.h"

EthSwt_MgmtObjectType           *txMgmtObject = &txGlobalMgmtObject;

#define ETH_43_PFE_STOP_SEC_VAR_INIT_32
#include "Eth_43_PFE_MemMap.h"

/*==================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/


/*==================================================================================================
*                                       LOCAL FUNCTIONS
==================================================================================================*/


/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/

#define ETH_43_PFE_START_SEC_CODE
#include "Eth_43_PFE_MemMap.h"

#ifdef PING_TEST
extern void rx_callback( uint8 ctrl_idx, Eth_FrameType frame_type, boolean is_broadcast,
                         uint8 *phys_addr_ptr, Eth_DataType *p_rx_data, uint16 len_byte
                       );
#endif

#ifdef LOOPBACK_TEST
void Tst_Pfe_LoopbackTestCheckRxFrame( uint8 CtrlIdx, \
                                                        Eth_FrameType FrameType, \
                                                        boolean IsBroadcast, \
                                                        uint8 * PhysAddrPtr, \
                                                        Eth_DataType *  DataPtr, \
                                                        uint16 LenByte
                                                      );
#endif /* LOOPBACK_TEST */

#ifdef LWIP
extern void Lwip_EthIf_RxIndication( uint8 CtrlIdx, \
                                        Eth_FrameType FrameType, \
                                        boolean IsBroadcast, \
                                        uint8 * PhysAddrPtr, \
                                        Eth_DataType *  DataPtr, \
                                        uint16 LenByte
                                        );
#endif

/*================================================================================================*/

/**
* @brief          This function handles the received Ethernet frame.
* @details        Function should parse the received frame and pass the gathered
*                 information to the appropriate upper layer module.
* @note           The passed data buffer is no longer valid after the function
*                 is exited.
* @warning        This is only an empty stub function provided only to be able
*                 to compile and link the Eth module.
* @param[in]      CtrlIdx Index of the controller which received the frame.
* @param[in]      FrameType The received frame Ethertype (from the frame header)
* @param[in]      IsBroadcast The value TRUE indicates that the received frame
*                 was sent to broadcast address (ff-ff-ff-ff-ff-ff)
* @param[in]      PhysAddrPtr Pointer to received frame source MAC address
*                 (6 bytes).
* @param[in]      DataPtr Data buffer containing the received Ethernet frame
*                 payload.
* @param[in]      LenByte Length of the data in the buffer DataPtr.
*
*/
void EthIf_RxIndication(uint8 CtrlIdx, \
                             Eth_FrameType FrameType, \
                             boolean IsBroadcast, \
                             uint8 * PhysAddrPtr, \
                             Eth_DataType *  DataPtr, \
                             uint16 LenByte)
{
#if defined(ETHSWT_43_SJA1110_MANAGEMENT_SUPPORT_API ) &&  (ETHSWT_43_SJA1110_MANAGEMENT_SUPPORT_API == STD_ON)
    Std_ReturnType Result = (Std_ReturnType)E_NOT_OK;
    uint8                   cnt = 0x0U;
    EthSwt_MgmtObjectType   rxGlobalMgmtObject;
    EthSwt_MgmtObjectType   *rxMgmtObject = &rxGlobalMgmtObject;

    do
    {
        Result = EthIf_GetRxMgmtObject(CtrlIdx, (DataPtr - 2U), &rxMgmtObject);
        cnt++;
    } while ((E_OK != Result) && (cnt != 10U));
#endif

#ifdef PING_TEST
    rx_callback(CtrlIdx, FrameType, IsBroadcast, PhysAddrPtr, DataPtr, LenByte);
#elif defined(LWIP)
    Lwip_EthIf_RxIndication(CtrlIdx, FrameType, IsBroadcast, PhysAddrPtr, DataPtr, LenByte);
#endif

#ifdef LOOPBACK_TEST
    /* Check if the received frame matches dummy frame sent */
    Tst_Pfe_LoopbackTestCheckRxFrame(CtrlIdx, FrameType, IsBroadcast, PhysAddrPtr, DataPtr, LenByte);
#else
    (void)FrameType;
    (void)IsBroadcast;
    (void)LenByte;
    (void)PhysAddrPtr;
    (void)DataPtr;
#endif
}


/*================================================================================================*/
/**
* @brief          This function confirms that transmission of an Ethernet frame
*                 was finished.
* @details        Function should notify the appropriate upper layer module that
*                 the data transmission was successfully finished.
* @warning        This is only an empty stub function provided only to be able
*                 to compile and link the Eth module.
* @param[in]      CtrlIdx Index of the controller which transmitted the frame.
* @param[in]      BufIdx Index of the transmitted data buffer.

*/
void EthIf_TxConfirmation(uint8 CtrlIdx, \
                                               Eth_BufIdxType BufIdx, \
                                              Std_ReturnType Result)
{
    (void)BufIdx;
    (void)Result;
}

/*================================================================================================*/
/**
* @brief          This function indicate that driver mode has been changed
* @details        Called asynchronously when mode has been read out. Triggered by previous
*                 Eth_SetControllerMode call. Can directly be called within the trigger functions.
* @warning        This is only an empty stub function provided only to be able
*                 to compile and link the Eth module.
* @param[in]      CtrlIdx Index of the controller which mode has been changed.
* @param[in]      CtrlMode New mode of correspond Eth driver.

*/
void EthIf_CtrlModeIndication( \
                                     uint8 CtrlIdx, \
                                     Eth_ModeType CtrlMode \
                                              )
{
    ; /* This is an empty stub function */
    (void)CtrlIdx;
    (void)CtrlMode;
}

/**
* @brief          Eth If Transceiver mode indication
* @details        Called asynchronously when mode has been read out. Triggered by previous Eth_SetTransceiverMode call. Can directly be called within the trigger functions.
* @warning        This is only an empty stub function provided only to be able
*                 to compile and link the Eth module.
* @param[in]      TrcvIdx Index of the Ethernet transceiver within the context of the Ethernet Interface
* @param[in]      TrcvMode Notified Ethernet transceiver mode
*/
void EthIf_TrcvModeIndication( uint8 TrcvIdx,
                                                  EthTrcv_ModeType TrcvMode
                                                )
{
    ; /* This is an empty stub function */
    (void)TrcvIdx;
    (void)TrcvMode;
}

/**
* @brief          Eth If Switch Port mode indication
* @details        The EthIf shall determine the expected notifications based on the EthSwtPort configuration. In case the EthSwtPort references an EthTrcv the EthIf expects a notification from the EthTrcv via API EthIf_TrcvModeIndication(). Otherwise the EthIf expects a notification from the EthSwt via API EthIf_SwitchPortModeIndication()
* @warning        This is only an empty stub function provided only to be able
*                 to compile and link the Eth module.
* @param[in]      SwitchIdx Index of the switch within the context of the Ethernet Switch Driver
* @param[in]      SwitchPortIdx Index of the port at the addressed switch.
* @param[in]      PortMode Notified Ethernet Switch port mode.
*/
void EthIf_SwitchPortModeIndication( uint8 SwitchIdx,
                                                               uint8 SwitchPortIdx,
                                                               EthTrcv_ModeType PortMode
                                                             )
{
    (void)SwitchIdx;
    (void)SwitchPortIdx;
    (void)PortMode;
}

/**
* @brief          Eth If Get Tx Mgmt object
* @details        Request the MgmtObject of the (in this context) unique BufIdx.
* @warning        This is only an empty stub function provided only to be able
*                 to compile and link the Eth module.
* @param[in]      CtrlIdx           Index of an Ethernet Interface controller
* @param[in]      BufIdx            Ethernet Rx Buffer index
* @param[out]     MgmtObjectPtr     Pointer to the management object
*/
Std_ReturnType EthIf_GetTxMgmtObject( uint8 CtrlIdx,
                                                            Eth_BufIdxType BufIdx,
                                                            EthSwt_MgmtObjectType **MgmtObjectPtr
                                                          )
{
    Std_ReturnType Result = (Std_ReturnType)E_NOT_OK;

#if defined(ETHSWT_43_SJA1110_MANAGEMENT_SUPPORT_API ) &&  (ETHSWT_43_SJA1110_MANAGEMENT_SUPPORT_API == STD_ON)
    Result = EthSwt_43_SJA1110_GetTxMgmtObject(CtrlIdx, BufIdx, MgmtObjectPtr);

    if (Result != E_NOT_OK)
    {
        if (ETHSWT_MGMT_OBJ_OWNED_BY_UPPER_LAYER == (**MgmtObjectPtr).Ownership)
        {
            if (E_OK == (**MgmtObjectPtr).Validation.MgmtInfoValid)
            {
                TxMgmtInfoIndicationBuffer[TxMgmtInfoIndicationIndex].CtrlIdx = CtrlIdx;
                TxMgmtInfoIndicationBuffer[TxMgmtInfoIndicationIndex].PortIdx = (**MgmtObjectPtr).MgmtInfo.SwitchPortIdx;
                TxMgmtInfoIndicationBuffer[TxMgmtInfoIndicationIndex].SwitchIdx = (**MgmtObjectPtr).MgmtInfo.SwitchIdx;

                TxMgmtInfoIndicationIndex = (TxMgmtInfoIndicationIndex + 1U) % ETHSWT_TST_BUFFER_SIZE;
                MgmtInfoNotifications++;
            }
            if (E_OK == (**MgmtObjectPtr).Validation.IngressTimestampValid)
            {
                TxIngressTimeStampBuffer[TxIngressTimeStampIndex].CtrlIdx = CtrlIdx;
                TxIngressTimeStampBuffer[TxIngressTimeStampIndex].PortIdx = (**MgmtObjectPtr).MgmtInfo.SwitchPortIdx;
                TxIngressTimeStampBuffer[TxIngressTimeStampIndex].SwitchIdx = (**MgmtObjectPtr).MgmtInfo.SwitchIdx;
                TxIngressTimeStampBuffer[TxIngressTimeStampIndex].TimeStamp.secondsHi = (**MgmtObjectPtr).IngressTimestamp.secondsHi;
                TxIngressTimeStampBuffer[TxIngressTimeStampIndex].TimeStamp.seconds = (**MgmtObjectPtr).IngressTimestamp.seconds;
                TxIngressTimeStampBuffer[TxIngressTimeStampIndex].TimeStamp.nanoseconds = (**MgmtObjectPtr).IngressTimestamp.nanoseconds;

                TxIngressTimeStampIndex = (TxIngressTimeStampIndex + 1U) % ETHSWT_TST_BUFFER_SIZE;
                IngressNotifications++;
            }
            if (E_OK == (**MgmtObjectPtr).Validation.EgressTimestampValid)
            {
                TxEgressTimeStampBuffer[TxEgressTimeStampIndex].CtrlIdx = CtrlIdx;
                TxEgressTimeStampBuffer[TxEgressTimeStampIndex].PortIdx = (**MgmtObjectPtr).MgmtInfo.SwitchPortIdx;
                TxEgressTimeStampBuffer[TxEgressTimeStampIndex].SwitchIdx = (**MgmtObjectPtr).MgmtInfo.SwitchIdx;
                TxEgressTimeStampBuffer[TxEgressTimeStampIndex].TimeStamp.secondsHi = (**MgmtObjectPtr).EgressTimestamp.secondsHi;
                TxEgressTimeStampBuffer[TxEgressTimeStampIndex].TimeStamp.seconds = (**MgmtObjectPtr).EgressTimestamp.seconds;
                TxEgressTimeStampBuffer[TxEgressTimeStampIndex].TimeStamp.nanoseconds = (**MgmtObjectPtr).EgressTimestamp.nanoseconds;

                TxEgressTimeStampIndex = (TxEgressTimeStampIndex + 1U) % ETHSWT_TST_BUFFER_SIZE;
                EgressNotifications++;
            }
        }
        (**MgmtObjectPtr).Ownership = ETHSWT_MGMT_OBJ_UNUSED;
    }
#else
    (void)CtrlIdx;
    (void)BufIdx;
    (void)MgmtObjectPtr;
#endif

    return Result;
}

/**
* @brief          Eth If Get Rx Mgmt object
* @details        Request the MgmtObject of the (in this context) unique DataPtr.
* @warning        This is only an empty stub function provided only to be able
*                 to compile and link the Eth module.
* @param[in]      CtrlIdx           Index of an Ethernet Interface controller
* @param[in]      BufIdx            EEthernet data pointer
* @param[out]     MgmtObjectPtr     Pointer to the management object
*/
Std_ReturnType EthIf_GetRxMgmtObject( uint8 CtrlIdx,
                                                        Eth_DataType *  DataPtr,
                                                        EthSwt_MgmtObjectType **MgmtObjectPtr
                                                      )
{
    Std_ReturnType Result = (Std_ReturnType)E_NOT_OK;

#if defined(ETHSWT_43_SJA1110_MANAGEMENT_SUPPORT_API ) &&  (ETHSWT_43_SJA1110_MANAGEMENT_SUPPORT_API == STD_ON)
    Result = EthSwt_43_SJA1110_GetRxMgmtObject(CtrlIdx, DataPtr, MgmtObjectPtr);

    if (Result != E_NOT_OK)
    {
        if (ETHSWT_MGMT_OBJ_OWNED_BY_UPPER_LAYER == (**MgmtObjectPtr).Ownership)
        {
            if (E_OK == (**MgmtObjectPtr).Validation.MgmtInfoValid)
            {
                RxMgmtInfoIndicationBuffer[RxMgmtInfoIndicationIndex].CtrlIdx = CtrlIdx;
                RxMgmtInfoIndicationBuffer[RxMgmtInfoIndicationIndex].PortIdx = (**MgmtObjectPtr).MgmtInfo.SwitchPortIdx;
                RxMgmtInfoIndicationBuffer[RxMgmtInfoIndicationIndex].SwitchIdx = (**MgmtObjectPtr).MgmtInfo.SwitchIdx;

                RxMgmtInfoIndicationIndex = (RxMgmtInfoIndicationIndex + 1U) % ETHSWT_TST_BUFFER_SIZE;
                MgmtInfoNotifications++;
            }
            if (E_OK == (**MgmtObjectPtr).Validation.IngressTimestampValid)
            {
                RxIngressTimeStampBuffer[RxIngressTimeStampIndex].CtrlIdx = CtrlIdx;
                RxIngressTimeStampBuffer[RxIngressTimeStampIndex].PortIdx = (**MgmtObjectPtr).MgmtInfo.SwitchPortIdx;
                RxIngressTimeStampBuffer[RxIngressTimeStampIndex].SwitchIdx = (**MgmtObjectPtr).MgmtInfo.SwitchIdx;
                RxIngressTimeStampBuffer[RxIngressTimeStampIndex].TimeStamp.secondsHi = (**MgmtObjectPtr).IngressTimestamp.secondsHi;
                RxIngressTimeStampBuffer[RxIngressTimeStampIndex].TimeStamp.seconds = (**MgmtObjectPtr).IngressTimestamp.seconds;
                RxIngressTimeStampBuffer[RxIngressTimeStampIndex].TimeStamp.nanoseconds = (**MgmtObjectPtr).IngressTimestamp.nanoseconds;

                RxIngressTimeStampIndex = (RxIngressTimeStampIndex + 1U) % ETHSWT_TST_BUFFER_SIZE;
                IngressNotifications++;
            }
            if (E_OK == (**MgmtObjectPtr).Validation.EgressTimestampValid)
            {
                RxEgressTimeStampBuffer[RxEgressTimeStampIndex].CtrlIdx = CtrlIdx;
                RxEgressTimeStampBuffer[RxEgressTimeStampIndex].PortIdx = (**MgmtObjectPtr).MgmtInfo.SwitchPortIdx;
                RxEgressTimeStampBuffer[RxEgressTimeStampIndex].SwitchIdx = (**MgmtObjectPtr).MgmtInfo.SwitchIdx;
                RxEgressTimeStampBuffer[RxEgressTimeStampIndex].TimeStamp.secondsHi = (**MgmtObjectPtr).EgressTimestamp.secondsHi;
                RxEgressTimeStampBuffer[RxEgressTimeStampIndex].TimeStamp.seconds = (**MgmtObjectPtr).EgressTimestamp.seconds;
                RxEgressTimeStampBuffer[RxEgressTimeStampIndex].TimeStamp.nanoseconds = (**MgmtObjectPtr).EgressTimestamp.nanoseconds;

                RxEgressTimeStampIndex = (RxEgressTimeStampIndex + 1U) % ETHSWT_TST_BUFFER_SIZE;
                EgressNotifications++;
            }
        }
        (**MgmtObjectPtr).Ownership = ETHSWT_MGMT_OBJ_UNUSED;
    }
#else
    (void)CtrlIdx;
    (void)DataPtr;
    (void)MgmtObjectPtr;
#endif

    return Result;
}

#define ETH_43_PFE_STOP_SEC_CODE
#include "Eth_43_PFE_MemMap.h"

#ifdef __cplusplus
}
#endif
/** @} */
