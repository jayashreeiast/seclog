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

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
 1) system and project includes
 2) needed interfaces from external units
 3) internal and external interfaces from this unit
==================================================================================================*/

#include "sample_app_ethswt_data_definition.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/


/*==================================================================================================
                                      FILE VERSION CHECKS
==================================================================================================*/


/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/


/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/

#define ETH_43_PFE_START_SEC_CONST_8
#include "Eth_43_PFE_MemMap.h"

const uint8                     ethCtrl0 = 0U;

#define ETH_43_PFE_STOP_SEC_CONST_8
#include "Eth_43_PFE_MemMap.h"

#define ETH_43_PFE_START_SEC_VAR_INIT_8
#include "Eth_43_PFE_MemMap.h"

/* Configure the ptpMulticastMac used as a destination address for sending a sync frame to the PTP multicast address */
uint8   ptpMulticastMac[6U] = {0x01U, 0x80U, 0xC2U, 0x00U, 0x00U, 0x0EU};
/* Configure a broadcastMac */
uint8    broadcastMac[6U] = {0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU};

uint8   unicastMac[6U] = {0x0AU, 0x0BU, 0x0CU, 0x00U, 0x00U, 0x01U};

#define ETH_43_PFE_STOP_SEC_VAR_INIT_8
#include "Eth_43_PFE_MemMap.h"

#define ETH_43_PFE_START_SEC_VAR_INIT_8_NO_CACHEABLE
#include "Eth_43_PFE_MemMap.h"

/* Flags for switching between tasks */
volatile uint16             gptNotificationCount = 0U;
volatile uint8              getRxTimeStamp = 0U;
volatile uint8              sendFrames = 0U;
volatile uint8              getCounters = 0U;
volatile uint8              switchMainFunction = 0U;
#ifdef NXP_LOG_ENABLED
volatile uint8              clearCacheMemory = 0U;
#endif
volatile uint8              pfeMainFunction = 0U;
volatile uint32             pitIsrCount = 0U;
volatile uint8              RestartA53Req = 0U;

#define ETH_43_PFE_STOP_SEC_VAR_INIT_8_NO_CACHEABLE
#include "Eth_43_PFE_MemMap.h"

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/


/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL FUNCTIONS
==================================================================================================*/


#ifdef __cplusplus
}
#endif
