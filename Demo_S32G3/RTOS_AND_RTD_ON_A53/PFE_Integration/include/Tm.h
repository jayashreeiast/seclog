/*
 *  Copyright 2025 NXP
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

/*
 *  This file is a stub. It shall be replaced with Tm.h from AUTOSAR Time Service module
 *
 */

#ifndef TM_H
#define TM_H

#include "StandardTypes.h"

/*
 * The value depends on underlying GPT timer configuration.
 * It must be in range 1 to 16843008
 */
#define TM_STUB_CFG_TICKS_PER_US        133U

#define ETH_43_PFE_START_SEC_CODE

#include "Eth_43_PFE_MemMap.h"

extern Std_ReturnType Tm_BusyWait1us32bit(uint8 WaitingTimeMin);

#define ETH_43_PFE_STOP_SEC_CODE
#include "Eth_43_PFE_MemMap.h"

#endif /* TM_H */

