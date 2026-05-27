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
 *  This file contains configuration for a stub. It shall be replaced with Tm.h from AUTOSAR Time Service module
 *
 */

/*==================================================================================================
*                                        INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#include "Gpt.h"
#include "Tm.h"

/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/

#define ETH_43_PFE_START_SEC_CODE
#include "Eth_43_PFE_MemMap.h"

/**
* @brief          Performs busy waiting by polling with a guaranteed minimum waiting time.
* @param[in]      WaitingTimeMin Minimum waiting time in microseconds
* @retval         E_OK
*/
Std_ReturnType Tm_BusyWait1us32bit(uint8 WaitingTimeMin)
{
    volatile Gpt_ValueType TimeStart;
    volatile Gpt_ValueType TimeNow;

    /* Shall be 0xFFFFFFFFUL, subtracting 1 to work around a bug in Pit driver */
    Gpt_StartTimer(TM_STUB_GPT_CHANNEL, 0xFFFFFFFFUL-1U);

    TimeStart = Gpt_GetTimeElapsed(TM_STUB_GPT_CHANNEL);
    do
    {
        TimeNow = Gpt_GetTimeElapsed(TM_STUB_GPT_CHANNEL);
    /* Time difference calculation works correctly through wraps if it wraps at max uint32 value */
    } while ((TimeNow - TimeStart) < ((uint32)WaitingTimeMin * TM_STUB_CFG_TICKS_PER_US));

    Gpt_StopTimer(TM_STUB_GPT_CHANNEL);

    return E_OK;
}

#define ETH_43_PFE_STOP_SEC_CODE
#include "Eth_43_PFE_MemMap.h"

