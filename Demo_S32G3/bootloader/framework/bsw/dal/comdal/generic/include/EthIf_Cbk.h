/**
*   @file           EthIf_Cbk.h
*   @version        24.12.0
*
*   @brief          ComDal header file for the Ethernet driver interface.
*   @details        ComDal interface for handling the Ethernet driver.
*
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*   All Rights Reserved.
*   This file contains sample code only. It is not part of the production code deliverables.
*
*   @addtogroup COMDAL
*   @{
*/
/*=====================================================================================================================
*   Platform             : CORTEXM
*
*   Autosar Version      : 4.4.0
*   Autosar Revision     : ASR_REL_4_4_REV_0000
*
*   SW Version           : 24.12.0
*   Build Version        : S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
======================================================================================================================*/

#ifndef ETHIF_CBK_H
#define ETHIF_CBK_H

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

/*=====================================================================================================================
                               INCLUDE FILES
======================================================================================================================*/
#include "Std_Types.h"

#include "ComDal_Cfg.h"
#include "ComDal_Types.h"
#ifdef IFWK_USE_ETH
#include "Eth.h"
#endif
/*====================================================================================================================
*                              SOURCE FILE VERSION INFORMATION
======================================================================================================================*/
/**
* @file          EthIf_Cbk.h
*/
#define COMDAL_ETH_DRVIF_VENDOR_ID                           43
#define COMDAL_ETH_DRVIF_MODULE_ID                           100

#define COMDAL_ETH_DRVIF_AR_RELEASE_MAJOR_VERSION            4
#define COMDAL_ETH_DRVIF_AR_RELEASE_MINOR_VERSION            4
#define COMDAL_ETH_DRVIF_AR_RELEASE_REVISION_VERSION         0

#define COMDAL_ETH_DRVIF_SW_MAJOR_VERSION                    24
#define COMDAL_ETH_DRVIF_SW_MINOR_VERSION                    12
#define COMDAL_ETH_DRVIF_SW_PATCH_VERSION                    0

#define ETHIF_AR_RELEASE_MAJOR_VERSION                       4 
#define ETHIF_AR_RELEASE_MINOR_VERSION                       4
/*===================================================================================================================
*                                       FILE VERSION CHECKS
====================================================================================================================*/
/**
* @file           EthIf_Cbk.h
*/
#if (COMDAL_ETH_DRVIF_VENDOR_ID != COMDAL_CFG_VENDOR_ID)
    #error "EthIf_Cbk.h and ComDal_Cfg.h have different vendor ids"
#endif

#if (COMDAL_ETH_DRVIF_VENDOR_ID != COMDAL_TYPES_VENDOR_ID)
    #error "EthIf_Cbk.h and ComDal_Types.h have different vendor ids"
#endif

#if ((COMDAL_ETH_DRVIF_AR_RELEASE_MAJOR_VERSION != COMDAL_CFG_AR_RELEASE_MAJOR_VERSION) || \
     (COMDAL_ETH_DRVIF_AR_RELEASE_MINOR_VERSION != COMDAL_CFG_AR_RELEASE_MINOR_VERSION) || \
     (COMDAL_ETH_DRVIF_AR_RELEASE_REVISION_VERSION != COMDAL_CFG_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of EthIf_Cbk.h and ComDal_Cfg.h are different"
#endif

#if ((COMDAL_ETH_DRVIF_AR_RELEASE_MAJOR_VERSION != COMDAL_TYPES_AR_RELEASE_MAJOR_VERSION) || \
     (COMDAL_ETH_DRVIF_AR_RELEASE_MINOR_VERSION != COMDAL_TYPES_AR_RELEASE_MINOR_VERSION) || \
     (COMDAL_ETH_DRVIF_AR_RELEASE_REVISION_VERSION != COMDAL_TYPES_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of EthIf_Cbk.h and ComDal_Types.h are different"
#endif

#if ((COMDAL_ETH_DRVIF_SW_MAJOR_VERSION != COMDAL_CFG_SW_MAJOR_VERSION) || \
     (COMDAL_ETH_DRVIF_SW_MINOR_VERSION != COMDAL_CFG_SW_MINOR_VERSION) || \
     (COMDAL_ETH_DRVIF_SW_PATCH_VERSION != COMDAL_CFG_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of EthIf_Cbk.h and ComDal_Cfg.h are different"
#endif

#if ((COMDAL_ETH_DRVIF_SW_MAJOR_VERSION != COMDAL_TYPES_SW_MAJOR_VERSION) || \
     (COMDAL_ETH_DRVIF_SW_MINOR_VERSION != COMDAL_TYPES_SW_MINOR_VERSION) || \
     (COMDAL_ETH_DRVIF_SW_PATCH_VERSION != COMDAL_TYPES_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of EthIf_Cbk.h and ComDal_Types.h are different"
#endif

/*====================================================================================================================
*                              GLOBAL FUNCTION DECLARATIONS
======================================================================================================================*/
#if COMDAL_MAX_ETH_INTERFACES > 0U
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
void EthIf_RxIndication
(
    uint8 CtrlIdx,
    Eth_FrameType FrameType,
    boolean IsBroadcast,
    const uint8* PhysAddrPtr,
    const Eth_DataType* DataPtr,
    uint16 LenByte
);
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
void EthIf_TxConfirmation
(
    uint8 CtrlIdx,
    Eth_BufIdxType BufIdx,
    Std_ReturnType Result
);


/**
* @brief          This function indicate that driver mode has been changed
* @details        Called asynchronously when mode has been read out. Triggered by previous 
*                 Eth_SetControllerMode call. Can directly be called within the trigger functions.
* @warning        This is only an empty stub function provided only to be able
*                 to compile and link the Eth module.
* @param[in]      CtrlIdx Index of the controller which mode has been changed.
* @param[in]      CtrlMode New mode of correspond Eth driver.

*/
void EthIf_CtrlModeIndication
( 
    uint8 CtrlIdx,
    Eth_ModeType CtrlMode
);

/**
* @brief          This function indicates that an ingress timestamp was captured
* @details        Returns an ingress timestamp value out of the Switch.
*                 If the HW resolution is lower than the Eth_TimeStampType resolution resp. range,
*                 than the remaining bits will be filled with 0.
* @warning        This is only an empty stub function provided only to be able
*                 to compile and link the EthSwt module.
* @param[in]      CtrlIdx       Index of the controller which mode has been changed.
* @param[in]      MgmtInfoPtr   Management information
* @param[in]      timeStampPtr  Current timestamp
* @param[out]     DataPtr       Ethernet data pointer
*/
void EthIf_SwitchIngressTimeStampIndication
(
    uint8 CtrlIdx,
    Eth_DataType* DataPtr,
    EthSwt_MgmtInfoType* MgmtInfoPtr,
    Eth_TimeStampType* timeStampPtr
);

/**
* @brief          This function indicates that an egress timestamp was captured
* @details        Returns an egress timestamp value out of the Switch.
*                 If the HW resolution is lower than the Eth_TimeStampType resolution resp. range,
*                 than the remaining bits will be filled with 0.
* @warning        This is only an empty stub function provided only to be able
*                 to compile and link the EthSwt module.
* @param[in]      CtrlIdx       Index of the controller which mode has been changed.
* @param[intout]  DataPtr       Ethernet data pointer
* @param[out]     MgmtInfoPtr   Management information
* @param[out]     timeStampPtr  Current timestamp
*/
void EthIf_SwitchEgressTimeStampIndication
(
    uint8 CtrlIdx,
    Eth_DataType* DataPtr,
    EthSwt_MgmtInfoType* MgmtInfoPtr,
    Eth_TimeStampType* timeStampPtr
);

/**
* @brief          This function indicates that mgmt information was received
* @details        Ingress Switch management info indication redirected call to upper layers who registered for the call.
* @warning        This is only an empty stub function provided only to be able
*                 to compile and link the EthSwt module.
* @param[in]      CtrlIdx       Index of the controller which mode has been changed.
* @param[intout]  DataPtr       Ethernet data pointer
* @param[out]     MgmtInfoPtr   Management information
*/
void EthIf_SwitchMgmtInfoIndication
(
    uint8 CtrlIdx,
    Eth_DataType* DataPtr,
    EthSwt_MgmtInfoType* MgmtInfoPtr
);

/**
* @brief          This function indicates that a transceiver's mode was changed
* @details        Called asynchronously when a mode change has been read out. If the function is triggered by previous call of
*                 EthTrcv_SetTransceiverMode it can directly be called within the trigger function.
* @warning        This is only an empty stub function provided only to be able
*                 to compile and link the EthSwt module.
* @param[in]      TrcvIdx       Index of the Ethernet transceiver within the context of the Ethernet Interface
* @param[in]      TrcvMode      Notified Ethernet transceiver mode
*/
void EthIf_TrcvModeIndication
(
    uint8 TrcvIdx,
    EthTrcv_ModeType TrcvMode
);

/**
* @brief          This function indicates that a switch port mode was changed
* @details        The EthIf shall determine the expected notifications based on the EthSwtPort configuration.
*                 In case the EthSwtPort references an EthTrcv the EthIf expects a notification from the EthTrcv via API EthIf_TrcvModeIndication().
*                 Otherwise the EthIf expects a notification from the EthSwt via API EthIf_SwitchPortModeIndication().
* @warning        This is only an empty stub function provided only to be able
*                 to compile and link the EthSwt module.
* @param[in]      SwitchIdx     Index of the switch within the context of the Ethernet Switch Driver
* @param[in]      SwitchPortIdx Index of the port at the addressed switch.
* @param[in]      PortMode      Notified Ethernet Switch port mode.
*/
void EthIf_SwitchPortModeIndication
(
    uint8 SwitchIdx,
    uint8 SwitchPortIdx,
    EthTrcv_ModeType PortMode
);

/*=================================================================================================================================*/
/**
* @brief      Stop forwarding the packets for a multicast group
* @details    The multicast MAC group is removed from the hash filter table.
* 
* @param[in]  IfNum                       The interface number
* @param[in]  pHwAddr                     Pointer to the hardware address to be removed
* 
* @return     None
*/
/*=================================================================================================================================*/
FUNC(void, AUTOMATIC) ComDal_Eth_DrvIf_RemoveFromHashFilter
(
    VAR(uint8, AUTOMATIC) IfNum,
    P2CONST(uint8, AUTOMATIC, ETH_APPL_CONST) pHwAddr
);

/*=================================================================================================================================*/
/**
* @brief      Start forwarding the packets for a multicast group
* @details    The multicast MAC group is added to the hash filter table.
* 
* @param[in]  IfNum                       The interface number
* @param[in]  pHwAddr                     Pointer to the hardware address to be added
* 
* @return     None
*/
/*=================================================================================================================================*/
FUNC(void, AUTOMATIC) ComDal_Eth_DrvIf_AddToHashFilter
(
    VAR(uint8, AUTOMATIC) IfNum,
    P2CONST(uint8, AUTOMATIC, ETH_APPL_CONST) pHwAddr
);
/*=================================================================================================================================*/
/**
* @brief      Try to send an Ethernet frame having the given payload
* @details    Fill the TX buffer descriptor and then transmit the frame.
*             If the driver times out when sending the frame, ERR_TIMEOUT is returned.
*             If the the payload is illegal, ERR_VAL is returned.
*             If the frame was sent successfully, ERR_OK is returned.
* 
* @param[in]  IfNum                       The interface number
* @param[in]  pPayload                    Pointer to the frame payload
* @param[in]  PayloadLen                  Payload length
* 
* @return     The error status: ERR_TIMEOUT, ERR_VAL, ERR_OK
*/
/*=================================================================================================================================*/
FUNC(ComDal_Eth_TcpipReturnType, AUTOMATIC) ComDal_Eth_DrvIf_WriteFrame
(
    VAR(uint8, AUTOMATIC) IfNum,
    P2VAR(void, AUTOMATIC, ETH_APPL_DATA) pPayload,
    VAR(uint16, AUTOMATIC) PayloadLen
);

FUNC(void, AUTOMATIC) ComDal_Eth_DrfIf_PhyInit
(
    VAR(uint8, AUTOMATIC) CtrlIdx
);


#endif /* COMDAL_MAX_ETH_INTERFACES > 0U */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ETHIF_CBK_H */
/** @} */
