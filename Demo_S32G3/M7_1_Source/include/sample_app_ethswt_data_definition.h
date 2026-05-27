/*
 *  (c) Copyright 2006-2016 Freescale Semiconductor, Inc.
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

#ifndef SAMPLE_APP_ETHSWT_DATA_DEFINITION_H
#define SAMPLE_APP_ETHSWT_DATA_DEFINITION_H

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                         INCLUDE FILES
 1) system and project includes
 2) needed interfaces from external units
 3) internal and external interfaces from this unit
==================================================================================================*/
/**
@file        modules.h
@brief Include Standard types & defines
*/

#include "typedefs.h"
#include "Std_Types.h"
#include "modules.h"

#include "Det.h"
#include "Dem.h"
#include "Mcu.h"
#include "Gpt.h"
#ifdef CONFIGURE_SOC
    #include "Port.h"
#endif
#include "EthIf_Cbk.h"
#include "Eth_43_PFE.h"


/*==================================================================================================
                               SOURCE FILE VERSION INFORMATION
==================================================================================================*/

/*==================================================================================================
                                      FILE VERSION CHECKS
==================================================================================================*/

/*==================================================================================================
                                           CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/
#define ETHSWT_TST_BUFFER_SIZE                  (100U)

#define ETHSWT_FAST_ETHERNET_PHY_ADDR           (0x6U)

#define ETHSWT_TRCV_BASIC_MODE_STATUS_REG_ADDR  (0x1U)
#define ETHSWT_TRCV_LINK_STATUS_MASK            (0x4U)

#define NUM_CONTROLLER_USED                     (ETH_43_PFE_NUM_CONTROLLER_CFG)
#if (defined(PFE_CFG_FCI_ENABLE) && (defined(FCI_L2BR_TEST) || defined(FCI_RTABLE_TEST)))
    #define NUM_TX_CONTROLLER_USED                     (ETH_43_PFE_NUM_CONTROLLER_CFG)
#else
    #define NUM_TX_CONTROLLER_USED                     (ETH_43_PFE_NUM_CONTROLLER_CFG - 1U)
#endif

/*==================================================================================================
                                             ENUMS
==================================================================================================*/

/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/

/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

#define ETH_43_PFE_START_SEC_CONST_8
#include "Eth_43_PFE_MemMap.h"

extern const uint8                      ethCtrl0;

#define ETH_43_PFE_STOP_SEC_CONST_8
#include "Eth_43_PFE_MemMap.h"

#define ETH_43_PFE_START_SEC_VAR_INIT_8
#include "Eth_43_PFE_MemMap.h"

extern uint8    ptpMulticastMac[6U];
extern uint8    broadcastMac[6U];
extern uint8    unicastMac[6U];

#define ETH_43_PFE_STOP_SEC_VAR_INIT_8
#include "Eth_43_PFE_MemMap.h"


#define ETH_43_PFE_START_SEC_VAR_INIT_8_NO_CACHEABLE
#include "Eth_43_PFE_MemMap.h"

/* Flags for switching between tasks */
extern volatile uint16              gptNotificationCount;
extern volatile uint8               getRxTimeStamp;
extern volatile uint8               sendFrames;
extern volatile uint8               getCounters;
extern volatile uint8               switchMainFunction;
#ifdef NXP_LOG_ENABLED
extern volatile uint8               clearCacheMemory;
#endif
extern volatile uint8               pfeMainFunction;
extern volatile uint32              pitIsrCount;
extern volatile uint8               RestartA53Req;

#define ETH_43_PFE_STOP_SEC_VAR_INIT_8_NO_CACHEABLE
#include "Eth_43_PFE_MemMap.h"

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* SAMPLE_APP_ETHSWT_DATA_DEFINITION_H */

