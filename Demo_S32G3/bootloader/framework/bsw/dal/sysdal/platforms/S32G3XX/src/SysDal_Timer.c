/*===================================================================================================*/
/*
*   @file              SysDal_Timer.c
*   project            AUTOSAR 4.4 GOLDVIP
*   platform           CORTEXM
*
*   @brief             Integration Framework - SysDal timers module
*   @details           This module is responsible for hardware timer configuration and will provide
*                      time measurement services
*
*   Autosar Version    4.4.0
*   Build Version      S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*   All Rights Reserved.
*/
/*===================================================================================================*/

#ifdef __cplusplus
extern "C"{
#endif

/*===================================================================================================*/
/*                                         INCLUDE FILES
   1) system and project includes
   2) needed interfaces from external units
   3) internal and external interfaces from this unit
*/
/*===================================================================================================*/
#include "SysDal_Rtm_Cfg.h"

#if (SYSDAL_RTM_ENABLED == STD_ON)

#include "SysDal_Timer.h"
#include "Mcal.h"

#if (SYSDAL_RTM_USE_SYSTICK == STD_OFF)
#include "Gpt.h"
#else
#include "OsIf.h"
#include "S32G399A.h"
#endif /* SYSDAL_RTM_USE_GPT == STD_OFF */

/*===================================================================================================*/
/*                              SOURCE FILE VERSION INFORMATION                                      */
/*===================================================================================================*/
#define SYSDAL_TIMER_VENDOR_ID_C                            43

#define SYSDAL_TIMER_AR_RELEASE_MAJOR_VERSION_C             4
#define SYSDAL_TIMER_AR_RELEASE_MINOR_VERSION_C             4
#define SYSDAL_TIMER_AR_RELEASE_REVISION_VERSION_C          0

#define SYSDAL_TIMER_SW_MAJOR_VERSION_C                     24
#define SYSDAL_TIMER_SW_MINOR_VERSION_C                     12
#define SYSDAL_TIMER_SW_PATCH_VERSION_C                     0

/*===================================================================================================*/
/*                                      FILE VERSION CHECKS                                          */
/*===================================================================================================*/

/* Check if source file and SYSDAL TIMER header file are of the same vendor */
#if (SYSDAL_TIMER_VENDOR_ID_C != SYSDAL_TIMER_VENDOR_ID)
    #error "SysDal_Timer.c and SysDal_Timer.h have different vendor ids"
#endif

/* Check if source file and SYSDAL TIMER header file are of the same Autosar version */
#if ((SYSDAL_TIMER_AR_RELEASE_MAJOR_VERSION_C != SYSDAL_TIMER_AR_RELEASE_MAJOR_VERSION) || \
     (SYSDAL_TIMER_AR_RELEASE_MINOR_VERSION_C != SYSDAL_TIMER_AR_RELEASE_MINOR_VERSION) || \
     (SYSDAL_TIMER_AR_RELEASE_REVISION_VERSION_C != SYSDAL_TIMER_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of SysDal_Timer.c and SysDal_Timer.h are different"
#endif

/* Check if source file and SYSDAL TIMER header file are of the same Software version */
#if ((SYSDAL_TIMER_SW_MAJOR_VERSION_C != SYSDAL_TIMER_SW_MAJOR_VERSION) || \
     (SYSDAL_TIMER_SW_MINOR_VERSION_C != SYSDAL_TIMER_SW_MINOR_VERSION) || \
     (SYSDAL_TIMER_SW_PATCH_VERSION_C != SYSDAL_TIMER_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of SysDal_Timer.c and SysDal_Timer.h are different"
#endif

/* Check if source file and SYSDAL TIMER configuration header file are of the same vendor */
#if (SYSDAL_TIMER_VENDOR_ID_C != SYSDAL_RTM_CFG_VENDOR_ID)
    #error "SysDal_Timer.c and SysDal_Rtm_Cfg.h have different vendor ids"
#endif

/* Check if source file and SYSDAL TIMER configuration header file are of the same Autosar version */
#if ((SYSDAL_TIMER_AR_RELEASE_MAJOR_VERSION_C != SYSDAL_RTM_CFG_AR_RELEASE_MAJOR_VERSION) || \
     (SYSDAL_TIMER_AR_RELEASE_MINOR_VERSION_C != SYSDAL_RTM_CFG_AR_RELEASE_MINOR_VERSION) || \
     (SYSDAL_TIMER_AR_RELEASE_REVISION_VERSION_C != SYSDAL_RTM_CFG_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of SysDal_Timer.c and SysDal_Rtm_Cfg.h are different"
#endif

/* Check if source file and SYSDAL TIMER configuration header file are of the same software version */
#if ((SYSDAL_TIMER_SW_MAJOR_VERSION_C != SYSDAL_RTM_CFG_SW_MAJOR_VERSION) || \
     (SYSDAL_TIMER_SW_MINOR_VERSION_C != SYSDAL_RTM_CFG_SW_MINOR_VERSION) || \
     (SYSDAL_TIMER_SW_PATCH_VERSION_C != SYSDAL_RTM_CFG_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of SysDal_Timer.c and SysDal_Rtm_Cfg.h are different"
#endif

/*===================================================================================================*/
/*                                             LOCAL VARIABLES                                       */
/*===================================================================================================*/

/* SysDal timer overflows counter */
static VAR(uint32, AUTOMATIC) SysDal_u32TimerOverflows;

/*===================================================================================================*/
/**
* @brief      Initialize the SysDal timer
* @details    This function is used for time measurements and will configure either a GPT timer or
*             the system SysTick timer. Where available the SysTick timer should be used as default
*
* @param[in]  None
*
* @return     None
*/
/*===================================================================================================*/
FUNC(void, AUTOMATIC) SysDal_Timer_Init(void)
{
#if (SYSDAL_RTM_USE_SYSTICK == STD_ON)
    OsIf_Init(NULL_PTR);
    /* Enable sys_tick */
    S32_SysTick->CSRr = (S32_SysTick->CSRr | S32_SysTick_CSR_TICKINT(1u));
#else
    Gpt_EnableNotification(SYSDAL_RTM_TIMER_ID);
    Gpt_StartTimer(SYSDAL_RTM_TIMER_ID, SYSDAL_RTM_TIMER_PERIOD);
    SysDal_u32TimerOverflows = 0;
#endif /* SYSDAL_RTM_USE_SYSTICK == STD_ON */
}


/*===================================================================================================*/
/**
* @brief      De-initialize the SysDal timer
* @details    Stops the previously configured timer (SysTick/General Purpose Timer)
*
* @param[in]  None
*
* @return     None
*/
/*===================================================================================================*/
FUNC(void, AUTOMATIC) SysDal_Timer_DeInit(void)
{
#if (SYSDAL_RTM_USE_SYSTICK == STD_ON)
    S32_SysTick->CSRr = S32_SysTick_CSR_ENABLE(0u) | S32_SysTick_CSR_TICKINT(0u);
#else
    Gpt_DisableNotification(SYSDAL_RTM_TIMER_ID);
    Gpt_StopTimer(SYSDAL_RTM_TIMER_ID);
#endif /* SYSDAL_RTM_USE_SYSTICK == STD_OFF */
}


/*===================================================================================================*/
/**
* @brief      Get the timer overflows counter
* @details    The timer overflows counter reflects how many times the timer reseted his value to 0x0
*
* @param[out] pTimerOverflows - number of elapsed timer overflows
*
* @return     None
*/
/*===================================================================================================*/
FUNC(void, AUTOMATIC) SysDal_Timer_GetOverflows(P2VAR(uint32, AUTOMATIC, RTM_APPL_DATA) pTimerOverflows)
{
    *pTimerOverflows = SysDal_u32TimerOverflows;
}

/*===================================================================================================*/
/**
* @brief      Get the timer current ticks value
* @details    The UP/DOWN counting direction will be internally handled, and the value returned
*             represents the number of ticks until the timer overflow.
*
* @param[out] pTimerTicks - current number of timer ticks
*
* @return     None
*/
/*===================================================================================================*/
FUNC(void, AUTOMATIC) SysDal_Timer_GetTicks(P2VAR(uint32, AUTOMATIC, RTM_APPL_DATA) pTimerTicks)
{
#if (SYSDAL_RTM_USE_SYSTICK == STD_ON)
    *pTimerTicks = OsIf_GetCounter((OsIf_CounterType)OSIF_COUNTER_SYSTEM);
#else
    *pTimerTicks = Gpt_GetTimeRemaining((Gpt_ChannelType)SYSDAL_RTM_TIMER_ID);
#endif /* SYSDAL_RTM_USE_SYSTICK == STD_ON */
}

#endif /* (SYSDAL_RTM_ENABLED == STD_ON) */

/*===================================================================================================*/
/**
* @brief      Interrupt handler for RTM timer
* @details    This interrupt will increment the timer counter
*
* @param[in]  None
*
* @return     None
*/
/*===================================================================================================*/
FUNC(void, AUTOMATIC) SysDal_Rtm_EventHandler(void)
{
#if (SYSDAL_RTM_ENABLED == STD_ON)
    SuspendAllInterrupts();

    SysDal_u32TimerOverflows++;

    ResumeAllInterrupts();
#endif /* (SYSDAL_RTM_ENABLED == STD_ON) */
}

/*===================================================================================================*/
#ifdef __cplusplus
}
#endif
