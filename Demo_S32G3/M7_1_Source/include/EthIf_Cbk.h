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

#ifndef ETHIF_CBK_H
  #define ETHIF_CBK_H
#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
*                                        INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
/* Common types for all communication modules*/
#include "Eth_43_PFE.h"

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/

#define ETHIF_VENDOR_ID                    43
#define ETHIF_MODULE_ID                    65
#define ETHIF_AR_RELEASE_MAJOR_VERSION     4
#define ETHIF_AR_RELEASE_MINOR_VERSION     4
#define ETHIF_AR_RELEASE_REVISION_VERSION  0
#define ETHIF_SW_MAJOR_VERSION             0
#define ETHIF_SW_MINOR_VERSION             8
#define ETHIF_SW_PATCH_VERSION             0
/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/

/*==================================================================================================
*                                          CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/

/*==================================================================================================
*                                            ENUMS
==================================================================================================*/

/*==================================================================================================
*                                STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
*                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
*                                    FUNCTION PROTOTYPES
==================================================================================================*/

#define ETH_43_PFE_START_SEC_CODE
#include "Eth_43_PFE_MemMap.h"

extern void EthIf_RxIndication(\
                           uint8 CtrlIdx,\
                           Eth_FrameType FrameType, \
                           boolean IsBroadcast, \
                           uint8 * PhysAddrPtr, \
                           Eth_DataType *  DataPtr,\
                           uint16 LenByte);

extern void EthIf_TxConfirmation(\
                                            uint8 CtrlIdx,\
                                            Eth_BufIdxType BufIdx, \
                                            Std_ReturnType Result);
extern void EthIf_CtrlModeIndication( \
                                            uint8 CtrlIdx, \
                                            Eth_ModeType CtrlMode \
                                                     );

extern void EthIf_TrcvModeIndication( uint8 TrcvIdx,
                                                      EthTrcv_ModeType TrcvMode
                                                    );

extern void EthIf_SwitchPortModeIndication( uint8 SwitchIdx,
                                                               uint8 SwitchPortIdx,
                                                               EthTrcv_ModeType PortMode
                                                             );

extern Std_ReturnType EthIf_GetTxMgmtObject( uint8 CtrlIdx,
                                                            Eth_BufIdxType BufIdx,
                                                            EthSwt_MgmtObjectType **MgmtObjectPtr
                                                          );
extern Std_ReturnType EthIf_GetRxMgmtObject( uint8 CtrlIdx,
                                                        Eth_DataType *  DataPtr,
                                                        EthSwt_MgmtObjectType **MgmtObjectPtr
                                                      );

#define ETH_43_PFE_STOP_SEC_CODE
#include "Eth_43_PFE_MemMap.h"

#ifdef __cplusplus
}
#endif

#endif /* ETHIF_CBK_H */
/** @} */
