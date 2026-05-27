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

#include "sample_app_ethswt_gpt.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/
/* To make 1ms period for GPT interrupt, PIT timer is used, clocked by XBAR_DIV3_CLK
 * With current MCU configuration, XBAR_DIV3_CLK is 133MHz = CORE_DFS1_CLK (800MHz) divided by 6
 * Then need to divide by 1000 to get miliseconds
 * Resulting value is 133333
 */
#define GPT_TIMER_VALUE (800000000UL / 6UL / 1000UL)

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


/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/


/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL FUNCTIONS
==================================================================================================*/

#if (USE_GPT_MODULE==STD_ON)

#define ETH_43_PFE_START_SEC_CODE
#include "Eth_43_PFE_MemMap.h"

/*================================================================================================*/

#ifdef LWIP
extern uint32 g_sys_milliseconds;
#endif

void EthSwt_Tst_PitNotification(void)
{
    pitIsrCount++;

#ifdef LWIP
    g_sys_milliseconds++;
#endif

    gptNotificationCount++;
    /* One notification per 1ms */
    if ((gptNotificationCount % ETH_43_PFE_CFG_MAIN_FCN_PERIOD_MS) == 0U)
    {
        pfeMainFunction = 1U;
    }
    if ((gptNotificationCount % 50U) == 0U)
    {
        switchMainFunction = 1U;
    }
#ifdef NXP_LOG_ENABLED
    if ((gptNotificationCount % 100U) == 0U)
    {
        clearCacheMemory = 1U;
    }
#endif
    if ((gptNotificationCount % 1000U) == 0U)
    {
        sendFrames = 1U;
    }
    if ((gptNotificationCount % 500U) == 0U)
    {
        getCounters = 1U;
    }
}


/*================================================================================================*/
/**
@brief      SampleAppGptInit - Initialize the Gpt task
@details    After calling this function the task containing Gpt driver should be initialized

@return     Returns the value of success
@retval     E_OK or E_NOT_OK

@pre None
@post None
*/
Std_ReturnType SampleAppGptInit(void)
{
    Gpt_Init(NULL_PTR);

    /* Enable interrupt and start PIT timer channel used for switching tasks */
    Gpt_EnableNotification(SWITCH_TASK_GPT_CHANNEL);
    Gpt_StartTimer(SWITCH_TASK_GPT_CHANNEL, GPT_TIMER_VALUE);

    return(E_OK);
}

#define ETH_43_PFE_STOP_SEC_CODE
#include "Eth_43_PFE_MemMap.h"

#endif /* USE_GPT_MODULE==STD_ON */

/*================================================================================================*/
#ifdef __cplusplus
}
#endif
